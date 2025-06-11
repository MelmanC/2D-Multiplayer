#include <raylib.h>
#include "client.h"
#include "packet.h"
#include <unistd.h>

static
void check_events(client_t *client) {
    if (IsKeyPressed(KEY_ESCAPE) || WindowShouldClose()) {
        client->running = 0;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        client->players[client->id]->x += client->players[client->id]->speed;
        send_player_position_packet(client->socket, client->players[client->id]->x, client->players[client->id]->y);
    }
    if (IsKeyDown(KEY_LEFT)) {
        client->players[client->id]->x -= client->players[client->id]->speed;
        send_player_position_packet(client->socket, client->players[client->id]->x, client->players[client->id]->y);
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
    return 0;
}
