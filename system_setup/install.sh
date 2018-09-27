#!/bin/bash
BASEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if [ "$#" -ne 1 ]; then
	sudo apt-get install -y ansible
	ansible-playbook -K -v -i "localhost," -c local $BASEDIR/source_build.yml
	exit 0
fi

if [ ! -f $BASEDIR/$1.yml ]; then
    echo "Error: Invalid play! Usage is ./install.sh {playbook name}."
    echo "Available sample options include basic_install, full_install, webservices, reconfigure, and source_build."
    echo "To add a play, create a .yml file in this directory."
    exit -1
fi

sudo apt-get update
sudo apt-get install -y ansible
ansible-playbook -K -v -i "localhost," -c local $BASEDIR/$1.yml
# ansible-playbook -v -i "localhost," -c local --extra-vars "ansible_become_pass=SUDO_PASSWORD" $BASEDIR/$1.yml