#include "handmade.h"

global_var f32 tone_hz;

internal void
render_weird_gradient(GameOffscreenBuffer *b, i32 x_offset, i32 y_offset)
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

internal void
game_output_sound(GameSound *buffer)
{
    local_persist f32 tsine;

    //f32 tone_hz = 256.0f;
    int wave_period = buffer->samples_per_second / (int)tone_hz;

    i16 *sample_out = (i16*)buffer->memory;

    for(int i = 0; i < buffer->length; i++) {
        f32 sine_value = sinf(tsine);
        i16 s = (i16)(sine_value * buffer->volume);
        *sample_out++ = s;
        *sample_out++ = s;
        tsine += 2.0f * PI32 * (1.0f / (f32)wave_period);

        if(tsine > (2.0f * PI32)) tsine -= (f32)(PI32 * 2.0f);
    }
}

internal void
game_update_and_render(GameOffscreenBuffer *b, GameInput *input)
{
    local_persist i32 x_offset = 0;
    local_persist i32 y_offset = 0;

    if (input->is_analog)
    {
        x_offset += (int)(8.0f * input->endx);
        tone_hz = 256 + (int)(128.0f * (input->endy));
    }
    else
    {
        // NOTE: Use digital movement tuning
    }
    if(input->down.ended_down)
    {
        y_offset += 1;
    }


    render_weird_gradient(b, x_offset, y_offset);


}