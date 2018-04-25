#!/bin/bash

if [ -z "$COLTENV" ]; then
	echo "WARNING! No value for COLTENV, make sure you run generate_coltenv first"
	return -1
fi

if [ $COLTE_EPC == 1 ]; then
	ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/epc/install_scripts/debian-9.4.yml
fi

if [ $COLTE_ENBRAINS == 1 ]; then
	ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/enbrains/install_scripts/debian-9.4.yml
fi

if [ $COLTE_EMERGENCY_WEBSERVICES == 1 ]; then
	ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/emergency_webservices/install_scripts/debian-9.4.yml
fi

if [ $COLTE_IMS == 1 ]; then
	ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/lte_extras/ims/debian-9.4.yml
fi

if [ $COLTE_NODE == 1 ]; then
	ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/webservices/node/install_scripts/debian-9.4.yml
fi

if [ $COLTE_BILLING == 1 ]; then
	ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/webservices/billing/install_scripts/debian-9.4.yml
fi

