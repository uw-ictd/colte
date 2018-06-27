#!/bin/bash

ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/system_setup/debian-9.4/play.yml
