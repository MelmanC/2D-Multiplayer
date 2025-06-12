#pragma once

#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>

#define PACKET_TYPE_MESSAGE 0x01
#define PACKET_TYPE_MOVE 0x02
#define PACKET_TYPE_NEW_PLAYER 0x03
#define PACKET_TYPE_PLAYER_INFO 0x04
#define PACKET_TYPE_PLAYER_POSITION 0x05

#define ALIGNED __attribute__((aligned(4)))

typedef struct ALIGNED packet_s {
    uint8_t type;
    uint32_t size;
} packet_t;

/* Client and Server Packets */
typedef struct ALIGNED packet_message_s {
    packet_t header;
    uint32_t timestamp;
    char message[256];
} packet_message_t;

/* Server Packets */
typedef struct ALIGNED packet_move_s {
    packet_t header;
    uint32_t player_id;
    int sequence_number;
    float x;
    float y;
} packet_move_t;

/* Server Packets */
typedef struct ALIGNED packet_new_player_s {
    packet_t header;
    uint32_t player_id;
    float x;
    float y;
} packet_new_player_t;

/* Client Packets */
typedef struct ALIGNED packet_player_info_s {
    packet_t header;
    char name[32];
} packet_player_info_t;

/* Client Packets */
typedef struct ALIGNED packet_position_s {
    packet_t header;
    int sequence_number;
    float x;
    float y;
} packet_position_t;

int receive_packet(int socket, void* packet, size_t size);

void send_move_packet(int socket, uint32_t player_id, float x, float y, int sequence_number,
                        struct sockaddr_in *addr, socklen_t addr_len);

void send_message_packet(int socket, struct sockaddr_in *addr,
            socklen_t addr_len, const char *message);

void send_new_player_packet(int socket, uint32_t player_id, float x, float y,
                            struct sockaddr_in *addr, socklen_t addr_len);

void send_player_info_packet(int socket, const char *name,
                            struct sockaddr_in *addr, socklen_t addr_len);

void send_player_position_packet(int socket, float x, float y, int sequence_number,
                                struct sockaddr_in *addr, socklen_t addr_len);
