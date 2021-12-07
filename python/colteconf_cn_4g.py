#!/usr/bin/env python3

import logging
import ruamel.yaml
from ruamel.yaml.comments import CommentedMap, CommentedSeq
import fileinput
import os
import sys
from netaddr import IPNetwork

# This version saves comments/edits in YAML files
yaml = ruamel.yaml.YAML()
yaml.indent(sequence=4, mapping=2, offset=2)

# EPC conf-files
hss = "/etc/open5gs/hss.yaml"
pcrf = "/etc/open5gs/pcrf.yaml"
mme = "/etc/open5gs/mme.yaml"
sgwc = "/etc/open5gs/sgwc.yaml"
sgwu = "/etc/open5gs/sgwu.yaml"
smf = "/etc/open5gs/smf.yaml"
upf = "/etc/open5gs/upf.yaml"

# Setup logging
log = logging.getLogger(__name__)


def update_all_components(colte_data):
    """Update all core network component configs to match the colte config
    """
    # Update yaml files
    _update_hss(colte_data)
    _update_mme(colte_data)
    _update_pcrf(colte_data)
    _update_sgwc(colte_data)
    _update_sgwu(colte_data)
    _update_smf(colte_data)
    _update_upf(colte_data)

    # Update other files
    _update_colte_nat_script(colte_data)
    _update_network_vars(colte_data)

    # always enable kernel ipv4 ip_forward
    _enable_ip_forward()


def stop_all_services():
    _control_epc_services("stop")
    _control_nat_services("stop")

def sync_service_state(colte_data):
    """Start enabled services and update enabled/disabled state
    """
    if colte_data["epc"] == True:
        _control_epc_services("start")
        _control_epc_services("enable")
    else:
        _control_epc_services("disable")

    if colte_data["nat"] == True:
        _control_nat_services("start")
        _control_nat_services("enable")
    else:
        _control_nat_services("disable")


def _enable_ip_forward():
    _replaceAll(
        "/etc/sysctl.conf", "net.ipv4.ip_forward", "net.ipv4.ip_forward=1\n", True
    )
    os.system("sysctl -w net.ipv4.ip_forward=1")


def _update_colte_nat_script(colte_data):
    _replaceAll(
        "/usr/bin/coltenat",
        "ADDRESS=",
        "ADDRESS=" + colte_data["lte_subnet"] + "\n",
        False,
    )


def _update_network_vars(colte_data):
    net = IPNetwork(colte_data["lte_subnet"])
    netstr = str(net[1]) + "/" + str(net.prefixlen)
    _replaceAll("/etc/systemd/network/99-open5gs.network", "Address=", "Address=" + netstr + "\n", True)


def _replaceAll(file, searchExp, replaceExp, replace_once):
    is_replaced = False
    for line in fileinput.input(file, inplace=1):
        if searchExp in line:
            if replace_once:
                if not is_replaced:
                    line = replaceExp
                    is_replaced = True
                else:
                    line = ""
            else:
                line = replaceExp
        sys.stdout.write(line)


def _update_hss(colte_data):
    hss_data = {}

    with open(hss, "r+") as file:
        hss_data = yaml.load(file.read())

        # Disable internal file logging since journald is capturing stdout
        _create_field_if_not_exist(hss_data, ["logger"], CommentedMap())
        hss_data["logger"]["file"] = "/dev/null"

    with open(hss, "w") as file:
        # Save the results
        yaml.dump(hss_data, file)


def _update_mme(colte_data):
    mme_data = {}

    with open(mme, "r+") as file:
        mme_data = yaml.load(file.read())

        # Create fields in the data if they do not yet exist
        _create_field_if_not_exist(mme_data, ["mme"], CommentedMap())

        _create_field_if_not_exist(mme_data, ["mme", "gummei"], CommentedMap())
        _create_field_if_not_exist(
            mme_data, ["mme", "gummei", "plmn_id"], CommentedMap()
        )
        mme_data["mme"]["gummei"]["plmn_id"]["mcc"] = colte_data["mcc"]
        mme_data["mme"]["gummei"]["plmn_id"]["mnc"] = colte_data["mnc"]

        _create_field_if_not_exist(mme_data, ["mme", "tai"], CommentedMap())
        _create_field_if_not_exist(mme_data, ["mme", "tai", "plmn_id"], CommentedMap())
        mme_data["mme"]["tai"]["plmn_id"]["mcc"] = colte_data["mcc"]
        mme_data["mme"]["tai"]["plmn_id"]["mnc"] = colte_data["mnc"]

        mme_data["mme"]["s1ap"] = CommentedSeq()
        mme_data["mme"]["s1ap"].append({"addr": colte_data["enb_iface_addr"]})

        _create_field_if_not_exist(mme_data, ["mme", "network_name"], CommentedMap())
        mme_data["mme"]["network_name"]["full"] = colte_data["network_name"]

        mme_data["mme"]["gtpc"] = CommentedSeq()
        mme_data["mme"]["gtpc"].append({"addr": "127.0.0.2"})

        _create_field_if_not_exist(mme_data, ["sgwc"], CommentedMap())

        mme_data["sgwc"]["gtpc"] = CommentedSeq()
        mme_data["sgwc"]["gtpc"].append({"addr": "127.0.0.3"})

        _create_field_if_not_exist(mme_data, ["smf"], CommentedMap())

        mme_data["smf"]["gtpc"] = CommentedSeq()
        mme_data["smf"]["gtpc"].append({"addr": "127.0.0.4"})
        mme_data["smf"]["gtpc"].append({"addr": "::1"})

        # Disable internal file logging since journald is capturing stdout
        _create_field_if_not_exist(mme_data, ["logger"], CommentedMap())
        mme_data["logger"]["file"] = "/dev/null"

    with open(mme, "w") as file:
        # Save the results
        yaml.dump(mme_data, file)


def _update_pcrf(colte_data):
    pcrf_data = {}

    with open(pcrf, "r+") as file:
        pcrf_data = yaml.load(file.read())

        # Disable internal file logging since journald is capturing stdout
        _create_field_if_not_exist(pcrf_data, ["logger"], CommentedMap())
        pcrf_data["logger"]["file"] = "/dev/null"

    with open(pcrf, "w") as file:
        # Save the results
        yaml.dump(pcrf_data, file)


def _update_sgwc(colte_data):
    sgwc_data = {}
    with open(sgwc, "r") as file:
        sgwc_data = yaml.load(file.read())

        # Create fields in the data if they do not yet exist
        _create_field_if_not_exist(sgwc_data, ["sgwc"], CommentedMap())

        sgwc_data["sgwc"]["gtpc"] = CommentedSeq()
        sgwc_data["sgwc"]["gtpc"].append({"addr": "127.0.0.3"})

        sgwc_data["sgwc"]["pfcp"] = CommentedSeq()
        sgwc_data["sgwc"]["pfcp"].append({"addr": "127.0.0.3"})

        # Link towards the SGW-U
        _create_field_if_not_exist(sgwc_data, ["sgwu"], CommentedMap())

        sgwc_data["sgwu"]["pfcp"] = CommentedSeq()
        sgwc_data["sgwu"]["pfcp"].append({"addr": "127.0.0.6"})

        # Disable internal file logging since journald is capturing stdout
        _create_field_if_not_exist(sgwc_data, ["logger"], CommentedMap())
        sgwc_data["logger"]["file"] = "/dev/null"

    with open(sgwc, "w") as file:
        # Save the results
        yaml.dump(sgwc_data, file)


def _update_sgwu(colte_data):
    sgwu_data = {}
    with open(sgwu, "r") as file:
        sgwu_data = yaml.load(file.read())

        # Create fields in the data if they do not yet exist
        _create_field_if_not_exist(sgwu_data, ["sgwu"], CommentedMap())

        sgwu_data["sgwu"]["gtpu"] = CommentedSeq()
        sgwu_data["sgwu"]["gtpu"].append({"addr": colte_data["enb_iface_addr"]})

        sgwu_data["sgwu"]["pfcp"] = CommentedSeq()
        sgwu_data["sgwu"]["pfcp"].append({"addr": "127.0.0.6"})

        # Link towards the SGW-C
        # TODO(matt9j) This might be the wrong address! Not included in the default
        # create_fields_if_not_exist(sgwu_data, ["sgwc", "pfcp"])
        # sgwu_data["sgwc"]["pfcp"][0]["addr"] = "127.0.0.3"

        # Disable internal file logging since journald is capturing stdout
        _create_field_if_not_exist(sgwu_data, ["logger"], CommentedMap())
        sgwu_data["logger"]["file"] = "/dev/null"

    with open(sgwu, "w") as file:
        # Save the results
        yaml.dump(sgwu_data, file)


def _update_smf(colte_data):
    smf_data = {}
    with open(smf, "r+") as file:
        smf_data = yaml.load(file.read())

        # Create fields in the data if they do not yet exist
        _create_field_if_not_exist(smf_data, ["smf"], CommentedMap())

        smf_data["smf"]["gtpc"] = CommentedSeq()
        smf_data["smf"]["gtpc"].append({"addr": "127.0.0.4"})
        smf_data["smf"]["gtpc"].append({"addr": "::1"})

        smf_data["smf"]["pfcp"] = CommentedSeq()
        smf_data["smf"]["pfcp"].append({"addr": "127.0.0.4"})
        smf_data["smf"]["pfcp"].append({"addr": "::1"})

        smf_data["smf"]["subnet"] = CommentedSeq()
        net = IPNetwork(colte_data["lte_subnet"])
        netstr = str(net[1]) + "/" + str(net.prefixlen)
        smf_data["smf"]["subnet"].append({"addr": netstr})

        smf_data["smf"]["dns"] = CommentedSeq()
        smf_data["smf"]["dns"].append(colte_data["dns"])

        smf_data["smf"]["mtu"] = 1400

        # Create link to UPF
        _create_field_if_not_exist(smf_data, ["upf"], CommentedMap())

        smf_data["upf"]["pfcp"] = CommentedSeq()
        smf_data["upf"]["pfcp"].append({"addr": "127.0.0.7"})

        # Disable 5GC NRF link while operating EPC only
        if "nrf" in smf_data:
            del smf_data["nrf"]

        # Disable internal file logging since journald is capturing stdout
        _create_field_if_not_exist(smf_data, ["logger"], CommentedMap())
        smf_data["logger"]["file"] = "/dev/null"

    with open(smf, "w") as file:
        # Save the results
        yaml.dump(smf_data, file)


def _update_upf(colte_data):
    upf_data = {}
    with open(upf, "r+") as file:
        upf_data = yaml.load(file.read())

        # Create fields in the data if they do not yet exist
        _create_field_if_not_exist(upf_data, ["upf"], CommentedMap())

        upf_data["upf"]["pfcp"] = CommentedSeq()
        upf_data["upf"]["pfcp"].append({"addr": "127.0.0.7"})

        upf_data["upf"]["gtpu"] = CommentedSeq()
        upf_data["upf"]["gtpu"].append({"addr": "127.0.0.7"})

        upf_data["upf"]["subnet"] = CommentedSeq()
        net = IPNetwork(colte_data["lte_subnet"])
        netstr = str(net[1]) + "/" + str(net.prefixlen)
        upf_data["upf"]["subnet"].append({"addr": netstr})

        # Link to the SMF
        # TODO(matt9j) Might not be needed
        # create_fields_if_not_exist(upf_data, ["smf"]["pfcp"])
        # upf_data["smf"]["pfcp"] = {'addr': "127.0.0.3"}

        # Disable internal file logging since journald is capturing stdout
        _create_field_if_not_exist(upf_data, ["logger"], CommentedMap())
        upf_data["logger"]["file"] = "/dev/null"

    with open(upf, "w") as file:
        # Save the results
        yaml.dump(upf_data, file)


def _create_field_if_not_exist(dictionary, field_path, value):
    current_entry = dictionary
    for i, field in enumerate(field_path):
        if (i == len(field_path) - 1) and (
            field not in current_entry or current_entry[field] is None
        ):
            current_entry[field] = value
        else:
            try:
                current_entry = current_entry[field]
            except KeyError as e:
                log.error("Failed to create key at path %s", str(field_path))
                log.error("Current configuration state is: %s", str(dictionary))
                raise KeyError(
                    "Failed to create key at path {}, with base error {}".format(
                        field_path, e
                    )
                )


def _control_nat_services(action):
    os.system("systemctl {} colte-nat".format(action))


def _control_epc_services(action):
    os.system(
        "systemctl {} open5gs-hssd open5gs-mmed open5gs-sgwcd open5gs-sgwud open5gs-pcrfd open5gs-smfd open5gs-upfd".format(
            action
        )
    )

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    if os.geteuid() != 0:
        log.error("Must run as root!")
        raise PermissionError("The current implementation must run as root")

    # Read old vars and update yaml
    with open("/etc/colte/config.yml", "r") as file:
        colte_data = yaml.load(file.read())

    update_all_components(colte_data)

    # Restart everything to pick up new configurations, and don't restart
    # networkd while the EPC or metering are running.
    stop_all_services()
    os.system("systemctl restart systemd-networkd")
    sync_service_state(colte_data)
