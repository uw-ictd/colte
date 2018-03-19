import MySQLdb

# example cost: 5 dollars per gb
cost_per_gb = 5.00
cost_per_mb = cost_per_gb / 1024.0
cost_per_byte = cost_per_mb / 1024.0
def calculate_cost(bytes_down, bytes_up):
	# NAIVE APPROACH SO FAR: cost per byte * bytes
	total_bytes = bytes_down + bytes_up
	total_cost = total_bytes * cost_per_byte
	print "new bytes: " + str(total_bytes) + "\ncost per byte: " + str(cost_per_byte) + "\ntotal cost: " + str(total_cost)
	return total_cost

def cut_off_user(imsi):
	print "CUTTING OFF USER " + str(imsi)
	# 1: send text!
	# 2: CUT USER OFF
	# 3: still preserve the record so that we can settle-up or re-activate sim without drama

def make_new_user(vals):
        print "Make new user? No, don't! How'd they even get on the network?"


print os.environ.get('COLTE_USER')
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
	
	# data is only incremented (cumulatively, duh) so the only way these values will ever be less than previous val
	# is if the counter reset. hopefully this never happens but edge-cases are important
    if (new_bytes_down < previous_bytes_down) or (new_bytes_up < previous_bytes_up):
		# LOG SOMETHING!!!
		bytes_down_in_period = new_bytes_down
		bytes_up_in_period = new_bytes_up
    else:
		bytes_down_in_period = int(new_bytes_down) - int(previous_bytes_down)
		bytes_up_in_period = int(new_bytes_up) - int(previous_bytes_up)

	# billing math here
	cost_in_period = calculate_cost(bytes_down_in_period, bytes_up_in_period)
	new_balance = previous_balance - cost_in_period

	# check for certain thresholds and potentially send warnings or take other action?

	if new_balance <= 0:
		cut_off_user(1234)

	# END: store the record locally and onto the next user
	new_record = (new_bytes_down, new_bytes_up, str(new_balance), table_id)
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

