#include "client.h"
#include "packet.h"
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

int handle_server_packet(int socket, client_t *client) {
    char buffer[2048];
    struct sockaddr_in src_addr;
    socklen_t addr_len = sizeof(src_addr);

    ssize_t recv_len = recvfrom(socket, buffer, sizeof(buffer), 0,
                                (struct sockaddr *)&src_addr, &addr_len);
    if (recv_len < (ssize_t)sizeof(packet_t)) {
        return 84;
    }
    packet_t *header = (packet_t *)buffer;

    switch (header->type) {
        case PACKET_TYPE_MESSAGE: {
            packet_message_t *message_packet = (packet_message_t *)buffer;
            printf("Message reçu (timestamp: %u): %s\n", message_packet->timestamp, message_packet->message);
            break;
        }
        case PACKET_TYPE_NEW_PLAYER: {
            packet_new_player_t *new_player_packet = (packet_new_player_t *)buffer;
            create_player(client, new_player_packet->player_id, new_player_packet);
            break;
        }
        case PACKET_TYPE_MOVE: {
            packet_move_t *move_packet = (packet_move_t *)buffer;
            for (int i = 0; i < MAX_PLAYERS; i++) {
                if (client->players[i] && client->players[i]->id == move_packet->player_id) {
                    client->players[i]->x = move_packet->x;
                    client->players[i]->y = move_packet->y;
                }
            }
            break;
        }
        default:
            printf("Packet non reconnu (type: %d)\n", header->type);
            break;
    }
    return 0;
}
