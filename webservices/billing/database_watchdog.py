import MySQLdb
import os
import socket

# database_watchdog.py
# This script runs once-per-day (hour?) and simply ensures that the two
# databases ("users/pdn", used by OAI HSS, and "customers", used by billing sw)
# are in sync with respect to which users are active/allowed on the network.
# If there are problems/collisions, just log them out for now! (We'll bugfix)
# NOTE: If I had to choose, I think that "customers" should be king, and that the
# OAI HSS databases should simply be slaves (since all they provide is connectivity)

# pseudo mysql:

#1: SELECT imsi from customers where (bit is on) except select imsi from users
# Gives us all the IMSIs that are allowed in customers db that are NOT in oai_hss users db.
# The correct line of action for each of these IMSIs is to enable them.
# NOTE: Make sure you check for $$$ to make sure they weren't JUST kicked off

#2: SELECT imsi from users except select imsi from customers where (bit is on)
# Gives us all the IMSIs that oai_hss is allowing onto the network that are NOT allowed by our db.
# Proper course of action: (1) make sure the user exists, and that we think he's not allowed on
# Step (2) disable user
# Step (3) if customers db doesn't know about user, we should just erase all info(?!?)

#3: SELECT imsi from customers where (bit is off) except select imsi from disabled_users
# This query gives us all the disabled customers that oai isn't aware of. We really should
# never get any values for this query, but it means one of two things: either oai_hss is
# completely unaware of this user (and we should create) or the user is currently listed
# as "enabled" (in which case how did we get past case #2?!??)
# Depending on the case, we move a user from "enabled" to "disabled" in the oai hss databases
# (EASY) or we create a brand new oai_hss entry for the user (WAIT - HOW TO GET KI?!?!?!)

#4: SELECT imsi from disabled_users except select imsi from customers where (bit is off)
# This query gives us any users in the oai_hss "disabled users" db that are NOT in the customer's
# disabled users DB. This could be because (1) they're in the customer's "enabled users" db (in which
# case why weren't they caught by case 1?!?); (2) they're in BOTH databases (what?!?); or (3) they don't
# exist in the customer db at all.


SPENCER_REMOVE_IMSI_COMMAND = "\0"
SPENCER_ADD_IMSI_COMMAND = "\1"
HSS_SERVICE_PORT = 62880
HSS_SERVICE_ADDR = "127.0.0.1"
def disable_user(imsi):
	print "Disabling User: " + str(imsi)
	message = SPENCER_REMOVE_IMSI_COMMAND + imsi
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock.sendto(message, HSS_SERVICE_ADDR, HSS_SERVICE_PORT)
	# SMS TODO: (still preserve the record so that we can settle-up or re-activate sim without drama)

def enable_user(imsi):
	print "Enabling User: " + str(imsi)
	message = SPENCER_ADD_IMSI_COMMAND + imsi
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock.sendto(message, HSS_SERVICE_ADDR, HSS_SERVICE_PORT)
	# SMS TODO: (bookkeeping w.r.t. customers db?)

record_list = []
db = MySQLdb.connect(host="localhost",
                     user=os.environ.get('COLTE_USER'),
                     passwd=os.environ.get('COLTE_DBPASS'),
		     db="colte_db")
cursor = db.cursor()
filename = os.environ.get('COLTE_DIR') + "/webservices/billing/tmp_dump.txt"
#filename = "/home/vagrant/colte/webservices/billing/tmp_dump.txt"
file = open(filename, 'r')

# PROCESS FIRST ROW IS JUST TIME OF LAST ENTRY
timestr = file.readline().split()
print timestr

for line in file:
	vals = line.split()
	ip_addr = vals[0]
	new_bytes_down = vals[1]
	new_bytes_up = vals[2]

	query = ("SELECT * FROM customers WHERE ip = '" + str(ip_addr) + "'")
	numrows = cursor.execute(query)

	if numrows == 0:
		make_new_entry(vals)

	if numrows > 1:
		print "More than one entry for same IP? What happened???"

	answer_tuple = cursor.fetchone()
	table_id = answer_tuple[0]
	previous_bytes_down = answer_tuple[2]
	previous_bytes_up = answer_tuple[3]
	previous_balance = answer_tuple[4]
	
	# END: store the record locally and onto the next user
	new_record = (new_bytes_down, new_bytes_up, str(new_balance), table_id)
	record_list.append(new_record)
	
# (commit all updates at once to save DB operations)
commit_str = "UPDATE customers SET raw_down = %s, raw_up = %s, balance = %s WHERE idcustomers = %s"
cursor.executemany(commit_str, record_list)