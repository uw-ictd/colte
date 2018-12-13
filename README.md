# CoLTE
CoLTE is the Community LTE Project. It is designed to be an all-in-one turnkey solution that sets up a small-scale locally-run LTE network. CoLTE consists of several main elements working together:
1) An all-in-one software EPC, powered by our [fork] (https://github.com/uw-ictd/openair-cn.git) of OpenAirInterface (OAI).
2) Network monitoring software, powered by [haulage](https://github.com/uw-ictd/haulage), to keep track of how many bytes each user uses and bill appropriately.
3) A Web GUI that lets users check the status of their account, top up, transfer/resell credit, and buy data packages.
4) Local Web and DNS serving/caching via Nginx and BIND.

# Installation
### Basic System Requirements:
Currently we support and test Debian 9 (stretch) and Ubuntu 18.04 (bionic).

### Quickstart: Debian Packages!
Starting with Release 0.9.2, we've switched over to using .deb packages for Ubuntu 18.04 (bionic) and Debian 9 (stretch). We **strongly** recommend using these packages. To add our apt repository and clone them, use the following commands:
```
echo "deb http://colte.cs.washington.edu $(lsb_release -sc) main" | sudo tee /etc/apt/sources.list.d/colte.list
sudo wget -O /etc/apt/trusted.gpg.d/colte.gpg http://colte.cs.washington.edu/keyring.gpg
sudo apt-get update
sudo apt-get -y install colte
```
The `colte` package is a meta-package consisting of `colte-epc`, `haulage`, `colte-webgui`, and `colte-conf`. `colte-epc` consists of four packages: `colte-hss`, `colte-mme`, `colte-spgw`, and `colte-db`. These packages come with a default database configuration that lets you start and play around with every component. After installation, the webgui will be automatically listening on [http://localhost:7999](http://localhost:7999); the other components can be started with `sudo {oai_hss | mme | spgw}` or `sudo systemctl start {oai_hss | mme | spgw | colte-webgui}`.

### Building Packages From Source:
The top-level Makefile allows you to build the metapackages `colte` and `colte-webservices` as well as the software packages `colte-conf` and `colte-webgui`. You can edit the source of these packages (and learn more about them) in their respective directories. Once you manually build packages, they will be in `/BUILD`, and you can install them with `sudo dpkg-deb -i package.deb`. Other packages are built via Makefiles in the other repositories mentioned above.

# Configuration
All config files can be found in `/usr/local/etc/colte/`. Global configurations (that span multiple services) can be found in `config.yml`. After you edit any of the global options, you must run `colteconf update` to reconfigure all components. `colteconf prompt` provides an interactive configuration utility to help, but you can also edit `config.yml` directly. Note that you **must** run colteconf at least once after installing CoLTE, because there is no way for us to guess some of the options (e.g. upstream and downstream interfaces).

Your machine will need two network connections: one to the Internet (the upstream WAN) and another to the eNodeB (the downstream LAN). These can actually be the same interface, it doesn't matter. Both of these connections must be already configured with IP addresses (doesn't matter if Static or Dynamic) and must be up. Note that if the LAN interface is down, you won't be able to start the MME, and if the WAN interface is down, you won't be able to start the SPGW.

Set `wan_iface` to your upstream (Internet) interface, `enb_iface` to the downstream LAN interface, and `enb_iface_addr` to the downstream interface's address/subnet. Don't worry about matching `lte_subnet` to any value in particular, because this subnet is created and assigned to the virtual gtp0 interface once the SPGW brings it up. Finally, `network_name` lets you give your network a specific name for serving DNS entries (e.g. if it's set to "seattle" then connected phones can access the webgui under "http://network.seattle".

# Running CoLTE
## EPC:
The EPC has three separate components: the hss, mme, and spgw. You should start them in that order. Once installed, you can run each component in a terminal window with the following command:
```
sudo {oai_hss|mme|spgw}
```

With each new connected component, you should see them log some startup messages, connect to the other components, and then go quiet, except for the MME, which prints out a status update every ten seconds.

We also provide systemd integration. You can start any of these operations as a service, and read the output in journalctl, with the following commands:
```
sudo systemctl start colte-{hss|mme|spgw}
sudo journalctl -f -u colte-{hss|mme|spgw} --output cat
```

## WebGUI:
The WebGUI is started automatically after installation. You can start/stop it with:
```
sudo systemctl {start|stop} colte-webgui
```
For more details, consult `/webgui/README`

# Log Files
Any service run with `systemd` will have its log in `journalctl`; all other logfiles can be found in `/var/log/colte`.

# Exposed Webservices and Ports
There are a bunch of different Web-based services exposed on this machine. Here's an authoritative list of the different services that are exposed, and what port they're assigned to by default.

1. CoLTE Admin: http://localhost:7998/
2. User Webgui: http://localhost:7999/
3. Emergency Homepage: http://localhost:9080/
4. Emergency Rocketchat: http://localhost:9081/
5. Emergency Wikipedia: http://localhost:9082/
6. Emergency Registration: http://localhost:9083/
7. Emergency OpenStreetMaps: http://localhost:9084/
8. OSM Tileserver: http://localhost:9085/
9. Local Mediaserver: http://localhost:9086/
