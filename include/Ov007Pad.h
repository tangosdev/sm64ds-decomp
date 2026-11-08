#ifndef OV007_PAD_H
#define OV007_PAD_H

#include "types.h"

/* A reconstructed layout and name, not recovered class identity. */

/* The 0x70-byte pad record func_ov007_020c2304 allocates and
 * func_ov007_020c22a0 clears; Ov007Work::pad points at it and
 * Pad_Update updates it each frame. heldFrames and releasedFrames
 * keep one counter per button, in the order Pad_Update tests the
 * key bits: up, down, right, left, A, B, X, Y, L, R, START, SELECT. */
typedef struct Ov007Pad {
    u16 keys;     /* 0x00: held this frame */
    u16 prevKeys; /* 0x02: held last frame */
    /* 0x04: frames each button has been held, 1 on the press frame; 0 while
     * it is up */
    u32 heldFrames[12];
    /* 0x34: frames each button has been up, 1 on the release frame; 0 while
     * it is held */
    u32 releasedFrames[12];
    /* 0x64: bit 0 makes Pad_Update read no keys until every key is
     * released, then clears it */
    int inputLock;
    /* 0x68..0x6d: this frame's d-pad x (right 1, left -1), d-pad y (up 1,
     * down -1) and shoulder (L 1, R -1), each beside last frame's */
    s8 dirX;
    s8 prevDirX;
    s8 dirY;
    s8 prevDirY;
    s8 shoulder;
    s8 prevShoulder;
    char pad_6e[2];
} Ov007Pad;

#ifndef SM64DS_PLATFORM_PC
typedef char Ov007Pad_size_must_be_0x70[sizeof(Ov007Pad) == 0x70 ? 1 : -1];
#endif

#endif
