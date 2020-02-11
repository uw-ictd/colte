#!/bin/bash
if ["$EUID" -ne 0]; then
    echo "colteconf: Must run as root!"
    exit 1;
else
    cd /etc/colte/colteconf
    sudo virtualenv env
    source env/bin/activate
    sudo pip install ruamel.yaml
    sudo pip install netaddr
    python colteconf.py
fi