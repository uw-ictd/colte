#!/bin/bash

# Step 0: COMPLAIN if not Debian 9.3?!?

# Step 2: Internet interface:
# We want the Internet gateway to be XXXX
# And we want it on interface XXXX

# Step 2.5: Local network interface:
# We want the eNB to be on 10.0.101.2
# The EPC will be 10.0.101.3

# Step 3:
# Write these settings into the initial configuration file

# Step 4: Install prereqs (ansible and python)
sudo add-apt-repository -y ppa:ansible/ansible-2.4
sudo apt-get update
sudo apt-get install -y ansible python2.7

# Step 5: Run Ansible script to do the rest
ansible-playbook -v -i "localhost," -c local epc_playbook.yml 

# Step 6: Any final-final configs?!? Setting IP addresses in config files, etc?!?
