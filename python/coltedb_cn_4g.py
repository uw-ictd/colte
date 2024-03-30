import logging
import subprocess

log = logging.getLogger(__name__)


def add_user(imsi, ip, ki, opc, apn):
    if apn is not None:
        open5gs_entry = [str(imsi), str(ip), str(ki), str(opc), str(apn)]
    else:
        open5gs_entry = [str(imsi), str(ip), str(ki), str(opc)]

    subprocess.run(["/etc/colte/colte_open5gsdb", "add"] + open5gs_entry, check=True)


def remove_user(imsi):
    subprocess.run(["/etc/colte/colte_open5gsdb", "remove", str(imsi)], check=True)

def speed_user(imsi, dl_value, dl_unit, ul_value, ul_unit):
    open5gs_entry = [str(imsi), str(dl_value), str(dl_unit), str(ul_value), str(ul_unit)]
    subprocess.run(["/etc/colte/colte_open5gsdb", "speed"] + open5gs_entry, check=True)

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    raise NotImplementedError(
        "The coltedb_cn_4g module must be run as part of coltedb, and does not support standalone execution"
    )
