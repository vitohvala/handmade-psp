#ifndef __HANDMADE_H__
#define __HANDMADE_H__

#include <math.h>

#define PI32             3.1415926535897932f

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

typedef struct {
    void *memory;
    u32 width, height;
    u32 pitch;
} GameOffscreenBuffer;

typedef struct {
    void *memory;
    int length;
    int samples_per_second;
    f32 volume;
} GameSound;

typedef struct {
    i32 half_transitionc;
    b32 ended_down;
} GameButtonState;

typedef struct {
    b32 is_analog;

    f32 startx, starty;
    f32 minx, miny;
    f32 maxx, maxy;
    f32 endx, endy;

    GameButtonState up;
    GameButtonState down;
    GameButtonState right;
    GameButtonState left;

    GameButtonState rtrigger;
    GameButtonState ltrigger;

    GameButtonState square;
    GameButtonState triangle;
    GameButtonState circle;
    GameButtonState cross;
} GameInput;

internal void game_update_and_render(GameOffscreenBuffer *b, GameInput *input);
internal void game_output_sound(GameSound *buffer);

#endif
