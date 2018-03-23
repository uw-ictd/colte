#!/bin/bash
#sudo iptables -A INPUT -i lo -j ACCEPT
sudo iptables -A INPUT -i gtp0 -j ACCEPT
sudo iptables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT
sudo iptables -t nat -A POSTROUTING -o enp3s0 -j MASQUERADE
sudo iptables -A FORWARD -i gtp0 -o enp3s0 -j ACCEPT
sudo iptables -A FORWARD -i enp3s0 -o gtp0 -m state --state RELATED,ESTABLISHED -j ACCEPT

