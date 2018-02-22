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
sudo cp /home/enbrain/preseed/ansible_sources.list /etc/apt/sources.list
sudo apt-get update
sudo apt-get install -y --allow-unauthenticated ansible python2.7



#sudo add-apt-repository -y ppa:ansible/ansible-2.4
#sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 93C4A3FD7BB9C367
#sudo apt-get update

# Step 5: Run Ansible script to do the rest
sudo cp /home/enbrain/preseed/sources.list /etc/apt/sources.list
sudo apt-get update

sudo apt-get -y install vim
ansible-playbook -v -i "localhost," -c local /home/enbrain/preseed/ansible/epc_playbook.yml 

# Step 6: Any final-final configs?!? Setting IP addresses in config files, etc?!?
