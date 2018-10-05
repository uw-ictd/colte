import MySQLdb
import os
import sys

db = MySQLdb.connect(host="localhost",
                     user=os.environ.get('COLTE_USER'),
                     passwd=os.environ.get('COLTE_DBPASS'),
		     	 	 db="colte_db")
cursor = db.cursor()

num_args = len(sys.argv)

if num_args < 2:  # the program name and the arguments
  # stop the program and print an error message
  sys.exit("Must enter a command, {add imsi msisdn key opc} or {rm imsi}")

command = sys.argv[1]
#########################################################################
############### OPTION ONE: ADD A USER TO THE DATABASE ##################
#########################################################################
if (command == "add"):
	if num_args != 7:  # the program name and the arguments
	  # stop the program and print an error message
	  sys.exit("Incorrect number of arguments: format is add imsi msisdn ip key opc")

	imsi = sys.argv[2]
	msisdn = sys.argv[3]
	msisdn = sys.argv[4]
	key = sys.argv[5]
	opc = sys.argv[6]

	commit_str = "INSERT INTO pdn (apn, pgw_id, users_imsi) VALUES ('ltebox', 3, " + imsi + ")"
	cursor.execute(commit_str)
	print commit_str

	commit_str = "INSERT INTO users (imsi, msisdn, `key`, OPc, sqn, rand) VALUES ('" + imsi + "', '" + msisdn + "', " + key + ", " + opc + ", 351, 0x0)"
	cursor.execute(commit_str)
	print commit_str

	commit_str = "INSERT INTO customers (imsi, raw_down, raw_up, data_balance, balance, bridged, enabled, msisdn) VALUES (" + imsi + ", 0, 0, 10000000, 500, 1, 1, '" + msisdn + "')"
	cursor.execute(commit_str)
	print commit_str

	commit_str = "INSERT INTO static_ips (imsi, ip) VALUES ('" + imsi + "', '" + ip + "')"
	cursor.execute(commit_str)
	print commit_str

#########################################################################
############### OPTION TWO: REMOVE USER FROM THE DATABASE ###############
#########################################################################
if (command == "rm"):
	if num_args != 3:  # the program name and the arguments
	  # stop the program and print an error message
	  sys.exit("Incorrect number of arguments: format is rm imsi")

	imsi = sys.argv[2]

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
