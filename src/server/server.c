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
    if (listen(server->fd, SOMAXCONN) == -1) {
        printf("Error listening\n");
        return 84;
    }
    return 0;
}

static
void accept_client(server_t *server, int client_fd)
{
    int idx = define_index(server);

    server->fds[idx].fd = client_fd;
    server->fds[idx].events = POLLIN;
    server->clients[idx] = malloc(sizeof(client_t));
    init_client_struct(server->clients[idx], client_fd, server);
    if (idx == server->nfds)
        server->nfds++;
    server->player_count++;
    server->player_ids++;
    printf("Client connecté (fd=%d, total clients: %d)\n", client_fd, server->player_count);
}

void disconnect_client(server_t *server, int idx)
{
    if (server->clients[idx] != NULL) {
        printf("Client déconnecté (fd=%d)\n", server->clients[idx]->fd);
        close(server->clients[idx]->fd);
        free(server->clients[idx]);
        server->clients[idx] = NULL;
        server->fds[idx].fd = -1;
        server->player_count--;
    }
}

int get_new_connection(server_t *server)
{
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    int client_fd = 0;

    if (server->fds[0].revents & POLLIN) {
        client_fd = accept(server->fd, (struct sockaddr *)&addr,
            &addr_len);
        if (client_fd == -1)
            return 84;
        if (server->fds == NULL || server->clients == NULL)
            return 84;
        accept_client(server, client_fd);
        send_message_packet(client_fd, "Hello from Server!");
    }
    return 0;
}

static
int server_loop(server_t *server) {
    while (1) {
        if (poll(server->fds, server->nfds, 1000) < 0) {
            perror("poll");
            return 84;
        }
        if (get_new_connection(server) == 84)
            return 84;
        for (int i = 1; i < server->nfds; i++) {
            if (server->fds[i].fd != -1 && (server->fds[i].revents & POLLIN)) {
                if (handle_client_data(server, i) == 84)
                    return 84;
            }
        }
    }
}

int main(void) {
    server_t *server = malloc(sizeof(server_t));
    server->clients = malloc(sizeof(client_t *) * MAX_PLAYERS);

    if (error_handling(server) == 84)
        return 84;
    printf("Serveur démarré sur le port 7777\n");
    printf("En attente de connexions clients...\n");
    server_loop(server);
    free(server);
    return 0;
}
