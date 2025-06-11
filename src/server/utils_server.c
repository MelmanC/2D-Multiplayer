#include "server.h"
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>

int init_server_struct(server_t *server)
{
    server->fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server->fd == -1)
        return 84;
    server->addr.sin_family = AF_INET;
    server->addr.sin_port = htons(7777);
    server->addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server->addr_len = sizeof(server->addr);
    server->clients = malloc(sizeof(client_t *) * SOMAXCONN);
    for (int i = 0; i < MAX_PLAYERS; i++) {
        server->clients[i] = NULL;
    }
    server->player_count = 0;
    return 0;
}
