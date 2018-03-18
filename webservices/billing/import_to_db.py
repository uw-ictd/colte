import MySQLdb

# example cost: 5 dollars per gb
cost_per_gb = 5.00
cost_per_mb = cost_per_gb / 1024.0
cost_per_byte = cost_per_mb / 1024.0
def calculate_cost(bytes_down, bytes_up):
	# NAIVE APPROACH SO FAR: cost per byte * bytes
	total_bytes = bytes_down + bytes_up
	total_cost = total_bytes * cost_per_byte
	return total_cost

def cut_off_user(imsi):
	print "CUTTING OFF USER " + str(imsi)
	# 1: send text!
	# 2: CUT USER OFF
	# 3: still preserve the record so that we can settle-up or re-activate sim without drama

def make_new_user(vals):
        print "MAKE NEW USER?!? (NO!)"





record_list = []
db = MySQLdb.connect(host="localhost",
#			user="vagrant",
                        user="colte"
                        passwd="correcthorsebatterystaple",
			db="billing")
cursor = db.cursor()
#file = open('$COLTE_DIR/webservices/billing/tmp_dump.txt', 'r')
file = open('tmp_dump.txt', 'r')

# PROCESS FIRST ROW IS JUST TIME OF LAST ENTRY
timestr = file.readline().split()
print timestr

for line in file:
	vals = line.split()
	query = ("SELECT * FROM customers WHERE ip = '" + str(vals[0]) + "'")
	numrows = cursor.execute(query)

	if numrows == 0:
		make_new_entry(vals)

	if numrows > 1:
		print "SHIT?!?"

	answer_tuple = cursor.fetchone()
	#print answer_tuple

	i = answer_tuple[1]
	rd = answer_tuple[2]
	ru = answer_tuple[3]
	b = answer_tuple[4]
	
	# data is only incremented (cumulatively, duh) so the only way these values will ever be less than previous val
	# is if the counter reset. hopefully this never happens but edge-cases are important
        if (vals[1] < rd) or (vals[2] < ru):
		# LOG SOMETHING!!!
		new_bytes_down = vals[1]
		new_bytes_up = vals[2]
        else:
		new_bytes_down = int(vals[1]) - int(rd)
		new_bytes_up = int(vals[2]) - int(ru)

	# obj.raw_down = vals[1]
	# obj.raw_up = vals[2]

	# BILLING MATH HERE!
	cost_in_period = calculate_cost(new_bytes_down, new_bytes_up)
	new_balance = b - cost_in_period

	# check for certain thresholds and potentially send warnings or take other action?

        if new_balance <= 0:
		cut_off_user(1234)

	# END: store the record locally and onto the next user
	new_record = (vals[1], vals[2], str(new_balance), answer_tuple[0])
	record_list.append(new_record)
	
# (commit all updates at once to save DB operations)
commit_str = "UPDATE customers SET raw_down = %s, raw_up = %s, balance = %s WHERE idcustomers = %s"
cursor.executemany(commit_str, record_list)

# rows = file.readlines()
# rows = file.readlines()[1:]
# vals = [line.split() for line in rows]
# query = "INSERT INTO customers (ip, raw_up, raw_down) VALUES (%s, %s, %s)"
# db.commit()
# db.close()

