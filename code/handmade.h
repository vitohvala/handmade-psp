#ifndef __HANDMADE_H__
#define __HANDMADE_H__

#include <math.h>

#define PI32             3.1415926535897932f

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

internal void game_update_and_render(GameOffscreenBuffer *b,
                                     i32 x_offset, i32 y_offset);
internal void game_output_sound(GameSound *buffer);

#endif
