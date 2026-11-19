#include "CaptureEffect.h"
#include "nitro/mi/memory.h"
#include "nitro/snd/interface.h"

extern int NNS_SndCaptureStartEffect(void *buffer, unsigned int size, int format, int rate,
                                     int interval, void *callback, void *arg);

/* Start the sound-capture effect: clear its state (0xc0 bytes at
   data_020a4c70), set the effect kind data_020a4c64 to mode, and for kind
   1 call SNDi_SetSurroundDecay(0x3000) as func_02049c48 does. Then start the
   capture (NNS_SndCaptureStartEffect) on buffer, size bytes, at 32000 Hz,
   interval 2, with func_02049ba8 as the callback and data_020a4c64 as its
   context.
   Sound.cpp's sound setup (func_020133bc) starts it with 0x1000 bytes and
   kind 0. */
void func_02049cd8(void *buffer, int size, int mode) {
    volatile unsigned short fill = 0;
    MI_CpuFill16(&data_020a4c70, fill, sizeof(data_020a4c70));
    data_020a4c64.kind = mode;
    if (mode == 1)
        SNDi_SetSurroundDecay(0x3000);
    NNS_SndCaptureStartEffect(buffer, size, 0, 32000, 2, (void *)func_02049ba8, &data_020a4c64);
}
