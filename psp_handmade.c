#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspge.h>
#include <pspctrl.h>
#include <pspgu.h>

#include <stdint.h>
#include <stdlib.h>



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

#define false 0
#define true 1



PSP_MODULE_INFO("Handmade Hero", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

#define SCREEN_WIDTH     (480)
#define SCREEN_HEIGHT    (272)
#define BUFF_WIDTH       (512)
#define PIXEL_SIZE       (4)
#define FRAMEBUFFER_SIZE (BUFF_WIDTH * SCREEN_HEIGHT * PIXEL_SIZE)


/*********************************************************/
/*                      GLOBALS                          */
/*********************************************************/
global_var usize __attribute__((aligned(16))) list[262144];
global_var b32 running;



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
CallbackThread(SceSize args, void *argp) 
{
    int cbid;
    cbid = sceKernelCreateCallback("Exit Callback", exit_callback, 0);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

internal int 
SetupCallbacks(void) 
{
    int thid = 0;
    thid = sceKernelCreateThread("Update Thread", CallbackThread, 0x11, 0xFA0, 0, 0);
    if(thid >= 0) {
        sceKernelStartThread(thid, 0, 0);
    }
    return thid;
}

int 
main() 
{
    SetupCallbacks();

    sceGuInit();
    sceGuStart(GU_DIRECT, list);
    sceGuDispBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, (void*)0, BUFF_WIDTH);
    sceGuDrawBuffer(GU_PSM_8888, (void*)FRAMEBUFFER_SIZE, BUFF_WIDTH);
    sceGuDisplay(GU_TRUE);

    sceGuFinish();

    u32 *backbuffer = (u32*)(0x40000000 | 0x04000000);

    int x_offset = 0;
    i32 disp_buffer = 0;

    running = true;

    for(;running;) {
        u32 *vram = backbuffer;

        if(!disp_buffer) vram += FRAMEBUFFER_SIZE / sizeof(u32);

        u8 *row = (u8*)vram;
        for(int y = 0; y < SCREEN_HEIGHT; ++y) {
            u32 *pixel = (u32*)row;
            for(int x = 0; x < SCREEN_WIDTH; ++x) {
                u8 green = (u8)(y) * 3;
                u8 blue = (u8)(x + x_offset) * 3;

                *pixel++ = blue << 16 | green << 8;
            }
            row += (BUFF_WIDTH * 4);
        }

        x_offset++;
        disp_buffer ^= 1;   
    }
    sceGuTerm();
    sceKernelExitGame();
    return 0;
}