#include "types.h"
#include "Ov007Pad.h"
#include "nitro/pad/pad.h"

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
            /* Kept (#821, measured 2026-11-16 against ov007 0x020c20b8,
               0x1e8). The ROM reloads inputLock and clears bit 0 through
               [r0,#0x64] (ldreq/biceq/streq).
                 `&= ~1`, `= inputLock & ~1`, `&= 0xfffffffe`, `^= 1`, or
                   an empty else: 0x1ec, +1 word (addeq r2,r0,#0x64, then
                   ldr/str through r2).
                 `-= 1`: 0x1ec, 4 words replaced.
                 if/else arms swapped: 0x1ec, 7 words differ.
                 `= inputLock & ~1u`, an int local first, or an (int) cast
                   on the read: 0x1e4, -1 word (the reload is CSE'd into
                   the outer test).
                 u32 field: 0x1ec. 1-bit bitfield: 0x1f0.
                 (#821 r20) signed 1-bit bitfield: 0x1f0. A union of the
                   int word and a 1-bit field: 0x1ec when the word is
                   tested (clearing the bit or the word), 0x1f0 when the
                   bit is tested.
               ov007's menu (unit020b2cdc.cpp) sets bit 0 of its own
               inputLock with the same reload-through-a-cast shape. The cast is
               what forces the reload; a `volatile` qualifier on it changes
               nothing. */
            u32 w = *(u32 *)&pad->inputLock;
            pad->inputLock = w & ~1u;
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
