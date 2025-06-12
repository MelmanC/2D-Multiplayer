#include <raylib.h>
#include "client.h"
#include "packet.h"
#include <unistd.h>
#include <stdio.h>

static
void update_position(client_t *client) {
    client->sequence_number++;
    int idx = client->input_end % BUFFER_SIZE;
    client->input[idx].sequence_number = client->sequence_number;
    client->input[idx].x = client->players[client->id]->x;
    client->input[idx].y = client->players[client->id]->y;
    client->input_end++;
    send_player_position_packet(client->socket, client->players[client->id]->x, client->players[client->id]->y,
        client->sequence_number, &client->addr, client->addr_len);
}

static
void check_events(client_t *client) {
    if (IsKeyPressed(KEY_ESCAPE) || WindowShouldClose()) {
        client->running = 0;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        client->players[client->id]->x += client->players[client->id]->speed;
        update_position(client);
    }
    if (IsKeyDown(KEY_LEFT)) {
        client->players[client->id]->x -= client->players[client->id]->speed;
        update_position(client);
    }
}

int game_loop(client_t *client) {
    InitWindow(800, 600, "Game Client");
    SetWindowState(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    SetTargetFPS(60);
    while (client->running) {
        check_events(client);
        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (client->players[i] != NULL) {
                DrawCircle(client->players[i]->x, client->players[i]->y, 20, BLUE);
            }
        }

        DrawText("Game Loop Running", 10, 10, 20, DARKGRAY);
        EndDrawing();

    }
    CloseWindow();
    // TODO: Disconnect from server and clean up resources
    return 0;
}
