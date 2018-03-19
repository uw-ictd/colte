#!/bin/bash

# SCRIPTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# COLTE_DIR="$SCRIPTDIR/../.."

curl --cookie "user=admin; password=password" http://localhost:3000/lua/colte_log_bandwidth.lua > $COLTE_DIR/webservices/billing/tmp_dump.txt

python $COLTE_DIR/webservices/billing/import_to_db.py
