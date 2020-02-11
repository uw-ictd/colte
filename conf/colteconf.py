import ruamel.yaml
from ruamel.ordereddict import ordereddict
import fileinput
import sys
from netaddr import IPNetwork

# This version saves comments/edits in YAML files
yaml = ruamel.yaml.YAML()

# Input
colte_vars = "/etc/colte/config.yml"

# EPC conf-files
mme = "/etc/open5gs/mme.yaml"
pgw = "/etc/open5gs/pgw.yaml"
sgw = "/etc/open5gs/sgw.yaml"

# Haulage
haulage = "/etc/haulage/config.yaml"

# Other files
nat_script = "/etc/colte/nat_script.sh"
webgui_env = "/etc/colte/webgui.env"
webadmin_env = "/etc/colte/webadmin.env"

def main():
    # Read old vars
    with open(colte_vars, 'r') as file:
        colte_data = yaml.load(file.read())

        # Update yaml files
        update_mme(colte_data)
        update_pgw(colte_data)
        update_sgw(colte_data)
        update_haulage(colte_data)

        # Update other files
        # update_nat_script(colte_data)
        update_env_file(webadmin_env, colte_data)
        update_env_file(webgui_env, colte_data)

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

def update_nat_script(colte_data):
    replaceAll(nat_script, "ip addr add", "sudo ip addr add " + colte_data["lte_subnet"] + " dev ogstun\n")
    replaceAll(nat_script, "POSTROUTING", "sudo iptables -t nat -A POSTROUTING -o " + colte_data["wan_iface"] + " -j MASQUERADE\n")

def replaceAll(file,searchExp,replaceExp):
    for line in fileinput.input(file, inplace=1):
        if searchExp in line:
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
        create_fields_if_not_exist(mme_data, ["sgw", "gtpc"])
        create_fields_if_not_exist(mme_data, ["pgw", "gtpc"])
        # print(mme_data)

        # MCC values
        mme_data["mme"]["gummei"]["plmn_id"]["mcc"] = colte_data["mcc"]
        mme_data["mme"]["tai"]["plmn_id"]["mcc"] = colte_data["mcc"]

        # MNC values
        mme_data["mme"]["gummei"]["plmn_id"]["mnc"] = colte_data["mnc"]
        mme_data["mme"]["tai"]["plmn_id"]["mnc"] = colte_data["mnc"]

        # Other values
        mme_data["mme"]["s1ap"]["addr"] = colte_data["enb_iface_addr"]
        mme_data["mme"]["network_name"]["full"] = colte_data["network_name"]

        # Hard-coded values
        mme_data["mme"]["gtpc"]["addr"] = "127.0.0.1"
        mme_data["sgw"]["gtpc"]["addr"] = "127.0.0.2"
        mme_data["pgw"]["gtpc"]["addr"] = ["127.0.0.3", "::1"]

    with open(mme, 'w') as file:
        # Save the results
        yaml.dump(mme_data, file)

def update_pgw(colte_data):
    pgw_data = {}
    with open(pgw, 'r+') as file:
        pgw_data = yaml.load(file.read())

        # Safe deletions
        if "pgw" in pgw_data and "dns" in pgw_data["pgw"]:
            del pgw_data["pgw"]["dns"][:]

        if "pgw" in pgw_data and "ue_pool" in pgw_data["pgw"]:
            del pgw_data["pgw"]["ue_pool"][:]

        if "pgw" in pgw_data and "gtpc" in pgw_data["pgw"]:
            del pgw_data["pgw"]["gtpc"][:]
        
        if "pgw" in pgw_data and "gtpu" in pgw_data["pgw"]:
            del pgw_data["pgw"]["gtpu"][:]

        # Create fields in the data if they do not yet exist
        create_fields_if_not_exist(pgw_data, ["pgw"])
        print(pgw_data)

        # Set default values of list fields if they do not exist
        if "dns" not in pgw_data["pgw"]:
            pgw_data["pgw"]["dns"] = []

        if "ue_pool" not in pgw_data["pgw"]:
            pgw_data["pgw"]["ue_pool"] = []

        if "gtpc" not in pgw_data["pgw"]:
            pgw_data["pgw"]["gtpc"] = []

        if "gtpu" not in pgw_data["pgw"]:
            pgw_data["pgw"]["gtpu"] = []

        pgw_data["pgw"]["dns"].append(colte_data["dns"])
        pgw_data["pgw"]["ue_pool"].append(colte_data["lte_subnet"])
        pgw_data["pgw"]["gtpc"].insert(0, {'addr': "127.0.0.3"})
        pgw_data["pgw"]["gtpc"].insert(1, {'addr': "::1"})
        pgw_data["pgw"]["gtpu"].insert(0, {'addr': "127.0.0.3"})
        pgw_data["pgw"]["gtpu"].insert(1, {'addr': "::1"})

    with open(pgw, 'w') as file:
        # Save the results
        yaml.dump(pgw_data, file)

def update_sgw(colte_data):
    sgw_data = {}
    with open(sgw, 'r') as file:
        sgw_data = yaml.load(file.read())

        # Create fields in the data if they do not yet exist
        create_fields_if_not_exist(sgw_data, ["sgw", "gtpu"])
        create_fields_if_not_exist(sgw_data, ["sgw", "gtpc"])
        print(sgw_data)

        sgw_data["sgw"]["gtpu"]["addr"] = colte_data["enb_iface_addr"]

        # Hard-coded values
        sgw_data["sgw"]["gtpc"]["addr"] = "127.0.0.2"

    with open(sgw, 'w') as file:
        # Save the results
        yaml.dump(sgw_data, file)


def update_haulage(colte_data):
    haulage_data = {}
    with open(haulage, 'r') as file:
        haulage_data = yaml.load(file.read())

        # Create fields in the data if they do not yet exist
        create_fields_if_not_exist(haulage_data, ["custom"])
        print(haulage_data)

        haulage_data["monitoredBlock"] = colte_data["lte_subnet"]
        haulage_data["myIP"] = str(IPNetwork(colte_data["lte_subnet"])[1])

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
            dictionary[fields[index]] = ordereddict()
            print(fields[index])
            print(dictionary)

        create_fields_helper(dictionary[fields[index]], fields, index + 1)

if __name__== "__main__":
    main()
