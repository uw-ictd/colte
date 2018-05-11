#!/bin/bash

source /usr/local/etc/colte/coltenv

echo "curl http://localhost:3002/lua/colte_log_bandwidth.lua > colte/lte_extras/billing/tmp_dump.txt"

curl --cookie "user=$COLTE_NT_USERNAME; password=$COLTE_NT_PASS" http://localhost:3002/lua/colte_log_bandwidth.lua > $COLTE_DIR/lte_extras/billing/tmp_dump.txt

python $COLTE_DIR/lte_extras/billing/import_to_db.py
