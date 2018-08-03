from helpers import iptables

import logging
import MySQLdb
import os

db = MySQLdb.connect(host="localhost",
                     user=os.environ.get('COLTE_USER'),
                     passwd=os.environ.get('COLTE_DBPASS'),
                     db="colte_db")
cursor = db.cursor()

query = "SELECT customers.imsi, ip FROM customers, static_ips WHERE customers.imsi=static_ips.imsi AND bridged=0 AND data_balance>0"
cursor.execute(query)

for row in cursor:
    imsi = row[0]
    ip = row[1]
    logging.info("IPTables Watchdog Re-Enabling IMSI " + imsi + "'s access to the Internet")
    iptables.disable_forward_filter(ip)

