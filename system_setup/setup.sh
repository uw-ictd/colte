#!/bin/bash

if [ -z "$COLTENV" ]; then
	echo "COLTENV not set, you must run generate_coltenv before proceeding"
	exit
fi

sudo apt-get update
sudo apt-get install -y ansible
ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/system_setup/play.yml
