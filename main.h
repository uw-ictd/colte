#ifndef _MAIN_H
#define _MAIN_H

#define MAX_PKT_SIZE 16384

#define ENBRAINS_PORT       49150
#define ENBRAINS_MME_PORT   49151
#define ENBRAINS_HSS_PORT   49152
#define ENBRAINS_SPGW_PORT  49153

#define EB_CODE_HELLO		0x0
#define EB_CODE_GOODBYE		0x1

typedef struct enbrains_msg {
	uint8_t code;
} eb_msg_t;

#endif // _MAIN_H