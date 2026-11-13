#include "CaptureEffect.h"
#include "nitro/mi/memory.h"
#include "nitro/snd/interface.h"

extern int _Z13func_020507e0PvjiiiS_S_(void *buffer, unsigned int size, int format, int rate, int interval,
                         void *callback, void *arg);

/* Start the sound-capture effect: clear its state (0xc0 bytes at
   data_020a4c70), set the effect kind data_020a4c64 to mode, and for kind
   1 call SNDi_SetSurroundDecay(0x3000) as func_02049c48 does. Then start the
   capture (func_020507e0) on buffer, size bytes, at 32000 Hz, interval 2,
   with func_02049ba8 as the callback and data_020a4c64 as its context.
   Sound.cpp's sound setup (func_020133bc) starts it with 0x1000 bytes and
   kind 0. */
void func_02049cd8(void *buffer, int size, int mode) {
    volatile unsigned short tmp = 0;
    MI_CpuFill16(&data_020a4c70, tmp, sizeof(data_020a4c70));
    data_020a4c64.kind = mode;
    if (mode == 1)
        _Z21SNDi_SetSurroundDecayi(0x3000);
    _Z13func_020507e0PvjiiiS_S_(buffer, size, 0, 0x7d00, 2, (void *)func_02049ba8, &data_020a4c64);
}
