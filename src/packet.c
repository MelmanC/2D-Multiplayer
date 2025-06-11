#include "packet.h"
#include <stdio.h>
#include <time.h>
#include <errno.h>

void send_message_packet(int socket, struct sockaddr_in *addr,
            socklen_t addr_len, const char *message) {
    packet_message_t packet;
    packet.header.type = PACKET_TYPE_MESSAGE;
    packet.header.size = sizeof(packet_message_t);
    packet.timestamp = (uint32_t)time(NULL);
    snprintf(packet.message, sizeof(packet.message), "%s\n", message);

    if (sendto(socket, &packet, sizeof(packet), 0, (struct sockaddr *)addr, addr_len) == -1) {
        perror("sendto");
    }
}

void send_move_packet(int socket, uint32_t player_id, float x, float y,
                      struct sockaddr_in *addr, socklen_t addr_len) {
    packet_move_t packet;
    packet.header.type = PACKET_TYPE_MOVE;
    packet.header.size = sizeof(packet_move_t);
    packet.player_id = player_id;
    packet.x = x;
    packet.y = y;

    if (sendto(socket, &packet, sizeof(packet), 0, (struct sockaddr *)addr, addr_len) == -1) {
        perror("send");
    }
}

void send_new_player_packet(int socket, uint32_t player_id, float x, float y,
                            struct sockaddr_in *addr, socklen_t addr_len) {
    packet_new_player_t packet;
    packet.header.type = PACKET_TYPE_NEW_PLAYER;
    packet.header.size = sizeof(packet_new_player_t);
    packet.player_id = player_id;
    packet.x = x;
    packet.y = y;

    if (sendto(socket, &packet, sizeof(packet), 0, (struct sockaddr *)addr, addr_len) == -1) {
        perror("send");
    }
}

void send_player_info_packet(int socket, const char *name,
                                struct sockaddr_in *addr, socklen_t addr_len) {
    packet_player_info_t packet;
    packet.header.type = PACKET_TYPE_PLAYER_INFO;
    packet.header.size = sizeof(packet_player_info_t);
    snprintf(packet.name, sizeof(packet.name), "%s\n", name);

    if (sendto(socket, &packet, sizeof(packet), 0, (struct sockaddr *)addr, addr_len) == -1) {
        perror("send");
    }
}

void send_player_position_packet(int socket, float x, float y,
                                    struct sockaddr_in *addr, socklen_t addr_len) {
    packet_position_t packet;
    packet.header.type = PACKET_TYPE_PLAYER_POSITION;
    packet.header.size = sizeof(packet_position_t);
    packet.x = x;
    packet.y = y;
    if (sendto(socket, &packet, sizeof(packet), 0, (struct sockaddr *)addr, addr_len) == -1) {
        perror("send");
    }
}

int receive_packet(int socket, void *packet, size_t size) {
    size_t total_received = 0;
    char* buffer = (char*)packet;

    while (total_received < size) {
        ssize_t received = recv(socket, buffer + total_received, size - total_received, 0);
        if (received <= 0) {
            if (received == 0) {
                printf("Connexion fermée\n");
                return -1;
            }
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }
            perror("recv");
            return 84;
        }
        total_received += received;
    }
    return 0;
}
