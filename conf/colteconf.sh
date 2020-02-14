#!/bin/bash

RED='\033[0;31m'
NC='\033[0m'

if [ "$EUID" -ne 0 ]; then
    printf "colteconf: ${RED}error:${NC} Must run as root! \n"
    exit 1;
fi

cd /etc/colte/colteconf
sudo virtualenv env
source env/bin/activate
sudo pip install ruamel.yaml
sudo pip install netaddr
python colteconf.py
systemctl restart systemd-networkd
