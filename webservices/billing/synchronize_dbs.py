# synchronize_dbs.py
# This script is to be run once, after both databases (oai_hss and customers) are installed.
# The script uses the "customers" db as the master, runs some sanity checks on it, and then
# uses it to configure the oai_hss database. This means the following rules:
# (1) every entry in "customers" MUST have valid (+0) balance, or it will be disabled
# (2) every entry in "oai_hss" MUST have an enabled entry in customers database or will be disabled

import MySQLdb

db = MySQLdb.connect(host="localhost",
                     user=os.environ.get('COLTE_USER'),
                     passwd=os.environ.get('COLTE_DBPASS'),
		     	 	 db="colte_db")
cursor = db.cursor()

#########################################################################
############### STEP ONE: VALIDATE THE CUSTOMERS DATABASE ###############
#########################################################################

enabled_imsis = []
record_list = []
file_update = False
query = ("SELECT (idcustomers, imsi, raw_down, raw_up, balance, enabled) FROM customers")
numrows = cursor.execute(query)

for row in cursor:
	table_id = row[0]
	imsi = row[1]
	raw_down = row[2]
	raw_up = row[3]
	balance = row[4]
	enabled = row[5]
	update = False

	if raw_down < 0:
		raw_down = "0"
		update = True

	if raw_up < 0:
		raw_up = "0"
		update = True

	if balance > 0:
		enabled_imsis.append(imsi)
		if enabled == 0:
			enabled = "1"
			update = True

	if balance <= 0 and enabled == 1:
		enabled = "0"
		update = True

	if update:
		new_record = (raw_down, raw_up, balance, enabled, table_id)
		record_list.append(new_record)
		file_update = True

if file_update:
	commit_str = "UPDATE customers SET raw_down = %s, raw_up = %s, balance = %s, enabled = %s WHERE idcustomers = %s"
	cursor.executemany(commit_str, record_list)

#########################################################################
################ STEP TWO: VALIDATE THE OAI_HSS DATABASE  ###############
#########################################################################

# We're doing three checks here.
# Check #1: Ensure that each entry in 'users' is also in 'all_users'
query = "INSERT INTO all_users SELECT * FROM users WHERE imsi NOT IN (SELECT imsi FROM all_users)"
numrows = cursor.execute(query)

# Check #2: Ensure that each entry in 'customers' is also in 'all_users'
# Note that this check does include disabled users in the customers db.
# Note that it's a HUGE error to have a customer without a user. We can't recover
# from that problem because we don't know what KI, etc. should be set to.
query = ("SELECT imsi FROM customers WHERE imsi NOT IN (SELECT imsi FROM all_users)")
numrows = cursor.execute(query)
for row in cursor:
	imsi = row[0]
	print "ERROR: IMSI " + imsi + " in table customers but not in all_users. What to do?!?"

# Check #3: Ensure that each entry in 'all_users' is also in 'customers'
# QUESTION: How to handle? Right now creating a "dummy" customer. We could also delete from all_users.
query = ("SELECT imsi FROM all_users WHERE imsi NOT IN (SELECT imsi FROM customers)")
numrows = cursor.execute(query)
for row in cursor:
	imsi = row[0]
	commit_str = "INSERT INTO customers (imsi, raw_down, raw_up, balance, enabled, msisdn) VALUES ('" + imsi + "', 0, 0, 0, 0, 'NotUsed')"
	cursor.execute(commit_str)	

#########################################################################
################ STEP THREE: UPDATE THE OAI_HSS DATABASE  ###############
#########################################################################

# Step 1: Find all entries in 'users' that aren't enabled in the customers db.
#         Have to remove all of these values from the users database
query = ("DELETE FROM users WHERE imsi NOT IN (SELECT imsi FROM customers WHERE enabled = 1)")
numrows = cursor.execute(query)

# Step 2: Find all enabled entries in 'customers' that aren't in 'users' db.
# We have to look for these values in the all_users table and move them over
# if we find it. Note: If we DON'T find it in the all_users table, we need to
# throw up our hands and let people know. Note that we can't really create a user 
# here because we don't know KI.
query = "INSERT INTO users SELECT * FROM all_users WHERE imsi in (SELECT imsi FROM customers WHERE enabled = 1 AND imsi NOT IN (SELECT imsi FROM users))"
numrows = cursor.execute(query)
