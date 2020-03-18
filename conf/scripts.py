import ruamel.yaml
from ruamel.yaml.comments import CommentedMap
import fileinput
import sys
from netaddr import IPNetwork

# This version saves comments/edits in YAML files
yaml = ruamel.yaml.YAML()
yaml.indent(sequence=4, mapping=2, offset=2)

# Input
colte_vars = "/etc/colte/config.yml"

# EPC conf-files
mme = "/etc/open5gs/mme.yaml"
pgw = "/etc/open5gs/pgw.yaml"
sgw = "/etc/open5gs/sgw.yaml"

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

def update_colte_nat_script(colte_data):
    replaceAll(colte_nat_script, "ADDRESS=", "ADDRESS=" + colte_data["lte_subnet"]+ "\n", False)

def update_network_vars(colte_data):
    replaceAll(network_vars, "Address=", "Address=" + str(IPNetwork(colte_data["lte_subnet"])[1]) + "\n", True)

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
        create_fields_if_not_exist(mme_data, ["sgw", "gtpc"])
        create_fields_if_not_exist(mme_data, ["pgw", "gtpc"])

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

        # Set default values of list fields
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

        haulage_data["monitoredBlock"] = colte_data["lte_subnet"]
        # haulage_data["myIP"] = str(IPNetwork(colte_data["lte_subnet"])[1])

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

def conf_main():
    # Read old vars
    with open(colte_vars, 'r') as file:
        colte_data = yaml.load(file.read())

        # Update yaml files
        update_mme(colte_data)
        update_pgw(colte_data)
        update_sgw(colte_data)
        update_haulage(colte_data)

        # Update other files
        update_colte_nat_script(colte_data)
        update_network_vars(colte_data)
        update_env_file(webadmin_env, colte_data)
        update_env_file(webgui_env, colte_data)

def db_main():
    # Read old vars
    with open(colte_vars, 'r') as file:
        colte_data = yaml.load(file.read())

    dbname = colte_data["mysql_db"]
    db_user = colte_data["mysql_user"]
    db_pass = colte_data["mysql_password"]
    db = MySQLdb.connect(host="localhost",
                         user=db_user,
                         passwd=db_pass,
                         db=dbname)
    cursor = db.cursor()

    operator = sys.argv[2]

    if (operator == "topup"):
        imsi = sys.argv[3]
        amount = decimal.Decimal(sys.argv[4])
        old_balance = 0
        new_balance = 0

        commit_str = "SELECT balance FROM customers WHERE imsi = " + imsi + " FOR UPDATE"
        numrows = cursor.execute(commit_str)
        if (numrows == 0):
            print "coltedb error: imsi " + str(imsi) + " does not exist!"
            exit(1)

        for row in cursor:
            old_balance = decimal.Decimal(row[0])
            new_balance = amount + old_balance

        # STEP TWO: prompt for confirmation
        promptstr = "coltedb: topup user " + str(imsi) + " add " + str(amount) + " to current balance " + str(old_balance) + " to create new balance " + str(new_balance) + "? [Y/n] "
        while True:
            answer = raw_input(promptstr)
            if (answer == 'y' or answer == 'Y' or answer == ''):
                print "coltedb: updating user " + str(imsi) + " setting new balance to " + str(new_balance)
                commit_str = "UPDATE customers SET balance = " + str(new_balance) + " WHERE imsi = " + imsi
                cursor.execute(commit_str)
                break
            if (answer == 'n' or answer == 'N'):
                print "coltedb: cancelling topup operation\n"
                break

    elif (operator == "admin"):
        imsi = sys.argv[3]
        print "coltedb: giving admin privileges to user " + str(imsi)
        commit_str = "UPDATE customers SET admin = 1 WHERE imsi = " + imsi
        cursor.execute(commit_str)

    elif (operator == "noadmin"):
        imsi = sys.argv[3]
        print "coltedb: removing admin privileges from user " + str(imsi)
        commit_str = "UPDATE customers SET admin = 0 WHERE imsi = " + imsi
        cursor.execute(commit_str)

    else:
        exit(1)

    db.commit()
    cursor.close()
    db.close()

# MAIN ENTRY POINT STARTS HERE
if (len(sys.argv) <= 1):
    exit(1)

command = sys.argv[1]

if (command == "conf"):
    conf_main()
elif (command == "db"):
    db_main()
else:
    exit(1)