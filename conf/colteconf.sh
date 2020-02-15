#!/bin/bash

RED='\033[0;31m'
NC='\033[0m'

if [ "$EUID" -ne 0 ]; then
    printf "colteconf: ${RED}error:${NC} Must run as root! \n"
    exit 1;
fi

cd /etc/colte/colteconf
virtualenv env
source env/bin/activate
pip install ruamel.yaml
pip install netaddr
python colteconf.py
# this command is still necessary but currently conflicts with open5gs-pgwd
#systemctl restart systemd-networkd
