#include "server.h"
#include "packet.h"
#include <stdint.h>
#include <stdio.h>
#include <poll.h>
#include <unistd.h>

static
void broadcast_new_player(server_t *server, int new_player_idx)
{
    for (int i = 1; i < server->nfds; i++) {
        if (server->clients[i] && server->clients[i]->connected) {
            send_new_player_packet(server->fds[i].fd,
                server->clients[new_player_idx]->player_id,
                server->clients[new_player_idx]->x,
                server->clients[new_player_idx]->y);
        }
    }
}

static
void broadcast_ancient_players(server_t *server, int fd, int new_player_idx)
{
    for (int i = 1; i < server->nfds; i++) {
        if (i == new_player_idx)
            continue;
        if (server->clients[i] && server->clients[i]->connected) {
            send_new_player_packet(fd,
                server->clients[i]->player_id,
                server->clients[i]->x,
                server->clients[i]->y);
        }
    }
}

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
        case PACKET_TYPE_PLAYER_INFO: {
            packet_player_info_t player_info;
            player_info.header = header;

            receive_packet(server->fds[idx].fd, ((char *)&player_info) + sizeof(packet_t),
                header.size - sizeof(packet_t));
            printf("Info joueur reçu: %s\n", player_info.name);
            broadcast_new_player(server, idx);
            broadcast_ancient_players(server, server->fds[idx].fd, idx);
            break;
        }
        case PACKET_TYPE_PLAYER_POSITION: {
            packet_position_t player_move;
            player_move.header = header;
            receive_packet(server->fds[idx].fd, ((char *)&player_move) + sizeof(packet_t),
                header.size - sizeof(packet_t));
            client_t *cli = server->clients[idx];
            if (cli) {
                cli->x = player_move.x;
                cli->y = player_move.y;
                for (int i = 1; i < server->nfds; i++) {
                    if (server->clients[i] && server->clients[i]->connected) {
                        send_move_packet(server->clients[i]->fd, cli->player_id, cli->x, cli->y);
                    }
                }
            }
            break;
        }
        default:
            printf("Type de packet inconnu: %d\n", header.type);
            break;
    }
    
    return 0;
}
