# dolte-epc
Standalone LTE EPC based on OAI

Instructions

1) Read this page so that you have an understanding of where I"m coming from. This walkthrough is kinda half baked, but the guy forked a branch of openairinterface5g and openair-cn and wrote some patches that allow it to compile/work with Ubuntu 17.04. He's also the only person, to my knowledge, who configured the MME/EPC on the same machine in a way that doesn't break the codebase.
https://open-cells.com/index.php/2017/08/22/all-in-one-openairinterface-august-22nd/

2) Assume that you're starting with a stock installation of Ubuntu 17.04, kernel version > 4.7, I used 4.10.0-19.

3) We are, in order, building the HSS, MME, and SPGW. Starting in this directory:

HSS:
source oaienv
./scripts/build_hss -i

Answering the prompts:
set your MySQL password and remember it. For simplicity I use the same password for all database operations here
install freeDiameter: YES
phpmyadmin: choose your preference (I use Apache)
configure database for phpmyadmin with dbconfig-common: YES

./scripts/build_mme -i

Answering the prompts:
freeDiameter: NO
asn1c rev 1516 patched: NO
libgtpnl: YES
wireshark: (whatever you want, I usually allow captures)

./scripts/build_spgw -i

libgtpnl: NO

./scripts/build_hss
./scripts/build_mme
./scripts/build_spgw

Configuration:
1) Edit ./spencer_configs/hss.conf to change "PASSWORD" to the mysql password you set earlier

2) Edit ./spencer_configs/spgw.conf field PGW_INTERFACE_NAME_FOR_SGI to the name of your outbound (internet-connected) interface.

3) Install the configurations with ./spencer_scripts/install_epc_config

4) IMPORTANT: This configuration script copies the files into /usr/local/etc/oai where they're read by the programs. It will create the directory ./spencer_configs/config_files which are pointers to these files. All subsequent configuration edits must be done to these pointers!

5) Load the sample database for the HSS:
./scripts/hss_db_import 127.0.0.1 root <PASSWORD> colte_db ./spencer_configs/sample_db.sql
