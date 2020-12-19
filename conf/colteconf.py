#!/usr/bin/env python3

import ruamel.yaml
from ruamel.yaml.comments import CommentedMap
import fileinput
import os
import sys
from netaddr import IPNetwork

# This version saves comments/edits in YAML files
yaml = ruamel.yaml.YAML()
yaml.indent(sequence=4, mapping=2, offset=2)

# Input
colte_vars = "/etc/colte/config.yml"

# EPC conf-files
mme = "/etc/open5gs/mme.yaml"
sgwc = "/etc/open5gs/sgwc.yaml"
sgwu = "/etc/open5gs/sgwu.yaml"
smf = "/etc/open5gs/smf.yaml"
upf = "/etc/open5gs/upf.yaml"

# Haulage
haulage = "/etc/haulage/config.yml"

# Other files
colte_nat_script = "/usr/bin/coltenat"
network_vars = "/etc/systemd/network/99-open5gs.network"
webgui_env = "/etc/colte/webgui.env"
webadmin_env = "/etc/colte/webadmin.env"

def update_env_file(file_name, colte_data):
    env_data = {}
    with open(file_name, 'r') as file:
        env_data = yaml.load(file.read().replace("=", ": "))

        env_data["DB_USER"] = colte_data["mysql_user"]
        env_data["DB_PASSWORD"] = colte_data["mysql_password"]
        env_data["DB_NAME"] = colte_data["mysql_db"]

    # Get data in YAML format
    with open(file_name, 'w') as file:
        # Save the results
        yaml.dump(env_data, file)

    # Update data in correct format
    new_text = ""
    with open(file_name, 'r') as file:
        new_text = file.read().replace(": ", "=")

    # Save in correct format
    with open(file_name, 'w') as file:
        file.write(new_text)

def enable_ip_forward():
    replaceAll("/etc/sysctl.conf", "net.ipv4.ip_forward", "net.ipv4.ip_forward=1", True)

def update_colte_nat_script(colte_data):
    replaceAll(colte_nat_script, "ADDRESS=", "ADDRESS=" + colte_data["lte_subnet"]+ "\n", False)

def update_network_vars(colte_data):
    net = IPNetwork(colte_data["lte_subnet"])
    netstr = str(net[1]) + "/" + str(net.prefixlen)
    replaceAll(network_vars, "Address=", "Address=" + netstr + "\n", True)

def replaceAll(file, searchExp, replaceExp, replace_once):
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

def update_mme(colte_data):
    mme_data = {}

    with open(mme, 'r+') as file:
        mme_data = yaml.load(file.read())

        # Create fields in the data if they do not yet exist
        create_fields_if_not_exist(mme_data, ["mme", "gummei", "plmn_id"])
        create_fields_if_not_exist(mme_data, ["mme", "tai", "plmn_id"])
        create_fields_if_not_exist(mme_data, ["mme", "s1ap"])
        create_fields_if_not_exist(mme_data, ["mme", "network_name"])
        create_fields_if_not_exist(mme_data, ["mme", "gtpc"])
        create_fields_if_not_exist(mme_data, ["sgwc", "gtpc"])
        create_fields_if_not_exist(mme_data, ["smf", "gtpc"])

        # MCC values
        mme_data["mme"]["gummei"]["plmn_id"]["mcc"] = colte_data["mcc"]
        mme_data["mme"]["tai"]["plmn_id"]["mcc"] = colte_data["mcc"]

        # MNC values
        mme_data["mme"]["gummei"]["plmn_id"]["mnc"] = colte_data["mnc"]
        mme_data["mme"]["tai"]["plmn_id"]["mnc"] = colte_data["mnc"]

        # Other values
        mme_data["mme"]["s1ap"][0]["addr"] = colte_data["enb_iface_addr"]
        mme_data["mme"]["network_name"]["full"] = colte_data["network_name"]

        # Hard-coded values
        mme_data["mme"]["gtpc"][0]["addr"] = "127.0.0.2"
        mme_data["sgwc"]["gtpc"][0]["addr"] = "127.0.0.3"
        mme_data["smf"]["gtpc"][0]["addr"] = ["127.0.0.4", "::1"]

    with open(mme, 'w') as file:
        # Save the results
        yaml.dump(mme_data, file)


def update_sgwc(colte_data):
    sgwc_data = {}
    with open(sgwc, 'r') as file:
        sgwc_data = yaml.load(file.read())

        # Create fields in the data if they do not yet exist
        create_fields_if_not_exist(sgwc_data, ["sgwc", "gtpc"])
        create_fields_if_not_exist(sgwc_data, ["sgwc", "pfcp"])

        sgwc_data["sgwc"]["gtpc"][0]["addr"] = "127.0.0.3"
        sgwc_data["sgwc"]["pfcp"][0]["addr"] = "127.0.0.3"

        # Link towards the SGW-U
        create_fields_if_not_exist(sgwc_data, ["sgwu", "pfcp"])
        sgwc_data["sgwu"]["pfcp"][0]["addr"] = "127.0.0.6"

    with open(sgwc, 'w') as file:
        # Save the results
        yaml.dump(sgwc_data, file)


def update_sgwu(colte_data):
    sgwu_data = {}
    with open(sgwu, 'r') as file:
        sgwu_data = yaml.load(file.read())

        # Create fields in the data if they do not yet exist
        create_fields_if_not_exist(sgwu_data, ["sgwu", "gtpu"])
        create_fields_if_not_exist(sgwu_data, ["sgwu", "pfcp"])

        sgwu_data["sgwu"]["gtpu"][0]["addr"] = colte_data["enb_iface_addr"]
        sgwu_data["sgwu"]["pfcp"][0]["addr"] = "127.0.0.6"

        # Link towards the SGW-C
        # TODO(matt9j) This might be the wrong address! Not included in the default
        #create_fields_if_not_exist(sgwu_data, ["sgwc", "pfcp"])
        #sgwu_data["sgwc"]["pfcp"][0]["addr"] = "127.0.0.3"

    with open(sgwu, 'w') as file:
        # Save the results
        yaml.dump(sgwu_data, file)


def update_smf(colte_data):
    smf_data = {}
    with open(smf, 'r+') as file:
        smf_data = yaml.load(file.read())

        # Safe deletions
        if "smf" in smf_data and "gtpc" in smf_data["smf"]:
            del smf_data["smf"]["gtpc"][:]

        if "smf" in smf_data and "pfcp" in smf_data["smf"]:
            del smf_data["smf"]["pfcp"][:]

        if "smf" in smf_data and "pdn" in smf_data["smf"]:
            del smf_data["smf"]["pdn"][:]

        if "smf" in smf_data and "dns" in smf_data["smf"]:
            del smf_data["smf"]["dns"][:]

        # Create fields in the data if they do not yet exist
        create_fields_if_not_exist(smf_data, ["smf"])
        create_fields_if_not_exist(smf_data, ["smf", "sbi"])
        create_fields_if_not_exist(smf_data, ["smf", "gtpc"])
        create_fields_if_not_exist(smf_data, ["smf", "pfcp"])
        create_fields_if_not_exist(smf_data, ["smf", "pdn"])
        create_fields_if_not_exist(smf_data, ["smf", "dns"])

        # Set default values of list fields
        if "gtpc" not in smf_data["smf"]:
            smf_data["smf"]["gtpc"] = []
        if "pfcp" not in smf_data["smf"]:
            smf_data["smf"]["pfcp"] = []
        if "pdn" not in smf_data["smf"]:
            smf_data["smf"]["pdn"] = []
        if "dns" not in smf_data["smf"]:
            smf_data["smf"]["dns"] = []

        smf_data["smf"]["gtpc"].append({'addr': "127.0.0.4"})
        smf_data["smf"]["gtpc"].append({'addr': "::1"})

        smf_data["smf"]["pfcp"].append({'addr': "127.0.0.4"})
        smf_data["smf"]["pfcp"].append({'addr': "::1"})

        smf_data["smf"]["pdn"].append({'addr': colte_data["lte_subnet"]})

        smf_data["smf"]["dns"].append(colte_data["dns"])

        smf_data["smf"]["mtu"] = 1400

        # Create link to UPF
        create_fields_if_not_exist(smf_data, ["upf"])
        if "upf" in smf_data and "pfcp" in smf_data["upf"]:
            del smf_data["upf"]["pfcp"][:]
        if "pfcp" not in smf_data["upf"]:
            smf_data["upf"]["pfcp"] = []
        smf_data["upf"]["pfcp"].append({'addr': "127.0.0.7"})

        # Disable 5GC NRF link while operating EPC only
        if "nrf" in smf_data:
            del smf_data["nrf"]

    with open(smf, 'w') as file:
        # Save the results
        yaml.dump(smf_data, file)


def update_upf(colte_data):
    upf_data = {}
    with open(upf, 'r+') as file:
        upf_data = yaml.load(file.read())

        # Safe deletions
        if "upf" in upf_data and "pfcp" in upf_data["upf"]:
            del upf_data["upf"]["pfcp"][:]
        if "upf" in upf_data and "gtpu" in upf_data["upf"]:
            del upf_data["upf"]["gtpu"][:]
        if "upf" in upf_data and "pdn" in upf_data["upf"]:
            del upf_data["upf"]["pdn"][:]

        # Create fields in the data if they do not yet exist
        create_fields_if_not_exist(upf_data, ["upf"])

        # Set default values of list fields
        if "pfcp" not in upf_data["upf"]:
            upf_data["upf"]["pfcp"] = []
        if "gtpu" not in upf_data["upf"]:
            upf_data["upf"]["gtpu"] = []
        if "pdn" not in upf_data["upf"]:
            upf_data["upf"]["pdn"] = []

        upf_data["upf"]["pfcp"].append({'addr': "127.0.0.7"})
        upf_data["upf"]["gtpu"].append({'addr': "127.0.0.7"})
        upf_data["upf"]["pdn"].append({'addr': colte_data["lte_subnet"]})

        # Link to the SMF
        # TODO(matt9j) Might not be needed
        # create_fields_if_not_exist(upf_data, ["smf"]["pfcp"])
        # upf_data["smf"]["pfcp"] = {'addr': "127.0.0.3"}

    with open(upf, 'w') as file:
        # Save the results
        yaml.dump(upf_data, file)


def update_haulage(colte_data):
    haulage_data = {}
    with open(haulage, 'r') as file:
        haulage_data = yaml.load(file.read())

        # Create fields in the data if they do not yet exist
        create_fields_if_not_exist(haulage_data, ["custom"])

        haulage_data["userSubnet"] = colte_data["lte_subnet"]
        haulage_data["ignoredUserAddresses"] = [str(IPNetwork(colte_data["lte_subnet"])[1])]

        haulage_data["custom"]["dbUser"] = colte_data["mysql_user"]
        haulage_data["custom"]["dbLocation"] = colte_data["mysql_db"]
        haulage_data["custom"]["dbPass"] = colte_data["mysql_password"]

        # Hard-coded values
        haulage_data["interface"] = "ogstun"

    with open(haulage, 'w') as file:
        # Save the results
        yaml.dump(haulage_data, file)

def create_fields_if_not_exist(dictionary, fields):
    create_fields_helper(dictionary, fields, 0)

def create_fields_helper(dictionary, fields, index):
    if index < len(fields):
        if fields[index] not in dictionary or dictionary[fields[index]] == None:
            dictionary[fields[index]] = CommentedMap()

        create_fields_helper(dictionary[fields[index]], fields, index + 1)

RED='\033[0;31m'
NC='\033[0m'

if os.geteuid() != 0:
    print("colteconf: ${RED}error:${NC} Must run as root! \n")
    exit(1)

os.system('systemctl stop colte-nat')

# Read old vars and update yaml
with open(colte_vars, 'r') as file:
    colte_data = yaml.load(file.read())

    # Update yaml files
    update_mme(colte_data)
    update_sgwc(colte_data)
    update_sgwu(colte_data)
    update_smf(colte_data)
    update_upf(colte_data)
    update_haulage(colte_data)

    # Update other files
    update_colte_nat_script(colte_data)
    update_network_vars(colte_data)
    update_env_file(webadmin_env, colte_data)
    update_env_file(webgui_env, colte_data)

# always enable kernel ip_forward
enable_ip_forward()

# START/STOP SERVICES
if (colte_data["metered"] == True):
    os.system('systemctl restart haulage colte-webgui colte-webadmin')
    os.system('systemctl enable haulage colte-webgui colte-webadmin')
else:
    os.system('systemctl stop haulage colte-webgui colte-webadmin')
    os.system('systemctl disable haulage colte-webgui colte-webadmin')

if (colte_data["epc"] == True):
    os.system('systemctl restart open5gs-hssd open5gs-mmed open5gs-sgwcd open5gs-sgwud open5gs-pcrfd open5gs-smfd open5gs-upfd')
    os.system('systemctl enable open5gs-hssd open5gs-mmed open5gs-sgwcd open5gs-sgwud open5gs-pcrfd open5gs-smfd open5gs-upfd')
else:
    os.system('systemctl stop open5gs-hssd open5gs-mmed open5gs-sgwcd open5gs-sgwud open5gs-pcrfd open5gs-smfd open5gs-upfd')
    os.system('systemctl disable open5gs-hssd open5gs-mmed open5gs-sgwcd open5gs-sgwud open5gs-pcrfd open5gs-smfd open5gs-upfd')

if (colte_data["nat"] == True):
    os.system('systemctl start colte-nat')
    os.system('systemctl enable colte-nat')
else:
    os.system('systemctl stop colte-nat')
    os.system('systemctl disable colte-nat')

os.system('systemctl restart systemd-networkd')
os.system('sysctl -w net.ipv4.ip_forward=1')
