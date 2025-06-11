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
    client->players = malloc(sizeof(player_t *) * MAX_PLAYERS);
    client->thread = 0;
    client->running = 1;
    client->id = 843;
    return 0;
}

void create_player(client_t *client, uint32_t id, packet_new_player_t *new_player_packet) {
    player_t *player = malloc(sizeof(player_t));
    if (player == NULL) {
        perror("malloc");
        return;
    }
    player->id = id;
    player->x = new_player_packet->x;
    player->y = new_player_packet->y;
    player->speed = 10.0f;
    client->players[id] = player;
    if (client->id == 843) {
        client->id = id;
    }
}

void *server_loop(void *arg) {
    client_t *client = (client_t *)arg;
    while (client->running) {
        if (handle_server_packet(client->socket, client) == 84)
            break;
    }
    close(client->socket);
    return NULL;
}

int main(void) {
    client_t *client = malloc(sizeof(client_t));

    if (init_client_struct(client) == 84) {
        free(client);
        return ERROR;
    }
    printf("Connexion au serveur %s:%d...\n", IP, PORT);
    if (connect(client->socket, (struct sockaddr *)&client->addr, client->addr_len) == -1) { 
        perror("connect");
        return ERROR;
    }
    printf("Connecté au serveur!\n");
    send_message_packet(client->socket, "Hello from Client!");
    send_player_info_packet(client->socket, "New Player");
    pthread_create(&client->thread, NULL, server_loop, client);
    game_loop(client);
    pthread_join(client->thread, NULL);
    close(client->socket);
    free(client);
    return 0;
}
