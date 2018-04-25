#!/bin/bash
################################################################################
# Licensed to the Apache Software Foundation (ASF) under one or more
#
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The ASF licenses this file to You under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with
# the License.  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
################################################################################
# <CONTACT_INFO_TO_BE_FILLED_OAI_ALLIANCE>
################################################################################

if [ -s $OPENAIR_DIR/cmake_targets/tools/build_helper ] ; then
   source $OPENAIR_DIR/cmake_targets/tools/build_helper
else
   echo "Error: no file in the file tree: is OPENAIR_DIR variable set?"
   exit 1
fi

dbin=$OPENAIR_DIR/cmake_targets/autotests/bin
dlog=$OPENAIR_DIR/cmake_targets/autotests/log

run_test() {
case=case$1; shift
cmd=$1; shift
expected=$3; shift
$cmd > $dlog/$case.txt 2>&1
if [ $expected = "true" ] ; then	 
  if $* $dlog/$case.txt; then
    echo_success "test $case, command: $cmd ok"
  else
    echo_error "test $case, command: $cmd Failed"
  fi
else 
  if $* $dlog/$case.txt; then
    echo_error "test $case, command: $cmd Failed"
  else
    echo_success "test $case, command: $cmd ok"
  fi
fi
}

run_test 0200 "$dbin/oaisim.r8 -a -A AWGN -n 100" false grep -q '(Segmentation.fault)|(Exiting)|(FATAL)'

run_test 0201 "$dbin/oaisim.r8 -a -A AWGN -n 100" false fgrep -q '[E]'

