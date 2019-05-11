import fileinput
import sys

#config_file = "/usr/local/etc/colte/config.yml"
config_file = "/usr/bin/colte/roles/configure/vars/main.yml"

def replace(searchExp,replaceExp):
    for line in fileinput.input(config_file, inplace=1):
        if searchExp in line:
            line = replaceExp
        sys.stdout.write(line)

def get_data(prompt, default):
    if (default):
        prompt += ' (default \'%s\')' % default
    res = raw_input(prompt+": ")
    if (res.strip() == "" and default): #use default
        res = default
    return res
        
enb_iface = get_data('network interface that eNB connects to', 'eth0') 
enb_iface_addr = get_data('address of network interface mentioned above', '1.2.3.4/24')
wan_iface = get_data('network interface that connects to Internet', 'eth0')
lte_subnet = get_data('subnet for assigning LTE addresses', '192.168.151.0/24')
network_name = get_data('name of LTE network', 'colte')

replace("enb_iface:", "enb_iface: \"" + enb_iface + "\"\n")
replace("enb_iface_addr:", "enb_iface_addr: \"" + enb_iface_addr + "\"\n")
replace("wan_iface:", "wan_iface: \"" + wan_iface + "\"\n")
replace("lte_subnet:", "lte_subnet: \"" + lte_subnet + "\"\n")
replace("network_name:", "network_name: \"" + network_name + "\"\n")

