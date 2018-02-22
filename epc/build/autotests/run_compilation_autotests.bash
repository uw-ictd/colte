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

# include the jUnit-like logging functions
source $OPENAIR_DIR/cmake_targets/tools/test_helper

test_compile() {
    xUnit_start
    mkdir -p $tdir/$1/build
    cd $tdir/$1/build
    {
        cmake ..
        rm -f $3
        make -j4 $2
    } > $tdir/log/$1.txt 2>&1
    if [ -s $3 ] ; then
        cp $3 $tdir/bin/`basename $3`.$1
        echo_success "$1 test compiled"
        xUnit_success "compilation" $1
    else
        echo_error "$1 test compilation failed"
        xUnit_fail "compilation" $1
    fi
}

tdir=$OPENAIR_DIR/cmake_targets/autotests
mkdir -p $tdir/bin $tdir/log

updated=$(svn st -q $OPENAIR_DIR)
if [ "$updated" != "" ] ; then
	echo_warning "some files are not in svn: $updated"
fi

cd $tdir 

test_compile \
    test.0101 oaisim \
    oaisim  $tdir/bin/oaisim.r8

test_compile \
    test.0102 oaisim \
    oaisim  $tdir/bin/oaisim.r8.nas

test_compile \
    test.0103 lte-softmodem \
    lte-softmodem  $tdir/bin/lte-softmodem.r8.rf

test_compile \
    test.0104 dlsim \
    dlsim  $tdir/bin/dlsim

test_compile \
    test.0104 ulsim \
    ulsim  $tdir/bin/ulsim

test_compile \
    test.0106 oaisim \
    oaisim  $tdir/bin/oaisim.r8.itti

test_compile \
    test.0107 oaisim \
    oaisim  $tdir/bin/oaisim.r10

test_compile \
    test.0108 oaisim \
    oaisim  $tdir/bin/oaisim.r10.itti

test_compile \
    test.0114 oaisim \
    oaisim  $tdir/bin/oaisim.r8.itti.ral

test_compile \
    test.0115 oaisim \
    oaisim  $tdir/bin/oaisim.r10.itti.ral

test_compile \
    test.0120 nasmesh \
    CMakeFiles/nasmesh/nasmesh.ko $tdir/bin/nasmesh.ko

# write the test results into a file
xUnit_write "$tdir/log/compilation_autotests.xml"
