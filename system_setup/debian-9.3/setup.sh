#!/bin/bash

if [ -z "$COLTENV" ]; then
    echo "WARNING: Using default values from $COLTE_DIR/generate_coltenv. Make sure you look at them!"
    SOURCEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
    source $SOURCEDIR/../../generate_coltenv
fi

# Step 1: Install prereqs (ansible and python)
sudo cp $COLTE_DIR/system_setup/debian-9.3/sources/ansible_sources.list /etc/apt/sources.list
sudo apt-get update
sudo apt-get install -y --allow-unauthenticated ansible python2.7

#sudo add-apt-repository -y ppa:ansible/ansible-2.4
#sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 93C4A3FD7BB9C367
#sudo apt-get update

# Step 2: Run Ansible script to do all the rest of the setup
sudo cp $COLTE_DIR/system_setup/debian-9.3/sources/sources.list /etc/apt/sources.list
sudo apt-get update

sudo apt-get -y install vim curl

if [ $COLTE_EPC == 1 ]; then
	ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/epc/install_scripts/debian-9.3.yml
fi

if [ $COLTE_ENBRAINS == 1 ]; then
	ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/enbrains/install_scripts/debian-9.3.yml
fi

if [ $COLTE_EMERGENCY_WEBSERVICES == 1 ]; then
	ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/emergency_webservices/install_scripts/debian-9.3.yml
fi

if [ $COLTE_NODE == 1 ]; then
	ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/webservices/node/install_scripts/debian-9.3.yml
fi

if [ $COLTE_BILLING == 1 ]; then
	ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/webservices/billing/install_scripts/debian-9.3.yml
fi

