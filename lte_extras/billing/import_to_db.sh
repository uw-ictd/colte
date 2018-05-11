#!/bin/bash

source /usr/local/etc/colte/coltenv

echo "curl http://localhost:3002/lua/colte_log_bandwidth.lua > colte/lte_extras/billing/tmp_dump.txt"

curl --cookie "user=$COLTE_NT_USERNAME; password=$COLTE_NT_PASS" http://localhost:3002/lua/colte_log_bandwidth.lua > $COLTE_DIR/lte_extras/billing/tmp_dump.txt

# this script does all billing calculations and disables users if necessary
python $COLTE_DIR/lte_extras/billing/import_to_db.py

# this script re-enables any users that may have topped-up and bought a new package
python $COLTE_DIR/lte_extras/billing/iptables_watchdog.py
