#include "types.h"
#include "Ov007Pad.h"
#include "nitro/pad/pad.h"

/* The pad's inputLock word. Pad_Update clears bit 0 by reading the word
 * again through this accessor: the cartridge reloads it through [r0,#0x64]
 * (ldreq/biceq/streq) after the outer test has already read it.
 * Measured (#821 r20): `&= ~1` and its spellings are a word longer, an
 * (int) cast or int local on a plain read folds the reload into the outer
 * test, and a u32 field or 1-bit bitfield is 0x1ec/0x1f0, not 0x1e8. */
static inline int Pad_GetInputLock(const Ov007Pad *pad) {
    return pad->inputLock;
}

/* Reads the keys (KEYINPUT and the ARM7's X/Y, or *src when given) into the
 * pad, honours inputLock, sets the three axes and steps each button's held
 * and released frame counts. */
void Pad_Update(Ov007Pad *pad, u16 *src) {
    int flags[12];
    int i;
    u16 keys;

    for (i = 0; i < 12; i++) {
        flags[i] = 0;
    }
    pad->prevKeys = pad->keys;
    if (src == 0) {
        pad->keys = PAD_Read();
    } else {
        pad->keys = *src;
    }
    if ((pad->inputLock & 1) != 0) {
        if (pad->keys == 0) {
            pad->inputLock = Pad_GetInputLock(pad) & ~1;
        } else {
            pad->keys = 0;
        }
    }
    pad->prevDirX = pad->dirX;
    pad->prevDirY = pad->dirY;
    pad->prevShoulder = pad->shoulder;
    pad->dirX = pad->dirY = pad->shoulder = 0;
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
    flags[4] = keys & PAD_BUTTON_A;
    flags[5] = keys & PAD_BUTTON_B;
    flags[6] = keys & PAD_BUTTON_X;
    flags[7] = keys & PAD_BUTTON_Y;
    flags[10] = keys & PAD_BUTTON_START;
    flags[11] = keys & PAD_BUTTON_SELECT;
    for (i = 0; i < 12; i++) {
        if (flags[i] != 0) {
            pad->heldFrames[i]++;
            pad->releasedFrames[i] = 0;
        } else {
            pad->heldFrames[i] = 0;
            pad->releasedFrames[i]++;
        }
    }
}
