#!/usr/bin/env python3

import decimal
import logging
import os
import subprocess
import sys

import psycopg2
import ruamel.yaml

log = logging.getLogger(__name__)

# input
colte_vars = "/etc/colte/config.yml"

def display_help():
    print("COMMANDS:")
    print("   add {imsi msisdn ip key opc}: adds a user to the network")
    print("   remove {imsi}: removes a user from the network")
    print("   topup {imsi} {money}: adds money to a user's account")
    print("   topup_data {imsi} {data}: adds bytes to a user's account")
    print("   admin {imsi}: gives a user administrative privileges")
    print("   noadmin {imsi}: removes a user's administrative privileges")
    print("   help: displays this message and exits")


def add_user(imsi, msisdn, ip, ki, opc, apn, currency):
    # TODO: error-handling? Check if imsi/msisdn/ip already in system?
    log.info("coltedb: adding user %s", str(imsi))
    try:
        cursor.execute("BEGIN TRANSACTION")

        cursor.execute(
            """
            SELECT id FROM currencies WHERE code=%s
            """,
            [currency],
        )
        currency_ids = cursor.fetchall()
        if len(currency_ids) != 1:
            raise RuntimeError(
                "Invalid currency code {}, try one like IDR or USD".format(currency)
            )

        cursor.execute(
            """
            INSERT INTO customers (imsi, username, balance, currency, enabled, admin, msisdn)
            VALUES
            (%s, %s, %s, %s, %s, %s, %s)
            """,
            [imsi, "", 0, currency_ids[0][0], True, False, msisdn],
        )
        cursor.execute("COMMIT")
    except psycopg2.IntegrityError as e:
        log.warning("Failed to add user due to error %s", str(e))

    subprocess.run(["haulagedb", "add", str(imsi), str(ip)], check=True)


def add_user_open5gs(imsi, ip, ki, opc, apn):
    if apn is not None:
        open5gs_entry = imsi + " " + ip + " " + ki + " " + opc + " " + apn
    else:
        open5gs_entry = imsi + " " + ip + " " + ki + " " + opc

    subprocess.run(["/etc/colte/colte_open5gsdb", "add" + open5gs_entry], check=True)


def remove_user(imsi):
    try:
        cursor.execute("BEGIN TRANSACTION")

        cursor.execute(
            """
            DELETE FROM customers WHERE imsi=%s
            """,
            [imsi],
        )

        cursor.execute("COMMIT")
    except Exception as e:
        log.exception("Failed to remove due to error: %s", str(e), exc_info=True)

    subprocess.run(["haulagedb", "remove", str(imsi)], check=True)


def remove_user_open5gs(imsi):
    subprocess.run(["/etc/colte/colte_open5gsdb", "remove", str(imsi)], check=True)


def topup(imsi, amount):
    old_balance = 0
    new_balance = 0

    commit_str = "SELECT balance FROM customers WHERE imsi = '" + imsi + "' FOR UPDATE"
    numrows = cursor.execute(commit_str)
    if numrows == 0:
        log.error("coltedb error: imsi %s does not exist!",  str(imsi))
        exit(1)

    for row in cursor:
        old_balance = decimal.Decimal(row[0])
        new_balance = decimal.Decimal(amount) + old_balance

    # prompt for confirmation
    promptstr = (
        "coltedb: topup user "
        + str(imsi)
        + " add "
        + str(amount)
        + " to current balance "
        + str(old_balance)
        + " to create new balance "
        + str(new_balance)
        + "? [Y/n] "
    )
    while True:
        answer = input(promptstr)
        if answer == "y" or answer == "Y" or answer == "":
            log.info(
                "coltedb: updating user %s setting new balance to %d",
                str(imsi),
                new_balance,
            )
            commit_str = (
                "UPDATE customers SET balance = "
                + str(new_balance)
                + " WHERE imsi = '"
                + imsi
                + "'"
            )
            cursor.execute(commit_str)
            break
        if answer == "n" or answer == "N":
            log.info("coltedb: cancelling topup\n")
            break


def topup_data(imsi, amount):
    subprocess.run(["haulagedb", "topup", str(imsi), str(amount)], check=True)


def set_admin(imsi):
    log.info("coltedb: giving admin privileges to user %s", str(imsi))
    commit_str = "UPDATE customers SET admin = true WHERE imsi = '" + imsi + "'"
    cursor.execute(commit_str)


def unset_admin(imsi):
    log.info("coltedb: removing admin privileges from user %s", str(imsi))
    commit_str = "UPDATE customers SET admin = false WHERE imsi = '" + imsi + "'"
    cursor.execute(commit_str)


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    print("coltedb: CoLTE Database Configuration Tool")

    if len(sys.argv) <= 1:
        display_help()
        exit(0)

    command = sys.argv[1]

    if command == "help":
        display_help()
        exit(0)

    if os.geteuid() != 0:
        log.error("coltedb: Must run as root!")
        exit(1)

    # load database connection information
    yaml = ruamel.yaml.YAML()
    yaml.indent(sequence=4, mapping=2, offset=2)
    with open(colte_vars, "r") as file:
        colte_data = yaml.load(file.read())

    dbname = colte_data["mysql_db"]
    db_user = colte_data["mysql_user"]
    db_pass = colte_data["mysql_password"]
    db = psycopg2.connect(host="localhost", user=db_user, password=db_pass, dbname=dbname)
    cursor = db.cursor()

    if command == "add":
        if (len(sys.argv) > 9) or (len(sys.argv) < 7):
            log.error('coltedb: incorrect number of args, format is "coltedb add imsi msisdn ip key opc [apn] [currency]". static IP and msisdn currently requred.')
            open5gs_entry = "NULL"
        elif len(sys.argv) == 9:
            add_user(
                imsi = sys.argv[2], msisdn = sys.argv[3], ip = sys.argv[4], ki = sys.argv[5], opc = sys.argv[6], apn = sys.argv[7], currency = sys.argv[8]
            )
            add_user_open5gs(imsi=sys.argv[2], ip=sys.argv[4] , ki=sys.argv[5], opc=sys.argv[6], apn=sys.argv[7])
        elif len(sys.argv) == 8:
            add_user(
                imsi = sys.argv[2], msisdn = sys.argv[3], ip = sys.argv[4], ki = sys.argv[5], opc = sys.argv[6], apn = sys.argv[7], currency = "XXX"
            )
            add_user_open5gs(imsi=sys.argv[2], ip=sys.argv[4] , ki=sys.argv[5], opc=sys.argv[6], apn=sys.argv[7])
        else:
            add_user(
                imsi = sys.argv[2], msisdn = sys.argv[3], ip = sys.argv[4], ki = sys.argv[5], opc = sys.argv[6], apn = sys.argv[7], currency = "XXX"
            )
            add_user_open5gs(imsi=sys.argv[2], ip=sys.argv[4] , ki=sys.argv[5], opc=sys.argv[6], apn=None)
    elif command == "remove":
        if len(sys.argv) != 3:
            log.error('coltedb: incorrect number of args, format is "coltedb remove imsi"')

        remove_user(imsi=sys.argv[2])
        remove_user_open5gs(imsi=sys.argv[2])
    elif command == "topup":
        if len(sys.argv) != 4:
            log.error('coltedb: incorrect number of args, format is "coltedb topup imsi money"')

        topup(imsi=sys.argv[2], amount=sys.argv[3])
    elif command == "topup_data":
        if len(sys.argv) != 4:
            log.error(
                'coltedb: incorrect number of args, format is "coltedb topup_data imsi data"'
            )
        topup_data(imsi=sys.argv[2], amount=sys.argv[3])
    elif command == "admin":
        if len(sys.argv) != 3:
            log.error('coltedb: incorrect number of args, format is "coltedb admin imsi"')
        set_admin(imsi=sys.argv[2])
    elif command == "noadmin":
        if len(sys.argv) != 3:
            log.error('coltedb: incorrect number of args, format is "coltedb noadmin imsi"')
        unset_admin(imsi=sys.argv[2])
    else:
        display_help()

    db.commit()
    cursor.close()
    db.close()
