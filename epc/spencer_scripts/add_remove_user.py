# synchronize_dbs.py
# This script is to be run once, after both databases (oai_hss and customers) are installed.
# The script uses the "customers" db as the master, runs some sanity checks on it, and then
# uses it to configure the oai_hss database. This means the following rules:
# (1) every entry in "customers" MUST have valid (+0) balance, or it will be disabled
# (2) every entry in "oai_hss" MUST have an enabled entry in customers database or will be disabled

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
if (command == "add")
	if num_args != 6:  # the program name and the arguments
	  # stop the program and print an error message
	  sys.exit("Incorrect number of arguments: format is add imsi msisdn key opc")

	imsi = sys.argv[2]
	msisdn = sys.argv[3]
	key = sys.argv[4]
	opc = sys.argv[5]

	commit_str = "INSERT INTO pdn (apn, users_imsi) VALUES (ltebox, " + imsi + ")"
	cursor.execute(commit_str)
	print commit_str

	commit_str = "INSERT INTO users (imsi, msisdn, key, OPc, sqn, rand) VALUES (" + imsi + ", " + msisdn + ", " + key + ", " + opc + ", 351, 0x0)"
	cursor.execute(commit_str)
	print commit_str

	commit_str = "INSERT INTO customers (imsi, raw_down, raw_up, balance, enabled, msisdn) VALUES (" + imsi + ", 0, 0, 0, 0, 'NotUsed')"
	cursor.execute(commit_str)
	print commit_str

#########################################################################
############### OPTION TWO: REMOVE USER FROM THE DATABASE ###############
#########################################################################
if (command == "rm")
	if num_args != 3:  # the program name and the arguments
	  # stop the program and print an error message
	  sys.exit("Incorrect number of arguments: format is rm imsi")

	imsi = sys.argv[2]

	commit_str = "DELETE FROM pdn WHERE imsi = " + imsi
	cursor.execute(commit_str)
	print commit_str

	commit_str = "DELETE FROM users WHERE imsi = " + imsi
	cursor.execute(commit_str)
	print commit_str

	commit_str = "DELETE FROM customers WHERE imsi = " + imsi
	cursor.execute(commit_str)
	print commit_str

