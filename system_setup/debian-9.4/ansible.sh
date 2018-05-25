#!/bin/bash

if [ -z "$COLTENV" ]; then
	echo "WARNING! No value for COLTENV, make sure you run generate_coltenv first"
	return -1
fi

if [ $COLTE_EPC == 1 ]; then
	ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/epc/install_scripts/debian-9.4.yml
fi

if [ $COLTE_BILLING == 1 ]; then
	ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/lte_extras/billing/install_scripts/debian-9.4.yml
fi

if [ $COLTE_NGINX == 1 ]; then
	ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/lte_extras/nginx/debian-9.4.yml
fi

if [ $COLTE_WEBGUI == 1 ]; then
	ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/lte_extras/webgui/install_scripts/debian-9.4.yml
fi

if [ $COLTE_BIND == 1 ]; then
	ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/lte_extras/bind/debian-9.4.yml
fi

if [ $COLTE_IMS == 1 ]; then
	ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/lte_extras/ims/debian-9.4.yml
fi

if [ $COLTE_MEDIA == 1 ]; then
	ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/lte_extras/media/debian-9.4.yml
fi

if [ $COLTE_MAP == 1 ]; then
	ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/lte_extras/maps/debian-9.4.yml
fi

if [ $COLTE_WIKI == 1 ]; then
	ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/lte_extras/wiki/debian-9.4.yml
fi

if [ $COLTE_CHAT == 1 ]; then
	ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/lte_extras/chat/debian-9.4.yml
fi

if [ $COLTE_ENBRAINS == 1 ]; then
	ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/enbrains/install_scripts/debian-9.4.yml
fi

if [ $COLTE_EMERGENCY == 1 ]; then
	ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/emergency/install_scripts/debian-9.4.yml
fi
