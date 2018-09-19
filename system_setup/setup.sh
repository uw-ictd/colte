#!/bin/bash

BASEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && cd .. && pwd )"
export COLTE_DIR=$BASEDIR
export COLTE_USER=$USER

sudo apt-get update
sudo apt-get install -y ansible
ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/system_setup/play.yml
