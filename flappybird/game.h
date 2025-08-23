#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include <stdbool.h>

// ----------------- Config -----------------
#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 650
#define GROUND_HEIGHT 50

#define GRAVITY 1350.0f
#define FLAP_STRENGTH -300.0f

#define PIPE_WIDTH 80
#define PIPE_GAP 180
#define PIPE_SPACING 300
#define MIN_TOP_HEIGHT 50
#define MAX_PIPES 5

#define MAX_VARIANTS 3
#define FRAMES_PER_VARIANT 16

// ----------------- Types -----------------
typedef struct {
    Rectangle rect;
    float velocityY;
    int frame;
    float frameTimer;
} Bird;

typedef struct {
    float x;
    int height;
    bool single;
    bool passed;
} Pipe;

typedef struct {
    Pipe pipes[MAX_PIPES];
    float scrollSpeed;
    float baseSpeed;
    float acceleration;
} PipeManager;

// ----------------- Bird API -----------------
Bird InitBird(void);
void LoadBirdAssets(void);
void UnloadBirdAssets(void);
void UpdateBird(Bird *bird, float dt, float acceleration);
void DrawBird(Bird bird);

// Active skin (no global exposed)
void SetActiveBirdVariant(int v);   // v in [0..MAX_VARIANTS-1]
int  GetActiveBirdVariant(void);

// ----------------- Pipes API -----------------
PipeManager InitPipes(float startSpeed);
void UpdatePipes(PipeManager *manager, float dt, Bird *bird, int *score);
void DrawPipes(PipeManager manager);
bool CheckCollision(Bird bird, PipeManager pipes);

#endif
