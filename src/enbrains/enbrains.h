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

int enbrains_init(void);

#define LOCALHOST_HEX		0x0100007F
#define ENBRAINS_PORT 		49150
#define ENBRAINS_MME_PORT	49151
#define ENBRAINS_HSS_PORT	49152
#define ENBRAINS_SPGW_PORT	49153

// enbrains.c
int enbrains_send_init_udp (char *address, uint16_t port_number);

#endif // _ENBRAINS_H