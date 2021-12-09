---
title: 'EPC "Network in a Box" Setup'
parent: Tutorials
layout: page
---

# EPC Installation and Setup

The CoLTE package can run on the 64-bit versions of Ubuntu 18.04 LTS (Bionic
Beaver), Ubuntu 20.04 (Focal Fossa), or Debian 10 (Buster). This tutorial
assumes you are using a fresh install of Ubuntu 20.04 on an x86-64-based
mini-PC.

Note: When you're installing Ubuntu You should choose the “basic install” option
that doesn’t install extra unnecessary software. In prior installs this has led
to version conflicts.

## I. LTE Architecture

![Diagram of LTE architecture including 4 main sections: User equipment (UE), eNodeB base station, Evolved Packet Core (EPC), Upstream IP networks/Internet](https://i.imgur.com/dMZQVDl.png)

CoLTE simplifies implementation and configuration of the Evolved Packet Core (EPC) elements of an LTE network using the Open5GS package. The EPC provides core software functions such as subscriber management and routing user traffic to the Internet. It connects to the radio "base station", called the eNodeB (eNB), which then talks to the User Equipment (UE)- i.e., your cell phone or access device.

For starters, the most important component to know about in the EPC is the "MME," which manages the process of the eNB and any end-user devices attaching themselves to the network (you can think of this as "signing on") so they can start sending data. In the case of users, the MME has to ask the HSS software component (essentially a user database) for credentials (shared secret keys unique to each user) to verify that a given SIM card is allowed to join the network. The MME is the software component whose output logs you should check on first for error messages if something is going wrong with the network.

(You can find more detailed documentation and diagrams of the Open5GS software architecture at the Open5GS [Quickstart](https://open5gs.org/open5gs/docs/guide/01-quickstart/) page. Their software supports both 4G and 5G, and you only need to run a subset of the software components for 4G. For your reference, as of Aug 2021 these software components are: `open5gs-mmed, open5gs-sgwcd, open5gs-sgwud, open5gs-hssd, open5gs-pcrfd, open5gs-smfd, open5gs-upfd`, and on Ubuntu they run as [systemd](http://manpages.ubuntu.com/manpages/bionic/man1/systemd.1.html) services.)

## II. CoLTE Installation

Ensure all Ubuntu packages are up-to-date:

```bash
sudo apt update && sudo apt full-upgrade
```

The CoLTE epc can be installed from our repository, or built from source. For
this tutorial we are going to install from the repository to make sure we're
getting a released version of the software and access to updates via apt.

```bash
echo "deb [signed-by=/usr/share/keyrings/colte-archive-keyring.gpg] http://colte.cs.washington.edu $(lsb_release -sc) main" | sudo tee /etc/apt/sources.list.d/colte.list
sudo wget -O /usr/share/keyrings/colte-archive-keyring.gpg http://colte.cs.washington.edu/colte-archive-keyring.gpg
sudo apt install software-properties-common
sudo add-apt-repository ppa:open5gs/latest
sudo apt update
sudo apt install colte-cn-4g
```

## III. Network Interface Configuration

### A. Recommended Configuration

For this recommended configuration, **we require an EPC
machine with 2 or more ethernet ports** (_in our case_, the ethernet interfaces corresponding to these ports are named enp1s0
and enp4s0). The ethernet port named "enp1s0" is used as the [WAN](https://en.wikipedia.org/wiki/Wide_area_network) port, which accesses upstream networks and eventually the Internet. It is physically connected via an ethernet cable to a router that can give it Internet access (e.g. our ISP's router). The one named "enp4s0" will connect to our private LTE network, and is physically connected via an ethernet cable to the eNB radio. (Our mini-PC model has 4 ethernet ports.)

To enter the appropriate values _in your case_, you will need to figure
out the names of your computer's ethernet interfaces. Use the command `ip a` on the command
line. A list of network interfaces will appear in the terminal. Find the ones
corresponding to your ethernet ports (their names usually start with “eth,”
“enp,” or “enx”).

For Ubuntu 20.04, we're currently using the Netplan program to manage our network configuration.
Create a file in the `/etc/netplan` directory (i.e. a folder) named
`99-colte-config.yaml`, and add the following lines, substituting the correct
interface names and subnets for your configuration:

```yaml
# CoLTE network configuration
network:
  ethernets:
    enp1s0: # name of interface used for upstream network
      dhcp4: yes
    enp4s0: # name of interface going to the eNB
      dhcp4: no
      addresses:
        - 192.168.150.2/24 # list all downstream networks
        - 192.168.151.2/24
  version: 2
```

Note: Netplan will apply configuration files in this directory in the numerical
order of the filename prefix (ie., 00-\*, 01-\*, etc.). Any interfaces
configured in an earlier file will be overwritten by higher-numbered
configuration files, so we create a file with the prefix 99-\* in order to
supersede all other configuration files.

**Quick explanation:**
In order to get Internet connectivity to the EPC, we configure the "upstream" or "WAN" ethernet interface (enp1s0) to request an IP address via [DHCP](https://en.wikipedia.org/wiki/Dynamic_Host_Configuration_Protocol)
from an upstream router it's connected to (as your computer usually does when you plug it into a typical home router), which passes its traffic to and from the global Internet. That's why we have the line `dhcp4: yes` under our interface name `enp1s0`. We don't need this interface to have any other IP addresses.

The "downstream" ethernet interface (enp4s0) connected to the eNB is assigned two IP addresses and
subnets, which are configured statically (_not_ by DHCP, hence the `dhcp4: no`).
In our case, we need this interface to talk to the Baicells Nova 233 eNB we use.
Our eNB has the default local (LAN) IP address of `192.168.150.1`. We also need to set its WAN address (for whatever reason this is required to be different) to `192.168.151.1`, as in [this eNB setup tutorial](https://docs.seattlecommunitynetwork.org/infrastructure/sas-setup.html). That's why we have the `addresses:` section that sets the static IP addresses of the EPC to `192.168.150.2/24` and `192.168.151.2/24`. Since these IP addresses are in the same subnet as the eNB IP addresses, they will be able to talk to each other automatically _without a router in between_ helping to route communications packets between the two addresses.

Below we also provide an alternate configuration in case you do not yet have a
machine with 2 ethernet ports or a USB to ethernet adapter dongle. However, only
the first configuration is recommended for deployments for security reasons.
**The alternative should be used for testing only**.

### B. NOT Recommended for deployment

If you don’t yet have a machine with 2 ethernet ports or a USB to ethernet
adapter dongle, you can temporarily use a machine with a single ethernet port
along with a simple switch or router. If using a simple switch, you can follow
the same instructions but connect all three of the EPC, eNB, and upstream
Internet router to the switch. If using a router, you may instead need to
configure the router to assign 2 private static IPs to each of the EPC (i.e.
`192.168.150.2`, `192.168.151.2`) and eNB (i.e. `192.168.150.1`,
`192.168.151.1`), such that it will correctly NAT upstream traffic and also
route local traffic between the EPC and eNB.

```yaml
# Network config EPC with single ethernet card
# A switch is used to connect all devices
network:
  ethernets:
    enp1s0: # name of ethernet interface
      dhcp4: true
      addresses:
        - 192.168.150.2/24 # list all downstream networks
        - 192.168.151.2/24
  version: 2
```

Once this file (or your router configuration) has been modified, restart the
network daemon to apply the configuration changes:

```bash
sudo netplan try
sudo netplan apply
```

If the eNB will be plugged into its own dedicated EPC ethernet port, as in the
recommended configuration above, you may need to connect that EPC ethernet port
to something (e.g. the eNB, a switch, another machine) via an ethernet cable to
wake the interface up (so that it becomes active and takes on the assigned IP
addresses). This is because the open5gs MME needs to "bind" (or associate) its S1 interface to one of those IP
addresses (in this case `192.168.0.2`). Until those IP addresses exist on your machine,
the MME will continually throw errors if you try to run it.

## IV. CoLTE Configuration

### A. Using `colteconf`

CoLTE simplifies LTE network configuration by consolidating relevant
configuration files into the directory `/etc/colte`. The primary configuration
file is `/etc/colte/config.yml`. Update this file as below:

```yaml
# REMEMBER TO CALL "sudo colteconf update" AFTER CHANGING THIS FILE!

# basic network settings
enb_iface_addr: 192.168.150.2 # local IP for eNB interface
wan_iface: enp1s0 # ethernet WAN (upstream) interface name
network_name: YourNetworkName
lte_subnet: 10.45.0.0/16 # End User subnet

# PLMN = first 5 digits of IMSI = MCC+MNC
mcc: 910
mnc: 54

# advanced EPC settings
dns: 8.8.8.8

# database connection settings (for Haulage + WebGui + WebAdmin)
mysql_user: haulage_db
mysql_password: haulage_db
mysql_db: haulage_db

# use these vars to turn services ON (also starts at boot) or OFF
metered: true
nat: true
epc: true
```

**Quick explanation:**
`enb_iface_addr` refers to the IP address of the ethernet interface connected to the eNB (which we set in section III above).
`wan_iface` refers to the _name_ of the WAN ethernet interface connected to the "upstream" Internet source (which we figured out in section III above).
`network_name` is a customizable name that we can set to identify your LTE network (adds some flavor!)
`lte_subnet` refers to the local/private IP addresses that the network will give to user devices internally (you don't need to worry about this).
`# PLMN` refers to the [Public Land Mobile Network](https://en.wikipedia.org/wiki/Public_land_mobile_network), in which our network has to have a unique carrier ID defined by the "mobile country code (MCC)" and "mobile network code (MNC)". We have used arbitrary unallocated numbers for now.
`dns` refers to the IP address of the [Domain Name System](https://developers.google.com/speed/public-dns) server the EPC will use, with the default value set to Google's public server at 8.8.8.8.
`# database connection settings` are internal parameters used to access the user info databases- these will break if you change them.
`metered: true` means the system will by default track the number of bits used by each user, as well as run a user management dashboard that assumes "prepaid" usage, if it is installed with the `colte-prepaid` package.

Once the file has been edited to your liking, run:

```bash
sudo colteconf update
```

This will update the configuration and reload services.

### B. Connecting the eNB to the Internet

Note that the `colteconf` tool as currently written only configures Network Address Translation ([NAT](https://en.wikipedia.org/wiki/Network_address_translation)) for what we have called the "LTE Subnet," which in our above example configuration is `10.45.0.0/16`. However, as explained above, the eNB currently has the IP addresses `192.168.150.1` and `192.168.151.1`-- _[private IP addresses](https://en.wikipedia.org/wiki/Private_network)_ that cannot be used on the public Internet. Therefore, to successfully route the eNB's network traffic to the Internet, we have to add a routing rule in the EPC computer that performs NAT, allowing packets from the eNB's subnet to exit the WAN port of the EPC _masquerading as_ coming from the EPC's IP address to the upstream network.

There might be an easier way to do this, but we've found the cleanest and most reliable way so far to be using the `iptables` command line tool. In the Terminal on the EPC, run the following command to add a NAT rule for the eNB's subnet:

```bash
sudo iptables -t nat -A POSTROUTING -s 192.168.151.0/24 -j MASQUERADE
```

**Quick explanation:** The `-t nat` option tells IPTables to install the rule in the correct "table" containing all the NAT rules, and the `-A` option means we're **A**dding the rule as opposed to **D**eleting it (`-D`). `POSTROUTING` is the "chain," or particular list of rules, that this type of NAT rule should go in (more on that [here](https://rlworkman.net/howtos/iptables/chunkyhtml/c962.html) and in this [diagram](https://upload.wikimedia.org/wikipedia/commons/3/37/Netfilter-packet-flow.svg) if you're interested). `-s 192.168.151.0/24` means that we're applying this rule to packets from the **S**ource IP addresses described by the subnet `192.168.151.0/24`. `-j MASQUERADE` means the action we'll be **J**umping to as a result of this rule is "masquerading" the source IP address as my EPC's WAN IP address.

### C. Monitoring CoLTE and Open5GS software services

Ubuntu’s built-in logging and monitoring services can be used to monitor the core network services. For example, for seeing the output logs of the MME software component we described in the first section, run the following command in the Terminal:

```bash
sudo journalctl -f -u open5gs-mmed.service
```

OR

```bash
sudo systemctl status open5gs-mmed.service
```

_Tab complete may be able to fill in the service name for systemctl at least._

Learning to read output logs is really important for managing software infrastructure! Simply Googling output messages that seem important but that you don't understand can be a good first step to figuring out how a system is working. Another interesting tool to investigate is [Wireshark](https://www.wireshark.org/), which is essentially a graphical user interface (GUI) version of the [tcpdump](https://www.tcpdump.org/) command line tool that can show you the communications [packets](https://en.wikipedia.org/wiki/Network_packet) flowing through the various network cards on your computer.

Here are some more useful commands for managing systemd services, which can be used to start, stop, and reload the software components after you've changed their configuration or they've run into errors and need to be restarted:

```bash
sudo systemctl start open5gs-mmed.service
sudo systemctl stop open5gs-mmed.service
sudo systemctl restart open5gs-mmed.service
```

## V. 'Persist' CoLTE IPTables Configuration

As mentioned above, CoLTE configures IPTables rules to make sure packets are routed correctly within
the EPC. IPTables rules must be made persistent across reboots with the
`iptables-persistent` package:

```bash
sudo apt install iptables-persistent
```

Installation of this package will save the current iptables rules to it’s
configuration file, `/etc/iptables/rules.v4`.

Note: `iptables-persistent` reads the contents of this file at boot and applies
all iptables rules it contains. If you need to update the rules, or re-apply
manually, you may use the following commands. This should not be necessary under
normal circumstances:

```bash
sudo iptables-save > /etc/iptables/rules.v4
sudo iptables-restore < /etc/iptables/rules.v4
```

## VI. User Administration and Management

### A. Command line using `coltedb`

CoLTE comes with the command `coltedb` which can be used to modify the user
database via the command line. Run `coltedb` without any arguments to see a
summary of the available commands.

To add a new user with a given SIM card, you will need several pieces of
information for each SIM card. These values should be made available to you as a
spreadsheet or text file by the SIM card manufacturer when you buy them.
**PLEASE KEEP THIS INFO SECRET!!!** This is essential for the privacy and
security of your network.

- IMSI
  - unique identifier for SIM card
  - manufacturer provides
- MSISDN
  - an arbitrary number representing the user’s “phone number”
  - could be the last 5 or more digits of the IMSI- make this up if not
    provided to you
- IP Address
  - this value sets a private static IP for each SIM card
  - you’re also free to set this
- Key
  - user’s private key used in LTE encryption
  - manufacturer provides
- OPC
  - “carrier” private key used in LTE encryption
  - manufacturer provides
- APN (_optional)_
  - access point name
  - for some CBRS LTE phone models such as the LG G8 ThinQ, the APN sent by
    the phone is hard-coded to be the string “ims”, so the only solution we’ve
    found is to set the APN on the EPC to match.

To add a single new user in the command line, use the following command format:

```bash
sudo coltedb add imsi msisdn ip key opc [apn]
```

For example, a line with some dummy values inserted could look like this (no APN):

```bash
sudo coltedb add 460660003400030 30 192.168.151.30 0x00112233445566778899AABBCCDDEEFF 0x000102030405060708090A0B0C0D0E0F
```

### B. Bulk add using a script

The shell script “bulk_add.sh” is provided for your convenience in the
[conf/](https://github.com/uw-ictd/colte/tree/main/conf) folder of the github
repo. It takes a single argument, the filename (full path if not in the same
directory) of a file (let’s say user_sims.txt) that contains the SIM card info
of multiple users, one per line.

Here’s an example of 3 lines from such a user_sims.txt file (with dummy SIM
info, and the APN set for each user):

```
460660003400032 32 192.168.151.32 0x00112233445566778899AABBCCDDEEFF 0x000102030405060708090A0B0C0D0E0F ims
460660003400033 33 192.168.151.33 0x00112233445566778899AABBCCDDEEFF 0x000102030405060708090A0B0C0D0E0F ims
460660003400034 34 192.168.151.34 0x00112233445566778899AABBCCDDEEFF 0x000102030405060708090A0B0C0D0E0F ims
```

Then, to add them all at once to the database, you would run:

```bash
sudo bulk_add.sh user_sims.txt
```
