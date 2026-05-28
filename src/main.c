// -- Imports --
#include "constants.h"

#include <raylib.h>
#include <raymath.h>

// -- Structures --
typedef struct
{
    bool spawned;
    Vector2 position;
    Vector2 direction;
    float speed;
    double spawn_time;
} Enemy;

typedef struct 
{
    Enemy enemy;
    int score;
    int streaks;
    int frame_count;
    int spawn_check;
} GameState;

// -- Helper functions --
void reset_game(GameState *game, bool game_over)
{
    game->enemy.spawned = false;
    game->enemy.position = Vector2Zero();
    game->enemy.direction = Vector2Zero();

    game->frame_count = 0;
    game->spawn_check = GetRandomValue(MIN_SPAWN_FRAMES, MAX_SPAWN_FRAMES);
    
    if (game_over)
    {
        game->enemy.speed = ENEMY_START_SPEED;
        game->score = 0;
        game->streaks = 0;
    }
}

Vector2 get_random_edge()
{
    int edge = GetRandomValue(1, 4);
    switch (edge)
    {
        case 1:
            return (Vector2){ GetRandomValue(0, WINDOW_WIDTH), 0 };
        case 2:
            return (Vector2){ WINDOW_WIDTH, GetRandomValue(0, WINDOW_HEIGHT) };
        case 3:
            return (Vector2){ GetRandomValue(0, WINDOW_WIDTH), WINDOW_HEIGHT };
        case 4:
            return (Vector2){ 0, GetRandomValue(0, WINDOW_HEIGHT) };
    }

    return Vector2Zero();
}

int calculate_score(GameState *game)
{
    double reaction_time = GetTime() - game->enemy.spawn_time;
    double time_mult = fmax(0, MAX_SCORE_TIME - reaction_time);
    double speed_mult = game->enemy.speed / 2.0;

    int total_bonus = (int)floor(BASE_SCORE_GAIN * time_mult * speed_mult);
    
    return BASE_SCORE_GAIN + total_bonus;
}

// -- Entry point --
int main(int argc, char *argv[])
{
    ChangeDirectory(GetApplicationDirectory());
    
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Reaction Aim");
    SetTargetFPS(60);

    InitAudioDevice();

    Sound hit_sfx = LoadSound("assets/sfx/hit.ogg");
    Sound over_sfx = LoadSound("assets/sfx/over.ogg");

    Vector2 center_position = {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2};

    GameState game;
    reset_game(&game, true);

    while (!WindowShouldClose()) {
        Vector2 mouse_pos = GetMousePosition();
        Vector2 mouse_dir = Vector2Normalize(Vector2Subtract(mouse_pos, center_position));

        Vector2 laser_end = Vector2Add(center_position, Vector2Scale(mouse_dir, LASER_LENGTH));

        if (game.frame_count > game.spawn_check && !game.enemy.spawned)
        {
            game.enemy.spawned = true;
            game.enemy.position = get_random_edge();
            game.enemy.direction = Vector2Normalize(Vector2Subtract(center_position, game.enemy.position));
            game.enemy.spawn_time = GetTime();
        }

        if (game.enemy.spawned)
        {
            game.enemy.position = Vector2Add(game.enemy.position, Vector2Scale(game.enemy.direction, game.enemy.speed));
            
            if (Vector2Distance(game.enemy.position, center_position) < 10)
            {
                reset_game(&game, true);

                PlaySound(over_sfx);
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
                && CheckCollisionCircleLine(game.enemy.position, ENEMY_COL_RADIUS, center_position, laser_end))
            {
                reset_game(&game, false);
                game.enemy.speed += ENEMY_SPEED_GAIN;
                game.score += calculate_score(&game);
                game.streaks++;

                PlaySound(hit_sfx);
            }
        }

        BeginDrawing();

        ClearBackground(BLACK);

        if (game.enemy.spawned) {
            DrawCircle(game.enemy.position.x, game.enemy.position.y, ENEMY_RADIUS, WHITE);
        }
        DrawLine(
            center_position.x,
            center_position.y,
            laser_end.x,
            laser_end.y,
            RED
        );
        
        for (int i = 0; i < game.streaks; i++)
        {
            DrawRectangle(
                WINDOW_WIDTH - 20 - (15 * (i % 8)),
                10 + (15 * (i / 8)),
                10,
                10,
                ORANGE
            );
        }
        DrawText(TextFormat("%04d", game.score), 10, 10, 24, WHITE);

        EndDrawing();

        game.frame_count++;
    }

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
