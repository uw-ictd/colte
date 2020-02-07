#!/bin/bash
cd /etc/colte/colteconf
virtualenv env
source env/bin/activate
pip install ruamel.yaml
python colteconf.py
exit