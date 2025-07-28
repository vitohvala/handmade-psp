/*********************************************************************

    TODO :

        - Saved game locations
        - Getting a handle to our own executable file
        - Asset loading path
        - ??Multithreading (launch a thread)
        - Sleep/timeBeginPeriod
        - QueryCancelAutoplay ?????
        - Hardware acceleration (pspgu)


**********************************************************************/




#include <pspkernel.h>
#include <pspdebug.h>
#include <pspaudiolib.h>
#include <pspaudio.h>
#include <pspdisplay.h>
#include <pspge.h>
#include <pspctrl.h>
#include <psprtc.h>
#include <pspgu.h>

#include <stdint.h>

#include <math.h>



/**********************************/
/*             TYPES              */
/**********************************/
#define global_var static
#define internal static
#define local_persist static

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef size_t   usize;
typedef unsigned int uint;

typedef uint32_t b32;

typedef float f32;
typedef double f64;

#define false 0
#define true 1

#include "handmade.c"

PSP_MODULE_INFO("Handmade Hero", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

#define SCREEN_WIDTH     (480)
#define SCREEN_HEIGHT    (272)
#define BUFF_WIDTH       (512)
#define PIXEL_SIZE       (4)
#define FRAMEBUFFER_SIZE (BUFF_WIDTH * SCREEN_HEIGHT * PIXEL_SIZE)
#define PI32             3.1415926535897932f
//#define SAMPLES_PER_SECOND 48000


/*********************************************************/
/*                      GLOBALS                          */
/*********************************************************/
global_var usize __attribute__((aligned(16))) list[262144];
global_var b32 running;
global_var int samples_per_second;
global_var f32 tsine;



/*********************************************************/
/*                      FUNCTIONS                        */
/*********************************************************/
internal int
exit_callback(int arg1, int arg2, void *common)
{
    running = false;
    //sceKernelExitGame();
    return 0;
}

internal int
callback_thread(SceSize args, void *argp)
{
    int cbid;
    cbid = sceKernelCreateCallback("Exit Callback", exit_callback, 0);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

internal int
setup_callbacks(void)
{
    int thid = 0;
    thid = sceKernelCreateThread("Update Thread", callback_thread, 0x11, 0xFA0, 0, 0);
    if(thid >= 0) {
        sceKernelStartThread(thid, 0, 0);
    }
    return thid;
}


internal void
psp_audio_callback(void *buf, uint length, void *userdata)
{
    f32 tone_hz = 256.0f;
    f32 tone_volume = PSP_VOLUME_MAX - 1.0f;
    int wave_period = samples_per_second / (int)tone_hz;

    i16 *sample_out = (i16*)buf;

    for(int i = 0; i < length; i++) {
        f32 sine_value = sinf(tsine);
        i16 s = (i16)(sine_value * tone_volume);
        *sample_out++ = s;
        *sample_out++ = s;
        tsine += 2.0f * PI32 * (1.0f / (f32)wave_period);

        if(tsine > (2.0f * PI32)) tsine -= (f32)(PI32 * 2.0f);
    }

}

int
main()
{
    setup_callbacks();

    // I just don't want to work with audio on lower level
    pspAudioInit();
    tsine = 0;
    samples_per_second = 48000;
    pspAudioSetChannelCallback(0, psp_audio_callback, 0);

    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    sceGuInit();
    sceGuStart(GU_DIRECT, list);
    sceGuDispBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, (void*)0, BUFF_WIDTH);
    sceGuDrawBuffer(GU_PSM_8888, (void*)FRAMEBUFFER_SIZE, BUFF_WIDTH);
    sceGuDisplay(GU_TRUE);

    sceGuFinish();

    u32 *backbuffer = (u32*)(0x40000000 | 0x04000000);

    i32 x_offset = 0;
    i32 y_offset = 0;
    i32 disp_buffer = 0;

    running = true;

    SceCtrlData pad;

    //TODO: maybe do something else????
    SceKernelSysClock t1;
    sceKernelGetSystemTime(&t1);

    for(;running;) {

        sceCtrlReadBufferPositive(&pad, 1);

        u8 stick_lx = pad.Lx;
        u8 stick_ly = pad.Ly;
        //i16 stick_rx = pad.Rx;
        //i16 stick_ry = pad.Ry;

        if (pad.Buttons != 0){
            b32 square = pad.Buttons & PSP_CTRL_SQUARE;
            b32 triangle = pad.Buttons & PSP_CTRL_TRIANGLE;
            b32 circle = pad.Buttons & PSP_CTRL_CIRCLE;
            b32 cross = pad.Buttons & PSP_CTRL_CROSS;

            b32 up = pad.Buttons & PSP_CTRL_UP;
            b32 down = pad.Buttons & PSP_CTRL_DOWN;
            b32 left = pad.Buttons & PSP_CTRL_LEFT;
            b32 right = pad.Buttons & PSP_CTRL_RIGHT;

            b32 start = pad.Buttons & PSP_CTRL_START;
            b32 select = pad.Buttons & PSP_CTRL_SELECT;

            b32 ltrigger = pad.Buttons & PSP_CTRL_LTRIGGER;
            b32 rtrigger = pad.Buttons & PSP_CTRL_RTRIGGER;

            if (up) y_offset += 20;
            if (down) y_offset -= 20;
            if (right) x_offset += 20;
            if (left) x_offset -= 20;
        }

        u32 *vram = backbuffer;

        if(!disp_buffer) vram += FRAMEBUFFER_SIZE / sizeof(u32);

        //render_weird_gradient(x_offset, y_offset, vram);

        GameOffscreenBuffer ob;
        ob.memory = (void *)vram;
        ob.height = SCREEN_HEIGHT;
        ob.width = SCREEN_WIDTH;
        ob.pitch = BUFF_WIDTH * 4;

        game_update_and_render(&ob, x_offset, y_offset);

        disp_buffer ^= 1;

        SceKernelSysClock t2;
        sceKernelGetSystemTime(&t2);
        u64 elapsed_us = t2.low - t1.low;
        t1 = t2;
    }
    sceGuTerm();
    sceKernelExitGame();
    return 0;
}