/* 
 * udpclient.c - A simple UDP client
 * usage: udpclient <host> <port>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define BUFSIZE 1024

#define SPENCER_COMMAND_REMOVE_IMSI 0
#define SPENCER_COMMAND_ADD_IMSI 1
typedef struct spencer_msg {
  uint8_t command;
  char    imsi[16];
} spencer_msg_t;

void error(char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char **argv) {
    int sockfd;
    int n;
    int portno = 62880;
    struct sockaddr_in serveraddr;
    spencer_msg_t msg;

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    serveraddr.sin_port = htons((unsigned short)portno);

    msg.command = SPENCER_COMMAND_REMOVE_IMSI;
    strncpy(msg.imsi, "910540000000999", 15);

    /* send the message to the server */
    n = sendto(sockfd,(char *)&msg, sizeof(spencer_msg_t), 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (n < 0) 
      error("ERROR in sendto");
    
    /* print the server's reply */
    // n = recvfrom(sockfd, buf, strlen(buf), 0, &serveraddr, &serverlen);
    // if (n < 0) 
    //   error("ERROR in recvfrom");
    // printf("Echo from server: %s", buf);
    return 0;
}
