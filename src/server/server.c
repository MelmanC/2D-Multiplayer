#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "packet.h"

static
int error_handling(server_t *server)
{
    if (server == NULL || init_server_struct(server) == 84)
        return 84;
    if (setsockopt(server->fd, SOL_SOCKET, SO_REUSEADDR, &(int){1},
                    sizeof(int)) == -1) {
        printf("Error setting socket options\n");
        return 84;
    }
    if (bind(server->fd, (struct sockaddr *)&server->addr,
                        sizeof(server->addr)) == -1) {
        printf("Error binding socket\n");
        return 84;
    }
    return 0;
}

static
int find_or_create_client(server_t *server, struct sockaddr_in *addr, socklen_t addr_len)
{
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (server->clients[i] && memcmp(&server->clients[i]->addr, addr, sizeof(struct sockaddr_in)) == 0) {
            return i;
        }
    }
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (server->clients[i] == NULL) {
            server->clients[i] = malloc(sizeof(client_t));
            server->clients[i]->addr = *addr;
            server->clients[i]->addr_len = addr_len;
            server->clients[i]->connected = true;
            server->clients[i]->player_id = server->player_ids++;
            server->clients[i]->x = 0.0f;
            server->clients[i]->y = 10.0f;
            server->player_count++;
            printf("Nouveau client enregistré (player_id=%d, total=%d)\n", server->clients[i]->player_id, server->player_count);
            return i;
        }
    }
    return -1;
}

static
int server_loop(server_t *server) {
    struct pollfd fds[1];
    fds[0].fd = server->fd;
    fds[0].events = POLLIN;

    while (1) {
        if (poll(fds, 1, 10) < 0) {
            perror("poll");
            return 84;
        }
        if (fds[0].revents & POLLIN) {
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);
            char buffer[2048];
            ssize_t recv_len = recvfrom(server->fd, buffer, sizeof(buffer), 0,
                                        (struct sockaddr *)&client_addr, &addr_len);
            if (recv_len < (ssize_t)sizeof(packet_t)) {
                printf("Paquet trop petit reçu\n");
                continue;
            }
            int idx = find_or_create_client(server, &client_addr, addr_len);
            if (idx == -1) {
                printf("Trop de clients connectés\n");
                continue;
            }
            handle_client_data(server, idx, buffer, recv_len);
        }
    }
}

int main(void) {
    server_t *server = malloc(sizeof(server_t));
    server->clients = malloc(sizeof(client_t *) * MAX_PLAYERS);

    if (error_handling(server) == 84)
        return 84;
    printf("Serveur démarré sur le port 7777\n");
    server_loop(server);
    free(server);
    return 0;
}
