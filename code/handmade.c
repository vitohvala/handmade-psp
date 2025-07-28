#include "handmade.h"

internal void
game_update_and_render(GameOffscreenBuffer *b, i32 x_offset, i32 y_offset)
{
    u8 *row = (u8*)b->memory;

    for(int y = 0; y < b->height; ++y) {
        u32 *pixel = (u32*)row;
        for(int x = 0; x < b->width; ++x) {
            u8 green = (u8)((y + y_offset) * 2);
            u8 blue = (u8)((x + x_offset) * 2);
            *pixel++ = blue << 16 | green << 8;
        }
        row += (b->pitch);
    }
}