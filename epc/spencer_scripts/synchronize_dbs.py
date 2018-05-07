# synchronize_dbs.py
# This script is to be run once, after both databases (oai_hss and customers) are installed.
# The script uses the "customers" db as the master, runs some sanity checks on it, and then
# uses it to configure the oai_hss database. This means the following rules:
# (1) every entry in "customers" MUST have valid (+0) balance, or it will be disabled
# (2) every entry in "oai_hss" MUST have an enabled entry in customers database or will be disabled

import MySQLdb
import os

db = MySQLdb.connect(host="localhost",
                     user=os.environ.get('COLTE_USER'),
                     passwd=os.environ.get('COLTE_DBPASS'),
		     	 	 db="colte_db")
cursor = db.cursor()

#########################################################################
############### STEP ONE: VALIDATE THE CUSTOMERS DATABASE ###############
#########################################################################

query = "UPDATE customers SET enabled=0 WHERE balance<=0"
numrows = cursor.execute(query)

query = "UPDATE customers SET enabled=1 WHERE balance>0"
numrows = cursor.execute(query)

#########################################################################
################ STEP TWO: VALIDATE THE OAI_HSS DATABASE  ###############
#########################################################################

# Check #1: Ensure that each entry in 'customers' is also in 'users'
# Note that this check does include disabled users in the customers db.
# Note that it's a HUGE error to have a customer without a user. We can't recover
# from that problem because we don't know what KI, etc. should be set to.
query = ("SELECT imsi FROM customers WHERE imsi NOT IN (SELECT imsi FROM users)")
numrows = cursor.execute(query)
for row in cursor:
	imsi = row[0]
	print "ERROR: IMSI " + imsi + " in table customers but not in users. What to do?!?"

# Check #2: Ensure that each entry in 'users' is also in 'customers'
# QUESTION: How to handle? Right now creating a "dummy" customer that has no $ and is disabled by default.
query = ("SELECT imsi FROM users WHERE imsi NOT IN (SELECT imsi FROM customers)")
numrows = cursor.execute(query)
for row in cursor:
	imsi = row[0]
	commit_str = "INSERT INTO customers (imsi, raw_down, raw_up, balance, enabled, msisdn) VALUES (" + imsi + ", 0, 0, 0, 0, 'NotUsed')"
	cursor.execute(commit_str)
