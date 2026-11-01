#include "types.h"
#include "Ov007Pad.h"

/* Reads the keys (KEYINPUT and the ARM7's X/Y, or *src when given) into the
 * pad, honours inputLock, sets the three axes and steps each button's held
 * and released frame counts. */
void func_ov007_020c20b8(Ov007Pad *pad, u16 *src) {
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
