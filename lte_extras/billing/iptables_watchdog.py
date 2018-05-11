import MySQLdb
import os
import subprocess

#query = "SELECT imsi, ip FROM customers WHERE bridged=1 AND data_balance<=0"
#numrows = cursor.execute(query)

#for row in cursor:
#	imsi = row[0]
#	ip = row[1]
#	print "IPTables Watchdog Disabling IMSI " + imsi + "'s access to the Internet"
#	print "ERROR/NOTE: THIS SHOULD NEVER HAVE HAPPENED!"
#	enable_iptables_filter(ip)

def enable_iptables_filter(c):
	# "sudo iptables -I FORWARD -s " + c.ip + " -j REJECT"
	p = subprocess.Popen(["iptables", "-I", "FORWARD", "-s", ip, "-j", "REJECT"], stdout=subprocess.PIPE)
	output , err = p.communicate()
	print output
	# rule = iptc.Rule()
	# rule.src = c.ip
	# rule.target = iptc.Target('REJECT')
	# chain = iptc.Chain(iptc.Table.(iptc.Table.FILTER), "FORWARD")
	# chain.insert_rule(rule)

def disable_iptables_filter(ip):
	# "sudo iptables -D FORWARD -s " + c.ip + " -j REJECT"
	p = subprocess.Popen(["iptables", "-D", "FORWARD", "-s", ip, "-j", "REJECT"], stdout=subprocess.PIPE)
	output , err = p.communicate()
	print output
	# rule = iptc.Rule()
	# rule.src = c.ip
	# rule.target = iptc.Target('REJECT')
	# chain = iptc.Chain(iptc.Table.(iptc.Table.FILTER), "FORWARD")
	# chain.delete_rule(rule)

db = MySQLdb.connect(host="localhost",
                     user=os.environ.get('COLTE_USER'),
                     passwd=os.environ.get('COLTE_DBPASS'),
		     	 	 db="colte_db")
cursor = db.cursor()

query = "SELECT customers.imsi, ip FROM customers,static_ips WHERE customers.imsi=static_ips.imsi AND bridged=0 AND data_balance>0"
numrows = cursor.execute(query)

for row in cursor:
	imsi = row[0]
	ip = row[1]
	print "IPTables Watchdog Re-Enabling IMSI " + imsi + "'s access to the Internet"
	disable_iptables_filter(ip)

