#!/bin/bash

if [ -z "$COLTENV" ]; then
    echo "ERROR: You must configure and run $COLTE_DIR/coltenv first!"
    exit 1
fi

# Step 1: Install prereqs (ansible and python)
sudo add-apt-repository -y ppa:ansible/ansible-2.4
sudo apt-get update
sudo apt-get install -y ansible python2.7

# Step 2: Run Ansible script to do the rest
ansible-playbook -v -i "localhost," -c local $COLTE_DIR/system_setup/ubuntu-17.10/main_playbook.yml

# Step 3: Any final-final configs?!? Setting IP addresses in config files, etc?!?

# Note 1: Internet interface:
# We want the Internet gateway to be XXXX
# And we want it on interface XXXX

# Note 2: Local network interface:
# We want the eNB to be on 10.0.101.2
# The EPC will be 10.0.101.3

