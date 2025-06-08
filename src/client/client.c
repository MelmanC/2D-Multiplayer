#include "client.h"
#include "packet.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <errno.h>

int init_client_struct(client_t *client) {
    client->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client->socket == -1) {
        perror("socket");
        return 84;
    }
    client->addr.sin_family = AF_INET;
    client->addr.sin_port = htons(PORT);
    client->addr.sin_addr.s_addr = inet_addr(IP);
    client->addr_len = sizeof(client->addr);
    return 0;
}

void server_loop(client_t *client) {
    while (1) {
        if (handle_server_packet(client->socket) == 84) {
            break;
        }
    }
}

int main(void) {
    client_t *client = malloc(sizeof(client_t));
    if (init_client_struct(client) == 84) {
        free(client);
        return 84;
    }
    printf("Connexion au serveur %s:%d...\n", IP, PORT);
    if (connect(client->socket, (struct sockaddr *)&client->addr, client->addr_len) == -1) { 
        perror("connect");
        return 84;
    }
    printf("Connecté au serveur!\n");
    send_message_packet(client->socket, "Hello from Client!");
    server_loop(client);
    close(client->socket);
    free(client);
    return 0;
}
