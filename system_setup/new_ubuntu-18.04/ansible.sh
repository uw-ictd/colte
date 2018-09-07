#!/bin/bash

ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/system_setup/ubuntu-18.04/play.yml
