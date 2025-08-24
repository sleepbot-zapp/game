#include "game.h"
#include <stdio.h>

static Texture2D birdSheets[MAX_VARIANTS];
static Rectangle birdFrames[MAX_VARIANTS][FRAMES_PER_VARIANT];
static int g_birdVariant = 0;

Bird InitBird(void) {
    return (Bird){
        .rect = { SCREEN_WIDTH / 4.0f, SCREEN_HEIGHT / 2.0f, 40, 40 },
        .velocityY = 0.0f,
        .frame = 0,
        .frameTimer = 0.0f
    };
}

void LoadBirdAssets(void) {
    char filename[128];
    for (int v = 0; v < MAX_VARIANTS; v++) {
        snprintf(filename, sizeof(filename), "assets/bird%d.png", v+1);
        birdSheets[v] = LoadTexture(filename);

        int cols = 4, rows = 4;
        int frameW = birdSheets[v].width / cols;
        int frameH = birdSheets[v].height / rows;

        int index = 0;
        for (int y = 0; y < rows; y++)
            for (int x = 0; x < cols; x++)
                birdFrames[v][index++] = (Rectangle){ x*frameW, y*frameH, frameW, frameH };
    }
}

void UnloadBirdAssets(void) {
    for (int v = 0; v < MAX_VARIANTS; v++)
        UnloadTexture(birdSheets[v]);
}

void UpdateBird(Bird *bird, float dt, float scrollSpeed, float acceleration) {
    if (IsKeyPressed(KEY_SPACE))
        bird->velocityY = FLAP_STRENGTH * (1.0f + acceleration / 100.0f);

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
    DrawTexturePro(birdSheets[g_birdVariant],
                   birdFrames[g_birdVariant][bird.frame],
                   (Rectangle){ bird.rect.x, bird.rect.y, bird.rect.width, bird.rect.height },
                   (Vector2){0,0}, 0.0f, WHITE);
}

void SetActiveBirdVariant(int v) {
    g_birdVariant = (v < 0) ? 0 : (v >= MAX_VARIANTS ? MAX_VARIANTS - 1 : v);
}

int GetActiveBirdVariant(void) {
    return g_birdVariant;
}

PipeManager InitPipes(float startSpeed) {
    PipeManager m;
    m.scrollSpeed = m.baseSpeed = startSpeed;
    m.acceleration = 5.0f;

    int maxTopHeight = SCREEN_HEIGHT - GROUND_HEIGHT - PIPE_GAP - MIN_TOP_HEIGHT;
    for (int i = 0; i < MAX_PIPES; i++) {
        m.pipes[i] = (Pipe){
            .x = SCREEN_WIDTH + i*PIPE_SPACING,
            .height = MIN_TOP_HEIGHT + GetRandomValue(0, maxTopHeight - MIN_TOP_HEIGHT),
            .single = false,
            .passed = false
        };
    }
    return m;
}

static float GetRightmostPipeX(PipeManager *m) {
    float maxX = 0;
    for (int i = 0; i < MAX_PIPES; i++)
        if (m->pipes[i].x > maxX) maxX = m->pipes[i].x;
    return maxX;
}

void UpdatePipes(PipeManager *m, float dt, Bird *bird, int *score) {
    m->scrollSpeed += m->acceleration * dt;
    int maxTopHeight = SCREEN_HEIGHT - GROUND_HEIGHT - PIPE_GAP - MIN_TOP_HEIGHT;

    for (int i = 0; i < MAX_PIPES; i++) {
        Pipe *p = &m->pipes[i];
        p->x -= m->scrollSpeed * dt;

        if (!p->passed && p->x + PIPE_WIDTH < bird->rect.x) {
            (*score)++;
            p->passed = true;
        }

        if (p->x + PIPE_WIDTH < 0) {
            p->x = GetRightmostPipeX(m) + PIPE_SPACING;
            p->single = (GetRandomValue(0,3) == 0);
            p->height = p->single ? SCREEN_HEIGHT - GROUND_HEIGHT - 100
                                  : MIN_TOP_HEIGHT + GetRandomValue(0, maxTopHeight - MIN_TOP_HEIGHT);
            p->passed = false;
        }
    }
}

static void DrawPipe3D(float x, float y, float w, float h, bool capAtBottom) {
    Color baseLight = {76,175,80,255}, baseDark = {46,125,50,255}, outline = {34,85,38,255};
    DrawRectangleGradientV(x, y, w, h, baseLight, baseDark);
    DrawRectangleGradientH(x, y, w, h, Fade(WHITE,0.06f), Fade(BLACK,0.12f));

    float hlW = w*0.18f;
    DrawRectangle(x + w*0.10f, y+2, hlW, h-4, Fade(WHITE,0.22f));
    DrawRectangle(x + w*0.10f + hlW*0.55f, y+4, hlW*0.25f, h-8, Fade(WHITE,0.18f));
    DrawRectangle(x + w - w*0.12f - w*0.06f, y+2, w*0.12f, h-4, Fade(BLACK,0.18f));

    float capH = 16.0f;
    float capY = capAtBottom ? (y+h-capH) : y;
    DrawRectangleGradientV(x-3, capY, w+6, capH, baseDark, baseLight);
    DrawRectangleLines(x-3, capY, w+6, capH, outline);
    DrawRectangleLines(x, y, w, h, outline);
}

void DrawPipes(PipeManager m) {
    for (int i = 0; i < MAX_PIPES; i++) {
        Pipe p = m.pipes[i];
        float x = p.x;

        DrawPipe3D(x, 0, PIPE_WIDTH, p.height, true);

        if (!p.single) {
            float bottomY = p.height + PIPE_GAP;
            float bottomH = (SCREEN_HEIGHT - GROUND_HEIGHT) - bottomY;
            if (bottomH < 0) bottomH = 0;
            DrawPipe3D(x, bottomY, PIPE_WIDTH, bottomH, false);
        }
    }
}

bool CheckCollision(Bird bird, PipeManager m) {
    if (bird.rect.y <= 0 || bird.rect.y + bird.rect.height >= SCREEN_HEIGHT - GROUND_HEIGHT) return true;

    for (int i = 0; i < MAX_PIPES; i++) {
        Pipe p = m.pipes[i];
        Rectangle top = { p.x, 0, PIPE_WIDTH, p.height };

        if (CheckCollisionRecs(bird.rect, top)) return true;

        if (!p.single) {
            Rectangle bottom = { p.x, p.height + PIPE_GAP,
                                 PIPE_WIDTH, SCREEN_HEIGHT - p.height - PIPE_GAP - GROUND_HEIGHT };
            if (CheckCollisionRecs(bird.rect, bottom)) return true;
        }
    }
    return false;
}

