# CoLTE
CoLTE is the Community LTE Project. It is designed to be an all-in-one turnkey solution that sets up a small-scale locally-run LTE network. CoLTE consists of several main elements working together:
1) An all-in-one software EPC powered by OpenAirInterface (OAI).
2) Network monitoring software (powered by [haulage](https://github.com/uw-ictd/haulage)) that keeps track of how many bytes each user uses and takes action at certain thresholds.
3) A Web GUI that lets users check the status of their account, top up, transfer/resell credit, and buy data packages.
4) Local Web and DNS serving/caching via Nginx and BIND.
5) Locally-hosted web services that include Rocketchat, Wikipedia, OpenStreetMaps, a media server, and more.

# Installation
## Basic System Requirements:
Currently we support and test Debian 9 (stretch) and Ubuntu 18.04 (bionic).

## Quickstart: Introducing Debian Packages!
We've recently released .deb packages for Ubuntu 18.04 (bionic) and Debian 9 (stretch). To add our apt repository and clone them, use the following commands:
```
echo "deb http://colte.cs.washington.edu $(lsb_release -sc) main" | sudo tee /etc/apt/sources.list.d/colte.list
sudo wget -O /etc/apt/trusted.gpg.d/colte.gpg http://colte.cs.washington.edu/keyring.gpg
sudo apt-get update
sudo apt-get -y install colte
```
The `colte` package is a meta-package consisting of `colte-epc` and `colte-webgui`. `colte-epc` consists of four packages: `colte-hss`, `colte-mme`, `colte-spgw`, and `colte-db`. These packages come with a default database configuration that lets you start and operate every component; after installation you still will have to configure CoLTE to do what you want. After installation, the webgui will be automatically listening on [http://localhost:7999](http://localhost:7999); the other components can be started with `sudo {oai_hss | mme | spgw}` or `sudo systemctl start {oai_hss | mme | spgw}`.

## Install From Source:
Our "master" branch should always compile, but is updated frequently with small feature-adds. For maximum stability, we recommend cloning the most recently tagged release. Once cloned, you can install the basic colte package (equivalent to `apt-get colte`) by running `./system_setup/install.sh basic_install`.

# Configuration
## Step 1: Configure Your Network
Your machine will need two network connections: one to the Internet (the upstream WAN) and another to the eNodeB (the downstream LAN). These can be set to the same interface, it doesn't matter. Both of these connections must be already configured with IP addresses (doesn't matter if Static or Dynamic) and must be up. Note that if the LAN interface is down, you won't be able to start the MME, and if the WAN interface is down, you won't be able to start the SPGW.

## Step 2: Configure CoLTE
Once your network configuration is correct, you need to look at (and change) generate_coltenv. Most of the options can and should be left alone (unless you know what you're doing), but you must look at and change the Network Configuration and Compilation Options sections. Please know that (1) reading through these options in detail will dramatically simplify your life down the road, because (2) we do NOT currently support dynamically changing these configured values after they're set.

### Network Configuration
Set COLTE_WAN_IFACE to your upstream (Internet) interface, COLTE_ENB_IFACE to the downstream LAN interface, and COLTE_ENB_IFACE_ADDR to the downstream interface's address/subnet. Don't worry about editing COLTE_LTE_SUBNET or matching it to anything, because this subnet is created and assigned to the virtual gtp0 interface once the SPGW brings it up.  COLTE_NETWORK_NAME lets you give your network a specific name for serving DNS entries (e.g. if it's set to "seattle" then connected phones can access the webgui under "http://network.seattle".

### Compilation Options
Each macro under this section refers to a different CoLTE feature that you can tun on or off as you please. COLTE_EPC refers to installing the core EPC code (needed for any LTE stuff), COLTE_BILLING refers to our network management and billing software, and COLTE_WEBGUI refers to our webGUI for users to check their balance and purchase credit. Note that COLTE_EPC just fetches the most recent binary release as .deb packages; to build the most recent libraries and/or source enable COLTE_BUILD_LIBRARIES and/or COLTE_BUILD_EPC (warning: this takes a long time!)

The final five options (MEDIA, WIKI, MAP, CHAT, and EMERGENCY) all refer to various locally-hosted webservices: a local mediaserver, Wikipedia, a mapping server (powered by OpenStreetMaps), a RocketChat server, and an emergency registration service. For more information about individual web services, check the WEBSERVICES.md document.

# Running CoLTE
## EPC:
The EPC has three separate components: the hss, mme, and spgw. I recommend starting them in that order. Once installed, you can start or stop each component by typing the following command in a terminal window:

```
sudo {oai_hss|mme|spgw}
```

With each new connected component, you should see them log some startup messages, connect to the other components, and then go quiet, except for the MME, which prints out a status update every ten seconds.

We also provide systemd integration. You can start any of these operations as a service, and read the output in journalctl, with the following commands:

```
sudo systemctl start colte-{hss|mme|spgw}
sudo journalctl -f -u colte-{hss|mme|spgw}
```

## WebGUI:
The WebGUI is started automatically after installation. You can start/stop it with:

```
sudo systemctl {start|stop} colte_webgui
```

# Configuration and Log Files
If you want/need to change any configuration files (for the core or other services) after installation, you can find them all in /usr/local/etc/colte. After changing any files, you will have to restart the corresponding service(s). Correspondingly, all logfiles can be found in /var/log/colte or with journalctl.

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
