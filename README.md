# CoLTE
CoLTE is the Community LTE Project. Its goal is to be an all-in-one turnkey solution to setting up a small-scale locally-run community LTE network. CoLTE consists of several main elements working together:
1) An all-in-one software EPC powered by OpenAirInterface (OAI).
2) Network monitoring software (powered by ntopng) that keeps track of how many bytes each user uses and takes action at certain thresholds.
3) Billing software that lets users top up, transfer/resell credit, and buy data packages.
4) A Web GUI that lets users check the status of their account and administrators configure the network.
5) Local Web and DNS caching via BIND and Nginx.
6) Locally-hosted web services that include Rocketchat, Wikipedia, OpenStreetMaps, a media server, and more.

# Installation

## Get The Code:
Our "master" branch should always compile, but is updated frequently with feature-adds. For maximum stability I recommend cloning the most recent version of the "release" branch.

## Basic System Requirements:
Currently our only supported/tested build environment is debian-9.4 (scratch). In the past we had success with Ubuntu-17.04 (now deprecated) and Ubuntu-17.10 (not currently supported to save ourselves time). Ubuntu-18.04 support will be coming soon. We had substantial problems with Ubuntu-16.04. For now, just use Debian.

## Step 1: Configure Your Network
Your machine will need two network connections: one to the Internet (the upstream WAN) and another to the eNodeB (the downstream LAN). These can be set to the same interface, it doesn't matter. Both of these connections must be already configured with IP addresses (doesn't matter if Static or Dynamic) and must be up. Note that if the LAN interface is down, you won't be able to start the MME, and if the WAN interface is down, you won't be able to start the SPGW.

## Step 2: Configure Coltenv
Once your network configuration is correct, have a look at generate_coltenv. Most of the options should be left alone (unless you know what you're doing) but you should absolutely edit the network options to match your configuration. Set WAN_IFACE to your upstream (Internet) interface, ENB_IFACE to the downstream LAN interface, and ENB_IFACE_ADDR to it's address/subnet. Don't worry about matching COLTE_LTE_SUBNET to anything, because this subnet is created and assigned to the virtual gtp0 interface once the SPGW brings it up.

You should also have a look at the compilation options to see what features you may want to add or not. WEBGUI refers to the configuration webgui, BILLING refers to the network monitoring/billing system, and the rest are self-explanatory. This space is changing rapidly, but I'll be making a much more detailed README soon.

## Step 3: Install Everything.
Run the following commands to install CoLTE. This will take a while (30min+ on some systems with everything installed) and, at the end, you should see an Ansible success message.

```
./generate_coltenv
./system_setup/$OS/setup.sh
./system_setup/$OS/ansible.sh
```

# Running CoLTE
Right now, the best way to run everything is just in a bunch of different terminal windows. I am working on servicectl/daemonizing everything, but this effort is currently under construction.

## EPC:
The EPC has three separate components: the hss, mme, and spgw. For each component, open a new terminal window, go to $COLTE_DIR/epc, type "source oaienv", and use the run script. For example:

```
source oaienv
./scripts/run_hss
```

I recommend starting the HSS, then the MME, then the SPGW (in that order). With each new connected component, you should see some good log messages, a connection being made, and then they should go relatively quiet, except for the MME, which prints out a status update every ten seconds.

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

<!--## Emergency Webservices:
The install scripts download every website (right now it's just Rocketchat and Xowa) as Docker containers and add them to Apache, with each site using it's own VirtualHost .conf directive. Therefore, you must (1) start Apache and (2) start the corresponding Docker containers. You can do this all by running the script "$COLTE_DIR/emergency/start.sh".-->

## MediaServer:
If you enable this option you'll configure and run a locally hosted media server (UMS, UniversalMediaServer) at media.networkname. You can add all the media you want to share to $COLTE_DIR/lte_extras/media/files and start/stop the webserver with sudo systemctl {start|stop} ums. It logs to /var/log/colte/ums.log. Note that if you add media to files, you'll have to restart the server.

# System Status
Logs exist. Each service prints out a log to a file in /var/log/colte. There's not a lot of unity here, yet, but more on this to come.

# Configuration 
If you want/need to change any of the configurations after install, go to /usr/local/etc/colte. In there, you will find config files for the hss, mme, and spgw. Ignore the freeDiameter files. As more and more of the components are configurable, we will add the conf files to this directory.

# Exposed Webservices and Ports
There are a bunch of different Web-based services exposed on this machine. Here's an authoritative list of the different services that are exposed, and what port they're assigned to by default.

1. phpMyAdmin: http://localhost/phpmyadmin (NOTE: THIS WILL BE DISABLED AS SOON AS I GET TO IT)
2. ntopng: http://localhost:3002/
3. Node Webgui: http://localhost:7999/
4. Emergency Homepage: http://localhost:9080/
5. Emergency Rocketchat: http://localhost:9081/
6. Emergency Wikipedia: http://localhost:9082/
7. Emergency Registration: http://localhost:9083/
8. Emergency OpenStreetMaps: http://localhost:9084/
9. OSM Tileserver: http://localhost:9085/
10. Local Mediaserver: http://localhost:9086/





<!-- ## Install Onto A Completely Fresh Machine: -->
<!-- WARNING: This approach will completely format the target machine's hard disk. DO NOT do this unless you know what that means and you're okay with it. -->

<!-- Step 1: use unetbootin (or some other such mechanism) to create a bootable install image. Note that you need to use unetbootin because if you copy the .iso directly, it will be read-only. -->

<!-- Step 2: Copy /system_setup/$YOUR_OS/preseed to /preseed on the bootable media. -->

<!-- Step 3: Edit the bootloader to load the file "/preseed/ccm.seed". This typically is in /boot/grub/grub.cfg but could be anywhere on the system (syslinux, isolinux, etc) depending on how fickle/special unetbootin feels. -->

<!-- Step 4: Use the bootable media to install onto your target machine. This will auto-skip through almost all the configuration steps, create a user named "colte" (password: password), and copy a script to /home/colte/setup.sh. -->

<!-- Step 5: Reboot into the system and sudo run ~/setup.sh. -->

<!-- ## Install Onto A Virtual Machine Using Vagrant: -->
<!-- If you want to install colte on a virtual machine with Vagrant, you can do so by cd'ing to /system_setup/$OS and running "vagrant up epc". This uses the same scripts/processes as above, except that the preseed configuration is represented by the Vagrantfile. Once complete, use "vagrant ssh epc" to get into the VM and run /colte/system_setup/$OS/setup-$OS.sh -->

<!-- ## Install on an Existing System: -->
<!-- Installation on an existing system should work fine by running /system_setup/$OS/setup-$OS.sh. -->

<!-- If you want to install colte on an already existing/configured system, you must first install python-2.7 and ansible-2.4 or greater. Please note that installing Ansible >= 2.4 can be as straightforward as specifying the version to apt-get, or a major pain if you're on a LTS version that doesn't want to support it (a lot of releases currently only go to ansible-2.2). -->

<!-- With debian-9.4, for example, this can be accomplished by adding "deb http://ppa.launchpad.net/ansible/ansible/ubuntu trusty main" to /etc/sources.list and then sudo running "apt-get install -y --allow-unauthenticated ansible". This might eventually (will inevitably?) change, version control is frustrating, YMMV. -->

<!-- Once ansible-2.4 or greater is installed, look at $COLTE_DIR/system_setup/$OS/ansible/main_playbook.yml to edit the username and mysql_user variables to be whatever user you want to install the system for. You can also change the mysql_password variable here as well (HIGHLY RECOMMENDED) but note that if you do, you'll also need to change it in /configs/hss.conf. -->

<!-- sudo run the following command: -->

<!-- ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/system_setup/$OS/ansible/main_playbook.yml -->

<!-- # Understanding What's Installed, Where It Is, And What It Does: -->
<!-- colte can be thought of as several different and independent components packaged together under one roof. Each one of these components can be run independently of the rest of the system, or can be extracted from the system without (much) difficulty. To start, have a look at /system_setup/$OS/ansible/main_playbook.yml. After defining various global variables used throughout the installation, main_playbook.yml simply calls a list of other Ansible scripts that also reside in /system_setup/$OS/ansible/. -->

<!-- Each one of these scripts represents a different component and can be commented out without affecting other components. Not coincidentally, each one of these components corresponds to a different directory in the main colte source tree. To learn more about a specific component, read its associated README.md file. -->

