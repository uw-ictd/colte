# colte
colte is the Community LTE Project. It consists of several main elements working together: 1) An onboard, all-in-one EPC powered by OpenAirInterface (OAI). This is mainly done.
2) Network monitor/management/billing software that supports prepaid cash transactions. This is still under construction.
3) Offline webservices to support emergency response per the Mozilla Challenge. More discussion on this later.
4) Distributed systems to do XYZ eventually, more on this later.

# Installation:

## Basic System Requirements:
Our main (best-tested) build environment is debian-9.3 (scratch) but I also have had success with Ubuntu-17.04 (now deprecated) and Ubuntu-17.10. We had problems with Ubuntu-16.04 so once 18.04 is released, I'm planning to port the code to it and then stop worrying about Ubuntu.

## Install Onto A Completely Fresh Machine:
WARNING: This approach will completely format the target machine's hard disk. DO NOT do this unless you know what that means and you're okay with it.

Step 1: use unetbootin (or some other such mechanism) to create a bootable install image. Note that you need to use unetbootin because if you copy the .iso directly, it will be read-only.

Step 2: Copy /system_setup/$YOUR_OS/preseed to /preseed on the bootable media.

Step 3: Edit the bootloader to load the file "/preseed/ccm.seed". This typically is in /boot/grub/grub.cfg but could be anywhere on the system (syslinux, isolinux, etc) depending on how fickle/special unetbootin feels.

Step 4: Use the bootable media to install onto your target machine. This will auto-skip through almost all the configuration steps, create a user named "colte" (password: password), and copy a script to /home/colte/setup.sh.

Step 5: Reboot into the system and sudo run ~/setup.sh.

## Install Onto A Virtual Machine Using Vagrant:
If you want to install colte on a virtual machine with Vagrant, you can do so by cd'ing to /system_setup/$OS and running "vagrant up epc". This uses the same scripts/processes as above, except that the preseed configuration is represented by the Vagrantfile. Once complete, use "vagrant ssh epc" to get into the VM and run /colte/system_setup/$OS/setup-$OS.sh

## Install on an Existing System:
Installation on an existing system should work fine by running /system_setup/$OS/setup-$OS.sh.

<!-- If you want to install colte on an already existing/configured system, you must first install python-2.7 and ansible-2.4 or greater. Please note that installing Ansible >= 2.4 can be as straightforward as specifying the version to apt-get, or a major pain if you're on a LTS version that doesn't want to support it (a lot of releases currently only go to ansible-2.2). -->

<!-- With debian-9.3, for example, this can be accomplished by adding "deb http://ppa.launchpad.net/ansible/ansible/ubuntu trusty main" to /etc/sources.list and then sudo running "apt-get install -y --allow-unauthenticated ansible". This might eventually (will inevitably?) change, version control is frustrating, YMMV. -->

<!-- Once ansible-2.4 or greater is installed, look at $COLTE_DIR/system_setup/$OS/ansible/main_playbook.yml to edit the username and mysql_user variables to be whatever user you want to install the system for. You can also change the mysql_password variable here as well (HIGHLY RECOMMENDED) but note that if you do, you'll also need to change it in /configs/hss.conf. -->

sudo run the following command:

ansible-playbook -K -v -i "localhost," -c local $COLTE_DIR/system_setup/$OS/ansible/main_playbook.yml

# Understanding What's Installed, Where It Is, And What It Does:
colte can be thought of as several different and independent components packaged together under one roof. Each one of these components can be run independently of the rest of the system, or can be extracted from the system without (much) difficulty. To start, have a look at /system_setup/$OS/ansible/main_playbook.yml. After defining various global variables used throughout the installation, main_playbook.yml simply calls a list of other Ansible scripts that also reside in /system_setup/$OS/ansible/.

Each one of these scripts represents a different component and can be commented out without affecting other components. Not coincidentally, each one of these components corresponds to a different directory in the main colte source tree. To learn more about a specific component, read its associated README.md file.

# Running colte
I need to write this section!