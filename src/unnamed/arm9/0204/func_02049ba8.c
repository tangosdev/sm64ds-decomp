#include "CaptureEffect.h"

/* The sound-capture effect callback. func_02049cd8 passes it to
   func_020507e0 (StartEffect-shaped: buffer, size, format, 32000 Hz,
   interval 2, callback, arg) with &data_020a4c64 as ctx. It gets the
   captured left and right buffers, the length in bytes and the format. The
   optional user callback runs first, then the built-in effect for ctx->kind:
   0 none, 1 func_020499bc, 2 func_020497a0. func_02049c48 switches the kind,
   and Sound.cpp's func_02011d5c calls it with the same 0/1/2 value. */
void func_02049ba8(void *left, void *right, unsigned int len, int format, CaptureEffect *ctx) {
    if (ctx->fn)
        ctx->fn(left, right, len, format, ctx->arg);
    switch (ctx->kind) {
    case 0:
        return;
    case 1:
        _Z13func_020499bcPsS_jP13CaptureEffect(left, right, len, ctx);
        return;
    case 2:
        _Z13func_020497a0PsS_jP13CaptureEffect(left, right, len, ctx);
        return;
    }
}
