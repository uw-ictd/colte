#!/bin/bash

# SCRIPTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# COLTE_DIR="$SCRIPTDIR/../.."

source /usr/local/etc/colte/coltenv

echo "RAN CRONSCRIPT" >> $COLTE_DIR/webservices/billing/cronscript_log.txt

# curl --cookie "user=$COLTE_PMA_USERNAME; password=$COLTE_PMA_PASSWORD" http://localhost/phpmyadmin/lua/colte_log_bandwidth.lua > $COLTE_DIR/webservices/billing/tmp_dump.txt
python $COLTE_DIR/webservices/billing/import_to_db.py