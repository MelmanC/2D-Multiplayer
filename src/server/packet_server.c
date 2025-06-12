#include "server.h"
#include "packet.h"
#include <stdint.h>
#include <stdio.h>
#include <poll.h>
#include <unistd.h>

static
void broadcast_new_player(server_t *server, int new_player_idx)
{
    for (int i = 0; i < server->player_ids; i++) {
        if (server->clients[i] && server->clients[i]->connected) {
            send_new_player_packet(server->fd,
                server->clients[new_player_idx]->player_id,
                server->clients[new_player_idx]->x,
                server->clients[new_player_idx]->y,
                &server->clients[i]->addr,
                server->clients[i]->addr_len);
        }
    }
}

static
void broadcast_ancient_players(server_t *server, int new_player_idx)
{
    for (int i = 0; i < server->player_ids; i++) {
        if (i == new_player_idx)
            continue;
        if (server->clients[i] && server->clients[i]->connected) {
            send_new_player_packet(server->fd,
                server->clients[i]->player_id,
                server->clients[i]->x,
                server->clients[i]->y,
                &server->clients[new_player_idx]->addr,
                server->clients[new_player_idx]->addr_len);
        }
    }
}

int handle_client_data(server_t *server, int idx, char *buffer, unsigned long recv_len)
{
    packet_t *header = (packet_t *)buffer;

    if (recv_len < sizeof(packet_t))
        return 0;
    switch (header->type) {
        case PACKET_TYPE_MESSAGE: {
            packet_message_t *message_packet = (packet_message_t *)buffer;
            message_packet->header = *header;

            printf("Message reçu de %d: %s", idx, message_packet->message);
            break;
        }
        case PACKET_TYPE_PLAYER_INFO: {
            packet_player_info_t *player_info_packet = (packet_player_info_t *)buffer;
            player_info_packet->header = *header;

            broadcast_new_player(server, idx);
            broadcast_ancient_players(server, idx);
            break;
        }
        case PACKET_TYPE_PLAYER_POSITION: {
            packet_position_t *player_move = (packet_position_t *)buffer;
            player_move->header = *header;
            client_t *cli = server->clients[idx];
            if (cli) {
                cli->x = player_move->x;
                cli->y = player_move->y;
                for (int i = 0; i < server->player_ids; i++) {
                    if (server->clients[i] && server->clients[i]->connected) {
                        usleep(150 * 1000); // Simulate network delay
                        send_move_packet(server->fd,
                            cli->player_id, cli->x, cli->y,
                            player_move->sequence_number,
                            &server->clients[i]->addr,
                            server->clients[i]->addr_len);
                    }
                }
            }
            break;
        }
        default:
            printf("Type de packet inconnu: %d\n", header->type);
            break;
    }
    
    return 0;
}
