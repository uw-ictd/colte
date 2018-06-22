#!/bin/bash

sudo systemctl start colte_webgui
sudo systemctl start emergencell_registry
sudo systemctl start ums

### Docker Containers
# Wikipedia
sudo docker start xowa

# OSM
sudo docker start osm_tileserver

# RocketChat
sudo docker start db
sleep 60
sudo docker start rocketchat

### Vagrant VMs
# TEMPLATE: cd /path/to/vagrant_vm1 && vagrant up
cd lte_extras/maps && vagrant up osm
### cd lte_extras/ims && vagrant up ims

