#!/bin/bash

sudo apt-get -y install git
git clone https://github.com/uw-ictd/colte.git /home/$USER/colte
/home/$USER/colte/system_setup/debian-9.4/setup.sh
