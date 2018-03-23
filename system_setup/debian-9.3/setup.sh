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

$COLTE_DIR/system_setup/debian-9.3/ansible.sh

