#pragma once

#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_PLAYERS 5

typedef struct client_s {
    struct sockaddr_in addr;
    socklen_t addr_len;
    int fd_open;
    bool connected;
    char *name;
    int player_id;
    float x, y;
    float speed;
} client_t;

typedef struct server_s {
    int fd;
    struct sockaddr_in addr;
    socklen_t addr_len;
    client_t **clients;
    int player_count;
    int port;
    int player_ids;
} server_t;

int define_index(server_t *server);

int init_server_struct(server_t *server);

int handle_client_data(server_t *server, int idx, char *buffer, unsigned long recv_len);
