#!/bin/bash

sudo apt-get update
sudo apt-get install -y ansible
ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/system_setup/play.yml
