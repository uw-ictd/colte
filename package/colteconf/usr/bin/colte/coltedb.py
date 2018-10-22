import MySQLdb
import os
import sys
import decimal

#########################################################################
############### SETUP: LOAD YAML VARS AND CONNECT TO DB #################
#########################################################################

db_user = sys.argv[1]
db_pass = sys.argv[2]
dbname = sys.argv[3]

db = MySQLdb.connect(host="localhost",
                     user=db_user,
                     passwd=db_pass,
		     	 	 db=dbname)
cursor = db.cursor()

num_args = len(sys.argv)
command = sys.argv[4]

#########################################################################
############### OPTION ONE: ADD A USER TO THE DATABASE ##################
#########################################################################
if (command == "add"):
	imsi = sys.argv[5]
	msisdn = sys.argv[6]
	ip = sys.argv[7]
	key = sys.argv[8]
	opc = sys.argv[9]

	commit_str = "INSERT INTO pdn (users_imsi) VALUES ('" + imsi + "')"
	cursor.execute(commit_str)
	print commit_str

	commit_str = "INSERT INTO users (imsi, msisdn, `key`, OPc) VALUES ('" + imsi + "', '" + msisdn + "', " + key + ", " + opc + ")"
	cursor.execute(commit_str)
	print commit_str

	commit_str = "INSERT INTO customers (imsi, msisdn) VALUES ('" + imsi + "', '" + msisdn + "')"
	cursor.execute(commit_str)
	print commit_str

	# ip = GENERATE IP ADDRESS?!?
	commit_str = "INSERT INTO static_ips (imsi, ip) VALUES ('" + imsi + "', '" + ip + "')"
	cursor.execute(commit_str)
	print commit_str

#########################################################################
############### OPTION TWO: REMOVE USER FROM THE DATABASE ###############
#########################################################################
else if (command == "remove"):
	imsi = sys.argv[5]
	commit_str = "DELETE FROM pdn WHERE users_imsi = " + imsi
	cursor.execute(commit_str)
	print commit_str

	commit_str = "DELETE FROM users WHERE imsi = " + imsi
	cursor.execute(commit_str)
	print commit_str

	commit_str = "DELETE FROM customers WHERE imsi = " + imsi
	cursor.execute(commit_str)
	print commit_str

	commit_str = "DELETE FROM static_ips WHERE imsi = " + imsi
	cursor.execute(commit_str)
	print commit_str

#########################################################################
############### OPTION THREE: TOPUP (ADD BALANCE TO USER) ###############
#########################################################################
else if (command == "topup"):
	imsi = sys.argv[5]
	amount = sys.argv[6]
	new_balance = amount

	#STEP ONE: query information
	# SMS TODO #1: check for decimal overflow
	# SMS TODO #2: improve this for-loop code
	commit_str = "SELECT balance FROM customers WHERE imsi = " + imsi + " FOR UPDATE"
	numrows = cursor.execute(commit_str)
	for row in cursor:
		balance = row[0]
		new_balance = decimal.Decimal(new_balance) + decimal.Decimal(balance)

	# STEP TWO: update balance
	commit_str = "UPDATE customers SET balance = " + str(new_balance) + " WHERE imsi = " + imsi
	cursor.execute(commit_str)
	print commit_str

#########################################################################
############### OPTION FOUR: DISABLE A USER (AND ZERO-OUT BALANCE???) ###
#########################################################################
else if (command == "disable"):
	imsi = sys.argv[5]

	commit_str = "UPDATE customers SET enabled = 0, data_balance = 0 WHERE imsi = " + imsi
	cursor.execute(commit_str)
	print commit_str

else if (command == "enable"):
	imsi = sys.argv[5]

	commit_str = "UPDATE customers SET enabled = 1, data_balance = 10000000 WHERE imsi = " + imsi
	cursor.execute(commit_str)
	print commit_str

else if (command == "admin"):
	imsi = sys.argv[5]

	commit_str = "UPDATE customers SET admin = 1 WHERE imsi = " + imsi
	cursor.execute(commit_str)
	print commit_str

else if (command == "noadmin"):
	imsi = sys.argv[5]

	commit_str = "UPDATE customers SET admin = 0 WHERE imsi = " + imsi
	cursor.execute(commit_str)
	print commit_str

# SMS TODO: sync ?!?

db.commit()
cursor.close()
db.close()