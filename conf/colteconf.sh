#!/bin/bash

RED='\033[0;31m'
NC='\033[0m'

if [ "$EUID" -ne 0 ]; then
    printf "colteconf: ${RED}error:${NC} Must run as root! \n"
    exit 1;
fi

# we need to teardown the colte-nat BEFORE we rewrite the script
restarted_nat=0
systemctl is-active --quiet colte-nat
if [ $? -eq 0 ]; then
        systemctl stop colte-nat
        restarted_nat=1
fi

cd /etc/colte/colteconf
source env/bin/activate
python colteconf.py

# Now: restart services that were already running

# open5gs
systemctl is-active --quiet open5gs-hssd
if [ $? -eq 0 ]; then
    echo "restarting open5gs-hssd"
    systemctl restart open5gs-hssd
fi

systemctl is-active --quiet open5gs-mmed
if [ $? -eq 0 ]; then
    echo "restarting open5gs-mmed"
    systemctl restart open5gs-mmed
fi

systemctl is-active --quiet open5gs-sgwd
if [ $? -eq 0 ]; then
    echo "restarting open5gs-sgwd"
    systemctl restart open5gs-sgwd
fi

systemctl is-active --quiet open5gs-pgwd
if [ $? -eq 0 ]; then
    echo "restarting open5gs-pgwd"
    systemctl restart open5gs-pgwd
fi

systemctl is-active --quiet open5gs-pcrfd
if [ $? -eq 0 ]; then
    echo "restarting open5gs-pcrfd"
    systemctl restart open5gs-pcrfd
fi

# haulage
systemctl is-active --quiet haulage
if [ $? -eq 0 ]; then
    echo "restarting haulage"
    systemctl restart haulage
fi

# colte services
systemctl is-active --quiet colte-webgui
if [ $? -eq 0 ]; then
    echo "restarting colte-webgui"
    systemctl restart colte-webgui
fi

systemctl is-active --quiet colte-webadmin
if [ $? -eq 0 ]; then
    echo "restarting colte-webadmin"
    systemctl restart colte-webadmin
fi

# always restart systemd-networkd
echo "restarting systemd-networkd"
systemctl restart systemd-networkd

# colte-nat teardown/restart
if [ restarted_nat ]; then
    echo "restarting colte-nat"
	systemctl start colte-nat
fi