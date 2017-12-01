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
# Author: laurent THOMAS, Lionel GAUTHIER
###############################################################################

cmake_minimum_required (VERSION 2.8)

#############################################
# Base directories, compatible with legacy OAI building
################################################
set (OPENAIRCN_DIR   $ENV{OPENAIRCN_DIR})
set (BUILD_TOP_DIR   ${OPENAIRCN_DIR}/build)
set (OPENAIRCN_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY})

project (OpenAirInterface)

###########################################
# macros to define options as there is numerous options in oai
################################################
macro(add_option name val helpstr)
  if(DEFINED ${name})
    set(value ${${name}})
  else(DEFINED ${name})
    set(value ${val})
  endif()
  set(${name} ${value} CACHE STRING "${helpstr}")
  add_definitions("-D${name}=${value}")
endmacro(add_option)

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

macro(add_integer_option name val helpstr)
  if(DEFINED ${name})
    set(value ${${name}})
  else(DEFINED ${name})
    set(value ${val})
  endif()
  set(${name} ${value} CACHE STRING "${helpstr}")
  add_definitions("-D${name}=${value}")
endmacro(add_integer_option)

macro(add_list1_option name val helpstr)
  if(DEFINED ${name})
    set(value ${${name}})
  else(DEFINED ${name})
    set(value ${val})
  endif()
  set(${name} ${value} CACHE STRING "${helpstr}")
  set_property(CACHE ${name} PROPERTY STRINGS ${ARGN})
  if(NOT "${value}" STREQUAL "False")
    add_definitions("-D${name}=${value}")
  endif()
endmacro(add_list1_option)

macro(add_list2_option name val helpstr)
  if(DEFINED ${name})
    set(value ${${name}})
  else(DEFINED ${name})
    set(value ${val})
  endif()
  set(${name} ${value} CACHE STRING "${helpstr}")
  set_property(CACHE ${name} PROPERTY STRINGS ${ARGN})
  if(NOT "${value}" STREQUAL "False")
    add_definitions("-D${value}=1")
  endif()
endmacro(add_list2_option)

macro(add_list_string_option name val helpstr)
  if(DEFINED ${name})
    set(value ${${name}})
  else(DEFINED ${name})
    set(value ${val})
  endif()
  set(${name} ${value} CACHE STRING "${helpstr}")
  set_property(CACHE ${name} PROPERTY STRINGS ${ARGN})
  if(NOT "${value}" STREQUAL "False")
    add_definitions("-D${name}=\"${value}\"")
  endif()
endmacro(add_list_string_option)
################################################################
# Compilation flags
################################################################
# Build type
################################################################
if (CMAKE_BUILD_TYPE STREQUAL "")
   set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()
add_list_string_option(CMAKE_BUILD_TYPE "RelWithDebInfo" "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." Debug Release RelWithDebInfo MinSizeRel)
Message("Build type is ${CMAKE_BUILD_TYPE}")
if (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  add_boolean_option(LOG_OAI True         "Thread safe logging API")
  add_boolean_option(LOG_OAI_MINIMAL True "Thread safe logging API, log only levels above NOTICE")
endif()
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_boolean_option(LOG_OAI True "Thread safe logging API")
endif()

################################################################
# Processor architecture
################################################################

Message("Architecture is ${CMAKE_SYSTEM_PROCESSOR}")
if (CMAKE_SYSTEM_PROCESSOR STREQUAL "armv7l")
  set(C_FLAGS_PROCESSOR "-gdwarf-2 -mfloat-abi=hard -mfpu=neon -lgcc -lrt")
else (CMAKE_SYSTEM_PROCESSOR STREQUAL "armv7l")
  set(C_FLAGS_PROCESSOR "-msse4.2")
endif()
#
set(CMAKE_C_FLAGS
  "${CMAKE_C_FLAGS} ${C_FLAGS_PROCESSOR} -std=gnu99 -Wall -Wstrict-prototypes -fno-strict-aliasing -rdynamic -funroll-loops -Wno-packed-bitfield-compat -fPIC ")
# add autoTOOLS definitions that were maybe used!
set(CMAKE_C_FLAGS
  "${CMAKE_C_FLAGS} -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_FCNTL_H=1 -DHAVE_ARPA_INET_H=1 -DHAVE_SYS_TIME_H=1 -DHAVE_SYS_SOCKET_H=1 -DHAVE_STRERROR=1 -DHAVE_SOCKET=1 -DHAVE_MEMSET=1 -DHAVE_GETTIMEOFDAY=1 -DHAVE_STDLIB_H=1 -DHAVE_MALLOC=1 -DHAVE_LIBSCTP"
)
# set a flag for changes in the source code
# these changes are related to hardcoded path to include .h files
add_definitions(-DCMAKER)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -fstack-protector-all -g -DMALLOC_CHECK_=3 -DDEBUG_IS_ON=1 -DTRACE_IS_ON=1 -O0")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS} -g -DMALLOC_CHECK_=3  -DDEBUG_IS_ON -O1") 

################################################################
# Git Version 
################################################################
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


add_list_string_option(PACKAGE_NAME "NotDefined" "As per attribute name")
add_definitions("-DPACKAGE_VERSION=\"Branch: ${GIT_BRANCH} Abrev. Hash: ${GIT_COMMIT_HASH} Date: ${GIT_COMMIT_DATE}\"")
add_definitions("-DPACKAGE_BUGREPORT=\"openaircn-user@lists.eurecom.fr\"")

###############################################################################
# Include CMake modules to find other library
###############################################################################

set(CMAKE_MODULE_PATH "${BUILD_TOP_DIR}/modules/")

###############################################################################
# Include Check library for unit tests
###############################################################################

include(CheckCSourceCompiles)
include(CheckCSourceRuns)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(CheckTypeSize)


###############################################################################
# Debug and build related options
###############################################################################
add_boolean_option( DAEMONIZE                       False    "Fork executable if true")

add_boolean_option( DISABLE_ITTI_DETECT_SUB_TASK_ID False    " ")
add_boolean_option( DISPLAY_LICENCE_INFO            False    "If a module has a licence banner to show")
add_boolean_option( ENABLE_ITTI                     True     "ITTI is internal messaging, should remain enabled for most targets")
add_boolean_option( ENABLE_ITTI_ANALYZER            False    "ITTI Analyzer is a GUI based on GTK that displays the ITTI messages exchanged between tasks")
add_integer_option( ITTI_TASK_STACK_SIZE            0        "pthread allocated stack size in bytes of an ITTI task, if 0, use default stack size ") 
add_integer_option( ITTI_LITE                       0        "Do not use ITTI systematically for each message exchanged between layer modules") 
add_boolean_option( MESSAGE_CHART_GENERATOR         False    "For generating sequence diagrams")
add_boolean_option( DISABLE_EXECUTE_SHELL_COMMAND   False    "disable execution of C int system(const char *command);")
# NAS LAYER OPTIONS
##########################
add_boolean_option( EPC_BUILD                       False    "BUILD MME-xGW executable")
add_boolean_option( GTPV1U_LINEAR_TEID_ALLOCATION   False    "Teid allocation id mode versus pseudo random")
# S1AP LAYER OPTIONS
##########################
add_boolean_option(S1AP_DEBUG_LIST                  False    "Traces, option to be removed soon")
# SCTP LAYER OPTIONS
##########################
add_boolean_option(SCTP_DUMP_LIST                   False    "Traces, option to be removed soon")

add_boolean_option( TRACE_HASHTABLE                 False    "Trace hashtables operations ")
add_boolean_option( LOG_OAI                         False    "Thread safe logging utility")
add_boolean_option( LOG_OAI_CLEAN_HARD              False    "Thread safe logging utility option for cleaning inner structs")
add_boolean_option( SECU_DEBUG                      False    "Traces, option to be removed soon")

add_boolean_option( NAS_FORCE_REJECT_SR             True     "TEMP, TO be disabled for community after development and test: Force the EMM NAS layer to respond to any SR by reject")
add_boolean_option( NAS_FORCE_REJECT_TAU            True     "TEMP, TO be disabled for community after development and test: Force the EMM NAS layer to respond to any TAU Request by reject")

add_boolean_option( TRACE_3GPP_SPEC                 True     "Log hits of 3GPP specifications requirements")


set (ITTI_DIR ${OPENAIRCN_DIR}/src/common/itti)
if (${ENABLE_ITTI})

  set(ITTI_FILES
    # add .h files if depend on (this one is generated)
    ${ITTI_DIR}/intertask_interface.h
    ${ITTI_DIR}/intertask_interface.c
    ${ITTI_DIR}/backtrace.c
    ${ITTI_DIR}/memory_pools.c
    ${ITTI_DIR}/signals.c
    ${ITTI_DIR}/timer.c
    )
  if (${ENABLE_ITTI_ANALYZER})
    set(ITTI_FILES
    ${ITTI_FILES}
    ${ITTI_DIR}/intertask_interface_dump.c 
    ${OPENAIRCN_BIN_DIR}/messages_xml.h )
    
    include_directories ("${OPENAIRCN_BIN_DIR}")
  endif (${ENABLE_ITTI_ANALYZER})
    
  add_library(ITTI ${ITTI_FILES})
    
  set(ITTI_LIB ITTI)
endif (${ENABLE_ITTI})


###############################################################################
# ASN.1 grammar C code generation & dependancies
###############################################################################
# A difficulty: asn1c generates C code of a un-predictable list of files
# so, generate the c from asn1c once at cmake run time
# So, if someone modify the asn.1 source file in such as way that it will create
# (so creating new asn.1 objects instead of modifying the object attributes)
# New C code source file, cmake must be re-run (instead of re-running make only)
###############################################################################

set(asn1c_call "${BUILD_TOP_DIR}/tools/generate_asn1")
set(asn1_generated_dir ${OPENAIRCN_BIN_DIR})

###############################################################################
# S1AP
# Same limitation as described in RRC: unknown generated file list
# so we generate it at cmake time
###############################################################################

add_list1_option(S1AP_VERSION R10 "S1AP Asn.1 grammar version" R8 R9 R10)

set(S1AP_DIR ${OPENAIRCN_DIR}/src/s1ap)
set(ASN1RELDIR r10.5)

set(S1AP_ASN_DIR ${S1AP_DIR}/messages/asn1/${ASN1RELDIR})
set(S1AP_ASN_FILES
  ${S1AP_ASN_DIR}/S1AP-CommonDataTypes.asn
  ${S1AP_ASN_DIR}/S1AP-Constants.asn
  ${S1AP_ASN_DIR}/S1AP-IEs.asn
  ${S1AP_ASN_DIR}/S1AP-PDU.asn
  )
set(S1AP_C_DIR ${asn1_generated_dir}/${ASN1RELDIR})

execute_process(COMMAND ${asn1c_call} ${S1AP_C_DIR} ${S1AP_ASN_FILES})
file(GLOB S1AP_source ${S1AP_C_DIR}/*.c)

set(S1AP_OAI_generated
  ${S1AP_C_DIR}/s1ap_decoder.c
  ${S1AP_C_DIR}/s1ap_encoder.c
  ${S1AP_C_DIR}/s1ap_xer_print.c
  ${S1AP_C_DIR}/s1ap_ies_defs.h
  )
file(GLOB s1ap_h ${S1AP_C_DIR}/*.h)
set(s1ap_h ${s1ap_h} )

add_custom_command (
  OUTPUT ${S1AP_OAI_generated}
  COMMAND ${asn1c_call} ${S1AP_C_DIR} ${S1AP_ASN_FILES}
  COMMAND python ${S1AP_DIR}/messages/asn1/asn1tostruct.py -f${S1AP_ASN_DIR}/S1AP-PDU-Contents.asn -o${S1AP_C_DIR}
  DEPENDS ${S1AP_ASN_FILES}
  )
add_library(S1AP_LIB
  ${S1AP_OAI_generated}
  ${S1AP_source}
  ${S1AP_DIR}/s1ap_common.c
  )

include_directories ("${S1AP_C_DIR}")
include_directories ("${S1AP_DIR}")


add_library(S1AP_EPC
  ${S1AP_C_DIR}/s1ap_ies_defs.h
  ${S1AP_DIR}/s1ap_mme_encoder.c
  ${S1AP_DIR}/s1ap_mme_decoder.c
  ${S1AP_DIR}/s1ap_mme_handlers.c
  ${S1AP_DIR}/s1ap_mme_nas_procedures.c
  ${S1AP_DIR}/s1ap_mme.c
  ${S1AP_DIR}/s1ap_mme_itti_messaging.c
  ${S1AP_DIR}/s1ap_mme_retransmission.c
  ${S1AP_DIR}/s1ap_mme_ta.c
  )



###############################################################################
# add the binary tree to the search path for include files
###############################################################################
# We will find ConfigOAI.h after generation in target directory
###############################################################################

include_directories("${OPENAIRCN_BIN_DIR}")

###############################################################################
# add directories to find all include files
# the internal rule is to use generic names such as defs.h
# but to make it uniq name as adding the relative path in the include directive
# example: #include "RRC/LITE/defs.h"
#find_path (include_dirs_all *.h ${OPENAIRCN_DIR}/SRC)
#find_path (include_dirs_all *.h PATHS /usr/include NO_CMAKE_PATH)
#include_directories("${include_dirs_all}")
###############################################################################

include_directories("${OPENAIRCN_DIR}/src/common")
include_directories("${OPENAIRCN_DIR}/src/utils")

include_directories("${OPENAIRCN_DIR}/src/common/itti")
include_directories("${OPENAIRCN_DIR}/src/nas/")
include_directories("${OPENAIRCN_DIR}/src/nas/api/mme")
include_directories("${OPENAIRCN_DIR}/src/nas/api/network")
include_directories("${OPENAIRCN_DIR}/src/nas/emm/msg")
include_directories("${OPENAIRCN_DIR}/src/nas/esm/msg")
include_directories("${OPENAIRCN_DIR}/src/nas/ies")
include_directories("${OPENAIRCN_DIR}/src/nas/util")
include_directories("${OPENAIRCN_DIR}/src/secu")
include_directories("${OPENAIRCN_DIR}/src/sctp")
include_directories("${OPENAIRCN_DIR}/src/s1ap")
include_directories("${OPENAIRCN_DIR}/src/udp")
include_directories("${OPENAIRCN_DIR}/src/gtpv1-u")
include_directories("${OPENAIRCN_DIR}/src/utils/hashtable")
include_directories("${OPENAIRCN_DIR}/src/utils/lfds/liblfds6.1.1/liblfds611/inc")
include_directories("${OPENAIRCN_DIR}/src/utils/msc")
include_directories("${OPENAIRCN_DIR}/src/utils/bstr")
include_directories("${OPENAIRCN_DIR}/src/sgw")
include_directories("${OPENAIRCN_DIR}/src/mme_app")
include_directories("${OPENAIRCN_DIR}/src/s6a")
include_directories("${OPENAIRCN_DIR}/SRC")


###############################################################################
# Utilities Library
###############################################################################
add_library(BSTR
  ${OPENAIRCN_DIR}/src/utils/bstr/bstraux.c
  ${OPENAIRCN_DIR}/src/utils/bstr/bstrlib.c
  ${OPENAIRCN_DIR}/src/utils/bstr/buniutil.c
  ${OPENAIRCN_DIR}/src/utils/bstr/utf8util.c
)
include_directories(${OPENAIRCN_DIR}/src/utils/bstr)

add_library(HASHTABLE
  ${OPENAIRCN_DIR}/src/utils/hashtable/hashtable.c
  ${OPENAIRCN_DIR}/src/utils/hashtable/obj_hashtable.c
)
include_directories(${OPENAIRCN_DIR}/src/utils/hashtable)

if (MESSAGE_CHART_GENERATOR)
  add_library(MSC  
    ${OPENAIRCN_DIR}/src/utils/msc/msc.c
  )
  set(MSC_LIB MSC)
endif()
include_directories(${OPENAIRCN_DIR}/src/utils/msc/msc)


###############################################################################
# Security library
###############################################################################

set(SECU_CN_SRC
  ${OPENAIRCN_DIR}/src/secu/kdf.c
  ${OPENAIRCN_DIR}/src/secu/rijndael.c
  ${OPENAIRCN_DIR}/src/secu/snow3g.c
  ${OPENAIRCN_DIR}/src/secu/key_nas_deriver.c
  ${OPENAIRCN_DIR}/src/secu/nas_stream_eea1.c
  ${OPENAIRCN_DIR}/src/secu/nas_stream_eia1.c
  ${OPENAIRCN_DIR}/src/secu/nas_stream_eea2.c
  ${OPENAIRCN_DIR}/src/secu/nas_stream_eia2.c
  )
add_library(SECU_CN ${SECU_CN_SRC})


###############################################################################
# Core Network Utils
###############################################################################

set(CN_UTILS_SRC
  ${OPENAIRCN_DIR}/src/utils/conversions.c
  ${OPENAIRCN_DIR}/src/utils/enum_string.c
  ${OPENAIRCN_DIR}/src/utils/mcc_mnc_itu.c
  ${OPENAIRCN_DIR}/src/utils/dynamic_memory_check.c
  ${OPENAIRCN_DIR}/src/utils/pid_file.c
  ${OPENAIRCN_DIR}/src/utils/TLVEncoder.c
  ${OPENAIRCN_DIR}/src/utils/TLVDecoder.c  
  )

if (LOG_OAI)
  set(CN_UTILS_SRC   ${CN_UTILS_SRC}   ${OPENAIRCN_DIR}/src/utils/log.c )
endif(LOG_OAI)

add_library(CN_UTILS ${CN_UTILS_SRC})

##########################
# Core network libs
##########################
set(GTPV1U_DIR ${OPENAIRCN_DIR}/src/gtpv1-u)
set (GTPV1U_SRC
  ${GTPV1U_DIR}/gtpv1u_task.c
  ${GTPV1U_DIR}/gtpv1u_teid_pool.c
  ${GTPV1U_DIR}/gtp_mod_kernel.c
)
add_library(GTPV1U ${GTPV1U_SRC})

set(GTPV2C_DIR  ${OPENAIRCN_DIR}/src/gtpv2-c/nwgtpv2c-0.11/src)
add_library(GTPV2C
  ${GTPV2C_DIR}/NwGtpv2cTrxn.c
  ${GTPV2C_DIR}/NwGtpv2cTunnel.c
  ${GTPV2C_DIR}/NwGtpv2cMsg.c
  ${GTPV2C_DIR}/NwGtpv2cMsgIeParseInfo.c
  ${GTPV2C_DIR}/NwGtpv2cMsgParser.c
  ${GTPV2C_DIR}/NwGtpv2c.c
  )
include_directories(${OPENAIRCN_DIR}/src/gtpv2-c/nwgtpv2c-0.11/include/)
include_directories(${OPENAIRCN_DIR}/src/gtpv2-c/nwgtpv2c-0.11/shared/)


add_library(SCTP_SERVER
  ${OPENAIRCN_DIR}/src/sctp/sctp_common.c
  ${OPENAIRCN_DIR}/src/sctp/sctp_itti_messaging.c
  ${OPENAIRCN_DIR}/src/sctp/sctp_primitives_server.c
  )


add_library(UDP_SERVER ${OPENAIRCN_DIR}/src/udp/udp_primitives_server.c)

set(S11_DIR ${OPENAIRCN_DIR}/src/s11)
add_library(S11_MME
  ${S11_DIR}/s11_common.c
  ${S11_DIR}/s11_ie_formatter.c
  ${S11_DIR}/s11_mme_task.c
  ${S11_DIR}/s11_mme_bearer_manager.c
  ${S11_DIR}/s11_mme_session_manager.c
)

add_library(S11_SGW
  ${S11_DIR}/s11_common.c
  ${S11_DIR}/s11_ie_formatter.c
  ${S11_DIR}/s11_sgw.c
  ${S11_DIR}/s11_sgw_session_manager.c
  ${S11_DIR}/s11_sgw_bearer_manager.c
)
include_directories(${S11_DIR})

set(S6A_DIR ${OPENAIRCN_DIR}/src/s6a)
add_library(S6A
  ${S6A_DIR}/s6a_auth_info.c
  ${S6A_DIR}/s6a_dict.c
  ${S6A_DIR}/s6a_error.c
  ${S6A_DIR}/s6a_peer.c
  ${S6A_DIR}/s6a_subscription_data.c
  ${S6A_DIR}/s6a_task.c
  ${S6A_DIR}/s6a_up_loc.c
  )

set(SGW_DIR ${OPENAIRCN_DIR}/src/sgw)
add_library (SGW
  ${SGW_DIR}/spgw_config.c
  ${SGW_DIR}/pgw_config.c
  ${SGW_DIR}/sgw_config.c
  ${SGW_DIR}/s11_causes.c
  ${SGW_DIR}/sgw_task.c
  ${SGW_DIR}/sgw_handlers.c
  ${SGW_DIR}/sgw_context_manager.c
  ${SGW_DIR}/pgw_lite_paa.c
  ${SGW_DIR}/pgw_pco.c
  )

set(MME_DIR ${OPENAIRCN_DIR}/src/mme_app)
add_library(MME_APP
  ${MME_DIR}/mme_app_capabilities.c
  ${MME_DIR}/mme_app_context.c
  ${MME_DIR}/mme_app_if_nas_transport.c
  ${MME_DIR}/mme_app_main.c
  ${MME_DIR}/mme_app_bearer.c
  ${MME_DIR}/mme_app_authentication.c
  ${MME_DIR}/mme_app_detach.c
  ${MME_DIR}/mme_app_location.c
  ${MME_DIR}/mme_app_transport.c
  ${MME_DIR}/mme_app_ue_context.c
  ${MME_DIR}/mme_app_statistics.c
  ${MME_DIR}/mme_config.c
  ${MME_DIR}/s6a_2_nas_cause.c
  )


set(NAS_SRC ${OPENAIRCN_DIR}/src/nas/)
set(libnas_api_OBJS
  ${NAS_SRC}api/network/as_message.c
  ${NAS_SRC}api/network/nas_message.c
  )
  
set(libnas_emm_msg_OBJS
  ${NAS_SRC}emm/msg/AttachAccept.c
  ${NAS_SRC}emm/msg/AttachComplete.c
  ${NAS_SRC}emm/msg/AttachReject.c
  ${NAS_SRC}emm/msg/AttachRequest.c
  ${NAS_SRC}emm/msg/AuthenticationFailure.c
  ${NAS_SRC}emm/msg/AuthenticationReject.c
  ${NAS_SRC}emm/msg/AuthenticationRequest.c
  ${NAS_SRC}emm/msg/AuthenticationResponse.c
  ${NAS_SRC}emm/msg/CsServiceNotification.c
  ${NAS_SRC}emm/msg/DetachAccept.c
  ${NAS_SRC}emm/msg/DetachRequest.c
  ${NAS_SRC}emm/msg/DownlinkNasTransport.c
  ${NAS_SRC}emm/msg/EmmInformation.c
  ${NAS_SRC}emm/msg/emm_msg.c
  ${NAS_SRC}emm/msg/EmmStatus.c
  ${NAS_SRC}emm/msg/ExtendedServiceRequest.c
  ${NAS_SRC}emm/msg/GutiReallocationCommand.c
  ${NAS_SRC}emm/msg/GutiReallocationComplete.c
  ${NAS_SRC}emm/msg/IdentityRequest.c
  ${NAS_SRC}emm/msg/IdentityResponse.c
  ${NAS_SRC}emm/msg/SecurityModeCommand.c
  ${NAS_SRC}emm/msg/SecurityModeComplete.c
  ${NAS_SRC}emm/msg/SecurityModeReject.c
  ${NAS_SRC}emm/msg/ServiceReject.c
  ${NAS_SRC}emm/msg/ServiceRequest.c
  ${NAS_SRC}emm/msg/TrackingAreaUpdateAccept.c
  ${NAS_SRC}emm/msg/TrackingAreaUpdateComplete.c
  ${NAS_SRC}emm/msg/TrackingAreaUpdateReject.c
  ${NAS_SRC}emm/msg/TrackingAreaUpdateRequest.c
  ${NAS_SRC}emm/msg/UplinkNasTransport.c
)
  
set(libnas_esm_msg_OBJS
  ${NAS_SRC}esm/msg/ActivateDedicatedEpsBearerContextAccept.c
  ${NAS_SRC}esm/msg/ActivateDedicatedEpsBearerContextReject.c
  ${NAS_SRC}esm/msg/ActivateDedicatedEpsBearerContextRequest.c
  ${NAS_SRC}esm/msg/ActivateDefaultEpsBearerContextAccept.c
  ${NAS_SRC}esm/msg/ActivateDefaultEpsBearerContextReject.c
  ${NAS_SRC}esm/msg/ActivateDefaultEpsBearerContextRequest.c
  ${NAS_SRC}esm/msg/BearerResourceAllocationReject.c
  ${NAS_SRC}esm/msg/BearerResourceAllocationRequest.c
  ${NAS_SRC}esm/msg/BearerResourceModificationReject.c
  ${NAS_SRC}esm/msg/BearerResourceModificationRequest.c
  ${NAS_SRC}esm/msg/DeactivateEpsBearerContextAccept.c
  ${NAS_SRC}esm/msg/DeactivateEpsBearerContextRequest.c
  ${NAS_SRC}esm/msg/EsmInformationRequest.c
  ${NAS_SRC}esm/msg/EsmInformationResponse.c
  ${NAS_SRC}esm/msg/esm_msg.c
  ${NAS_SRC}esm/msg/EsmStatus.c
  ${NAS_SRC}esm/msg/ModifyEpsBearerContextAccept.c
  ${NAS_SRC}esm/msg/ModifyEpsBearerContextReject.c
  ${NAS_SRC}esm/msg/ModifyEpsBearerContextRequest.c
  ${NAS_SRC}esm/msg/PdnConnectivityReject.c
  ${NAS_SRC}esm/msg/PdnConnectivityRequest.c
  ${NAS_SRC}esm/msg/PdnDisconnectReject.c
  ${NAS_SRC}esm/msg/PdnDisconnectRequest.c
)

set(libnas_ies_OBJS
  ${NAS_SRC}ies/AccessPointName.c
  ${NAS_SRC}ies/AdditionalUpdateResult.c
  ${NAS_SRC}ies/AdditionalUpdateType.c
  ${NAS_SRC}ies/ApnAggregateMaximumBitRate.c
  ${NAS_SRC}ies/AuthenticationFailureParameter.c
  ${NAS_SRC}ies/AuthenticationParameterAutn.c
  ${NAS_SRC}ies/AuthenticationParameterRand.c
  ${NAS_SRC}ies/AuthenticationResponseParameter.c
  ${NAS_SRC}ies/CipheringKeySequenceNumber.c
  ${NAS_SRC}ies/Cli.c
  ${NAS_SRC}ies/CsfbResponse.c
  ${NAS_SRC}ies/DaylightSavingTime.c
  ${NAS_SRC}ies/DetachType.c
  ${NAS_SRC}ies/DrxParameter.c
  ${NAS_SRC}ies/EmergencyNumberList.c
  ${NAS_SRC}ies/EmmCause.c
  ${NAS_SRC}ies/EpsAttachResult.c
  ${NAS_SRC}ies/EpsAttachType.c
  ${NAS_SRC}ies/EpsBearerContextStatus.c
  ${NAS_SRC}ies/EpsBearerIdentity.c
  ${NAS_SRC}ies/EpsMobileIdentity.c
  ${NAS_SRC}ies/EpsNetworkFeatureSupport.c
  ${NAS_SRC}ies/EpsQualityOfService.c
  ${NAS_SRC}ies/EpsUpdateResult.c
  ${NAS_SRC}ies/EpsUpdateType.c
  ${NAS_SRC}ies/EsmCause.c
  ${NAS_SRC}ies/EsmInformationTransferFlag.c
  ${NAS_SRC}ies/EsmMessageContainer.c
  ${NAS_SRC}ies/GprsTimer.c
  ${NAS_SRC}ies/GutiType.c
  ${NAS_SRC}ies/IdentityType2.c
  ${NAS_SRC}ies/ImeisvRequest.c
  ${NAS_SRC}ies/KsiAndSequenceNumber.c
  ${NAS_SRC}ies/LcsClientIdentity.c
  ${NAS_SRC}ies/LcsIndicator.c
  ${NAS_SRC}ies/LinkedEpsBearerIdentity.c
  ${NAS_SRC}ies/LlcServiceAccessPointIdentifier.c
  ${NAS_SRC}ies/LocationAreaIdentification.c
  ${NAS_SRC}ies/MessageType.c
  ${NAS_SRC}ies/MobileIdentity.c
  ${NAS_SRC}ies/MobileStationClassmark2.c
  ${NAS_SRC}ies/MobileStationClassmark3.c
  ${NAS_SRC}ies/MsNetworkCapability.c
  ${NAS_SRC}ies/MsNetworkFeatureSupport.c
  ${NAS_SRC}ies/NasKeySetIdentifier.c
  ${NAS_SRC}ies/NasMessageContainer.c
  ${NAS_SRC}ies/NasRequestType.c
  ${NAS_SRC}ies/NasSecurityAlgorithms.c
  ${NAS_SRC}ies/NetworkName.c
  ${NAS_SRC}ies/Nonce.c
  ${NAS_SRC}ies/PacketFlowIdentifier.c
  ${NAS_SRC}ies/PagingIdentity.c
  ${NAS_SRC}ies/PdnAddress.c
  ${NAS_SRC}ies/PdnType.c
  ${NAS_SRC}ies/PlmnList.c
  ${NAS_SRC}ies/ProcedureTransactionIdentity.c
  ${NAS_SRC}ies/ProtocolConfigurationOptions.c
  ${NAS_SRC}ies/ProtocolDiscriminator.c
  ${NAS_SRC}ies/PTmsiSignature.c
  ${NAS_SRC}ies/QualityOfService.c
  ${NAS_SRC}ies/RadioPriority.c
  ${NAS_SRC}ies/SecurityHeaderType.c
  ${NAS_SRC}ies/ServiceType.c
  ${NAS_SRC}ies/ShortMac.c
  ${NAS_SRC}ies/SsCode.c
  ${NAS_SRC}ies/SupportedCodecList.c
  ${NAS_SRC}ies/TimeZoneAndTime.c
  ${NAS_SRC}ies/TimeZone.c
  ${NAS_SRC}ies/TmsiStatus.c
  ${NAS_SRC}ies/TrackingAreaIdentity.c
  ${NAS_SRC}ies/TrackingAreaIdentityList.c
  ${NAS_SRC}ies/TrafficFlowAggregateDescription.c
  ${NAS_SRC}ies/TrafficFlowTemplate.c
  ${NAS_SRC}ies/TransactionIdentifier.c
  ${NAS_SRC}ies/UeNetworkCapability.c
  ${NAS_SRC}ies/UeRadioCapabilityInformationUpdateNeeded.c
  ${NAS_SRC}ies/UeSecurityCapability.c
  ${NAS_SRC}ies/VoiceDomainPreferenceAndUeUsageSetting.c
)

set (libnas_utils_OBJS
  ${NAS_SRC}util/nas_timer.c
)


set(libnas_mme_api_OBJS
  ${NAS_SRC}api/mme/mme_api.c
)

set(libnas_mme_emm_OBJS
  ${NAS_SRC}emm/Attach.c
  ${NAS_SRC}emm/Authentication.c
  ${NAS_SRC}emm/Detach.c
  ${NAS_SRC}emm/EmmCommon.c
  ${NAS_SRC}emm/emm_data_ctx.c
  ${NAS_SRC}emm/emm_main.c
  ${NAS_SRC}emm/EmmStatusHdl.c
  ${NAS_SRC}emm/Identification.c
  ${NAS_SRC}emm/LowerLayer.c
  ${NAS_SRC}emm/SecurityModeControl.c
  ${NAS_SRC}emm/ServiceRequestHdl.c
  ${NAS_SRC}emm/TrackingAreaUpdate.c
)
  
set(libnas_mme_emm_sap_OBJS
  ${NAS_SRC}emm/sap/emm_as.c
  ${NAS_SRC}emm/sap/emm_cn.c
  ${NAS_SRC}emm/sap/EmmCommonProcedureInitiated.c
  ${NAS_SRC}emm/sap/EmmDeregistered.c
  ${NAS_SRC}emm/sap/EmmDeregisteredInitiated.c
  ${NAS_SRC}emm/sap/emm_esm.c
  ${NAS_SRC}emm/sap/emm_fsm.c
  ${NAS_SRC}emm/sap/emm_recv.c
  ${NAS_SRC}emm/sap/emm_reg.c
  ${NAS_SRC}emm/sap/EmmRegistered.c
  ${NAS_SRC}emm/sap/emm_sap.c
  ${NAS_SRC}emm/sap/emm_send.c
  )

set (libnas_mme_esm_OBJS
  ${NAS_SRC}esm/DedicatedEpsBearerContextActivation.c
  ${NAS_SRC}esm/DefaultEpsBearerContextActivation.c
  ${NAS_SRC}esm/EpsBearerContextDeactivation.c
  ${NAS_SRC}esm/esm_data_context.c
  ${NAS_SRC}esm/esm_ebr.c
  ${NAS_SRC}esm/esm_ebr_context.c
  ${NAS_SRC}esm/esm_ip.c
  ${NAS_SRC}esm/esm_main.c
  ${NAS_SRC}esm/esm_pt.c
  ${NAS_SRC}esm/EsmStatusHdl.c
  ${NAS_SRC}esm/PdnConnectivity.c
  ${NAS_SRC}esm/PdnDisconnect.c
  )

set(libnas_mme_esm_sap_OBJS
  ${NAS_SRC}esm/sap/esm_recv.c
  ${NAS_SRC}esm/sap/esm_send.c
  ${NAS_SRC}esm/sap/esm_sap.c
  )
  
add_library(LIB_NAS_MME
  ${NAS_SRC}nas_itti_messaging.c
  ${NAS_SRC}nas_if_s1.c
  ${NAS_SRC}nas_if_s6a.c
  ${NAS_SRC}nas_network.c
  ${NAS_SRC}nas_proc.c
  ${libnas_api_OBJS}
  ${libnas_mme_api_OBJS}
  ${libnas_emm_msg_OBJS}
  ${libnas_esm_msg_OBJS}
  ${libnas_ies_OBJS}
  ${libnas_utils_OBJS}
  ${libnas_mme_emm_OBJS}
  ${libnas_mme_emm_sap_OBJS}
  ${libnas_mme_esm_OBJS}
  ${libnas_mme_esm_sap_OBJS} 
  )
  
include_directories(${NAS_SRC})
include_directories(${NAS_SRC}/api/mme)
include_directories(${NAS_SRC}/emm)
include_directories(${NAS_SRC}/emm/sap)
include_directories(${NAS_SRC}/esm)
include_directories(${NAS_SRC}/esm/sap)


# Make lfds as a own source code (even if it is a outside library)
# For better intergration with compilation flags & structure of cmake
###################################################################
set(lfds ${OPENAIRCN_DIR}/src/utils/lfds/liblfds6.1.1/liblfds611/src/)
file(GLOB lfds_queue ${lfds}/lfds611_queue/*.c)
file(GLOB lfds_ring ${lfds}/lfds611_ringbuffer/*.c)
file(GLOB lfds_slist ${lfds}/lfds611_slist/*.c)
file(GLOB lfds_stack ${lfds}/lfds611_stack/*.c)
file(GLOB lfds_freelist ${lfds}/lfds611_freelist/*.c)

include_directories(${lfds})
add_library(LFDS
  ${lfds_queue} ${lfds_ring} ${lfds_slist} ${lfds_stack} ${lfds_freelist}
  ${lfds}/lfds611_liblfds/lfds611_liblfds_abstraction_test_helpers.c
  ${lfds}/lfds611_liblfds/lfds611_liblfds_aligned_free.c
  ${lfds}/lfds611_liblfds/lfds611_liblfds_aligned_malloc.c
  ${lfds}/lfds611_abstraction/lfds611_abstraction_free.c
  ${lfds}/lfds611_abstraction/lfds611_abstraction_malloc.c
)


# System packages that are required
# We use either the cmake buildin, in ubuntu are in: /usr/share/cmake*/Modules/
# or cmake provide a generic interface to pkg-config that widely used
###################################
set(CMAKE_MODULE_PATH "${OPENAIRCN_DIR}/build/modules" "${CMAKE_MODULE_PATH}")

include(FindPkgConfig)

pkg_search_module(LIBXML2 libxml-2.0 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIRS})

pkg_search_module(OPENSSL openssl REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIRS})

pkg_search_module(CONFIG libconfig REQUIRED)
include_directories(${CONFIG_INCLUDE_DIRS})

pkg_search_module(CRYPTO libcrypto REQUIRED)
include_directories(${CRYPTO_INCLUDE_DIRS})


pkg_search_module(OPENPGM openpgm-5.1)
if(NOT ${OPENPGM_FOUND})
  message("PACKAGE openpgm-5.1 is required by binaries such as oaisim: will fail later if this target is built")
else()
  include_directories(${OPENPGM_INCLUDE_DIRS})
endif()

pkg_search_module(NETTLE nettle)
if(NOT ${NETTLE_FOUND})
  message("PACKAGE nettle not found: some targets will fail")
else()
  include_directories(${NETTLE_INCLUDE_DIRS})
endif()

pkg_search_module(NETTLE nettle)
if(NOT ${NETTLE_FOUND})
  message( FATAL_ERROR "PACKAGE nettle not found: some targets will fail. Run SCRIPTS/build_mme -i or SCRIPTS/build_hss -i  again!")
else()
  include_directories(${NETTLE_INCLUDE_DIRS})
endif()

message ("NETTLE VERSION_INSTALLED  = ${NETTLE_VERSION}")

string(REGEX REPLACE "([0-9]+).*" "\\1" NETTLE_VERSION_MAJOR ${NETTLE_VERSION})
string(REGEX REPLACE "[0-9]+\\.([0-9]+).*" "\\1" NETTLE_VERSION_MINOR ${NETTLE_VERSION})
message ("NETTLE_VERSION_MAJOR = ${NETTLE_VERSION_MAJOR}")
message ("NETTLE_VERSION_MINOR = ${NETTLE_VERSION_MINOR}")

if ("${NETTLE_VERSION_MAJOR}" STREQUAL "" OR "${NETTLE_VERSION_MINOR}" STREQUAL "")
  message( FATAL_ERROR "The nettle version not detected properly. Try to run SCRIPTS/build_mme -i or SCRIPTS/build_hss -i again" )
endif()

add_definitions("-DNETTLE_VERSION_MAJOR=${NETTLE_VERSION_MAJOR}")
add_definitions("-DNETTLE_VERSION_MINOR=${NETTLE_VERSION_MINOR}")


pkg_search_module(XPM xpm)
if(NOT ${XPM_FOUND})
  message("PACKAGE xpm not found: some targets will fail")
else()
  include_directories(${XPM_INCLUDE_DIRS})
endif()



# Hack on a test of asn1c version (already dirty)
add_definitions(-DASN1_MINIMUM_VERSION=924)

#################################
# add executables for operation
#################################

  
# MME standalone 
################################
add_executable(mme
  ${OPENAIRCN_DIR}/src/oai_mme/oai_mme_log.c
  ${OPENAIRCN_DIR}/src/oai_mme/oai_mme.c
  ${OPENAIRCN_DIR}/src/common/common_types.c
  ${OPENAIRCN_DIR}/src/common/3gpp_24.008.c
  ${OPENAIRCN_DIR}/src/nas/nas_mme_task.c
  )
if( ITTI_ANALYZER )
  add_executable(mme ${OPENAIRCN_BIN_DIR}/messages_xml.h )
endif( ITTI_ANALYZER )
target_link_libraries (mme
  -Wl,--start-group
   LIB_NAS_MME S1AP_LIB S1AP_EPC S11_MME GTPV2C SCTP_SERVER UDP_SERVER SECU_CN  S6A MME_APP LFDS ${MSC_LIB} ${ITTI_LIB} CN_UTILS HASHTABLE BSTR
  -Wl,--end-group
  pthread m sctp  rt crypt ${CRYPTO_LIBRARIES} ${OPENSSL_LIBRARIES} ${NETTLE_LIBRARIES} ${CONFIG_LIBRARIES} gnutls fdproto fdcore
  )


# spgw is S+P-GW  node implementation
################################
add_executable(spgw
  ${OPENAIRCN_DIR}/src/oai_sgw/oai_sgw.c
  ${OPENAIRCN_DIR}/src/common/3gpp_24.008.c
  )
if( ITTI_ANALYZER )
  add_executable(spgw ${OPENAIRCN_BIN_DIR}/messages_xml.h )
endif( ITTI_ANALYZER )
target_link_libraries (spgw
  -Wl,--start-group
  GTPV1U SGW S11_SGW GTPV2C UDP_SERVER LFDS ${MSC_LIB} ${ITTI_LIB} CN_UTILS HASHTABLE BSTR
  -Wl,--end-group
  pthread m rt gtpnl ${CONFIG_LIBRARIES}  
  )

# auth_request is a helper for scenario builder
################################
add_executable(auth_request
  ${OPENAIRCN_DIR}/src/test/auth_request.c
  ${OPENAIRCN_DIR}/src/common/itti/backtrace.c
  ${OPENAIRCN_DIR}/src/secu/key_nas_deriver.c
  ${OPENAIRCN_DIR}/src/secu/kdf.c
  ${OPENAIRCN_DIR}/src/utils/mcc_mnc_itu.c
  ${OPENAIRCN_DIR}/src/utils/dynamic_memory_check.c
  )
target_link_libraries (auth_request
  pthread m rt crypt ${NETTLE_LIBRARIES} gnutls fdproto fdcore
  )


IF( EPC_BUILD OR MME_BUILD )
  INCLUDE(FindFreeDiameter)
  # if standalone eNB or UE no need for FreeDiameter
  IF( FREEDIAMETER_FOUND )
    IF( NOT FREEDIAMETER_HSS_S6A_ENABLED )
      MESSAGE( SEND_ERROR "FreeDiameter is not enabled for OPENAIRHSS" )
    ENDIF( NOT FREEDIAMETER_HSS_S6A_ENABLED )
  ENDIF( FREEDIAMETER_FOUND )
ENDIF( EPC_BUILD OR MME_BUILD )


###################################"
# Add executables for tests
####################################

enable_testing()

add_subdirectory(${OPENAIRCN_DIR}/src/test/ ${CMAKE_CURRENT_BINARY_DIR}/tests/)

add_test(NAME test_imsi_convert COMMAND test_mme_app_ue_context_imsi)


# TODO
#unitary tests for Core NEtwork pieces
#################################
#foreach(myExe s1ap
#    secu_knas_encrypt_eia1
#    secu_kenb
#    aes128_ctr_encrypt
#    aes128_ctr_decrypt
#    secu_knas_encrypt_eea2
#    secu_knas secu_knas_encrypt_eea1
#    kdf
#    aes128_cmac_encrypt
#    secu_knas_encrypt_eia2)
#    
#  add_executable(test_${myExe}
#    ${OPENAIRCN_DIR}/src/test/test_util.c
#    ${OPENAIRCN_DIR}/src/test/test_${myExe}.c
#    )
#  target_link_libraries (test_${myExe}
#    -Wl,--start-group SECU_CN UTIL LFDS -Wl,--end-group m rt crypt ${CRYPTO_LIBRARIES} ${OPENSSL_LIBRARIES} ${NETTLE_LIBRARIES} ${CONFIG_LIBRARIES} fdproto fdcore
#    )
#endforeach(myExe)


##################################################
# Generated specific cases is not regular code
##################################################

#######################################################################
# itti symbolic debug print require to generate a specific include file
#######################################################################

  if( ENABLE_ITTI_ANALYZER )
  # retrieve the compiler options to send it to gccxml
    get_directory_property( DirDefs COMPILE_DEFINITIONS )
    foreach( d ${DirDefs} )
      list(APPEND itti_compiler_options "-D${d}")
    endforeach()
    get_directory_property( DirDefs INCLUDE_DIRECTORIES )
    foreach( d ${DirDefs} )
      list(APPEND itti_compiler_options "-I${d}")
    endforeach()

    set (ITTI_H ${ITTI_DIR}/intertask_interface_types.h)
    add_custom_command (
    OUTPUT ${OPENAIRCN_BIN_DIR}/messages.xml
      COMMAND gccxml ${itti_compiler_options} -fxml=${OPENAIRCN_BIN_DIR}/messages.xml ${ITTI_H}
      DEPENDS ${S1AP_OAI_generated} 
    )

    add_custom_command (
      OUTPUT ${OPENAIRCN_BIN_DIR}/messages_xml.h
      COMMAND sed -e 's/ *//'   -e 's/\"/\\\\\"/g' -e 's/^/\"/' -e 's/$$/\\\\n\"/' ${OPENAIRCN_BIN_DIR}/messages.xml  > ${OPENAIRCN_BIN_DIR}/messages_xml.h
      DEPENDS ${OPENAIRCN_BIN_DIR}/messages.xml 
    )
  endif(ENABLE_ITTI_ANALYZER)



