#include "game.h"
#include <stdio.h>

static Texture2D birdSheets[MAX_VARIANTS];
static Rectangle birdFrames[MAX_VARIANTS][FRAMES_PER_VARIANT];
static int g_birdVariant = 0;

Bird InitBird(void) {
    Bird bird;
    bird.rect.x = SCREEN_WIDTH / 4.0f;
    bird.rect.y = SCREEN_HEIGHT / 2.0f;
    bird.rect.width  = 40;
    bird.rect.height = 40;
    bird.velocityY = 0.0f;
    bird.frame = 0;
    bird.frameTimer = 0.0f;
    return bird;
}

void LoadBirdAssets(void) {
    char filename[128];

    for (int v = 0; v < MAX_VARIANTS; v++) {
        snprintf(filename, sizeof(filename), "assets/bird%d.png", v+1);
        birdSheets[v] = LoadTexture(filename);
        int cols = 4;
        int rows = 4;
        int frameWidth  = birdSheets[v].width / cols;
        int frameHeight = birdSheets[v].height / rows;

        int index = 0;
        for (int y = 0; y < rows; y++) {
            for (int x = 0; x < cols; x++) {
                birdFrames[v][index] = (Rectangle){
                    .x = x * frameWidth,
                    .y = y * frameHeight,
                    .width = frameWidth,
                    .height = frameHeight
                };
                index++;
            }
        }
    }
}

void UnloadBirdAssets(void) {
    for (int v = 0; v < MAX_VARIANTS; v++) {
        UnloadTexture(birdSheets[v]);
    }
}

void UpdateBird(Bird *bird, float dt, float scrollSpeed, float acceleration) {
    if (IsKeyPressed(KEY_SPACE)) {
        float flap = FLAP_STRENGTH * (1.0f + acceleration / 100.0f);
        bird->velocityY = flap;
    }

    bird->velocityY += GRAVITY * dt;
    bird->rect.y += bird->velocityY * dt;

    if (bird->rect.y < 0) bird->rect.y = 0;

    if (bird->rect.y + bird->rect.height > SCREEN_HEIGHT - GROUND_HEIGHT) {
        bird->rect.y = SCREEN_HEIGHT - GROUND_HEIGHT - bird->rect.height;
        bird->velocityY = 0;
    }

    bird->frameTimer += dt;
    if (bird->frameTimer >= 0.05f) {
        bird->frame = (bird->frame + 1) % FRAMES_PER_VARIANT;
        bird->frameTimer = 0.0f;
    }
}

void DrawBird(Bird bird) {
    Texture2D sheet = birdSheets[g_birdVariant];
    Rectangle src = birdFrames[g_birdVariant][bird.frame];
    Rectangle dest = { bird.rect.x, bird.rect.y, bird.rect.width, bird.rect.height };

    DrawTexturePro(sheet, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
}

void SetActiveBirdVariant(int v) {
    if (v < 0) v = 0;
    if (v >= MAX_VARIANTS) v = MAX_VARIANTS - 1;
    g_birdVariant = v;
}

int GetActiveBirdVariant(void) {
    return g_birdVariant;
}



PipeManager InitPipes(float startSpeed) {
    PipeManager manager;
    manager.scrollSpeed = startSpeed;
    manager.baseSpeed = startSpeed;
    manager.acceleration = 5.0f;

    int maxTopHeight = SCREEN_HEIGHT - GROUND_HEIGHT - PIPE_GAP - MIN_TOP_HEIGHT;

    for (int i = 0; i < MAX_PIPES; i++) {
        manager.pipes[i].x = SCREEN_WIDTH + i * PIPE_SPACING;
        manager.pipes[i].height = MIN_TOP_HEIGHT + GetRandomValue(0, maxTopHeight - MIN_TOP_HEIGHT);
        manager.pipes[i].single = false;
        manager.pipes[i].passed = false;
    }

    return manager;
}

void UpdatePipes(PipeManager *manager, float dt, Bird *bird, int *score) {
    // accelerate scroll speed over time
    manager->scrollSpeed += manager->acceleration * dt;

    int maxTopHeight = SCREEN_HEIGHT - GROUND_HEIGHT - PIPE_GAP - MIN_TOP_HEIGHT;

    for (int i = 0; i < MAX_PIPES; i++) {
        manager->pipes[i].x -= manager->scrollSpeed * dt;

        // scoring
        if (!manager->pipes[i].passed && manager->pipes[i].x + PIPE_WIDTH < bird->rect.x) {
            (*score)++;
            manager->pipes[i].passed = true;
        }

        if (manager->pipes[i].x + PIPE_WIDTH < 0) {
            float rightmostX = 0;
            for (int j = 0; j < MAX_PIPES; j++) {
                if (manager->pipes[j].x > rightmostX)
                    rightmostX = manager->pipes[j].x;
            }
            manager->pipes[i].x = rightmostX + PIPE_SPACING;

            manager->pipes[i].single = (GetRandomValue(0, 3) == 0);
            if (manager->pipes[i].single) {
                manager->pipes[i].height = SCREEN_HEIGHT - GROUND_HEIGHT - 100;
            } else {
                manager->pipes[i].height = MIN_TOP_HEIGHT + GetRandomValue(0, maxTopHeight - MIN_TOP_HEIGHT);
            }
            manager->pipes[i].passed = false;
        }
    }
}


static void DrawPipe3D(float x, float y, float w, float h, bool capAtBottom) {
    Color baseLight = (Color){ 76, 175,  80, 255};
    Color baseDark  = (Color){ 46, 125,  50, 255};
    Color outline   = (Color){ 34,  85,  38, 255};

    DrawRectangleGradientV(x, y, w, h, baseLight, baseDark);
    DrawRectangleGradientH(x, y, w, h, Fade(WHITE, 0.06f), Fade(BLACK, 0.12f));

    float hlW = w * 0.18f;
    DrawRectangle(x + w * 0.10f, y + 2, hlW, h - 4, Fade(WHITE, 0.22f));
    DrawRectangle(x + w * 0.10f + hlW * 0.55f, y + 4, hlW * 0.25f, h - 8, Fade(WHITE, 0.18f));

    float shW = w * 0.12f;
    DrawRectangle(x + w - shW - w * 0.06f, y + 2, shW, h - 4, Fade(BLACK, 0.18f));

    float capH = 16.0f;
    float capY = capAtBottom ? (y + h - capH) : y;
    DrawRectangleGradientV(x - 3, capY, w + 6, capH, baseDark, baseLight);
    DrawRectangleLines(x - 3, capY, w + 6, capH, outline);

    DrawRectangleLines(x, y, w, h, outline);
}

void DrawPipes(PipeManager manager) {
    for (int i = 0; i < MAX_PIPES; i++) {
        float x = manager.pipes[i].x;

        if (manager.pipes[i].single) {
            // Top-only pipe
            DrawPipe3D(x, 0, PIPE_WIDTH, manager.pipes[i].height, true);
        } else {
            // Top pipe
            DrawPipe3D(x, 0, PIPE_WIDTH, manager.pipes[i].height, true);

            // Bottom pipe
            float bottomY = manager.pipes[i].height + PIPE_GAP;
            float bottomH = (SCREEN_HEIGHT - GROUND_HEIGHT) - bottomY;

            if (bottomH < 0) bottomH = 0; // prevent gap bug

            DrawPipe3D(x, bottomY, PIPE_WIDTH, bottomH, false);
        }
    }
}


bool CheckCollision(Bird bird, PipeManager pipes) {
    for (int i = 0; i < MAX_PIPES; i++) {
        if (pipes.pipes[i].single) {
            Rectangle top = { pipes.pipes[i].x, 0, PIPE_WIDTH, pipes.pipes[i].height };
            if (CheckCollisionRecs(bird.rect, top)) return true;
        } else {
            Rectangle top = { pipes.pipes[i].x, 0, PIPE_WIDTH, pipes.pipes[i].height };
            Rectangle bottom = { pipes.pipes[i].x, pipes.pipes[i].height + PIPE_GAP,
                                 PIPE_WIDTH, SCREEN_HEIGHT - pipes.pipes[i].height - PIPE_GAP - GROUND_HEIGHT };
            if (CheckCollisionRecs(bird.rect, top) || CheckCollisionRecs(bird.rect, bottom))
                return true;
        }
    }
    return false;
}
