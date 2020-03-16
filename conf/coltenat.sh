#!/bin/bash

ADDRESS='10.45.0.0/16'

if [ "$EUID" -ne 0 ]; then
	echo "coltenat: Must run as root!"
	exit 1
fi

if [ "$#" -ne 1 ]; then
	echo "usage: coltenat {start | stop}"
	exit 0
fi

if [ "$1" = "start" ]; then
	iptables -C INPUT -i ogstun -j ACCEPT
	if [ $? != 0 ] ; then
		iptables -A INPUT -i ogstun -j ACCEPT
	fi

	iptables -t nat -C POSTROUTING -s $ADDRESS -j MASQUERADE
	if [ $? != 0 ] ; then
		iptables -t nat -A POSTROUTING -s $ADDRESS -j MASQUERADE
	fi	

	exit 0
fi

if [ "$1" = "stop" ]; then
	iptables -t nat -D POSTROUTING -s $ADDRESS -j MASQUERADE
	exit 0
fi

echo "usage: coltenat {start | stop}"