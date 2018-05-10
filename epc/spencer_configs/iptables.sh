#!/bin/bash

# This script essentially sets up NAT from the gtp0 interface (which is brought up by the SPGW)
# to the WAN interface of the box, so that UEs can access the Internet.
# Only needs to be run once but IS NOT persistent, so must be run again after system reboot.

#sudo iptables -A INPUT -i lo -j ACCEPT

iptables -C INPUT -i gtp0 -j ACCEPT
if [ $? != 0 ] ; then
	iptables -A INPUT -i gtp0 -j ACCEPT
fi

iptables -C INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT
if [ $? != 0 ] ; then
	iptables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT
fi

     iptables -t nat -C POSTROUTING -o enp3s0 -j MASQUERADE
if [ $? != 0 ] ; then
     iptables -t nat -A POSTROUTING -o enp3s0 -j MASQUERADE
fi

     iptables -C FORWARD -i gtp0 -o enp3s0 -j ACCEPT
if [ $? != 0 ] ; then
     iptables -A FORWARD -i gtp0 -o enp3s0 -j ACCEPT
fi

     iptables -C FORWARD -i enp3s0 -o gtp0 -m state --state RELATED,ESTABLISHED -j ACCEPT
if [ $? != 0 ] ; then
     iptables -A FORWARD -i enp3s0 -o gtp0 -m state --state RELATED,ESTABLISHED -j ACCEPT
fi

# SMS NOTE 1:

# I should be able to remove rules (1) and (2) if chain defaults are set to accept.
# Rule (1) ensures that packets from gtp0 can access me locally. We CAN REMOVE this
# if the default INPUT chain is set to accept (is it?).
# I have no clue wht Rule (2) does, really. It seems like a tautology? It only applies
# to FILTER table INPUT chain, which is destined inbound for local host. Why wouldn't
# these packets already be accepted, how was conn. established in the first place?!?!?
# Either way this rule should NOT ever be in the flow for NATed packets in or out - 
# FORWARD chain would be the only appropriate one in the FILTER table.

# Rule (3) is the heart of the NAT. MASQUERADE automatically sets up SNAT/DNAT
# and is vital for situations where we don't already know the source IP (note
# that it could be anything in 192.168.151.*)
# Rules (4) and (5) ensure that packets bridged to/from gtp0 don't get dropped by FILTER chain.

# SMS NOTE 2: HERE'S HOW TO ENABLE/DISABLE INDIVIDUAL CLIENT USERS!!!

# IPTABLES 
# iptables -I FORWARD -s {source_ip} -j REJECT
# iptables -I FORWARD -d {source_ip} -j REJECT

# iptables -D FORWARD -s {source_ip} -j REJECT
# iptables -D FORWARD -d {source_ip} -j REJECT
