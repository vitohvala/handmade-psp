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

/**********************************/
/*             TYPES              */
/**********************************/
#include <stdint.h>

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

typedef enum PspCtrlButtons PspCtrlButtons;

PSP_MODULE_INFO("Handmade Hero", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

#define SCREEN_WIDTH     (480)
#define SCREEN_HEIGHT    (272)
#define BUFF_WIDTH       (512)
#define PIXEL_SIZE       (4)
#define FRAMEBUFFER_SIZE (BUFF_WIDTH * SCREEN_HEIGHT * PIXEL_SIZE)
//#define SAMPLES_PER_SECOND 48000


/*********************************************************/
/*                      GLOBALS                          */
/*********************************************************/
global_var usize __attribute__((aligned(16))) list[262144];
global_var b32 running;
global_var GameSound gsound;


/*********************************************************/
/*                      FUNCTIONS                        */
/*********************************************************/
internal int
psp_exit_callback(int arg1, int arg2, void *common)
{
    running = false;
    //sceKernelExitGame();
    return 0;
}

internal int
psp_callback_thread(SceSize args, void *argp)
{
    int cbid;
    cbid = sceKernelCreateCallback("Exit Callback", psp_exit_callback, 0);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

internal int
psp_setup_callbacks(void)
{
    int thid = 0;
    thid = sceKernelCreateThread("Update Thread", psp_callback_thread,
                                 0x11, 0xFA0, 0, 0);
    if(thid >= 0) {
        sceKernelStartThread(thid, 0, 0);
    }
    return thid;
}


internal void
psp_audio_callback(void *buf, uint length, void *userdata)
{
    gsound.memory = buf;
    gsound.length = length;
    game_output_sound(&gsound);
}

internal void
psp_procces_digital_button(uint psp_button_state, PspCtrlButtons button_bit,
                           GameButtonState *old, GameButtonState *new)
{
    new->ended_down = ((psp_button_state & button_bit) == button_bit);
    new->half_transitionc = (old->ended_down != new->ended_down) ? 1 : 0;
}

int
main()
{
    psp_setup_callbacks();

    // I just don't want to work with audio on lower level
    pspAudioInit();
    gsound.volume = PSP_VOLUME_MAX - 1.0f;
    gsound.samples_per_second = 48000;
    pspAudioSetChannelCallback(0, psp_audio_callback, 0);

    //input
    SceCtrlData pad;
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    sceGuInit();
    sceGuStart(GU_DIRECT, list);
    sceGuDispBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, (void*)0, BUFF_WIDTH);
    sceGuDrawBuffer(GU_PSM_8888, (void*)FRAMEBUFFER_SIZE, BUFF_WIDTH);
    sceGuDisplay(GU_TRUE);
    sceGuFinish();
    u32 *backbuffer = (u32*)(0x40000000 | 0x04000000);

    i32 disp_buffer = 0;

    running = true;

    //TODO: maybe do something else????
    SceKernelSysClock t1;
    sceKernelGetSystemTime(&t1);

    GameInput input[2] = {};
    GameInput *old_input = &input[0];
    GameInput *new_input = &input[1];

    #if 0
        // TODO : try 	pspDebugScreenInitEx
        pspDebugScreenInit();
    #endif

    for(;running;) {

        sceCtrlReadBufferPositive(&pad, 1);

        {
            psp_procces_digital_button(pad.Buttons, PSP_CTRL_SQUARE,
                                       &old_input->square, &new_input->square);
            psp_procces_digital_button(pad.Buttons, PSP_CTRL_TRIANGLE,
                                       &old_input->triangle, &new_input->triangle);
            psp_procces_digital_button(pad.Buttons, PSP_CTRL_CIRCLE,
                                       &old_input->circle, &new_input->circle);
            psp_procces_digital_button(pad.Buttons, PSP_CTRL_CROSS,
                                       &old_input->cross, &new_input->cross);

            psp_procces_digital_button(pad.Buttons, PSP_CTRL_UP,
                                       &old_input->up, &new_input->up);
            psp_procces_digital_button(pad.Buttons, PSP_CTRL_DOWN,
                                       &old_input->down, &new_input->down);
            psp_procces_digital_button(pad.Buttons, PSP_CTRL_LEFT,
                                       &old_input->left, &new_input->left);
            psp_procces_digital_button(pad.Buttons, PSP_CTRL_RIGHT,
                                       &old_input->right, &new_input->right);

            b32 start = pad.Buttons & PSP_CTRL_START;
            b32 select = pad.Buttons & PSP_CTRL_SELECT;

            psp_procces_digital_button(pad.Buttons, PSP_CTRL_LTRIGGER,
                                       &old_input->ltrigger,
                                       &new_input->ltrigger);
            psp_procces_digital_button(pad.Buttons, PSP_CTRL_RTRIGGER,
                                       &old_input->rtrigger,
                                       &new_input->rtrigger);

            f32 sticklx = 0, stickly = 0;
            if (pad.Lx < 128) {
                sticklx = ((f32)pad.Lx - 128.0f) / 128.0f;
            } else {
                sticklx = ((f32)pad.Lx - 128.0f) / 127.0f;
            }

            if (pad.Ly < 128) {
                stickly = ((f32)pad.Ly - 128.0f) / 128.0f;
            } else {
                stickly = ((f32)pad.Ly - 128.0f) / 127.0f;
            }

            new_input->startx = old_input->endx;
            new_input->starty = old_input->endy;

            new_input->minx = new_input->maxx = new_input->endx = sticklx;
            new_input->miny = new_input->maxy = new_input->endy = stickly;

            new_input->is_analog = true;

            #if 0
                pspDebugScreenSetXY(0, 0);
                pspDebugScreenPrintf("%f\n", sticklx);
            #endif
        }

        u32 *vram = backbuffer;

        if(!disp_buffer) vram += FRAMEBUFFER_SIZE / sizeof(u32);

        //render_weird_gradient(x_offset, y_offset, vram);

        GameOffscreenBuffer ob;
        ob.memory = (void *)vram;
        ob.height = SCREEN_HEIGHT;
        ob.width = SCREEN_WIDTH;
        ob.pitch = BUFF_WIDTH * 4;

        game_update_and_render(&ob, new_input);

        disp_buffer ^= 1;

        SceKernelSysClock t2;
        sceKernelGetSystemTime(&t2);
        u64 elapsed_us = t2.low - t1.low;
        t1 = t2;

        GameInput *tmp_input = old_input;
        old_input = new_input;
        new_input = tmp_input;

    }
    sceGuTerm();
    sceKernelExitGame();
    return 0;
}