#include "client.h"
#include "packet.h"
#include <stdio.h>
#include <sys/time.h>

int handle_server_packet(int socket) {
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
        default:
            printf("Packet non reconnu (type: %d)\n", header.type);
            break;
    }
    return 0;
}
