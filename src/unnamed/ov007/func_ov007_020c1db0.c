#include "types.h"
#include "Ov007Touch.h"

extern void func_0205f040(void);
extern void func_0205efc0(void *c);

/* Updates the touch record once a frame from a stylus reading (b, or a
 * fresh calibrated sample when b is null): patches invalid axes from the
 * previous frame, then tracks press/release positions, held/idle frame
 * counts and the drag and per-frame deltas. */
void func_ov007_020c1db0(Ov007Touch *a, Ov007TouchSample *b) {
    a->prev = a->cur;
    a->cur = a->raw;
    if (b == 0) {
        u16 tmp[4];
        func_0205f040();
        func_0205efc0(tmp);
        a->raw.x = tmp[0];
        a->raw.y = tmp[1];
        a->raw.touch = tmp[2];
        a->raw.validity = tmp[3];
    } else {
        a->raw = *b;
    }
    a->cur = a->raw;

    if (*(volatile unsigned int *)&a->flags & 1) {
        if (a->cur.touch == 0)
            a->flags = ((const Ov007Touch *)a)->flags & ~1;
        else
            a->cur.touch = 0;
    }

    if (a->cur.touch != 0) {
        if (a->cur.validity & 1) {
            if (a->prev.touch != 0)
                a->cur.x = a->prev.x;
            else
                a->cur.touch = 0;
        }
        if (a->cur.validity & 2) {
            if (a->prev.touch != 0)
                a->cur.y = a->prev.y;
            else
                a->cur.touch = 0;
        }
    }

    if (a->cur.touch != 0) {
        a->lastDown = a->cur;
        if (a->prev.touch == 0) {
            a->pressX = a->cur.x;
            a->pressY = a->cur.y;
            a->heldFrames = 0;
            a->dragY = 0;
            a->dragX = a->dragY;
            a->pad_40 = -1;
            a->deltaY = 0;
            a->deltaX = a->deltaY;
        } else {
            a->heldFrames += 1;
            a->dragX = a->cur.x - a->pressX;
            a->dragY = a->cur.y - a->pressY;
            a->deltaX = a->cur.x - a->prev.x;
            a->deltaY = a->cur.y - a->prev.y;
        }
    } else {
        if (a->prev.touch == 0) {
            a->releasedFrames += 1;
        } else {
            a->releasedFrames = 0;
            a->releaseX = a->cur.x;
            a->releaseY = a->cur.y;
        }
    }

    a->countdown -= 1;
    if (a->countdown < 0)
        a->countdown = 0;
}
