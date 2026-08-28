#include "types.h"

// Button bit -> index into the held/released frame counters:
//   0 Up (0x40)     1 Down (0x80)   2 Right (0x10)  3 Left (0x20)
//   4 A  (0x01)     5 B    (0x02)   6 X     (0x400) 7 Y    (0x800)
//   8 L  (0x200)    9 R    (0x100) 10 Start (0x08) 11 Select (0x04)
typedef struct Ov007Input {
    u16 held;                // 0x00
    u16 prev;                // 0x02
    int heldFrames[12];      // 0x04
    int releasedFrames[12];  // 0x34
    vs32 flags;              // 0x64
    s8 dirX;                 // 0x68
    s8 dirXPrev;             // 0x69
    s8 dirY;                 // 0x6a
    s8 dirYPrev;             // 0x6b
    s8 dirLR;                // 0x6c
    s8 dirLRPrev;            // 0x6d
} Ov007Input;

void func_ov007_020c20b8(Ov007Input *p, u16 *src)
{
    int pressed[12];
    int i;

    for (i = 0; i < 12; i++)
        pressed[i] = 0;

    p->prev = p->held;
    if (src != 0) {
        p->held = *src;
    } else {
        p->held = ((*(u16 *)0x04000130 | *(u16 *)0x027fffa8) ^ 0x2fff) & 0x2fff;
    }

    if (p->flags & 1) {
        if (p->held == 0)
            p->flags = p->flags & ~1;
        else
            p->held = 0;
    }

    p->dirXPrev = p->dirX;
    p->dirYPrev = p->dirY;
    p->dirLRPrev = p->dirLR;
    p->dirX = p->dirY = p->dirLR = 0;

    if (p->held & 0x40) {
        p->dirY = 1;
        pressed[0] = 1;
    }
    if (p->held & 0x80) {
        p->dirY = -1;
        pressed[1] = 1;
    }
    if (p->held & 0x10) {
        p->dirX = 1;
        pressed[2] = 1;
    }
    if (p->held & 0x20) {
        p->dirX = -1;
        pressed[3] = 1;
    }
    if (p->held & 0x200) {
        p->dirLR = 1;
        pressed[8] = 1;
    }
    if (p->held & 0x100) {
        p->dirLR = -1;
        pressed[9] = 1;
    }

    pressed[4] = p->held & 1;
    pressed[5] = p->held & 2;
    pressed[6] = p->held & 0x400;
    pressed[7] = p->held & 0x800;
    pressed[10] = p->held & 8;
    pressed[11] = p->held & 4;

    for (i = 0; i < 12; i++) {
        if (pressed[i] != 0) {
            p->heldFrames[i]++;
            p->releasedFrames[i] = 0;
        } else {
            p->heldFrames[i] = 0;
            p->releasedFrames[i]++;
        }
    }
}
