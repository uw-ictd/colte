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
AM_CFLAGS = @ADD_CFLAGS@ \
	@CHECK_CFLAGS@	\
	-I$(top_srcdir)/common	\
	-I$(top_srcdir)/gtpv1-u			\
	-I$(top_srcdir)/gtpv1-u/nw-gtpv1u/include	\
	-I$(top_srcdir)/gtpv1-u/nw-gtpv1u/shared	\
	-I$(top_srcdir)/sctp			\
	-I$(top_srcdir)/INTERTASK_INTERFACE	\
	-I$(top_srcdir)/IPV4			\
	-I$(top_builddir)/s1ap/messages 	\
	-I$(top_builddir)/s1ap			\
	-I$(top_srcdir)/s1ap			\
	-I$(top_srcdir)/secu			\
	-I$(top_srcdir)/s6a			\
	-I$(top_srcdir)/sgw-LITE		\
	-I$(top_srcdir)/udp			\
	-I$(top_srcdir)/utils			\
	-I$(top_srcdir)/utils/TIMER		\
	-I$(top_srcdir)/utils/HASTABLE

if HAVE_CHECK
TESTS =	\
	test_kdf	\
	test_aes128_cmac_encrypt	\
	test_aes128_ctr_encrypt	\
	test_aes128_ctr_decrypt	\
	test_secu_kenb	\
	test_secu_knas	\
    test_secu_knas_encrypt_eea1 \
    test_secu_knas_encrypt_eia1 \
    test_secu_knas_encrypt_eea2 \
    test_secu_knas_encrypt_eia2
else
TESTS =
endif

libtest_util_la_SOURCES = \
	test_util.c	test_util.h

noinst_LTLIBRARIES = libtest_util.la 

common_ldadd = 	$(top_builddir)/secu/libsecu.la	\
	libtest_util.la	\
	$(top_builddir)/utils/libutils.la	\
	@CHECK_LIBS@

check_PROGRAMS =	\
	test_kdf	\
	test_aes128_cmac_encrypt	\
	test_aes128_ctr_encrypt	\
	test_aes128_ctr_decrypt	\
	test_secu_kenb	\
	test_secu_knas	\
    test_secu_knas_encrypt_eea1 \
    test_secu_knas_encrypt_eia1 \
    test_secu_knas_encrypt_eea2 \
    test_secu_knas_encrypt_eia2

test_kdf_LDADD =	\
	$(common_ldadd)

test_secu_knas_LDADD =	\
	$(common_ldadd)

test_secu_kenb_LDADD =	\
	$(common_ldadd)

test_secu_knas_encrypt_eea1_LDADD = \
    $(common_ldadd)

test_secu_knas_encrypt_eia1_LDADD = \
    $(common_ldadd)

test_secu_knas_encrypt_eea2_LDADD = \
    $(common_ldadd)

test_secu_knas_encrypt_eia2_LDADD = \
    $(common_ldadd)

test_aes128_ctr_encrypt_LDADD =	\
	$(common_ldadd)

test_aes128_ctr_decrypt_LDADD =	\
	$(common_ldadd)

test_aes128_cmac_encrypt_LDADD =	\
	$(common_ldadd)

test_s1ap_LDADD = libtest_util.la	\
 	$(top_builddir)/s1ap/libs1ap.la
# 
 test_s1ap_SOURCES = test_s1ap.c	\
 	$(top_srcdir)/s1ap/s1ap_eNB_decoder.c \
 	$(top_srcdir)/s1ap/s1ap_eNB_decoder.h \
 	$(top_srcdir)/s1ap/s1ap_eNB_encoder.c \
 	$(top_srcdir)/s1ap/s1ap_eNB_encoder.h
 
 test_secu_kenb_SOURCES = test_secu_kenb.c
 test_secu_knas_SOURCES = test_secu_knas.c
