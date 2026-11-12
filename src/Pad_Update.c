#include "types.h"
#include "Ov007Pad.h"
#include "nitro/pad/pad.h"

/* Reads the keys (KEYINPUT and the ARM7's X/Y, or *src when given) into the
 * pad, honours inputLock, sets the three axes and steps each button's held
 * and released frame counts. */
void Pad_Update(Ov007Pad *pad, u16 *src) {
    int flags[13];
    int i;
    u16 keys;
    int za;
    int zb;

    i = 0;
    do { flags[i] = 0; i = i + 1; } while (i < 0xc);
    pad->prevKeys = pad->keys;
    if (src == 0) {
        pad->keys = PAD_Read();
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
    if ((pad->keys & PAD_KEY_UP) != 0) {
        pad->dirY = 1;
        flags[0] = 1;
    }
    if ((pad->keys & PAD_KEY_DOWN) != 0) {
        pad->dirY = -1;
        flags[1] = 1;
    }
    if ((pad->keys & PAD_KEY_RIGHT) != 0) {
        pad->dirX = 1;
        flags[2] = 1;
    }
    if ((pad->keys & PAD_KEY_LEFT) != 0) {
        pad->dirX = -1;
        flags[3] = 1;
    }
    if ((pad->keys & PAD_BUTTON_L) != 0) {
        pad->shoulder = 1;
        flags[8] = 1;
    }
    if ((pad->keys & PAD_BUTTON_R) != 0) {
        pad->shoulder = -1;
        flags[9] = 1;
    }
    keys = pad->keys;
    i = 0;
    flags[4] = keys & PAD_BUTTON_A;
    flags[5] = keys & PAD_BUTTON_B;
    flags[6] = keys & PAD_BUTTON_X;
    flags[7] = keys & PAD_BUTTON_Y;
    flags[10] = keys & PAD_BUTTON_START;
    flags[11] = keys & PAD_BUTTON_SELECT;
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
