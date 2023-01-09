#!/usr/bin/env python3

import logging
import os
import sys

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
    print("   speed {imsi dl_value dl_unit ul_value ul_unit}: Change bandwith per UE unit values 0=bps 1=Kbps 2=Mbps 3=Gbps 4=Tbps")
    print("   default values are as follows: APN \"internet\", dl_bw/ul_bw 1 Gbps, PGW address is 127.0.0.3, IPv4 only ")
    print("   help: displays this message and exits")


if __name__ == "__main__":
    try:
        import colorlog

        handler = colorlog.StreamHandler()
        handler.setFormatter(
            colorlog.ColoredFormatter(
                "%(log_color)s%(levelname)s(%(name)s): %(message)s"
            )
        )
        log = colorlog.getLogger(__name__)
        log.setLevel(logging.INFO)
        log.addHandler(handler)
    except Exception as e:
        logging.basicConfig(level=logging.INFO)
        log = logging.getLogger(__name__)
        log.info(
            "System does not support colored logging due to exception:", exc_info=True
        )
        log.info("Continuing operation with standard logging")

    print("coltedb: CoLTE Database Configuration Tool")

    try:
        import coltedb_prepaid as accounting
    except ModuleNotFoundError as e:
        log.info("colte-prepaid db management not installed, skipping")
        accounting = None

    try:
        import coltedb_cn_4g as core_network
    except ModuleNotFoundError as e:
        log.info("colte-cn-4g db management not installed, skipping")
        core_network = None

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

    if command == "add":
        if (len(sys.argv) > 9) or (len(sys.argv) < 7):
            log.error(
                'coltedb: incorrect number of args, format is "coltedb add imsi msisdn ip key opc [apn] [currency]". static IP and msisdn currently requred.'
            )
            open5gs_entry = "NULL"
        elif len(sys.argv) == 9:
            if accounting is not None:
                accounting.add_user(
                    db_name=dbname,
                    db_user=db_user,
                    db_pass=db_pass,
                    imsi=sys.argv[2],
                    msisdn=sys.argv[3],
                    ip=sys.argv[4],
                    currency=sys.argv[8],
                )
            if core_network is not None:
                core_network.add_user(
                    imsi=sys.argv[2],
                    ip=sys.argv[4],
                    ki=sys.argv[5],
                    opc=sys.argv[6],
                    apn=sys.argv[7],
                )
        elif len(sys.argv) == 8:
            if accounting is not None:
                accounting.add_user(
                    db_name=dbname,
                    db_user=db_user,
                    db_pass=db_pass,
                    imsi=sys.argv[2],
                    msisdn=sys.argv[3],
                    ip=sys.argv[4],
                    currency="XXX",
                )
            if core_network is not None:
                core_network.add_user(
                    imsi=sys.argv[2],
                    ip=sys.argv[4],
                    ki=sys.argv[5],
                    opc=sys.argv[6],
                    apn=sys.argv[7],
                )
        else:
            if accounting is not None:
                accounting.add_user(
                    db_name=dbname,
                    db_user=db_user,
                    db_pass=db_pass,
                    imsi=sys.argv[2],
                    msisdn=sys.argv[3],
                    ip=sys.argv[4],
                    currency="XXX",
                )
            if core_network is not None:
                core_network.add_user(
                    imsi=sys.argv[2],
                    ip=sys.argv[4],
                    ki=sys.argv[5],
                    opc=sys.argv[6],
                    apn=None,
                )

    elif command == "remove":
        if len(sys.argv) != 3:
            log.error(
                'coltedb: incorrect number of args, format is "coltedb remove imsi"'
            )
        if accounting is not None:
            accounting.remove_user(
                db_name=dbname, db_user=db_user, db_pass=db_pass, imsi=sys.argv[2]
            )
        if core_network is not None:
            core_network.remove_user(imsi=sys.argv[2])

    elif command == "topup":
        if len(sys.argv) != 4:
            log.error(
                'coltedb: incorrect number of args, format is "coltedb topup imsi money"'
            )
        if accounting is None:
            raise NotImplementedError(
                "topup has no effect with no installed accounting module"
            )
        accounting.topup(
            db_name=dbname,
            db_user=db_user,
            db_pass=db_pass,
            imsi=sys.argv[2],
            amount=sys.argv[3],
        )

    elif command == "topup_data":
        if len(sys.argv) != 4:
            log.error(
                'coltedb: incorrect number of args, format is "coltedb topup_data imsi data"'
            )
        if accounting is None:
            raise NotImplementedError(
                "topup_data has no effect with no installed accounting module"
            )
        accounting.topup_data(imsi=sys.argv[2], amount=sys.argv[3])

    elif command == "admin":
        if len(sys.argv) != 3:
            log.error(
                'coltedb: incorrect number of args, format is "coltedb admin imsi"'
            )
        if accounting is None:
            raise NotImplementedError(
                "admin has no effect with no installed accounting module"
            )
        accounting.set_admin(
            db_name=dbname, db_user=db_user, db_pass=db_pass, imsi=sys.argv[2]
        )

    elif command == "noadmin":
        if len(sys.argv) != 3:
            log.error(
                'coltedb: incorrect number of args, format is "coltedb noadmin imsi"'
            )
        if accounting is None:
            raise NotImplementedError(
                "noadmin has no effect with no installed accounting module"
            )
        accounting.unset_admin(
            db_name=dbname, db_user=db_user, db_pass=db_pass, imsi=sys.argv[2]
        )

    elif command == "speed":
        if len(sys.argv) != 7 :
            log.error(
                'coltedb: incorrect number of args, format is "coltedb speed imsi dl_value dl_unit ul_value dl_unit units values are 0=bps 1=Kbps 2=Mbps 3=Gbps 4=Gbps"'
            )
        else:
            if core_network is not None:
                core_network.speed_user(
                    imsi=sys.argv[2],
                    dl_value=sys.argv[3],
                    dl_unit=sys.argv[4],
                    ul_value=sys.argv[5],
                    ul_unit=sys.argv[6],
                )


    else:
        display_help()
