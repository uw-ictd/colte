import decimal
import logging
import subprocess

import psycopg2

log = logging.getLogger(__name__)

def add_user(cursor, imsi, msisdn, ip, currency):
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


def remove_user(cursor, imsi):
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


def topup(cursor, imsi, amount):
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


def set_admin(cursor, imsi):
    log.info("coltedb: giving admin privileges to user %s", str(imsi))
    commit_str = "UPDATE customers SET admin = true WHERE imsi = '" + imsi + "'"
    cursor.execute(commit_str)


def unset_admin(cursor, imsi):
    log.info("coltedb: removing admin privileges from user %s", str(imsi))
    commit_str = "UPDATE customers SET admin = false WHERE imsi = '" + imsi + "'"
    cursor.execute(commit_str)


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    raise NotImplementedError(
        "The coltedb_prepaid module must be run as part of coltedb, and does not support standalone execution"
        )
