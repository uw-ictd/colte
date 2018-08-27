import logging
import MySQLdb
import os


class BillingConnection(object):
    def __init__(self):
        self.db = MySQLdb.connect(host="localhost",
                                  user=os.environ.get('COLTE_USER'),
                                  passwd=os.environ.get('COLTE_DBPASS'),
                                  db="colte_db")

        self.cursor = self.db.cursor()

    def imsi_from_ip(self, ip_addr):
        query = "SELECT imsi FROM static_ips WHERE ip = \"" + ip_addr + "\""
        logging.debug(query)
        numrows = self.cursor.execute(query)

        if numrows == 0:
            raise KeyError("No imsi for observed ip address " + ip_addr)

        if numrows > 1:
            raise ValueError("Database invariants are broken. IP address is assigned to multiple imsis")

        answer_tuple = self.cursor.fetchone()
        imsi = answer_tuple[0]
        return imsi

    def query_customer_info(self, imsi):
        query = "SELECT raw_down, raw_up, data_balance, balance, bridged, enabled FROM customers WHERE imsi = " + imsi
        numrows = self.cursor.execute(query)

        if numrows == 0:
            raise KeyError("No entry for imsi " + imsi + " in the customer database")
        if numrows > 1:
            raise ValueError("More than one entry for imsi " + imsi + "? What happened???")

        return self.cursor.fetchone()

    def batch_update_customer_info(self, customer_info_list):
        commit_str = "UPDATE customers SET raw_down = %s, raw_up = %s, data_balance = %s, enabled = %s, bridged = %s WHERE imsi = %s"

        # TODO(matt9j) Is there any kind of error reporting from this function?
        self.cursor.executemany(commit_str, customer_info_list)

    def commit_changes(self):
        self.db.commit()
