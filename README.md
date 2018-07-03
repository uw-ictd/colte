# CoLTE
CoLTE is the Community LTE Project. Its goal is to be an all-in-one turnkey solution to setting up a small-scale locally-run community LTE network. CoLTE consists of several main elements working together:
1) An all-in-one software EPC powered by OpenAirInterface (OAI).
2) Network monitoring software (powered by ntopng) that keeps track of how many bytes each user uses and takes action at certain thresholds.
3) A Web GUI that lets users check the status of their account, top up, transfer/resell credit, and buy data packages.
4) Local Web and DNS serving/caching via BIND and Nginx.
5) Locally-hosted web services that include Rocketchat, Wikipedia, OpenStreetMaps, a media server, and more.

# Installation

## Get The Code:
Our "master" branch should always compile, but is updated frequently with small feature-adds. For maximum stability I recommend cloning the most recently tagged release.

## Basic System Requirements:
Currently our only supported/tested build environment is debian-9.4 (scratch). In the past we had success with Ubuntu-17.04 (now deprecated). Ubuntu-18.04 support is coming soon. We found that OAI had substantial problems with Ubuntu-16.04, so don't bother trying to port that.

## Step 1: Configure Your Network
Your machine will need two network connections: one to the Internet (the upstream WAN) and another to the eNodeB (the downstream LAN). These can be set to the same interface, it doesn't matter. Both of these connections must be already configured with IP addresses (doesn't matter if Static or Dynamic) and must be up. Note that if the LAN interface is down, you won't be able to start the MME, and if the WAN interface is down, you won't be able to start the SPGW.

## Step 2: Configure CoLTE
Once your network configuration is correct, you need to look at (and change) generate_coltenv. Most of the options can and should be left alone (unless you know what you're doing), but you must look at and change the Network Configuration and Compilation Options sections. Please know that (1) reading through these options in detail will dramatically simplify your life down the road, because (2) we do NOT currently support dynamically changing these configured values after they're set.

Network Configuration: Set COLTE_WAN_IFACE to your upstream (Internet) interface, COLTE_ENB_IFACE to the downstream LAN interface, and COLTE_ENB_IFACE_ADDR to the downstream interface's address/subnet. Don't worry about editing COLTE_LTE_SUBNET or matching it to anything, because this subnet is created and assigned to the virtual gtp0 interface once the SPGW brings it up.  COLTE_NETWORK_NAME lets you give your network a specific name for serving DNS entries (e.g. if it's set to "seattle" then connected phones can access the webgui under "http://network.seattle".

Compilation Options: Each macro under this section refers to a different CoLTE feature that you can tun on or off as you please. COLTE_EPC refers to the core EPC code (needed for any LTE stuff), COLTE_BILLING refers to our network management and billing software, and COLTE_WEBGUI refers to our webGUI for users to check their balance and purchase credit. The final five options (MEDIA, WIKI, MAP, CHAT, and EMERGENCY) all refer to various locally-hosted webservices: a local mediaserver, Wikipedia, a mapping server (powered by OpenStreetMaps), a RocketChat server, and an emergency registration service. For more information about individual web services, check the WEBSERVICES.md document.

## Step 3: Install Everything.
Run the following commands to install CoLTE. This will take a while (15min+ on some systems with everything installed) and, at the end, you should see an Ansible success message.

```
source ./generate_coltenv
./system_setup/$OS/setup.sh
./system_setup/$OS/ansible.sh
```

# Running CoLTE

## EPC:
The EPC has three separate components: the hss, mme, and spgw. I recommend starting them in that order. Once installed, you can start or stop each component by typing the following command in a terminal window:
```
sudo systemctl {start|stop} {hss|mme|spgw}
```

With each new connected component, you should see them log some startup messages, connect to the other components, and then go quiet, except for the MME, which prints out a status update every ten seconds. You can find the logfiles at
```
/var/log/colte/{hss|mme|spgw}.log
```

## WebGUI:
The WebGUI is started automatically after installation. You can start/stop it with:
```
sudo systemctl {start|stop} colte_webgui
```

## Billing Services:
The Billing component is made of two services: ntopng and a repeating cronjob that checks and updates it. The cronjob just fails quietly if ntopng is not running. You can start/stop ntopng with:
```
sudo service ntopng {start|stop}
```

# Configuration and Log Files
If you want/need to change any configuration files (for the core or other services) after installation, you can find them all in /usr/local/etc/colte. After changing any files, you will have to restart the corresponding service(s). Correspondingly, all logfiles can be found in /var/log/colte.

# Exposed Webservices and Ports
There are a bunch of different Web-based services exposed on this machine. Here's an authoritative list of the different services that are exposed, and what port they're assigned to by default.

1. phpMyAdmin: http://localhost/phpmyadmin
2. ntopng: http://localhost:3002/
3. Node Webgui: http://localhost:7999/
4. Emergency Homepage: http://localhost:9080/
5. Emergency Rocketchat: http://localhost:9081/
6. Emergency Wikipedia: http://localhost:9082/
7. Emergency Registration: http://localhost:9083/
8. Emergency OpenStreetMaps: http://localhost:9084/
9. OSM Tileserver: http://localhost:9085/
10. Local Mediaserver: http://localhost:9086/
