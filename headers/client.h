#pragma once

#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdatomic.h>
#include "packet.h"

#define PORT 7777
#define IP "127.0.0.1"
#define BILLION 1000000
#define MAX_PLAYERS 5
#define ERROR 84

typedef struct player_s {
    uint32_t id;
    float x;
    float y;
    float speed;
} player_t;

typedef struct {
    int socket;
    struct sockaddr_in addr;
    socklen_t addr_len;
    player_t **players;
    pthread_t thread;
    atomic_bool running;
    uint32_t id;
} client_t;

int handle_server_packet(int socket, client_t *client);

int game_loop(client_t *client);

void create_player(client_t *client, uint32_t id, packet_new_player_t *new_player_packet);
