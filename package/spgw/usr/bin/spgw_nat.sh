#!/bin/bash

# This script essentially sets up NAT from the gtp0 interface (which is brought up by the SPGW)
# to the WAN interface of the box, so that UEs can access the Internet.
# Only needs to be run once but IS NOT persistent, so must be run again after system reboot.

iptables -C INPUT -i gtp0 -j ACCEPT
if [ $? != 0 ] ; then
	iptables -A INPUT -i gtp0 -j ACCEPT
fi

iptables -C INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT
if [ $? != 0 ] ; then
	iptables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT
fi

iptables -t nat -C POSTROUTING -o eth0 -j MASQUERADE
if [ $? != 0 ] ; then
     iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE
fi

iptables -C FORWARD -i gtp0 -o eth0 -j ACCEPT
if [ $? != 0 ] ; then
     iptables -A FORWARD -i gtp0 -o eth0 -j ACCEPT
fi

iptables -C FORWARD -i eth0 -o gtp0 -m state --state RELATED,ESTABLISHED -j ACCEPT
if [ $? != 0 ] ; then
     iptables -A FORWARD -i eth0 -o gtp0 -m state --state RELATED,ESTABLISHED -j ACCEPT
fi
