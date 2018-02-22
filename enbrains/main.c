/* For sockaddr_in */
#include <netinet/in.h>
/* For socket functions */
#include <sys/socket.h>
/* For fcntl */
#include <fcntl.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "main.h"

// SMS: GLOBAL VARS ARE NO GOOD
evutil_socket_t mme_sock, hss_sock, spgw_sock, local_sock;
int mme_connected, hss_connected, spgw_connected;
struct event_base *base;

void send_data(uint8_t *buf, size_t buflen, uint16_t port);


void mme_read_msg(evutil_socket_t listener, short event, void *arg)
{
    uint8_t *buf;
    buf = arg;
    // uint8_t buf[MAX_PKT_SIZE];

    printf("MME_READ_MSG\n");

    mme_connected = 1;
    // if not: create a new event to pass the message off to the right handler; mux based on source port
    // switch (ntohs(from.sin_port)) {
    //     case ENBRAINS_MME_PORT:
    //         event = event_new(base, -1, 0, mme_read_msg, buf);
    //         break;

    //     case ENBRAINS_HSS_PORT:
    //         event = event_new(base, -1, 0, hss_read_msg, buf);
    //         break;

    //     case ENBRAINS_SPGW_PORT:
    //         event = event_new(base, -1, 0, spgw_read_msg, buf);
    //         break;
    // }
    // (check event here for error/success?!?)
    // event_add(event, NULL);
    // event_activate(event);

    return;
}

void hss_read_msg(evutil_socket_t listener, short event, void *arg)
{
    printf("HSS_READ_MSG\n");
    hss_connected = 1;

    return;
}

void spgw_read_msg(evutil_socket_t listener, short event, void *arg)
{
    printf("SPGW_READ_MSG\n");
    spgw_connected = 1;

    return;
}


void connect_to_epc(evutil_socket_t listener, short event, void *arg)
{
    struct event *me = *(struct event **)arg;

    // SMS TODO: REPLACE THIS WITH AN ACTUAL INIT/HELLO MESSAGE
    eb_msg_t hello;
    hello.code = EB_CODE_HELLO;

	// printf("MME SOCKET CONNECT\n");

    // step 1: For each connection, ONLY send if we haven't already connected to it
    if (mme_connected == 0) {
        printf("RESENDING MESSAGE TO MME\n");
        send_data(&hello, sizeof(hello), ENBRAINS_MME_PORT);
        sleep(1);
    }

    if (hss_connected == 0) {
        printf("RESENDING MESSAGE TO HSS\n");
        send_data(&hello, sizeof(hello), ENBRAINS_HSS_PORT);
        sleep(1);
    }

    if (spgw_connected == 0) {
        printf("RESENDING MESSAGE TO SPGW\n");
        send_data(&hello, sizeof(hello), ENBRAINS_SPGW_PORT);
    }

    // step 2: If we're connected to all three components, then we can kill this timer
    if (mme_connected == 1 && hss_connected == 1 && spgw_connected == 1) {
        printf("All EPC connections made.\n");
        event_del(me);
    }

    return;
}


void received_data(evutil_socket_t listener, short event, void *arg)
{
    int size = 0;
    uint8_t buf[MAX_PKT_SIZE];
    struct sockaddr_in from;
    socklen_t fromlen = sizeof(from);
    struct event *ev = NULL;

    size = recvfrom(listener, buf, MAX_PKT_SIZE, 0, (struct sockaddr *) &from, &fromlen);

    printf("RECEIVED DATA\n");

    // first: was there a socket error
    if (size <= 0) {
        printf("ERROR\n");
        // do some error parsing/handling here
    }

    // if not: create a new event to pass the message off to the right handler; mux based on source port
    switch (ntohs(from.sin_port)) {
        case ENBRAINS_MME_PORT:
            ev = event_new(base, -1, 0, mme_read_msg, buf);
            break;

        case ENBRAINS_HSS_PORT:
            ev = event_new(base, -1, 0, hss_read_msg, buf);
            break;

        case ENBRAINS_SPGW_PORT:
            ev = event_new(base, -1, 0, spgw_read_msg, buf);
            break;
    }
    // (check event here for error/success?!?)
    event_add(ev, NULL);
    event_active(ev, 0, 0);

    return;
}

void send_data(uint8_t *buf, size_t buflen, uint16_t port) {
    int rval;
    // char hello[5] = "hello";
    struct sockaddr_in dst;
    dst.sin_family = AF_INET;
    dst.sin_addr.s_addr = 0x0100007F;
    dst.sin_port = htons(port);

    rval = sendto(local_sock, buf, buflen, 0, (struct sockaddr *)&dst, sizeof(dst));
    // printf("sending data: rval %d\n", rval);
    if (rval < 0) {
        fprintf(stderr, "send_data: sendto() failed: %s\n", strerror(errno));
    }


    return;
}

void
run(void)
{
    struct sockaddr_in mme_addr, hss_addr, spgw_addr, local_addr;
    struct event *listener_event = NULL;
    struct event *mme_connect_event = NULL;
	struct timeval five_sec = { 5, 0 };

    base = event_base_new();
    if (!base) {
        return; /*XXXerr*/
    }

    // step 1: (read config files or something)

    // step 2: these sockets are trying to connect to the local epc-dolte instance
    // (different, hard-coded ports for MME, HSS, and SPGW)
    // NOTE: enbrains should be run last. If we can't get a local connection we keep re-trying every 5 seconds or so.

    mme_addr.sin_family = AF_INET;
    mme_addr.sin_addr.s_addr = 0x0100007F;
    mme_addr.sin_port = htons(ENBRAINS_MME_PORT);

    mme_sock = socket(AF_INET, SOCK_DGRAM, 0);
    evutil_make_socket_nonblocking(mme_sock);
	// bind???
    connect(mme_sock, (struct sockaddr *)&mme_addr, sizeof(mme_addr));
    mme_connected = 0;

    int one = 1;
    setsockopt(mme_sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));



    local_sock = socket(AF_INET, SOCK_DGRAM, 0);
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = 0x0100007F;
    local_addr.sin_port = htons(ENBRAINS_PORT);
    if (bind(local_sock, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
        fprintf(stderr, "bind() failed: %s\n", strerror(errno));
    }
    setsockopt(local_sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));



    // if (bind(mme_sock, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
    //     perror("bind");
    //     return;
    // }

    // if (listen(listener, 16)<0) {
    //     perror("listen");
    //     return;
    // }

    mme_connect_event = event_new(base, -1, EV_TIMEOUT|EV_PERSIST, connect_to_epc, &mme_connect_event);
    event_add(mme_connect_event, &five_sec);

    listener_event = event_new(base, local_sock, EV_READ|EV_PERSIST, received_data, NULL);
    /*XXX check it */
    event_add(listener_event, NULL);

    event_base_dispatch(base);

    // fire off a "connect request" now that we're listening for responses
//    send_mme_connect_msg(mme_sock);
    printf("GOT TO END OF MAIN?!?\n");
}

int
main(int c, char **v)
{
    setvbuf(stdout, NULL, _IONBF, 0);

    run();
    return 0;
}




//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
///////// SAMPLE CODE I MAY-OR-MAY-NOT UTILIZE HERE //////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

void
readcb(struct bufferevent *bev, void *ctx)
{
    struct evbuffer *input, *output;
    char *line;
    size_t n;
    int i;
    input = bufferevent_get_input(bev);
    output = bufferevent_get_output(bev);

    while ((line = evbuffer_readln(input, &n, EVBUFFER_EOL_LF))) {
        // for (i = 0; i < n; ++i)
        //     line[i] = rot13_char(line[i]);
        evbuffer_add(output, line, n);
        evbuffer_add(output, "\n", 1);
        free(line);
    }

    if (evbuffer_get_length(input) >= MAX_PKT_SIZE) {
        /* Too long; just process what there is and go on so that the buffer
         * doesn't grow infinitely long. */
        char buf[1024];
        while (evbuffer_get_length(input)) {
            int n = evbuffer_remove(input, buf, sizeof(buf));
            // for (i = 0; i < n; ++i)
            //     buf[i] = rot13_char(buf[i]);
            evbuffer_add(output, buf, n);
        }
        evbuffer_add(output, "\n", 1);
    }
}

void
errorcb(struct bufferevent *bev, short error, void *ctx)
{
    if (error & BEV_EVENT_EOF) {
        /* connection has been closed, do any clean up here */
        /* ... */
    } else if (error & BEV_EVENT_ERROR) {
        /* check errno to see what error occurred */
        /* ... */
    } else if (error & BEV_EVENT_TIMEOUT) {
        /* must be a timeout event handle, handle it */
        /* ... */
    }
    bufferevent_free(bev);
}
