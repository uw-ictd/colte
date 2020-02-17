#!/bin/bash

RED='\033[0;31m'
NC='\033[0m'

if [ "$EUID" -ne 0 ]; then
    printf "colteconf: ${RED}error:${NC} Must run as root! \n"
    exit 1;
fi

# we need to teardown the colte-nat BEFORE we rewrite the script
restarted_nat=0
if [ !`systemctl is-active --quiet colte-nat` ]; then
        systemctl stop colte-nat
        restarted_nat=1
fi

cd /etc/colte/colteconf
virtualenv env
source env/bin/activate
pip install ruamel.yaml
pip install netaddr
python colteconf.py

# Now: restart services that were already running

# open5gs
if [ !`systemctl is-active --quiet open5gs-hssd` ]; then
        systemctl restart open5gs-hssd
fi

if [ !`systemctl is-active --quiet open5gs-mmed` ]; then
        systemctl restart open5gs-mmed
fi

if [ !`systemctl is-active --quiet open5gs-sgwd` ]; then
        systemctl restart open5gs-sgwd
fi

if [ !`systemctl is-active --quiet open5gs-pgwd` ]; then
        systemctl restart open5gs-pgwd
fi

if [ !`systemctl is-active --quiet open5gs-pcrfd` ]; then
        systemctl restart open5gs-pcrfd
fi

# haulage
if [ !`systemctl is-active --quiet haulage` ]; then
        systemctl restart haulage
fi

# colte services
if [ !`systemctl is-active --quiet colte-webgui` ]; then
        systemctl restart colte-webgui
fi

if [ !`systemctl is-active --quiet colte-webadmin` ]; then
        systemctl restart colte-webadmin
fi

# always restart systemd-networkd
systemctl restart systemd-networkd

# colte-nat teardown/restart
if [ restarted_nat ]; then
	systemctl start colte-nat
fi