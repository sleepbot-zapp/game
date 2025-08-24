#include "raylib.h"
#include "game.h"
#include <stdio.h>
#include <time.h>

void InitClouds(Cloud clouds[], int count, Texture2D cloudTexture) {
    for (int i = 0; i < count; i++) {
        clouds[i].scale = GetRandomValue(4, 8) / 100.0f;
        clouds[i].position = (Vector2){
            GetRandomValue(0, SCREEN_WIDTH - (int)(cloudTexture.width * clouds[i].scale)),
            GetRandomValue(0, SCREEN_HEIGHT / 2 - (int)(cloudTexture.height * clouds[i].scale))
        };
    }
}

void ResetGame(Bird *bird, PipeManager *pipes, Cloud clouds[], int cloudCount, Texture2D cloudTexture, int *score, bool *gameOver, bool *gameStarted, bool *paused) {
    SetActiveBirdVariant(GetRandomValue(0, MAX_VARIANTS - 1));
    *bird = InitBird();
    *pipes = InitPipes(100.0f);

    InitClouds(clouds, cloudCount, cloudTexture);

    *score = 0;
    *gameOver = false;
    *gameStarted = true;
    *paused = false;
}

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

    Texture2D cloudTexture = LoadTexture("assets/cloud.png");
    Texture2D groundTexture = LoadTexture("assets/ground.png");
    float groundOffset = 0.0f;

    Cloud clouds[MAX_CLOUDS];
    InitClouds(clouds, MAX_CLOUDS, cloudTexture);

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
                if (IsKeyPressed(KEY_SPACE)) paused = false;
            }

            if (!paused) {
                UpdateBird(&bird, dt, pipes.scrollSpeed, pipes.acceleration);
                UpdatePipes(&pipes, dt, &bird, &score);

                for (int i = 0; i < MAX_CLOUDS; i++) {
                    clouds[i].position.x -= pipes.scrollSpeed * dt;
                    if (clouds[i].position.x + cloudTexture.width * clouds[i].scale < 0) {
                        clouds[i].scale = GetRandomValue(4, 8) / 100.0f;
                        clouds[i].position.x = SCREEN_WIDTH + GetRandomValue(50, 200);
                        clouds[i].position.y = GetRandomValue(0, SCREEN_HEIGHT / 2 - (int)(cloudTexture.height * clouds[i].scale));
                    }
                }

                groundOffset -= pipes.scrollSpeed * dt;
                if (groundOffset <= -groundTexture.width) groundOffset += groundTexture.width;

                if (CheckCollision(bird, pipes))
                    gameOver = true;
            }
        } else {
            if (IsKeyPressed(KEY_SPACE)) {
                ResetGame(&bird, &pipes, clouds, MAX_CLOUDS, cloudTexture, &score, &gameOver, &gameStarted, &paused);
            }
        }

        BeginDrawing();
        ClearBackground(SKYBLUE);

        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT - GROUND_HEIGHT, SKYBLUE);

        for (int i = 0; i < MAX_CLOUDS; i++)
            DrawTextureEx(cloudTexture, clouds[i].position, 0.0f, clouds[i].scale, WHITE);

        if (gameStarted) {
            DrawPipes(pipes);
            DrawBird(bird);
            DrawText(TextFormat("Score: %d", score), 10, 10, 20, BLACK);

            if (paused) DrawText("PAUSED", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 10, 30, DARKGRAY);
            if (gameOver) {
                DrawText("GAME OVER", SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 - 20, 30, RED);
                DrawText("Press SPACE to Restart", SCREEN_WIDTH / 2 - 110, SCREEN_HEIGHT / 2 + 20, 20, BLACK);
            }
        } else if (firstRun) {
            DrawText("Press SPACE to Begin", SCREEN_WIDTH / 2 - 110, SCREEN_HEIGHT / 2 - 10, 20, BLACK);
        }

        Rectangle src = {0, 0, (float)groundTexture.width, (float)groundTexture.height};
        for (int x = 0; x <= SCREEN_WIDTH + groundTexture.width; x += groundTexture.width) {
            Rectangle dest = {groundOffset + x, SCREEN_HEIGHT - (GROUND_HEIGHT + 20),
                              (float)groundTexture.width, (float)(GROUND_HEIGHT + 20)};
            DrawTexturePro(groundTexture, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
        }

        EndDrawing();
    }

    CloseWindow();
    UnloadBirdAssets();
    UnloadTexture(cloudTexture);
    UnloadTexture(groundTexture);

    return 0;
}
