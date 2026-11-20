#include "CaptureEffect.h"
#include "nitro/mi/memory.h"
#include "nitro/snd/interface.h"

extern int NNS_SndCaptureStartEffect(void *buffer, unsigned int size, int format, int rate,
                                     int interval, void *callback, void *arg);

/* Start the sound-capture effect: clear its state (0xc0 bytes at
   gCaptureEffect.work), set the effect kind gCaptureEffect to mode, and for kind
   1 call SNDi_SetSurroundDecay(0x3000) as func_02049c48 does. Then start the
   capture (NNS_SndCaptureStartEffect) on buffer, size bytes, at 32000 Hz,
   interval 2, with func_02049ba8 as the callback and gCaptureEffect as its
   context.
   Sound.cpp's sound setup (func_020133bc) starts it with 0x1000 bytes and
   kind 0. */
void func_02049cd8(void *buffer, int size, int mode) {
    volatile unsigned short fill = 0;
    MI_CpuFill16(&gCaptureEffect.work, fill, sizeof(gCaptureEffect.work));
    gCaptureEffect.kind = mode;
    if (mode == 1)
        SNDi_SetSurroundDecay(0x3000);
    NNS_SndCaptureStartEffect(buffer, size, 0, 32000, 2, (void *)func_02049ba8, &gCaptureEffect);
}
