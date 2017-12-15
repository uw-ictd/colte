#!/bin/bash
################################################################################
# Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The OpenAirInterface Software Alliance licenses this file to You under 
# the Apache License, Version 2.0  (the "License"); you may not use this file
# except in compliance with the License.  
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#-------------------------------------------------------------------------------
# For more information about the OpenAirInterface (OAI) Software Alliance:
#      contact@openairinterface.org
################################################################################
# file build_mme
# brief
# author Lionel Gauthier
# company Eurecom
# email: lionel.gauthier@eurecom.fr
#
################################
# include helper functions
################################
THIS_SCRIPT_PATH=$(dirname $(readlink -f $0))
source $THIS_SCRIPT_PATH/../build/tools/build_helper

function help()
{
  echo_error " "
  echo_error "Usage: build_mme [OPTION]..."
  echo_error "Build the MME executable."
  echo_error " "
  echo_error "Options:"
  echo_error "Mandatory arguments to long options are mandatory for short options too."
  echo_error "  -c, --clean                               Clean the build generated files: config, object, executable files (build from scratch)"
  echo_error "  -D, --daemon                              Build MME as a daemon."
  echo_error "  -f, --force                               No interactive script for installation of software packages."
  echo_error "  -h, --help                                Print this help."
  echo_error "  -i, --check-installed-software            Check installed software packages necessary to build and run MME (support $SUPPORTED_DISTRO)."
  echo_error "  -u, --unit-tests                          Trigger unit tests."
  echo_error "  -v, --verbose                             Build process verbose."
  echo_error " "
}


NAME=enbrains
DAEMON=/usr/sbin/$NAME
DAEMON_ARGS=""
PIDFILE=/var/run/$NAME.pid

function main()
{
  local -i clean=0
  local -i daemon=0
  local -i force=0
  local -i unit_tests=0
  local -i verbose=0
  local -i var_check_install_oai_software=0
  local    cmake_args=" "
  local    make_args="-j`nproc`"


  until [ -z "$1" ]
    do
    case "$1" in
      -c | --clean)
        clean=1
        echo "Clean the build generated files (build from scratch)"
        shift;
        ;;
      -D | --daemon)
        daemon=1
        cmake_args="$cmake_args -DDAEMONIZE=1"
        echo "Build MME as a daemon"
        shift;
        ;;
      -e | --enbrains)
        cmake_args="$cmake_args -DENBRAINS=1"
        echo "Compile with ENBRAINS support"
        shift;
        ;;
      -f | --force)
        force=1
        echo "Force set (no interactive)"
        shift;
        ;;
      -h | --help)
        help
        shift;
        exit 0
        ;;
      -i | --check-installed-software)
        echo "Check installed software packages necessary to build and run EPC (support $SUPPORTED_DISTRO):"
        set_openair_env
        var_check_install_oai_software=1
        shift;
        ;;
      -u | --unit-tests)
        echo "Unit tests triggered"
        unit_tests=1
        shift;
        ;;
      -v | --verbose)
        echo "Make build process verbose"
        cmake_args="$cmake_args -DCMAKE_VERBOSE_MAKEFILE=ON"
        make_args="VERBOSE=1 $make_args"
        verbose=1
        shift;
        ;;
      *)   
        echo "Unknown option $1"
        help
        exit 1
        ;;
    esac
  done
  
  if [ $var_check_install_oai_software -gt 0 ];then
    update_package_db
    check_install_mme_software  $force
    echo "MME not compiled, to compile it, re-run build_mme without -i option"
    exit 0
  fi
  
  set_openair_env 

  if [[ $verbose -eq 1 ]]; then
    cecho "OPENAIRCN_DIR    = $OPENAIRCN_DIR" $green
  fi
  

  local dlog=$OPENAIRCN_DIR/build/log
  
  mkdir -m 777 -p $dlog
  
  if [ ! -d /usr/local/etc/oai ]; then
    $SUDO mkdir -p /usr/local/etc/oai/freeDiameter
  fi
  
  ##############################################################################
  # Clean
  ##############################################################################
  cd $OPENAIRCN_DIR/build/enbrains
  if [ $clean -ne 0 ]; then
    if [[ $verbose -eq 1 ]]; then
      echo "Cleaning MME: generated configuration files, obj files, mme executable"
    fi
    rm -Rf $OPENAIRCN_DIR/build/enbrains/build  2>&1
    mkdir -m 777 -p -v build
  fi

  
  ##############################################################################
  # Compile MME
  ##############################################################################
  cd $OPENAIRCN_DIR/build/enbrains
  if [ ! -d ./build ]; then
    mkdir -m 777 -p -v build
  fi
  cmake_file=./CMakeLists.txt
  cp $OPENAIRCN_DIR/build/enbrains/CMakeLists.template $cmake_file
  echo 'include(${CMAKE_CURRENT_SOURCE_DIR}/../CMakeLists.txt)' >> $cmake_file
  
  cd ./build
  $CMAKE $cmake_args .. > /dev/null 
  
  compilations enbrains enbrains enbrains

  # if [ $unit_tests -ne 0 ]; then
  #   make_test mme mme mme $verbose
  # fi 

  # if [ $daemon -ne 0 ]; then
    # TODO /usr/sbin ?
    # do_stop_daemon
    # $SUDO cp -upv $OPENAIRCN_DIR/build/mme/build/mme /usr/sbin/mmed && echo_success "mmed installed"
  # else
    $SUDO killall -q enbrains
    $SUDO cp -upv $OPENAIRCN_DIR/build/enbrains/build/enbrains /usr/local/bin && echo_success "enbrains installed" 
  # fi   
}


main "$@"


