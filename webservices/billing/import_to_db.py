import MySQLdb
import os
import socket

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

def get_imsi_from_ip(ip_addr):

	# Network Code
	rawip = socket.inet_aton(ip_addr)
#        print int(rawip)
	message = SPGW_COMMAND_REQUEST_IMSI + "\0\0\0" + rawip + "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock.sendto(message, (SPGW_SERVICE_ADDR, SPGW_SERVICE_PORT))
	data = sock.recv(24)

	# Check for errors and validate response before continuing
	if (data[0] == SPGW_COMMAND_REQUEST_IMSI_ANSWER_ERROR):
		print "get_imsi_from_ip ERROR: SPGW could not return an IMSI"
		return ZERO_IMSI

        if (data[0] != SPGW_COMMAND_REQUEST_IMSI_ANSWER_OK):
		print "get_imsi_from_ip ERROR: Unknown error?!?"
		return ZERO_IMSI

	raw_ip_response = data[4:8]
	if (raw_ip_response != rawip):
		print "get_imsi_from_ip ERROR: IP addresses don't match?!?"
		print "Origin IP: " + socket.inet_ntoa(rawip)
		print "Received IP: " + socket.inet_ntoa(raw_ip_response)
		return ZERO_IMSI

	imsi = str(data[8:24])
	# print "IMSI : " + imsi
	return imsi

def hss_disable_user(imsi):
	print "CUTTING OFF USER " + str(imsi)
	message = "\0" + imsi
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock.sendto(message, (HSS_SERVICE_ADDR, HSS_SERVICE_PORT))

# def hss_enable_user(imsi):
	# print "REENABLING USER " + str(imsi)
	# 1: add user back to HSS!
	# message = "\1" + imsi
	# sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	# sock.sendto(message, (HSS_SERVICE_ADDR, HSS_SERVICE_PORT))
	# 2: flip user bit
	# 3: send text?!?

def make_new_user(vals):
        print "Make new user? No, don't! How'd they even get on the network???"

# example cost: 5 dollars (units) per gb
cost_per_gb = 50.00
cost_per_mb = cost_per_gb / 1024
cost_per_byte = cost_per_mb / 1024
def calculate_cost(bytes_down, bytes_up):
	# NAIVE APPROACH SO FAR: cost per byte * bytes
	total_bytes = bytes_down + bytes_up
	total_cost = total_bytes * cost_per_byte
	# print "new bytes: " + str(total_bytes) + "\ncost per byte: " + str(cost_per_byte) + "\ntotal cost: " + str(total_cost)
	return total_cost

record_list = []
db = MySQLdb.connect(host="localhost",
                     user=os.environ.get('COLTE_USER'),
                     passwd=os.environ.get('COLTE_DBPASS'),
		     	 	 db="colte_db")
cursor = db.cursor()
filename = os.environ.get('COLTE_DIR') + "/webservices/billing/tmp_dump.txt"
#filename = "/home/vagrant/colte/webservices/billing/tmp_dump.txt"
file = open(filename, 'r')

# FIRST ROW IS JUST THE TIME OF THE ENTRY
timestr = file.readline().split()
print "Read Timestring: " + str(timestr)

for line in file:
	vals = line.split()
	ip_addr = vals[0]
	new_bytes_down = vals[1]
	new_bytes_up = vals[2]

	imsi = get_imsi_from_ip(ip_addr)
	if imsi == ZERO_IMSI:
		continue

	query = "SELECT idcustomers, raw_down, raw_up, balance, enabled FROM customers WHERE imsi = " + imsi 
        print query
	numrows = cursor.execute(query)

	if numrows == 0:
		print "ERROR: why do we not have info for this imsi in the database?!?!?"
		continue

	if numrows > 1:
		print "More than one entry for same imsi? What happened???"
		continue

	answer_tuple = cursor.fetchone()
	table_id = answer_tuple[0]
	previous_bytes_down = answer_tuple[1]
	previous_bytes_up = answer_tuple[2]
	previous_balance = answer_tuple[3]
	enabled = answer_tuple[4]
        print table_id

	# sanity check
	if enabled != 1:
		print "ERROR: Why is IMSI " + imsi + " not set to enabled in customers db? Value is " + str(enabled)

	# data is only incremented (cumulatively, duh) so the only way these values will ever be less than previous val
	# is if the counter reset. hopefully this never happens but edge-cases are important
        if (new_bytes_down < previous_bytes_down) or (new_bytes_up < previous_bytes_up):
	# LOG SOMETHING!?!
	    bytes_down_in_period = new_bytes_down
	    bytes_up_in_period = new_bytes_up
        else:
	    bytes_down_in_period = int(new_bytes_down) - int(previous_bytes_down)
	    bytes_up_in_period = int(new_bytes_up) - int(previous_bytes_up)

	# billing math here
	cost_in_period = calculate_cost(bytes_down_in_period, bytes_up_in_period)
	new_balance = previous_balance - cost_in_period

	# SMS TODO: check for certain thresholds, can potentially send warnings or take other action?

	if new_balance <= 0:
		hss_disable_user(imsi)
		enabled = "0"
	else:
		enabled = "1"

	# END: store the record locally and onto the next user
	new_record = (new_bytes_down, new_bytes_up, str(new_balance), enabled, table_id)
	record_list.append(new_record)
	
# (commit all updates at once to save DB operations)
commit_str = "UPDATE customers SET raw_down = %s, raw_up = %s, balance = %s, enabled = %s WHERE idcustomers = %s"

print record_list

cursor.executemany(commit_str, record_list)
