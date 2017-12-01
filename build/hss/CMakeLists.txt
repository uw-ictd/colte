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
cmake_minimum_required(VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE ON)


project(HSS)


ENABLE_LANGUAGE(C)
#Sends the -std=c99 flag to the gcc compiler
#add_definitions(-std=c99)

ADD_DEFINITIONS(-D_GNU_SOURCE)

if( NOT DEFINED OPENAIRCN_DIR)
  message(SEND_ERROR "OPENAIRCN_DIR not set: ${OPENAIRCN_DIR}")
endif( NOT DEFINED OPENAIRCN_DIR)



set(OPENAIR_CMAKE_SCRIPTS ${OPENAIRCN_DIR}/build/tools)
set(OAI_HSS_DIR           ${OPENAIRCN_DIR}/src/oai_hss)
set(OPENAIR_BIN_DIR       ${OPENAIRCN_DIR}/src/oai_hss)


set(CMAKE_MODULE_PATH "${OPENAIRCN_DIR}/build/modules" "${CMAKE_MODULE_PATH}")

macro(add_boolean_option name val helpstr)
  if(DEFINED ${name})
    set(value ${${name}})
  else(DEFINED ${name})
    set(value ${val})
  endif()
  set(${name} ${value} CACHE STRING "${helpstr}")
  set_property(CACHE ${name} PROPERTY TYPE BOOL)
  if (${value})
    add_definitions("-D${name}=1")
  else (${value})
    add_definitions("-D${name}=0")
  endif (${value})
endmacro(add_boolean_option)

macro(add_option name val helpstr)
  if(DEFINED ${name})
    set(value ${${name}})
  else(DEFINED ${name})
    set(value ${val})
  endif()
  set(${name} ${value} CACHE STRING "${helpstr}")
  add_definitions("-D${name}=${value}")
endmacro(add_option)


# compilation flags
# added CMAKER conditional compilation to not bother legacy building system
#############################################
if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)

#
set(CMAKE_C_FLAGS                  "${CMAKE_C_FLAGS} -msse4.2 -std=gnu99 -Wall -Wstrict-prototypes -fno-strict-aliasing -rdynamic -funroll-loops -Wno-packed-bitfield-compat -DCMAKER")
set(CMAKE_C_FLAGS_DEBUG            "${CMAKE_C_FLAGS_DEBUG} -ggdb -DMALLOC_CHECK_=3")
set(CMAKE_C_FLAGS_RELWITHDEBINFO   "${CMAKE_C_FLAGS_RELWITHDEBINFO} -ggdb -DMALLOC_CHECK_=3 -O2")


include(FindPkgConfig)

INCLUDE(FindMySQL)
IF( NOT MySQL_FOUND )
  MESSAGE( SEND_ERROR "MySQL Client is required" )
ENDIF( NOT MySQL_FOUND )

INCLUDE(FindNettle)
IF( NOT NETTLE_FOUND )
  MESSAGE( SEND_ERROR "Nettle is required" )
ENDIF( NOT NETTLE_FOUND )



INCLUDE(FindFreeDiameter)
IF( NOT FREEDIAMETER_FOUND )
  MESSAGE( SEND_ERROR "FreeDiameter is required (use OAI installer)" )
ENDIF( NOT FREEDIAMETER_FOUND )
IF( NOT FREEDIAMETER_HSS_S6A_ENABLED )
  MESSAGE( SEND_ERROR "FreeDiameter is not enabled for OAI_HSS" )
ENDIF( NOT FREEDIAMETER_HSS_S6A_ENABLED )
add_definitions("-DFREEDIAMETER_VERSION=${FREEDIAMETER_VERSION}")


set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)

pkg_search_module(CONFIG libconfig REQUIRED)
include_directories(${CONFIG_INCLUDE_DIRS})


set(GIT_BRANCH        "UNKNOWN")
set(GIT_COMMIT_HASH   "UNKNOWN")
set(GIT_COMMIT_DATE   "UNKNOWN")


find_package(Git)
if(GIT_FOUND)
  message("git found: ${GIT_EXECUTABLE}")
  # Get the current working branch
  execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  # Get the latest abbreviated commit hash of the working branch
  execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  
  # Get the latest commit date of the working branch
  execute_process(
    COMMAND git log -1 --format=%cd
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_DATE
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
endif()


# Below is a hard-coded info
add_definitions("-DPACKAGE_VERSION=\"Branch: ${GIT_BRANCH} Abrev. Hash: ${GIT_COMMIT_HASH} Date: ${GIT_COMMIT_DATE}\"")
add_definitions("-DPACKAGE_BUGREPORT=\"openaircn-user@lists.eurecom.fr\"")


################################################################################
# ACCESS RESTRICTION LIB
################################################################################
set(ar_SRC
    ${OAI_HSS_DIR}/access_restriction/access_restriction.c
)
set(ar_HDR
    ${OAI_HSS_DIR}/access_restriction/access_restriction.h
    ${OAI_HSS_DIR}/utils/conversion.h
    ${OAI_HSS_DIR}/utils/log.h
)

add_library(hss_access_restriction ${ar_SRC} ${ar_HDR})
target_include_directories(hss_access_restriction PRIVATE ${OAI_HSS_DIR}/utils)


################################################################################
# AUC LIB
################################################################################
set(auc_SRC
    ${OAI_HSS_DIR}/auc/fx.c
    ${OAI_HSS_DIR}/auc/kdf.c
    ${OAI_HSS_DIR}/auc/random.c
    ${OAI_HSS_DIR}/auc/rijndael.c
    ${OAI_HSS_DIR}/auc/sequence_number.c
)
set(auc_HDR
    ${OAI_HSS_DIR}/auc/auc.h
    ${OAI_HSS_DIR}/utils/log.h
)

add_library(hss_auc ${auc_SRC} ${auc_HDR})


################################################################################
# DB LIB
################################################################################
set(db_SRC
    ${OAI_HSS_DIR}/db/db_connector.c
    ${OAI_HSS_DIR}/db/db_epc_equipment.c
    ${OAI_HSS_DIR}/db/db_subscription_data.c
)
set(db_HDR
    ${OAI_HSS_DIR}/db/db_proto.h
    ${OAI_HSS_DIR}/utils/hss_config.h
    ${OAI_HSS_DIR}/utils/log.h
)

add_library(hss_db ${db_SRC} ${db_HDR})
target_include_directories(hss_db PRIVATE ${OAI_HSS_DIR}/utils)


################################################################################
# S6A LIB
################################################################################
set(s6a_SRC
    ${OAI_HSS_DIR}/s6a/s6a_auth_info.c
    ${OAI_HSS_DIR}/s6a/s6a_common.c
    ${OAI_HSS_DIR}/s6a/s6a_error.c
    ${OAI_HSS_DIR}/s6a/s6a_fd.c
    ${OAI_HSS_DIR}/s6a/s6a_in_addr.c
    ${OAI_HSS_DIR}/s6a/s6a_peers.c
    ${OAI_HSS_DIR}/s6a/s6a_purge_ue.c
    ${OAI_HSS_DIR}/s6a/s6a_subscription_data.c
    ${OAI_HSS_DIR}/s6a/s6a_supported_features.c
    ${OAI_HSS_DIR}/s6a/s6a_up_loc.c
)
set(s6a_HDR
    ${OAI_HSS_DIR}/s6a/s6a_proto.h
    ${OAI_HSS_DIR}/db/db_proto.h
    ${OAI_HSS_DIR}/utils/hss_config.h
    ${OAI_HSS_DIR}/auc/auc.h
    ${OAI_HSS_DIR}/access_restriction/access_restriction.h
    ${OAI_HSS_DIR}/utils/log.h
)

add_library(hss_s6a ${s6a_SRC} ${s6a_HDR})
target_include_directories(hss_s6a PRIVATE ${OAI_HSS_DIR}/utils)
target_include_directories(hss_s6a PRIVATE ${OAI_HSS_DIR}/db)
target_include_directories(hss_s6a PRIVATE ${OAI_HSS_DIR}/auc)
target_include_directories(hss_s6a PRIVATE ${OAI_HSS_DIR}/access_restriction)
target_include_directories(hss_s6a PRIVATE ${FREEDIAMETER_INCLUDE_DIR})

################################################################################
# UTILS LIB
################################################################################

set(UTILS_GENERATED_DIR ${OPENAIR_BIN_DIR})



set(utils_SRC
    ${OAI_HSS_DIR}/utils/conversion.c
    ${OAI_HSS_DIR}/utils/hss_config.c
    ${OAI_HSS_DIR}/utils/pid_file.c
)
set(utils_HDR
    ${OAI_HSS_DIR}/utils/conversion.h
    ${OAI_HSS_DIR}/utils/hss_config.h
    ${OAI_HSS_DIR}/utils/log.h
    ${OAI_HSS_DIR}/utils/queue.h
    ${OAI_HSS_DIR}/utils/pid_file.h
)

add_library(hss_utils ${utils_SRC} ${utils_HDR})

################################################################################
# EXECUTABLE oai_hss
################################################################################
include_directories( ${UTILS_GENERATED_DIR} ${OAI_HSS_DIR}/utils ${OAI_HSS_DIR}/s6a ${OAI_HSS_DIR}/db ${OAI_HSS_DIR}/auc ${OAI_HSS_DIR}/access_restriction)
ADD_EXECUTABLE(oai_hss  ${OAI_HSS_DIR}/hss_main.c)
target_link_libraries (oai_hss 
                       -Wl,-whole-archive 
                       hss_access_restriction  
                       hss_auc
                       hss_db
                       hss_s6a 
                       hss_utils
                       -Wl,-no-whole-archive
                       gmp
                       ${MySQL_LIBRARY} 
                       ${NETTLE_LIBRARIES} 
                       ${FREEDIAMETER_LIBRARIES}
                       ${CONFIG_LIBRARIES}  
                       ${CMAKE_THREAD_LIBS_INIT} 
                       gnutls)

# Default parameters
# Does not work on simple install (fqdn in /etc/hosts 127.0.1.1)
add_boolean_option(DAEMONIZE         false          "If true, HSS execute like a daemon (fork).")  

