#include "raylib.h"
#include "game.h"
#include <stdio.h>
#include <time.h>

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Flappy");
    SetTargetFPS(60);

    SetRandomSeed((unsigned int)time(NULL));

    Bird bird = InitBird();
    PipeManager pipes = InitPipes(100.0f);

    int score = 0;
    bool gameOver = false;
    bool firstRun = true;
    bool gameStarted = false;
    bool paused = false;

    LoadBirdAssets();
    SetActiveBirdVariant(GetRandomValue(0, MAX_VARIANTS - 1));

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (!gameStarted) {
            if (IsKeyPressed(KEY_SPACE)) {
                gameStarted = true;
                firstRun = false;
            }
        } else if (!gameOver) {
            if (!paused) {
                for (int key = 0; key < 512; key++) {
                    if (IsKeyPressed(key) && key != KEY_SPACE) {
                        paused = true;
                        break;
                    }
                }
            } else {
                for (int key = 0; key < 512; key++) {
                    if (IsKeyPressed(key)) {
                        paused = false;
                        break;
                    }
                }
            }

            if (!paused) {
                UpdateBird(&bird, dt, pipes.acceleration);
                UpdatePipes(&pipes, dt, &bird, &score);

                if (CheckCollision(bird, pipes) ||
                    bird.rect.y + bird.rect.height >= SCREEN_HEIGHT - GROUND_HEIGHT) {
                    gameOver = true;
                }
            }
        } else {
            if (IsKeyPressed(KEY_SPACE)) {
                SetActiveBirdVariant(GetRandomValue(0, MAX_VARIANTS - 1));
                bird = InitBird();
                pipes = InitPipes(100.0f);
                score = 0;
                gameOver = false;
                gameStarted = true;
                paused = false;
            }
        }

        BeginDrawing();
        ClearBackground(SKYBLUE);

        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT - GROUND_HEIGHT, SKYBLUE);
        DrawRectangle(0, SCREEN_HEIGHT - GROUND_HEIGHT, SCREEN_WIDTH, GROUND_HEIGHT, BROWN);

        if (gameStarted) {
            DrawPipes(pipes);
            DrawBird(bird);
            DrawText(TextFormat("Score: %d", score), 10, 10, 20, BLACK);

            if (paused) {
                DrawText("PAUSED", SCREEN_WIDTH/2 - 50, SCREEN_HEIGHT/2 - 10, 30, DARKGRAY);
            }

            if (gameOver) {
                DrawText("GAME OVER", SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 - 20, 30, RED);
                DrawText("Press SPACE to Restart", SCREEN_WIDTH/2 - 110, SCREEN_HEIGHT/2 + 20, 20, BLACK);
            }
        } else if (firstRun) {
            DrawText("Press SPACE to Begin", SCREEN_WIDTH/2 - 110, SCREEN_HEIGHT/2 - 10, 20, BLACK);
        }

        EndDrawing();
    }

    CloseWindow();
    UnloadBirdAssets();
    return 0;
}
