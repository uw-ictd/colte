from helpers import constants
from helpers import iptables

import db_interface
import logging
import os


class Customer(object):
    def __init__(self):
        self.imsi = constants.ZERO_IMSI
        self.ip = constants.ZERO_IPv4

        self.new_raw_down = 0
        self.new_raw_up = 0
        self.new_data_balance = 0

        self.old_raw_down = 0
        self.old_raw_up = 0
        self.old_data_balance = 0

        self.balance = 0.0
        self.bridged = 1
        self.enabled = 1


def parse_ntopng_log_file(filename, db_connection):
    with open(filename, 'r') as file:
        # The first row is just the time of the entry
        timestr = file.readline().split()
        print("Read Timestring: " + str(timestr))

        record_list = []
        for line in file:
            c = Customer()

            vals = line.split()
            c.ip = vals[0]
            c.new_bytes_down = int(vals[1])
            c.new_bytes_up = int(vals[2])

            try:
                c.imsi = db_connection.imsi_from_ip(c.ip)
            except LookupError as e:
                logging.error("Failed to lookup imsi for IP: " + c.ip + str(e))
                continue

            try:
                raw_customer_tuple = db_connection.query_customer_info(c.imsi)
            except LookupError as e:
                logging.error("Failed to get customer info for imsi: " + c.imsi + str(e))
                continue

            c.old_bytes_down = int(raw_customer_tuple[0])
            c.old_bytes_up = int(raw_customer_tuple[1])
            c.old_data_balance = int(raw_customer_tuple[2])
            c.balance = raw_customer_tuple[3]
            c.bridged = raw_customer_tuple[4]
            c.enabled = raw_customer_tuple[5]

            # sanity check
            if c.enabled != 1:
                logging.error("IMSI " + c.imsi + " not set to enabled in customers db? Value is " + str(c.enabled))

            # TODO(matt9j) Check if this is correct. Seems fishy with system restart and poweroff cases.
            # data is only incremented (cumulatively) so the only way these values will ever be less than previous val
            # is if the counter reset. hopefully this never happens but edge-cases are important
            # NOTE: some counters might reset when others don't!
            if c.new_bytes_down < c.old_bytes_down:
                bytes_down_in_period = c.new_bytes_down
            else:
                bytes_down_in_period = int(c.new_bytes_down) - int(c.old_bytes_down)

            if c.new_bytes_up < c.old_bytes_up:
                bytes_up_in_period = c.new_bytes_up
            else:
                bytes_up_in_period = int(c.new_bytes_up) - int(c.old_bytes_up)

            # SANITY CHECK
            if (bytes_down_in_period < 0) or (bytes_up_in_period < 0):
                logging.error("MAJOR BILLING SCRIPT ERROR: HOW COULD WE GET NEGATIVE BYTE USAGE?!?")
                bytes_down_in_period = 0
                bytes_up_in_period = 0

            total_bytes_in_period = bytes_down_in_period + bytes_up_in_period
            c.new_data_balance = c.old_data_balance - total_bytes_in_period

            logging.info("IMSI " + c.imsi + " used " + str(total_bytes_in_period) + " bytes. Bytes_remaining = " + str(c.new_data_balance) + ", raw_down = " + str(c.new_bytes_down) + ", raw_up = " + str(c.new_bytes_up))

            verify_balance(c)

            # END: store the record locally and onto the next user
            new_record = (c.new_bytes_down, c.new_bytes_up, str(c.new_data_balance), c.enabled, c.bridged, c.imsi)
            record_list.append(new_record)

        # Commit all updates at once to save on DB operations
        # TODO(matt9j) This does not use transactions, so it could be racy with async updates from the web UI
        db_connection.batch_update_customer_info(record_list)
        # TODO(matt9j) How do I know if the commit failed? I need to retry, right?
        db_connection.commit_changes()


def verify_balance(c):
    # if they still have a high data balance (LIKELY) then nothing to do here
    if c.new_data_balance > 10000000:  # 10MB
        return

    # send (ONLY ONE) alert if we're crossing a threshold. Go in reverse order
    # so that if we crosed multiple thresholds at once, only alert re: the lowest amt.
    if (c.new_data_balance <= 0) and (c.old_data_balance > 0):
        out_of_data(c)
    elif (c.new_data_balance <= 1000000) and (c.old_data_balance > 1000000):
        alert_crossed_1mb(c)
    elif (c.new_data_balance <= 5000000) and (c.old_data_balance > 5000000):
        alert_crossed_5mb(c)
    elif (c.new_data_balance <= 10000000) and (c.old_data_balance > 10000000):
        alert_crossed_10mb(c)


def alert_crossed_10mb(c):
    # TODO(sevilla) Once ims is working, send a message to the UE
    logging.info("IMSI " + c.imsi + " has less than 10MB of data remaining.")


def alert_crossed_5mb(c):
    # TODO(sevilla) Once ims is working, send a message to the UE
    logging.info("IMSI " + c.imsi + " has less than 5 MB of data remaining.")


def alert_crossed_1mb(c):
    # TODO(sevilla) Once ims is working, send a message to the UE
    logging.info("IMSI " + c.imsi + " has less than 1 MB of data remaining.")


def out_of_data(c):
    # TODO(sevilla) Once ims is working, send a message to the UE
    logging.info("IMSI " + c.imsi + " out of data, enabling iptables filter.")
    iptables.enable_forward_filter(c.ip)
    c.bridged = 0

    # TODO(sevilla) STEP 2: if they're also out of balance, cut them off entirely (figure this out later)


if __name__ == "__main__":
    db_connection = db_interface.BillingConnection()
    filename = os.environ.get('COLTE_DIR') + "/lte_extras/billing/tmp_dump.txt"
    parse_ntopng_log_file(filename, db_connection)
