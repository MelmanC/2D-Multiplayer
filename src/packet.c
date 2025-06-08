#include "packet.h"
#include <stdio.h>
#include <time.h>
#include <errno.h>

void send_message_packet(int socket, const char *message) {
    packet_message_t packet;
    packet.header.type = PACKET_TYPE_MESSAGE;
    packet.header.size = sizeof(packet_message_t);
    packet.timestamp = (uint32_t)time(NULL);
    snprintf(packet.message, sizeof(packet.message), "%s\n", message);

    if (send(socket, &packet, sizeof(packet), 0) == -1) {
        perror("send");
    }
}

void send_move_packet(int socket, uint32_t player_id, float x, float y) {
    packet_move_t packet;
    packet.header.type = PACKET_TYPE_MOVE;
    packet.header.size = sizeof(packet_move_t);
    packet.player_id = player_id;
    packet.x = x;
    packet.y = y;

    if (send(socket, &packet, sizeof(packet), 0) == -1) {
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
