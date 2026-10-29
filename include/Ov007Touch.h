#ifndef OV007_TOUCH_H
#define OV007_TOUCH_H

#include "types.h"

/* Reconstructed layouts and names, not recovered class identity.
 *
 * Ov007TouchSample is one stylus reading in the shape func_0205efc0 fills
 * (the SDK's x/y/touch/validity record): func_ov007_020c1db0 patches an axis
 * from the previous frame when validity bit 0 (x) or bit 1 (y) is set. */
typedef struct Ov007TouchSample {
    u16 x;        /* 0x0 */
    u16 y;        /* 0x2 */
    u16 touch;    /* 0x4: nonzero while the stylus is down */
    u16 validity; /* 0x6: bit 0 x invalid, bit 1 y invalid */
} Ov007TouchSample;

/* The 0x44-byte touch-panel record func_ov007_020c2068 allocates (cleared by
 * func_ov007_020c2024), stored at the ov007 manager's +0x50 and updated once
 * a frame by func_ov007_020c1db0. Every role below is from c1db0's own
 * reads and writes. */
typedef struct Ov007Touch {
    Ov007TouchSample raw;      /* 0x00: this frame's reading, before patching */
    Ov007TouchSample cur;      /* 0x08: raw with invalid axes patched */
    Ov007TouchSample prev;     /* 0x10: last frame's cur */
    Ov007TouchSample lastDown; /* 0x18: the latest cur with the stylus down */
    u32 heldFrames;            /* 0x20: 0 on the press frame, +1 per held frame */
    u32 releasedFrames;        /* 0x24: 0 on the release frame, +1 per idle frame */
    s32 countdown;             /* 0x28: c1db0 counts it down to 0 every frame */
    u32 flags;                 /* 0x2c: bit 0: c1db0 reports the stylus up until it lifts */
    u16 pressX;                /* 0x30: cur.x on the press frame */
    u16 pressY;                /* 0x32 */
    u16 releaseX;              /* 0x34: cur.x on the release frame */
    u16 releaseY;              /* 0x36 */
    s16 dragX;                 /* 0x38: cur - press while held */
    s16 dragY;                 /* 0x3a */
    s16 deltaX;                /* 0x3c: cur - prev while held */
    s16 deltaY;                /* 0x3e */
    s32 pad_40;                /* 0x40: -1 at init and on each press; no reader found */
} Ov007Touch;

typedef char Ov007Touch_size_must_be_0x44[sizeof(Ov007Touch) == 0x44 ? 1 : -1];

#endif
