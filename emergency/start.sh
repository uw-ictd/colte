#!/bin/bash

source /usr/local/etc/colte/coltenv
ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/emergency/install_scripts/run.yml