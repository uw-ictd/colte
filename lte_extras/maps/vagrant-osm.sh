#!/bin/bash
sudo -u colte echo $(date) >> /var/log/colte/osm_startup.log
cd /home/colte/colte/lte_extras/maps
sudo -u colte vagrant up osm >> /var/log/colte/osm_startup.log 2>&1
