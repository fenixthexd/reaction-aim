// -- Imports --
#include "constants.h"
#include "savefile.h"

#include <raylib.h>
#include <raymath.h>

// -- Structures --
typedef struct
{
    bool spawned;
    Vector2 position;
    Vector2 direction;
    float speed;
    float lifetime;
} Enemy;

typedef struct 
{
    Enemy enemy;
    int score;
    int streaks;
    float elapsed_time;
    float spawn_at;
} GameState;

// -- Helper functions --
void reset_game(GameState *game, bool game_over)
{
    game->enemy.spawned = false;
    game->enemy.position = Vector2Zero();
    game->enemy.direction = Vector2Zero();
    game->enemy.lifetime = 0.0f;

    game->elapsed_time = 0.0f;
    game->spawn_at = GetRandomValue(MIN_SPAWN_TIME * 10, MAX_SPAWN_TIME * 10) / 10.0f;
    
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
    float time_multiplier = fmax(0, MAX_SCORE_TIME - game->enemy.lifetime);
    float speed_multiplier = game->enemy.speed / 120.0;

    int total_bonus = (int)floor(BASE_SCORE_GAIN * time_multiplier * speed_multiplier);
    
    return BASE_SCORE_GAIN + total_bonus;
}

// -- Entry point --
int main(int argc, char *argv[])
{
    ChangeDirectory(GetApplicationDirectory());
    SetTargetFPS(120);

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Reaction Aim");
    InitAudioDevice();

    Sound hit_sfx = LoadSound("assets/sfx/hit.ogg");
    Sound over_sfx = LoadSound("assets/sfx/over.ogg");

    Vector2 center_position = {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2};

    GameState game;
    reset_game(&game, true);

    int high_score = LoadHighScore(SAVEFILE_NAME);

    Camera2D camera = {0};
    camera.zoom = 1.0f;

    float current_shake = 0.0f;
    float current_flash = 0.0f;

    while (!WindowShouldClose()) {
        float delta_time = GetFrameTime();

        Vector2 mouse_position = GetMousePosition();
        Vector2 mouse_direction = Vector2Normalize(Vector2Subtract(mouse_position, center_position));

        Vector2 laser_end = Vector2Add(center_position, Vector2Scale(mouse_direction, LASER_LENGTH));

        if (game.elapsed_time > game.spawn_at && !game.enemy.spawned)
        {
            game.enemy.spawned = true;
            game.enemy.position = get_random_edge();
            game.enemy.direction = Vector2Normalize(Vector2Subtract(center_position, game.enemy.position));
        }

        if (game.enemy.spawned)
        {
            game.enemy.position = Vector2Add(
                game.enemy.position,
                Vector2Scale(game.enemy.direction, game.enemy.speed * delta_time)
            );
            game.enemy.lifetime += delta_time;

            if (Vector2Distance(game.enemy.position, center_position) < KILL_RADIUS)
            {
                if (game.score > high_score)
                {
                    high_score = game.score;
                    SaveHighScore(SAVEFILE_NAME, high_score);
                }

                reset_game(&game, true);

                PlaySound(over_sfx);
                current_flash = 1.0f;
            }
            else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
                && CheckCollisionCircleLine(game.enemy.position, ENEMY_COL_RADIUS, center_position, laser_end))
            {
                game.score += calculate_score(&game);
                game.enemy.speed += ENEMY_SPEED_GAIN;
                game.streaks++;
                reset_game(&game, false);

                PlaySound(hit_sfx);
                current_shake += SHAKE_BASE_STRENGTH + (game.streaks * SHAKE_SCALE_STRENGTH);
            }
        }

        current_shake = fmaxf(0, current_shake - SHAKE_DECAY * delta_time);
        camera.offset.x = GetRandomValue(-1, 1) * current_shake;
        camera.offset.y = GetRandomValue(-1, 1) * current_shake;

        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode2D(camera);

        if (game.enemy.spawned)
        {
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
                WINDOW_WIDTH - 20 - (15 * (i % STREAK_ROW_SIZE)),
                10 + (15 * (i / STREAK_ROW_SIZE)),
                10,
                10,
                ORANGE
            );
        }

        DrawText(TextFormat("%04d", game.score), 10, 10, 24, WHITE);
        DrawText(TextFormat("BEST: %04d", high_score), 10, WINDOW_HEIGHT - 30, 20, DARKGRAY);

        EndMode2D();

        current_flash = fmaxf(0, current_flash - FLASH_DECAY * delta_time);
        DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Fade(WHITE, current_flash));

        EndDrawing();

        game.elapsed_time += delta_time;
    }

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
