# CoLTE

CoLTE is the Community LTE Project. It is designed to be an all-in-one turnkey solution that sets up a small-scale locally-run cellular network. CoLTE consists of several main elements working together:

1. An all-in-one software EPC, powered by [open5gs](https://github.com/open5gs/open5gs).
2. Network monitoring software, powered by [haulage](https://github.com/uw-ictd/haulage), to keep track of how many bytes each user uses and bill appropriately.
3. A Web GUI that lets network users check the status of their account, top up, transfer/resell credit, and buy data packages.
4. A Web-based admin tool that lets administrators manage all the information above.
5. Local Web and DNS serving/caching via Nginx and BIND.
6. CLI utilities for managing configuration and user accounts.

# Installation

Here is a [tutorial](https://docs.colte.network/tutorials/epc-setup.html) for novice users that attempts a step-by-step walkthrough of a full CoLTE EPC installation, produced in collaboration with the [Seattle Community Network](https://seattlecommunitynetwork.org/).

### Basic System Requirements:

We now support Ubuntu 18.04 (bionic), Ubuntu 20.04 (focal), and Debian 10
(buster). Our primary deployments are currently on buster and focal, and we
have better test coverage for those distributions. We recommend buster for new
installs.

## Which Components Do I Need?

The CoLTE project is made up of several components, which may not all be
necessary for all deployments. We currently build the following high-level
packages supporting different accounting/billing methodologies and different
core networks.

- **colte-cn-4g** - Provides a cellular "network in the box" (NITB) and
  associated configuration scripts for managing the network.

- **colte-prepaid** - Provides a prepaid accounting and billing suite integrated
  against a local colte core network.

- **colte** (_deprecated_) - For backwards compaitibility with existing
  installs, this package installs both `colte-prepaid` and `colte-cn-4g`.
  Installing this package is the same as installing both the `colte-cn-4g` and
  `colte-prepaid` components separately. We recommend new users install their
  specifically needed components rather than using this meta-package.

- **colte-essential** (_transitive_) - Provides basic shared capabilities across
  colte packages. This should be installed automatically as a dependency via apt
  if any of the other component packages are installed.

## Apt Packages

To ease deployment, we host pre-compiled apt packages on our server for x86_64
and arm64 for all supported distributions. You will need to add our apt
repository to get colte and haulage, and you will also need to add the open5gs
repository separately. To do this, use the following commands according to your
distribution:

### Debian 10 (buster) (**Recommended**)

```shell
echo "deb [signed-by=/usr/share/keyrings/colte-archive-keyring.gpg] http://colte.cs.washington.edu $(lsb_release -sc) main" | sudo tee /etc/apt/sources.list.d/colte.list
sudo wget -O /usr/share/keyrings/colte-archive-keyring.gpg http://colte.cs.washington.edu/colte-archive-keyring.gpg
wget -qO - https://www.mongodb.org/static/pgp/server-4.2.asc | sudo apt-key add -
echo "deb http://repo.mongodb.org/apt/debian buster/mongodb-org/4.2 main" | sudo tee /etc/apt/sources.list.d/mongodb-org.list
wget -qO - https://download.opensuse.org/repositories/home:/acetcom:/open5gs:/latest/Debian_10/Release.key | sudo apt-key add -
sudo sh -c "echo 'deb http://download.opensuse.org/repositories/home:/acetcom:/open5gs:/latest/Debian_10/ ./' > /etc/apt/sources.list.d/open5gs.list"
sudo apt update
sudo apt install colte-cn-4g colte-prepaid
```

### Ubuntu 18.04 or 20.04 (bionic or focal)

```shell
echo "deb [signed-by=/usr/share/keyrings/colte-archive-keyring.gpg] http://colte.cs.washington.edu $(lsb_release -sc) main" | sudo tee /etc/apt/sources.list.d/colte.list
sudo wget -O /usr/share/keyrings/colte-archive-keyring.gpg http://colte.cs.washington.edu/colte-archive-keyring.gpg
sudo apt install software-properties-common
sudo add-apt-repository ppa:open5gs/latest
sudo apt update
sudo apt install colte-cn-4g colte-prepaid
```

After installation, the admin tool will be running and listening on [http://localhost:7998](http://localhost:7998), and the user webgui will be running and listening on [http://localhost:7999](http://localhost:7999). You can start or stop these services with `systemctl {start | stop} {colte-webgui | colte-webadmin}`, respectively.

Haulage can be started with `sudo haulage` or `sudo systemctl start haulage`, but will likely fail if not first configured for your system (see [configuration](#Configuration)). To start Open5Gs, refer to the docs [here](https://open5gs.org/open5gs/docs/).

## Working With The Source:

The top-level Makefile will compile all source and generate `.deb` packages if
you type `make`. If you want to run the webgui or webadmin from source in a
local terminal without doing a system-wide installation, `cd` into the
corresponding directory and then do the following:

```
npm install
npm start
```

# Configuration

After installation, pointers to all the various config files can be found in `/etc/colte/`. The main config file is `config.yml`. After you edit this file, run `colteconf` to reconfigure all components and restart them as necessary. Note that you **must** run colteconf at least once after installing CoLTE, because there is no way for us to know some of the options (e.g. upstream and downstream interfaces).

## Basic Configs:

Conceptually, your machine will need two network connections: one to the Internet (the upstream WAN) and another to the eNodeB (the downstream LAN) - these can actually be the same interface, it doesn't matter.

Set `wan_iface` to your upstream (Internet) interface, and `enb_iface_addr` to the downstream interface's address/subnet. Don't worry about matching `lte_subnet` to any value in particular, because this subnet is created and assigned to the virtual `ogstun` interface used by the Open5Gs pgw.

## Adding Users

Once you’ve configured the system, you will have to add user accounts. The best way to do this is to use `coltedb`. You will have to provide the user’s IMSI, phone number (can be any value you choose), static IP address, and security values (KI and OPC).

## Configuring The Phone

Once you’ve added a user, you _may_ have to configure the phone’s APN settings as well. This is easy to do: go to Settings -> Mobile Network -> Advanced -> APN Settings and add an APN. The values for name and apn should both just be “internet”, you can leave everything else alone as it is.

# Money And Accounting

This section requires your deployment have an accouting system installed (like `colte-prepaid`). See [Which Components Do I Need?](#which-components-do-i-need) for more information.

## System Architecture

Setting the `metered` variable to `true` (this is on by default) turns on three services: `haulage`, `colte-webgui`, and `colte-webadmin`. [haulage](https://github.com/uw-ictd/haulage) monitors the `ogstun` interface, draws user accounts down from a quota, and turns them off when they hit zero. Users can interact with their account (transfer money, buy data packages, etc) via the webgui - accessible by default at `http://(your IP address):7999`. Similarly, we provide a separate web-based tool for network administrators to change account balances, enable or disable specific users, topup accounts, and transfer money from one user to another. It is accessible _only_ from the EPC, at `http://(your IP address):7998`. The username is `admin` and the default password is `password`.

## Administration

To add money to a user’s account, use `coltedb topup` or the webadmin tool. By default, users start out with a zero money balance and 10MB of data-balance. To configure the data packages users can buy, as well as their price-points, edit `/etc/colte/pricing.json`.

## WebAdmin and WebGUI

Both of these services are started automatically after installation. They have detailed configurations in `/etc/colte/{webgui.env|webadmin.env}`; for more details, consult `/{webadmin|webgui}/README`. You will have to restart these services after you change any of these variables, and you can do so with:

```
sudo systemctl {start|stop} {colte-webadmin|colte-webgui}
```

# Log Files

`/var/log/colte`.

# Known Issues

- Right now, the webservices are only hosted as high-number ports. In the interest of not dominating your system we do not currently integrate with any apps that serve DNS or HTTP (e.g. `bind` or `nginx`); we plan to eventually support this. In the meantime, you are responsible for either (a) changing the app to listen on 80 or (b) plumbing a port-forwarding solution using `nginx` or something similar.
- **open5gs-pgwd conflicts with systemd-networkd:** The open5gs package has a known issue wherein open5gs-pgwd conflicts with systemd-networkd. This is already fixed in source, and will be fixed in the packages as soon as Sukchan drafts a new release of open5gs. Until then, you will see an error after running colteconf, but the following commands will fix it:

```
sudo systemctl stop open5gs-pgwd
sudo systemctl restart systemd-networkd
sudo systemctl start open5gs-pgwd
```

- **systemd-networkd sometimes does not bring up the tun IP address:**
  I have seen this issue occasionally, but have not been able to reproduce it consistently. I am not entirely sure what causes it, but have found some other discussion about a similar issue. The issue is claimed to have been fixed in systemd-241, but Ubuntu 18.04 ships with version 237. This issue pops up occasionally, but usually right after you change the `lte_subnet` variable and run `colteconf`. Sometimes, but not always, starting (or restarting) `open5gs-pgwd` and/or `systemd-networkd` will fix this issue. If not, a system reboot usually does the trick.

# What about OAI?

Prior to Fall 2019 we maintained a stable branch of the Open Air
Interface 4G-LTE packet core. We encountered numerous issues
attempting to maintain this fork, including difficulty upstreaming
stability fixes and continuous large churn in the upstream
codebase. In Fall 2019 we migrated to
[open5gs](https://github.com/open5gs/open5gs , which has a more
responsive development team and (in our opinion) better code
hygene. We now only support open5gs, and no longer provide maintenance
support for OAI-based installs. We encourage OAI LTE users to check
out open5gs and join the open5gs community!
