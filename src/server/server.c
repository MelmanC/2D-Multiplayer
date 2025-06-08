#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "packet.h"

static
int init_client_struct(client_t *clients, int fd)
{
    clients->fd = fd;
    clients->addr_len = sizeof(clients->addr);
    clients->fd_open = 0;
    clients->connected = true;
    return 0;
}

static
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
    init_client_struct(server->clients[idx], client_fd);
    if (idx == server->nfds)
        server->nfds++;
    server->player_count++;
    printf("Client connecté (fd=%d, total clients: %d)\n", client_fd, server->player_count);
}

static
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

static
int handle_client_data(server_t *server, int idx)
{
    packet_t header;
    
    if (receive_packet(server->fds[idx].fd, &header, sizeof(packet_t)) == -1) {
        disconnect_client(server, idx);
        return 0;
    }
    switch (header.type) {
        case PACKET_TYPE_MESSAGE: {
            packet_message_t message_packet;
            message_packet.header = header;

            receive_packet(server->fds[idx].fd, ((char *)&message_packet) + sizeof(packet_t),
                header.size - sizeof(packet_t));
            printf("Message reçu (timestamp: %u): %s\n", message_packet.timestamp, message_packet.message);
            break;
        }
        default:
            printf("Type de packet inconnu: %d\n", header.type);
            break;
    }
    
    return 0;
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

    if (error_handling(server) == 84)
        return 84;
    printf("Serveur démarré sur le port 7777\n");
    printf("En attente de connexions clients...\n");
    server_loop(server);
    free(server);
    return 0;
}
