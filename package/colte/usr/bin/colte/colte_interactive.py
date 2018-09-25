import fileinput
import sys

#config_file = "/usr/local/etc/colte/config.yml"
config_file = "/usr/bin/colte/roles/configure/vars/main.yml"

def replace(searchExp,replaceExp):
    for line in fileinput.input(config_file, inplace=1):
        if searchExp in line:
            line = replaceExp
        sys.stdout.write(line)

enb_iface = raw_input('network interface that eNB connects to (default eth0): ') 
enb_iface_addr = raw_input('address of network interface mentioned above (default 1.2.3.4/24): ')
wan_iface = raw_input('network interface that connects to Internet (default eth0): ')
lte_subnet = raw_input('subnet for assigning LTE addresses (default 192.168.151.0/24): ')
network_name = raw_input('name of LTE network (default colte): ')

replace("enb_iface:", "enb_iface: \"" + enb_iface + "\"\n")
replace("enb_iface_addr:", "enb_iface_addr: \"" + enb_iface_addr + "\"\n")
replace("wan_iface:", "wan_iface: \"" + wan_iface + "\"\n")
replace("lte_subnet:", "lte_subnet: \"" + lte_subnet + "\"\n")
replace("network_name:", "network_name: \"" + network_name + "\"\n")

