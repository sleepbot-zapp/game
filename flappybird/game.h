#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include <stdbool.h>

#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 600

#define VIRTUAL_WIDTH 400
#define VIRTUAL_HEIGHT 600

#define GROUND_HEIGHT 100

#define BIRD_WIDTH 20
#define BIRD_HEIGHT 20
#define GRAVITY 500
#define FLAP_STRENGTH -200

#define PIPE_WIDTH 50
#define PIPE_GAP 120
#define PIPE_SPACING 200
#define MIN_TOP_HEIGHT 50
#define MAX_PIPES 4

typedef struct Bird {
    Rectangle rect;
    float velocityY;
    int frame;
    float frameTimer;
} Bird;


Bird InitBird(void);
void LoadBirdAssets(void);
void UnloadBirdAssets(void);
void UpdateBird(Bird *bird, float dt);
void DrawBird(Bird bird);

typedef struct {
    float x;
    float height;
    bool single;
    bool passed;
} Pipe;

typedef struct {
    Pipe pipes[MAX_PIPES];
    float scrollSpeed;
} PipeManager;

PipeManager InitPipes(float scrollSpeed);
void UpdatePipes(PipeManager *manager, float dt, Bird *bird, int *score);
void DrawPipes(PipeManager manager);

bool CheckCollision(Bird bird, PipeManager pipes);

#endif
