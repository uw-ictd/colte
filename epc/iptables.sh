#!/bin/bash

#sudo iptables -A INPUT -i lo -j ACCEPT

sudo iptables -C INPUT -i gtp0 -j ACCEPT
if [ $? != 0 ] ; then
	sudo iptables -A INPUT -i gtp0 -j ACCEPT
fi

sudo iptables -C INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT
if [ $? != 0 ] ; then
	sudo iptables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT
fi

sudo iptables -t nat -C POSTROUTING -o enp3s0 -j MASQUERADE
if [ $? != 0 ] ; then
	sudo iptables -t nat -A POSTROUTING -o enp3s0 -j MASQUERADE
fi

sudo iptables -C FORWARD -i gtp0 -o enp3s0 -j ACCEPT
if [ $? != 0 ] ; then
	sudo iptables -A FORWARD -i gtp0 -o enp3s0 -j ACCEPT
fi

sudo iptables -C FORWARD -i enp3s0 -o gtp0 -m state --state RELATED,ESTABLISHED -j ACCEPT
if [ $? != 0 ] ; then
	sudo iptables -A FORWARD -i enp3s0 -o gtp0 -m state --state RELATED,ESTABLISHED -j ACCEPT
fi
