#include "server.h"
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>

int define_index(server_t *server)
{
    int idx = 1;

    for (; idx < server->nfds; idx++) {
        if (server->fds[idx].fd == -1)
            break;
    }
    return idx;
}

int init_client_struct(client_t *clients, int fd, server_t *server)
{
    clients->fd = fd;
    clients->addr_len = sizeof(clients->addr);
    clients->fd_open = 0;
    clients->connected = true;
    clients->player_id = server->player_ids;
    clients->x = 0.0f;
    clients->y = 100.0f;
    return 0;
}

int init_server_struct(server_t *server)
{
    server->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->fd == -1)
        return 84;
    server->addr.sin_family = AF_INET;
    server->addr.sin_port = htons(7777);
    server->addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server->addr_len = sizeof(server->addr);
    server->fds = malloc(sizeof(struct pollfd) * SOMAXCONN);
    server->clients = malloc(sizeof(client_t *) * SOMAXCONN);
    for (int i = 0; i < SOMAXCONN; i++) {
        server->clients[i] = NULL;
    }
    server->nfds = 1;
    server->fds[0].fd = server->fd;
    server->fds[0].events = POLLIN;
    server->player_count = 0;
    return 0;
}
