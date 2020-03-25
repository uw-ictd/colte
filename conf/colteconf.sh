#!/bin/bash

RED='\033[0;31m'
NC='\033[0m'

if [ "$EUID" -ne 0 ]; then
    printf "colteconf: ${RED}error:${NC} Must run as root! \n"
    exit 1;
fi

# we need to teardown the colte-nat BEFORE we rewrite the script
systemctl stop colte-nat

python /etc/colte/scripts.py conf

# always enable ip_forwarding
sysctl -w net.ipv4.ip_forward=1

systemctl is-active --quiet colte-webadmin
if [ $? -eq 0 ]; then
    echo "restarting colte-webadmin"
    systemctl restart colte-webadmin
fi

# always restart systemd-networkd
echo "restarting systemd-networkd"
systemctl restart systemd-networkd
