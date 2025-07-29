#ifndef __HANDMADE_H__
#define __HANDMADE_H__

#include <math.h>

#define PI32             3.1415926535897932f

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#define Kilobytes(Value) ((Value) * 1024LL)
#define Megabytes(Value) (Kilobytes(Value) * 1024LL)
#define Gigabytes(Value) (Megabytes(Value) * 1024LL)
#define Terabytes(Value) (Gigabytes(Value) * 1024LL)

#if HANDMADE_SLOW
#define Assert(Expression) if (!(Expression)) { *(int *)0 = 0; }
#else
#define Assert(Expression)
#endif

typedef struct {
    void *memory;
    u32 width, height;
    u32 pitch;
} GameOffscreenBuffer;

typedef struct {
    void *memory;
    int length;
    int samples_per_second;
    //  should this be max_volume????
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

typedef struct {
    int xoffset, yoffset;
    f32 tone_hz;
} GameState;

typedef struct {
    void *memory;
    usize size;
} Storage;

typedef struct {
    Storage transient;
    Storage permanent;
    GameState state;
    b32 is_init;
} GameMemory;

internal void game_update_and_render(GameMemory *mem,
                                     GameOffscreenBuffer *b,
                                     GameInput *input,
                                     GameSound *sound);
//internal void game_output_sound(GameSound *buffer);

#endif
