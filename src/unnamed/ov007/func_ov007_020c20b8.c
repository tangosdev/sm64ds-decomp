#include "types.h"

/* The 0x70-byte pad record func_ov007_020c2304 allocates and
 * func_ov007_020c22a0 clears; names as in func_ov007_020b2e64 and
 * func_ov007_020b3360. The buttons run up, down, right, left, A, B, START,
 * SELECT, L, R, X, Y. */
typedef struct Ov007Pad {
    u16 keys;     /* 0x00: held this frame */
    u16 prevKeys; /* 0x02: held last frame */
    /* 0x04: frames each button has been held, 1 on the press frame; 0 while
     * it is up */
    u32 heldFrames[12];
    /* 0x34: frames each button has been up, 1 on the release frame; 0 while
     * it is held */
    u32 releasedFrames[12];
    /* 0x64: bit 0 makes this function read no keys until every key is
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

typedef char Ov007Pad_size_must_be_0x70[sizeof(Ov007Pad) == 0x70 ? 1 : -1];

#pragma opt_strength_reduction off
/* Reads the keys (KEYINPUT and the ARM7's X/Y, or *src when given) into the
 * pad, honours inputLock, sets the three axes and steps each button's held
 * and released frame counts. p keeps its u16 * spelling; `pad` is the typed
 * view. */
void func_ov007_020c20b8(u16 *p, u16 *src)
{
    Ov007Pad *pad = (Ov007Pad *)p;
    int flags[13];
    int i;
    u16 keys;
    int za;
    int zb;

    i = 0;
    do { flags[i] = 0; i = i + 1; } while (i < 0xc);
    pad->prevKeys = pad->keys;
    if (src == 0) {
        pad->keys = ((*(volatile u16 *)0x4000130 | *(volatile u16 *)0x27fffa8) ^ 0x2fff) & 0x2fff;
    } else {
        pad->keys = *src;
    }
    if ((pad->inputLock & 1) != 0) {
        if (pad->keys == 0) {
            u32 w = *(volatile u32 *)&pad->inputLock;
            pad->inputLock = w & ~1u;
        } else {
            pad->keys = 0;
        }
    }
    i = 0;
    pad->prevDirX = pad->dirX;
    pad->prevDirY = pad->dirY;
    pad->prevShoulder = pad->shoulder;
    pad->shoulder = (s8)i;
    pad->dirY = pad->shoulder;
    pad->dirX = pad->dirY;
    if ((pad->keys & 0x40) != 0) {
        pad->dirY = 1;
        flags[0] = 1;
    }
    if ((pad->keys & 0x80) != 0) {
        pad->dirY = -1;
        flags[1] = 1;
    }
    if ((pad->keys & 0x10) != 0) {
        pad->dirX = 1;
        flags[2] = 1;
    }
    if ((pad->keys & 0x20) != 0) {
        pad->dirX = -1;
        flags[3] = 1;
    }
    if ((pad->keys & 0x200) != 0) {
        pad->shoulder = 1;
        flags[8] = 1;
    }
    if ((pad->keys & 0x100) != 0) {
        pad->shoulder = -1;
        flags[9] = 1;
    }
    keys = pad->keys;
    i = 0;
    flags[4] = keys & 1; flags[5] = keys & 2; flags[6] = keys & 0x400;
    flags[7] = keys & 0x800; flags[10] = keys & 8; flags[11] = keys & 4;
    za = i; zb = i;
    do {
        if (flags[i] != 0) {
            pad->heldFrames[i]++;
            pad->releasedFrames[i] = zb;
        } else {
            pad->heldFrames[i] = za;
            pad->releasedFrames[i]++;
        }
        i = i + 1;
    } while (i < 0xc);
}
