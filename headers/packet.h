#pragma once

#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>

#define PACKET_TYPE_MESSAGE 0x01
#define PACKET_TYPE_MOVE 0x02

#define ALIGNED __attribute__((aligned(4)))

typedef struct ALIGNED packet_s {
    uint8_t type;
    uint32_t size;
} packet_t;

typedef struct ALIGNED packet_message_s {
    packet_t header;
    uint32_t timestamp;
    char message[256];
} packet_message_t;

typedef struct ALIGNED packet_move_s {
    packet_t header;
    uint32_t player_id;
    float x;
    float y;
} packet_move_t;

int receive_packet(int socket, void* packet, size_t size);

void send_move_packet(int socket, uint32_t player_id, float x, float y);

void send_message_packet(int socket, const char *message);
