#!/usr/bin/env python3

import argparse
import logging
import shutil
import subprocess

from pathlib import Path

import MySQLdb
import psycopg2
import yaml

logging.basicConfig(level=logging.DEBUG)


def read_colte_config(config_path):
    with open(config_path) as f:
        config_file = yaml.load(f, Loader=yaml.SafeLoader)
        try:
            db_name = config_file["mysql_db"]
            db_user = config_file["mysql_user"]
            db_pass = config_file["mysql_password"]
        except KeyError as e:
            logging.error(
                "Unable to read database information from config file %s", config_path
            )
            raise e

    return (db_name, db_user, db_pass)


def canonicalize_currency(code, symbol, name, pg_conn):
    # Check if the code is already registered
    code = code.upper()
    cursor = pg_conn.cursor()

    cursor.execute(
        """
        SELECT id, name, code, symbol
        FROM currencies
        WHERE code=%s;
        """,
        [code],
    )

    rows = cursor.fetchall()

    if len(rows) == 0:
        # If inserting, ensure the inserted values are valid
        if name is None:
            logging.error(
                "The currency %s, is new, you must also provide a plaintext currency name with the option --currency-name ",
                code,
            )
        if symbol is None:
            logging.error(
                "The currency %s, is new, you must also provide the currency's symbol with the option --currency-symbol ",
                code,
            )

        # Insert the new currency
        cursor.execute(
            """
            INSERT INTO currencies("name", "code", "symbol")
            VALUES
            (%s, %s, %s)
            """,
            [name, code, symbol],
        )

        cursor.execute(
            """
            SELECT id, name, code, symbol
            FROM currencies
            WHERE code=%s;
            """,
            [code],
        )
        inserted_id = cursor.fetchall()
        if len(inserted_id) != 1:
            raise RuntimeError(
                "The just inserted currency code didn't match exactly one row, which should never happen."
            )

        return inserted_id[0][0]

    elif len(rows) == 1:
        # There was a single match, assert that the other parts match
        if (name is not None and rows[0][1] != name) or (
            symbol is not None and rows[0][3] != symbol
        ):
            logging.error(
                "Could not set the currency because the provided name and symbol do not match those of an existing currency already inserted at the same code"
            )
            logging.error("You attempted to insert %s, %s, %s", code, name, symbol)
            logging.error(
                "Which conflicts with the existing %s, %s, %s",
                rows[0][2],
                rows[0][1],
                rows[0][3],
            )
            raise RuntimeError("Cannot proceed with a currency conflict")

        return rows[0][0]
    else:
        raise RuntimeError(
            "The provided currency code matches multiple rows, which should never happen."
        )


def migrate_customers(mysql_conn, pg_conn, currency_id):
    mysql_cursor = mysql_conn.cursor()
    mysql_cursor.execute("BEGIN")
    pg_cursor = pg_conn.cursor()

    mysql_cursor.execute(
        "select imsi, username, balance, enabled, admin, msisdn from customers;"
    )
    for row in mysql_cursor:
        enabled = bool(row[3] == 1)
        admin = bool(row[4] == 1)
        username = row[1]
        if username is None:
            username = ""

        new_sub_row = [row[0], username, row[2], currency_id, enabled, admin, row[5]]

        try:
            pg_cursor.execute("BEGIN TRANSACTION")
            pg_cursor.execute(
                """
                INSERT INTO customers("imsi", "username", "balance", "currency", "enabled", "admin", "msisdn")
                VALUES
                (%s, %s, %s, %s, %s, %s, %s)""",
                new_sub_row,
            )
            logging.debug(
                "Migrating customer -> customer source row %s -> new customer %s",
                row,
                new_sub_row,
            )
            pg_cursor.execute("COMMIT")
        except psycopg2.IntegrityError as e:
            logging.warning(
                "Skipping insert subscriber %s due to error: %s ",
                new_sub_row,
                e,
            )
            pg_cursor.execute("ROLLBACK")

    mysql_cursor.close()
    mysql_conn.commit()


def migrate_haulage(source_db_name, source_db_user, source_db_pass):
    haulage_pg_migrate = shutil.which("haulage-pg-migrate")
    if haulage_pg_migrate is None:
        raise Warning(
            "Unable to find haulage migration tool on path, skipping haulage migration"
        )

    logging.info(
        "Starting haulage migration with haualge-pg-migrate at %s", haulage_pg_migrate
    )

    subprocess.run(
        [
            haulage_pg_migrate,
            "--mysql-db-name",
            source_db_name,
            "--mysql-db-user",
            source_db_user,
            "--mysql-db-pass",
            source_db_pass,
        ],
        shell=False,
        check=True,
    )


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Migrate from the legacy mysql/mariadb system to postgres."
    )
    parser.add_argument(
        "--mysql-db-name",
        help="The database name for the mysql data source, assumed the same as provided in the configuration file unless proveded.",
    )
    parser.add_argument(
        "--mysql-db-user",
        help="The database user for the mysql data source, assumed the same as provided in the configuration file unless proveded.",
    )
    parser.add_argument(
        "--mysql-db-pass",
        help="The database password for the mysql data source, assumed the same as provided in the configuration file unless proveded.",
    )
    parser.add_argument(
        "--currency",
        required=True,
        help="The three character ISO 4217 currency code of the balance currency used by the legacy database",
    )
    parser.add_argument(
        "--currency-symbol",
        help="The currency symbol for the currency used by the legacy database",
    )
    parser.add_argument(
        "--currency-name",
        help="The plain name of the currency used by the legacy database",
    )

    parser.add_argument(
        "-c",
        "--config",
        default=Path("/etc/colte/config.yml"),
        help="The location of the CoLTE config file (version 1)",
    )

    args = parser.parse_args()

    (config_db_name, config_db_user, config_db_pass) = read_colte_config(args.config)

    pg_name = config_db_name
    pg_user = config_db_user
    pg_pass = config_db_pass

    mysql_name = args.mysql_db_name
    mysql_user = args.mysql_db_user
    mysql_pass = args.mysql_db_pass

    if mysql_name is None:
        mysql_name = config_db_name
    if mysql_user is None:
        mysql_user = config_db_user
    if mysql_pass is None:
        mysql_pass = config_db_pass

    mysql_connection = MySQLdb.connect(
        host="localhost", user=mysql_user, passwd=mysql_pass, db=mysql_name
    )
    logging.info("Connected to mysql/mariadb at db=%s, user=%s", mysql_name, mysql_user)

    pg_connection = psycopg2.connect(
        dbname=pg_name, user=pg_user, password=pg_pass, host="127.0.0.1"
    )
    logging.info("Connected to postgres at db=%s, user=%s", pg_name, pg_user)

    logging.info("Beginning migration!")
    legacy_currency_id = canonicalize_currency(
        args.currency, args.currency_symbol, args.currency_name, pg_connection
    )
    migrate_customers(mysql_connection, pg_connection, legacy_currency_id)

    migrate_haulage(mysql_name, mysql_user, mysql_pass)
