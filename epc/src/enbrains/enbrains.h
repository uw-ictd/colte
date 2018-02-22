// enbrains.h

#ifndef _ENBRAINS_H
#define _ENBRAINS_H

#include "assertions.h"
#include "dynamic_memory_check.h"
#include "queue.h"
#include "log.h"
#include "msc.h"
#include "conversions.h"
#include "intertask_interface.h"

#define LOCALHOST_HEX		0x0100007F

#define ENBRAINS_PORT 		49150
#define ENBRAINS_MME_PORT	49151
#define ENBRAINS_HSS_PORT	49152
#define ENBRAINS_SPGW_PORT	49153

#define EB_CODE_HELLO		0x0
#define EB_CODE_GOODBYE		0x1

typedef struct enbrains_msg {
	uint8_t code;
} eb_msg_t;

// enbrains.c
int enbrains_send_init_udp (char *address, uint16_t port_number);
int enbrains_send_udp_msg (uint8_t * buffer, uint32_t buffer_len);

// enbrains_mme.c
int enbrains_mme_init(void);

// enbrains_spgw.c
int enbrains_spgw_init(void);

#endif // _ENBRAINS_H