#!/usr/bin/env python3

import logging
import os
import sys

import psycopg2
import ruamel.yaml

import coltedb_prepaid as accounting
import coltedb_cn_4g as core_network

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
            accounting.add_user(
                cursor=cursor,
                imsi = sys.argv[2], msisdn = sys.argv[3], ip = sys.argv[4], ki = sys.argv[5], opc = sys.argv[6], apn = sys.argv[7], currency = sys.argv[8]
            )
            core_network.add_user(imsi=sys.argv[2], ip=sys.argv[4] , ki=sys.argv[5], opc=sys.argv[6], apn=sys.argv[7])
        elif len(sys.argv) == 8:
            accounting.add_user(
                cursor=cursor,
                imsi = sys.argv[2], msisdn = sys.argv[3], ip = sys.argv[4], ki = sys.argv[5], opc = sys.argv[6], apn = sys.argv[7], currency = "XXX"
            )
            core_network.add_user(imsi=sys.argv[2], ip=sys.argv[4] , ki=sys.argv[5], opc=sys.argv[6], apn=sys.argv[7])
        else:
            accounting.add_user(
                cursor=cursor,
                imsi = sys.argv[2], msisdn = sys.argv[3], ip = sys.argv[4], ki = sys.argv[5], opc = sys.argv[6], apn = sys.argv[7], currency = "XXX"
            )
            core_network.add_user(imsi=sys.argv[2], ip=sys.argv[4] , ki=sys.argv[5], opc=sys.argv[6], apn=None)
    elif command == "remove":
        if len(sys.argv) != 3:
            log.error('coltedb: incorrect number of args, format is "coltedb remove imsi"')

        accounting.remove_user(cursor=cursor, imsi=sys.argv[2])
        core_network.remove_user(imsi=sys.argv[2])
    elif command == "topup":
        if len(sys.argv) != 4:
            log.error('coltedb: incorrect number of args, format is "coltedb topup imsi money"')

        accounting.topup(cursor=cursor, imsi=sys.argv[2], amount=sys.argv[3])
    elif command == "topup_data":
        if len(sys.argv) != 4:
            log.error(
                'coltedb: incorrect number of args, format is "coltedb topup_data imsi data"'
            )
        accounting.topup_data(imsi=sys.argv[2], amount=sys.argv[3])
    elif command == "admin":
        if len(sys.argv) != 3:
            log.error('coltedb: incorrect number of args, format is "coltedb admin imsi"')
        accounting.set_admin(cursor=cursor, imsi=sys.argv[2])
    elif command == "noadmin":
        if len(sys.argv) != 3:
            log.error('coltedb: incorrect number of args, format is "coltedb noadmin imsi"')
        accounting.unset_admin(cursor=cursor, imsi=sys.argv[2])
    else:
        display_help()

    db.commit()
    cursor.close()
    db.close()
