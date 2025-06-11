#include "client.h"
#include "packet.h"
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

int handle_server_packet(int socket, client_t *client) {
    packet_t header;
    
    if (receive_packet(socket, &header, sizeof(packet_t)) == -1) {
        return 84;
    }
    
    switch (header.type) {
        case PACKET_TYPE_MESSAGE: {
            packet_message_t message_packet;
            message_packet.header = header;

            receive_packet(socket, ((char *)&message_packet) + sizeof(packet_t),
                header.size - sizeof(packet_t));
            printf("Message reçu (timestamp: %u): %s\n", message_packet.timestamp, message_packet.message);
            break;
        }
        case PACKET_TYPE_NEW_PLAYER: {
            packet_new_player_t new_player_packet;
            new_player_packet.header = header;

            receive_packet(socket, ((char *)&new_player_packet) + sizeof(packet_t),
                header.size - sizeof(packet_t));
            create_player(client, new_player_packet.player_id, &new_player_packet);
            break;
        }
        case PACKET_TYPE_MOVE: {
            packet_move_t move_packet;
            move_packet.header = header;
            receive_packet(socket, ((char *)&move_packet) + sizeof(packet_t),
                header.size - sizeof(packet_t));
            for (int i = 0; i < MAX_PLAYERS; i++) {
                if (client->players[i] && client->players[i]->id == move_packet.player_id) {
                    client->players[i]->x = move_packet.x;
                    client->players[i]->y = move_packet.y;
                }
            }
            break;
        }
        default:
            printf("Packet non reconnu (type: %d)\n", header.type);
            break;
    }
    return 0;
}
