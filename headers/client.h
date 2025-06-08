#pragma once

#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 7777
#define IP "127.0.0.1"
#define BILLION 1000000

typedef struct {
    int socket;
    struct sockaddr_in addr;
    socklen_t addr_len;
} client_t;

int handle_server_packet(int socket);
