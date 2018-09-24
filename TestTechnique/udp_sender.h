#ifndef UDP_SENDER_H_INCLUDED
#define UDP_SENDER_H_INCLUDED


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#define MAX_PORT 5
#define PORT_ARRAY_SIZE (MAX_PORT+1)
#define MAX_IP 15
#define IP_ARRAY_SIZE (MAX_IP+1)



int send_msg(char * msg, char * ip_addr, char * server_port);

#endif // UDP_SENDER_H_INCLUDED
