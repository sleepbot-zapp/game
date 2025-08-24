#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include <stdbool.h>

#define SCREEN_WIDTH 450
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
#define MAX_CLOUDS 10

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

typedef struct {
    Vector2 position;
    float scale;
} Cloud;

Bird InitBird(void);
void LoadBirdAssets(void);
void UnloadBirdAssets(void);
void UpdateBird(Bird *bird, float dt, float scrollSpeed,float acceleration);
void DrawBird(Bird bird);

void SetActiveBirdVariant(int v);
int  GetActiveBirdVariant(void);

PipeManager InitPipes(float startSpeed);
void UpdatePipes(PipeManager *manager, float dt, Bird *bird, int *score);
void DrawPipes(PipeManager manager);
bool CheckCollision(Bird bird, PipeManager pipes);

#endif
