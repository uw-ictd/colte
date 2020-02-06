# CoLTE
CoLTE is the Community LTE Project. It is designed to be an all-in-one turnkey solution that sets up a small-scale locally-run LTE network. CoLTE consists of several main elements working together:
1) An all-in-one software EPC, powered by [open5gs] (https://github.com/open5gs/open5gs).
2) Network monitoring software, powered by [haulage](https://github.com/uw-ictd/haulage), to keep track of how many bytes each user uses and bill appropriately.
3) A Web GUI that lets users check the status of their account, top up, transfer/resell credit, and buy data packages.
4) A Web-based admin tool that lets administrators manage all the information above.
5) Local Web and DNS serving/caching via Nginx and BIND.

# Installation
### Basic System Requirements:
We now support and test **only Ubuntu 18.04 (bionic)**.

### Apt Packages
To ease deployment, we host apt packages on our server. You will need to add our apt repository to get colte and haulage, and you will also need to add the open5gs repository separately. Use the following commands:
```
echo "deb http://colte.cs.washington.edu $(lsb_release -sc) main" | sudo tee /etc/apt/sources.list.d/colte.list
sudo wget -O /etc/apt/trusted.gpg.d/colte.gpg http://colte.cs.washington.edu/keyring.gpg
sudo apt-get -y install software-properties-common
sudo add-apt-repository ppa:open5gs/latest
sudo apt-get update
sudo apt-get -y install colte
```

These packages come with sane default configurations that should enable you to start and play around with every component. After installation, the admin tool will be running and listening on [http://localhost:7998](http://localhost:7998), and the user webgui will be running and listening on [http://localhost:7999](http://localhost:7999). You can start or stop these services with `systemctl {start | stop} {colte_webgui | colte_webadmin}`, respectively. Haulage can be started with `sudo haulage` or `sudo systemctl start haulage`. To start Open5Gs, refer to the docs [here](https://open5gs.org/open5gs/docs/).

### Working With The Source:
The top-level Makefile will simply compile all source and generate a `.deb` package if you type `make`. If you want to run the webgui or webadmin from source in a terminal, `cd` into the corresponding directory and then do the following:
```
npm install
npm start
```

# Configuration
After installation, all config files can be found in `/etc/colte/`. Global configurations (that span multiple services) can be found in `config.yml`. After you edit any of the global options, you must run `colteconf update` to reconfigure all components. Note that you **must** run colteconf at least once after installing CoLTE, because there is no way for us to know some of the options (e.g. upstream and downstream interfaces).

## Basic Network Config:
Conceptually, your machine will need two network connections: one to the Internet (the upstream WAN) and another to the eNodeB (the downstream LAN) - these can actually be the same interface, it doesn't matter. Both of these connections must be already configured with IP addresses (doesn't matter if static or dynamic) and must be up.

Set `wan_iface` to your upstream (Internet) interface, and `enb_iface_addr` to the downstream interface's address/subnet. Don't worry about matching `lte_subnet` to any value in particular, because this subnet is created and assigned to the virtual `ogstun` interface once the Open5Gs pgw brings it up.

## WebAdmin:
The WebAdmin is a tool for CoLTE network administrators to change account balances, enable or disable specific users, topup accounts, and transfer money from one user to another. It is started automatically after installation. You can start/stop it with:
```
sudo systemctl {start|stop} colte-webadmin
```
For more details, consult `/webadmin/README`

## WebGUI:
The WebGUI is a tool for CoLTE users to top up, buy data packages, and send money to other users. It is started automatically after installation. You can start/stop it with:
```
sudo systemctl {start|stop} colte-webgui
```
For more details, consult `/webgui/README`

# Log Files
`/var/log/colte`.

# Known Issues
- Right now, the webservices are only hosted as high-number ports. In the interest of not dominating your system we do not currently integrate with any apps that serve DNS or HTTP (e.g. `bind` or `nginx`); we plan to eventually support this. In the meantime, you are responsible for either (a) changing the app to listen on 80 or (b) plumbing a port-forwarding solution using `nginx` or something similar.
