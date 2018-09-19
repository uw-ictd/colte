#!/bin/bash

BASEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ansible-playbook -K -v -i "localhost," -c local $BASEDIR/play.yml
