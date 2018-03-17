import MySQLdb

db = MySQLdb.connect(host="localhost",
			user="colte",
			passwd="correcthorsebatterystaple",
			db="billing")
cursor = db.cursor
#file = open('$COLTE_DIR/webservices/billing/tmp_dump.txt', 'r')
file = open('tmp_dump.txt', 'r')

# PROCESS FIRST ROW IS JUST TIME OF LAST ENTRY
timestr = file.readline().split()
print timestr

for line in file:
	vals = line.split()
	query = ("SELECT * FROM customers WHERE ip EQUALS " + str(vals[0]))
	cursor.execute(query)

	count = 0
	i = "1.2.3.4"
	rd = 0
	ru = 0
	b = 0
	for (ip, raw_down, raw_up, balance) in cursor:
		count = count +1
		i = ip
		rd = raw_down
		ru = raw_up
		b = balance

	if count == 0:
		print "SHIT!"
		# WHAT?!?
		# CREATE A NEW BILLING ENTRY?!?!?
		# (PROBABLY SHOULD FILE AN ALERT AND CUTOFF SERVICE)
	if count > 1:
		print "SHIT!"

	# now we're guaranteed to have an object to work with

	# data is only used (cumulatively, duh) so the only way these values will ever be less than previous val
	# is if the counter reset. Ideally this never happens but edge-cases are important
	if (vals[1] < rd) or (vals[2] < ru)
		# LOG SOMETHING!!!
		new_bytes_down = vals[1]
		new_bytes_up = vals[2]
	else
		new_bytes_down = vals[1] - rd
		new_bytes_up = vals[2] - ru

	# obj.raw_down = vals[1]
	# obj.raw_up = vals[2]

	# BILLING MATH HERE!
	cost_in_period = calculate_cost(new_bytes_down, new_bytes_up)
	new_balance = b - cost_in_period

	# check for certain thresholds and potentially send warnings or take other action?

	if new_balance <= 0
		print "CUT OFF USER"
		#cut_off_user(obj)

	# END: store the record locally and onto the next user
	new_record = [vals[0], vals[1], vals[2], new_balance]
	record_list.add(new_record)
	
# (commit all updates at once to save DB operations)
commit_str = "INSERT INTO customers (ip, raw_down, raw_up, balance) VALUES (%s, %s, %s, %s)"
cursor.executemany(commit_str, record_list)
# rows = file.readlines()
# rows = file.readlines()[1:]
# vals = [line.split() for line in rows]
# query = "INSERT INTO customers (ip, raw_up, raw_down) VALUES (%s, %s, %s)"
# db.commit()
# db.close()

calculate_cost:
	# NAIVE APPROACH SO FAR: cost per byte * bytes

#cut_off_user:
	# 1: send text!
	# 2: CUT USER OFF
	# 3: still preserve the record so that we can settle-up or re-activate sim without drama
