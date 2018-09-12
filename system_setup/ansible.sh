#!/bin/bash

ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/system_setup/play.yml
