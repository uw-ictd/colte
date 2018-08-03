from helpers import constants
from helpers import iptables

import MySQLdb
import os


class Customer(object):
    def __init__(self):
        self.imsi = constants.ZERO_IMSI
        self.ip = constants.ZERO_IPv4

        self.new_raw_down = 0
        self.new_raw_up = 0
        self.new_data_balance = 0

        self.old_raw_down = 0
        self.old_raw_up = 0
        self.old_data_balance = 0

        self.balance = 0.0
        self.bridged = 1
        self.enabled = 1


def get_imsi_from_ip(ip_addr, cursor):
    query = "SELECT imsi FROM static_ips WHERE ip = \"" + ip_addr + "\""
    print(query)
    numrows = cursor.execute(query)

    if numrows == 0:
        print("ERROR: why do we not have an IMSI for ip address " + ip_addr + " in the database?!?!?")
        return constants.ZERO_IMSI

    if numrows > 1:
        print("More than one IMSI entry for same ip address? What happened???")
        return constants.ZERO_IMSI

    answer_tuple = cursor.fetchone()
    imsi = answer_tuple[0]
    return imsi


def main():
    record_list = []
    db = MySQLdb.connect(host="localhost",
                         user=os.environ.get('COLTE_USER'),
                         passwd=os.environ.get('COLTE_DBPASS'),
                         db="colte_db")
    cursor = db.cursor()
    filename = os.environ.get('COLTE_DIR') + "/lte_extras/billing/tmp_dump.txt"
    file = open(filename, 'r')

    # FIRST ROW IS JUST THE TIME OF THE ENTRY
    timestr = file.readline().split()
    print("Read Timestring: " + str(timestr))

    for line in file:
        c = Customer()

        vals = line.split()
        c.ip = vals[0]
        c.new_bytes_down = vals[1]
        c.new_bytes_up = vals[2]

        c.imsi = get_imsi_from_ip(c.ip, cursor)
        if c.imsi == constants.ZERO_IMSI:
            continue

        query = "SELECT raw_down, raw_up, data_balance, balance, bridged, enabled FROM customers WHERE imsi = " + c.imsi 
        numrows = cursor.execute(query)

        if numrows == 0:
            print("ERROR: why do we not have info for this imsi in the database?!?!?")
            continue

        if numrows > 1:
            print("More than one entry for same imsi? What happened???")
            continue

        answer_tuple = cursor.fetchone()

        c.old_bytes_down = answer_tuple[0]
        c.old_bytes_up = answer_tuple[1]
        c.old_data_balance = answer_tuple[2]

        c.balance = answer_tuple[3]
        c.bridged = answer_tuple[4]
        c.enabled = answer_tuple[5]

        # sanity check
        if c.enabled != 1:
            print("ERROR: Why is IMSI " + c.imsi + " not set to enabled in customers db? Value is " + str(c.enabled))

        # data is only incremented (cumulatively, duh) so the only way these values will ever be less than previous val
        # is if the counter reset. hopefully this never happens but edge-cases are important
        # NOTE: some counters might reset when others don't!
        if c.new_bytes_down < c.old_bytes_down:
            bytes_down_in_period = c.new_bytes_down
        else:
            bytes_down_in_period = int(c.new_bytes_down) - int(c.old_bytes_down)

        if c.new_bytes_up < c.old_bytes_up:
            bytes_up_in_period = c.new_bytes_up
        else:
            bytes_up_in_period = int(c.new_bytes_up) - int(c.old_bytes_up)

        # SANITY CHECK
        if (bytes_down_in_period < 0) or (bytes_up_in_period < 0):
            print("MAJOR BILLING SCRIPT ERROR: HOW COULD WE GET NEGATIVE BYTE USAGE?!?")
            bytes_down_in_period = 0
            bytes_up_in_period = 0

        total_bytes_in_period = bytes_down_in_period + bytes_up_in_period
        c.new_data_balance = c.old_data_balance - total_bytes_in_period

        print("IMSI " + c.imsi + " used " + str(total_bytes_in_period) + " bytes. Bytes_remaining = " + str(c.new_data_balance) + ", raw_down = " + str(c.new_bytes_down) + ", raw_up = " + str(c.new_bytes_up))

        verify_balance(c)

        # END: store the record locally and onto the next user
        new_record = (c.new_bytes_down, c.new_bytes_up, str(c.new_data_balance), c.enabled, c.bridged, c.imsi)
        record_list.append(new_record)

    # (commit all updates at once to save on DB operations)
    commit_str = "UPDATE customers SET raw_down = %s, raw_up = %s, data_balance = %s, enabled = %s, bridged = %s WHERE imsi = %s"

    cursor.executemany(commit_str, record_list)


def verify_balance(c):
    # if they still have a high data balance (LIKELY) then nothing to do here
    if c.new_data_balance > 10000000:  # 10MB
        return

    # send (ONLY ONE) alert if we're crossing a threshold. Go in reverse order
    # so that if we crosed multiple thresholds at once, only alert re: the lowest amt.
    if (c.new_data_balance <= 0) and (c.old_data_balance > 0):
        out_of_data(c)
    elif (c.new_data_balance <= 1000000) and (c.old_data_balance > 1000000):
        alert_crossed_1mb(c)
    elif (c.new_data_balance <= 5000000) and (c.old_data_balance > 5000000):
        alert_crossed_5mb(c)
    elif (c.new_data_balance <= 10000000) and (c.old_data_balance > 10000000):
        alert_crossed_10mb(c)


def alert_crossed_10mb(c):
    print("IMSI " + c.imsi + " has less than 10MB of data remaining.")


def alert_crossed_5mb(c):
    print("IMSI " + c.imsi + " has less than 5 MB of data remaining.")


def alert_crossed_1mb(c):
    print("IMSI " + c.imsi + " has less than 1 MB of data remaining.")


def out_of_data(c):
    # STEP 1: enable iptables filter to ensure that they can't get out on the general Internet
    print("IMSI " + c.imsi + " out of data, enabling iptables filter.")
    iptables.enable_forward_filter(c.ip)
    c.bridged = 0

    # STEP 2: if they're also out of balance, cut them off entirely (figure this out later)
    if c.balance <= 0:
        # HERE: they're out of data balance AND out of money. Cut them off completely!
        print("IMSI " + c.imsi + " also out of money, removing from network.")
        # hss_disable_user(imsi)
        # enabled = "0"


if __name__ == "__main__":
    main()
