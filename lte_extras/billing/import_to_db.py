import MySQLdb
import os
import socket
#import iptc
import subprocess

HSS_REMOVE_IMSI_COMMAND = 0
HSS_ADD_IMSI_COMMAND = 1
HSS_SERVICE_PORT = 62880
HSS_SERVICE_ADDR = "127.0.0.1"

SPGW_COMMAND_REQUEST_IMSI_ANSWER_OK = "\0"
SPGW_COMMAND_REQUEST_IMSI = "\1"
SPGW_COMMAND_REQUEST_IMSI_ANSWER_ERROR = "\2"
SPGW_SERVICE_PORT = 62881
SPGW_SERVICE_ADDR = "127.0.0.1"

ZERO_IMSI = "000000000000000"

class Customer:
    imsi = ZERO_IMSI
    ip = "0.0.0.0"

    new_raw_down = 0
    new_raw_up = 0
    new_data_balance = 0

    old_raw_down = 0
    old_raw_up = 0
    old_data_balance = 0

    balance = 0.0
    bridged = 1
    enabled = 1

def get_imsi_from_ip(ip_addr, cursor):
    query = "SELECT imsi FROM static_ips WHERE ip = \"" + ip_addr + "\""
        print query
    numrows = cursor.execute(query)

    if numrows == 0:
        print "ERROR: why do we not have an IMSI for ip address " + ip_addr + " in the database?!?!?"
        return ZERO_IMSI

    if numrows > 1:
        print "More than one IMSI entry for same ip address? What happened???"
        return ZERO_IMSI

    answer_tuple = cursor.fetchone()
    imsi = answer_tuple[0]
    return imsi

def hss_disable_user(imsi):
    message = "\0" + imsi
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.sendto(message, (HSS_SERVICE_ADDR, HSS_SERVICE_PORT))

# def old_get_imsi_from_ip(ip_addr):

#     # Network Code
#     rawip = socket.inet_aton(ip_addr)
# #        print int(rawip)
#     message = SPGW_COMMAND_REQUEST_IMSI + "\0\0\0" + rawip + "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
#     sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
#     sock.sendto(message, (SPGW_SERVICE_ADDR, SPGW_SERVICE_PORT))
#     data = sock.recv(24)

#     # Check for errors and validate response before continuing
#     if (data[0] == SPGW_COMMAND_REQUEST_IMSI_ANSWER_ERROR):
#         print "get_imsi_from_ip ERROR: SPGW has no IMSI for IP " + ip_addr
#         return ZERO_IMSI

#         if (data[0] != SPGW_COMMAND_REQUEST_IMSI_ANSWER_OK):
#         print "get_imsi_from_ip ERROR: Unknown error?!?"
#         return ZERO_IMSI

#     raw_ip_response = data[4:8]
#     if (raw_ip_response != rawip):
#         print "get_imsi_from_ip ERROR: IP address in SPGW response doesn't match query?!?"
#         print "Origin IP: " + socket.inet_ntoa(rawip)
#         print "Received IP: " + socket.inet_ntoa(raw_ip_response)
#         return ZERO_IMSI

#     imsi = str(data[8:24])
#     print "Translated IP address " + ip_addr + " to IMSI " + imsi

#     return imsi


# def hss_enable_user(imsi):
    # print "REENABLING USER " + str(imsi)
    # 1: add user back to HSS!
    # message = "\1" + imsi
    # sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    # sock.sendto(message, (HSS_SERVICE_ADDR, HSS_SERVICE_PORT))
    # 2: flip user bit
    # 3: send text?!?

# def make_new_user(vals):
#         print "Make new user? No, don't! How'd they even get on the network???"

# example cost: 5 dollars (units) per gb
# cost_per_gb = 50.00
# cost_per_mb = cost_per_gb / 1024
# cost_per_kb = cost_per_mb / 1024
# cost_per_byte = cost_per_kb / 1024
# def calculate_cost(total_bytes):
#     # NAIVE APPROACH SO FAR: cost per byte * bytes
#     total_cost = total_bytes * cost_per_byte
#     # print "new bytes: " + str(total_bytes) + "\ncost per byte: " + str(cost_per_byte) + "\ntotal cost: " + str(total_cost)
#     return total_cost

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
    print "Read Timestring: " + str(timestr)

    for line in file:
        c = Customer()

        vals = line.split()
        c.ip = vals[0]
        c.new_bytes_down = vals[1]
        c.new_bytes_up = vals[2]

        c.imsi = get_imsi_from_ip(c.ip, cursor)
        if c.imsi == ZERO_IMSI:
            continue

        query = "SELECT raw_down, raw_up, data_balance, balance, bridged, enabled FROM customers WHERE imsi = " + c.imsi 
        numrows = cursor.execute(query)

        if numrows == 0:
            print "ERROR: why do we not have info for this imsi in the database?!?!?"
            continue

        if numrows > 1:
            print "More than one entry for same imsi? What happened???"
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
            print "ERROR: Why is IMSI " + c.imsi + " not set to enabled in customers db? Value is " + str(c.enabled)

        # data is only incremented (cumulatively, duh) so the only way these values will ever be less than previous val
        # is if the counter reset. hopefully this never happens but edge-cases are important
        # NOTE: some counters might reset when others don't!
        if (c.new_bytes_down < c.old_bytes_down):
            bytes_down_in_period = c.new_bytes_down
        else:
            bytes_down_in_period = int(c.new_bytes_down) - int(c.old_bytes_down)

        if (c.new_bytes_up < c.old_bytes_up):
            bytes_up_in_period = c.new_bytes_up
        else:
            bytes_up_in_period = int(c.new_bytes_up) - int(c.old_bytes_up)

        # SANITY CHECK
        if (bytes_down_in_period < 0) or (bytes_up_in_period < 0):
            print "MAJOR BILLING SCRIPT ERROR: HOW COULD WE GET NEGATIVE BYTE USAGE?!?"
            bytes_down_in_period = 0
            bytes_up_in_period = 0

        total_bytes_in_period = bytes_down_in_period + bytes_up_in_period
        c.new_data_balance = c.old_data_balance - total_bytes_in_period

        print "IMSI " + c.imsi + " used " + str(total_bytes_in_period) + " bytes. Bytes_remaining = " + str(c.new_data_balance) + ", raw_down = " + str(c.new_bytes_down) + ", raw_up = " + str(c.new_bytes_up)

        verify_balance(c)

        # END: store the record locally and onto the next user
        new_record = (c.new_bytes_down, c.new_bytes_up, str(c.new_data_balance), c.enabled, c.bridged, c.imsi)
        record_list.append(new_record)

    # (commit all updates at once to save on DB operations)
    commit_str = "UPDATE customers SET raw_down = %s, raw_up = %s, data_balance = %s, enabled = %s, bridged = %s WHERE imsi = %s"

    cursor.executemany(commit_str, record_list)

def verify_balance(c):
    # if they still have a high data balance (LIKELY) then nothing to do here
    if c.new_data_balance > 10000000: #10MB
        return

    # send (ONLY ONE) alert if we're crossing a threshold. Go in reverse order
    # so that if we crosed multiple thresholds at once, only alert re: the lowest amt.
    if c.new_data_balance <= 0 and c.old_data_balance > 0:
        out_of_data(c)
    elif c.new_data_balance <= 1000000 and c.old_data_balance > 1000000:
        alert_crossed_1mb(c)
    elif c.new_data_balance <= 5000000 and c.old_data_balance > 5000000:
        alert_crossed_5mb(c)
    elif c.new_data_balance <= 10000000 and c.old_data_balance > 10000000:
        alert_crossed_10mb(c)

def alert_crossed_10mb(c):
    print "IMSI " + c.imsi + " has less than 10MB of data remaining."

def alert_crossed_5mb(c):
    print "IMSI " + c.imsi + " has less than 5 MB of data remaining."

def alert_crossed_1mb(c):
    print "IMSI " + c.imsi + " has less than 1 MB of data remaining."

def out_of_data(c):
    # STEP 1: enable iptables filter to ensure that they can't get out on the general Internet
    print "IMSI " + c.imsi + " out of data, enabling iptables filter."
    enable_iptables_filter(c)
    c.bridged = 0

    # STEP 2: if they're also out of balance, cut them off entirely (figure this out later)
    if c.balance <= 0:
        # HERE: they're out of data balance AND out of money. Cut them off completely!
        print "IMSI " + c.imsi + " also out of money, removing from network."
        # hss_disable_user(imsi)
        # enabled = "0"

def enable_iptables_filter(c):
    # command = "sudo iptables -I FORWARD -s " + c.ip + " -j REJECT"
    p = subprocess.Popen(["iptables", "-I", "FORWARD", "-s", c.ip, "-j", "REJECT"], stdout=subprocess.PIPE)
    output , err = p.communicate()
    print output
    # rule = iptc.Rule()
    # rule.src = c.ip
    # rule.target = iptc.Target('REJECT')
    # chain = iptc.Chain(iptc.Table.(iptc.Table.FILTER), "FORWARD")
    # chain.insert_rule(rule)

#def disable_iptables_filter(c):
    # command = "sudo iptables -D FORWARD -s " + c.ip + " -j REJECT"
    # rule = iptc.Rule()
    # rule.src = c.ip
    # rule.target = iptc.Target('REJECT')
    # chain = iptc.Chain(iptc.Table.(iptc.Table.FILTER), "FORWARD")
    # chain.delete_rule(rule)

if __name__ == "__main__":
    main()
