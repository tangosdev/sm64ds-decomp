/* unit020c1db0 -- ov007 unit 0x020c1db0..0x020c2090: 4 functions, .text + .data.
 *
 * The title menu's touch-panel record: once-per-frame sampling and button-like
 * press/release/drag state, plus its create, clear and destroy entry points.
 * The allocation tag "touch_panel" belongs to this unit. func_ov007_020c2090
 * begins the pad-record unit and is the end edge.
 *
 * Emission order: mwccarm 2004/b56 emits one .text section per function in
 * the reverse of source order, so the highest-address function is written
 * first here and the functions come out in ROM order.
 */
#include "types.h"
#include "nitro/spi/tp.h"
#include "Ov007Touch.h"
#include "Ov007MenuMem.h"

extern char data_ov007_02103240[];
void func_ov007_020c2024(Ov007Touch *t);

// @symbol func_ov007_020c2068
Ov007Touch *func_ov007_020c2068(void) {
    Ov007Touch *t = func_ov007_020c3df4(data_ov007_02103240, sizeof(Ov007Touch));
    func_ov007_020c2024(t);
    return t;
}

// @symbol func_ov007_020c2024
void func_ov007_020c2024(Ov007Touch *t) {
    t->heldFrames = 0;
    t->releasedFrames = 0;
    t->countdown = 0;
    t->pressY = 0;
    t->pressX = t->pressY;
    t->releaseY = 0;
    t->releaseX = t->releaseY;
    t->dragY = 0;
    t->dragX = t->dragY;
    t->flags = 0;
    t->pad_40 = -1;
}

// @symbol func_ov007_020c2018
#pragma long_calls on
void func_ov007_020c2018(void *p) {
    func_ov007_020c3d1c(p);
}
#pragma long_calls off

// @symbol func_ov007_020c1db0
void func_ov007_020c1db0(Ov007Touch *a, TPData *b) {
    a->prev = a->cur;
    a->cur = a->raw;
    if (b == 0) {
        TPData tp;
        /* Copied field by field: `a->raw = tp` compiles to a word copy, 6
         * words off the ROM. */
        TP_RequestCalibratedSampling(&tp);
        a->raw.x = tp.x;
        a->raw.y = tp.y;
        a->raw.touch = tp.touch;
        a->raw.validity = tp.validity;
    } else {
        a->raw = *b;
    }
    a->cur = a->raw;

    /* The clear reads flags through a const view so it reloads the word, as
     * the ROM does; plain `a->flags &= ~1` keeps the tested word live and
     * compiles 4 bytes long. */
    if (a->flags & 1) {
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

char data_ov007_02103240[] = "touch_panel";
