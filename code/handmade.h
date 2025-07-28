#ifndef __HANDMADE_H__
#define __HANDMADE_H__

typedef struct {
    void *memory;
    u32 width, height;
    u32 pitch;
} GameOffscreenBuffer;


internal void game_update_and_render(GameOffscreenBuffer *b,
                                     i32 x_offset, i32 y_offset);
#endif
