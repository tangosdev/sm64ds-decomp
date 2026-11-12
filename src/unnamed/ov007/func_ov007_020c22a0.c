#include "Ov007Pad.h"
#include "Ov007MenuMem.h"

extern char data_ov007_0210324c[]; /* "pad" */
extern void func_ov007_020c22a0(Ov007Pad *pad);

/* Allocates the pad record Ov007Work::pad points at and clears it. */
Ov007Pad *func_ov007_020c2304(void) {
    Ov007Pad *pad = func_ov007_020c3df4(data_ov007_0210324c, sizeof(Ov007Pad));
    func_ov007_020c22a0(pad);
    return pad;
}

/* Long-branch veneer: long_calls emits the pooled `ldr ip,[pc]; bx ip`
 * absolute tail-call; bracketed because the pragma is positional. */
#pragma long_calls on
void func_ov007_020c22f8(void *p) {
    func_ov007_020c3d1c(p);
}
#pragma long_calls off

/* Clears the pad: keys, the three axes, every frame count and inputLock. */
void func_ov007_020c22a0(Ov007Pad *pad) {
    int i;
    pad->prevKeys = 0;
    pad->keys = pad->prevKeys;
    pad->prevDirX = 0;
    pad->dirX = pad->prevDirX;
    pad->prevDirY = 0;
    pad->dirY = pad->prevDirY;
    pad->prevShoulder = 0;
    pad->shoulder = pad->prevShoulder;
    for (i = 0; i < 12; i++) {
        pad->heldFrames[i] = 0;
        pad->releasedFrames[i] = 0;
    }
    pad->inputLock = 0;
}
