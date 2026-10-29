//cpp
#include "types.h"
#include "nitro/fx/fx.h"
#include "Ov007MenuState.h"
#include "Ov007Sprite.h"
#include "Ov007Work.h"
#include "cstd.h"

extern "C" {

extern short data_02082214[];
extern u8 data_ov007_0210342c[];

/* The 10-byte layout row func_ov007_020aea6c's item points at (func_ov007_020b4c04
 * passes data_ov007_020d76e8 + index * 10). func_ov007_020ae810 seeds the item
 * position from home, func_ov007_020aeb34 slides it by slide. */
typedef struct Ov007ItemLayout {
    u16 kind;   /* 0x0: this file switches on it */
    s16 homeX;  /* 0x2 */
    s16 homeY;  /* 0x4 */
    s16 slideX; /* 0x6 */
    s16 slideY; /* 0x8 */
} Ov007ItemLayout;

/* The 0x2c-byte record func_ov007_020aea6c allocates; only the fields this
 * file touches are spelled. */
typedef struct Ov007Item {
    u16 id;                  /* 0x00: aea6c's first argument (func_ov007_020b4c04's index) */
    char pad_02[2];          /* 0x02 */
    Ov007MenuState *state;   /* 0x04: func_ov007_020c9388; ticked by func_ov007_020ae834 */
    char pad_08[8];          /* 0x08 */
    s32 fade;                /* 0x10: Q12; func_ov007_020ae834 eases it in states 5/6 */
    s32 slideOut;            /* 0x14: Q12; func_ov007_020ae834 eases it in states 3/4/7 and
                            * func_ov007_020aeb34 slides the item by its square */
    s32 wiggleTimer;         /* 0x18: func_ov007_020ae834 counts it down */
    s32 wiggleFrames;        /* 0x1c: the wiggle's length; func_ov007_020ae2d0 arms 0x3c */
    char pad_20[4];          /* 0x20 */
    Ov007ItemLayout *layout; /* 0x24 */
} Ov007Item;

/* The 0x1c record func_ov007_020b4c04 returns (names as in that file). */
typedef struct Ov007ItemView {
    Ov007Item *item;               /* 0x00 */
    Ov007Sprite *sprite;           /* 0x04: func_ov007_020c44e4 */
    char pad_08[8];                /* 0x08 */
    struct Ov007Tween *slideTween; /* 0x10: func_ov007_020c908c */
} Ov007ItemView;

/* The extern keeps this file's u8[] spelling; the manager pointer is stored there. */
#define WORK (*(Ov007Work **)data_ov007_0210342c)

extern unsigned short func_ov007_020c3be0(int t, int a, int b);
extern void func_ov007_020b5f64(void *a);
extern void func_ov007_020c8f98(int a);

void func_ov007_020b5bf0(Ov007ItemView *view) {
    Ov007Item *r7 = view->item;
    int r6 = 0;
    int r5 = r6;
    int r4 = r6;

    if (r7->layout->kind == 0) {
        if (r7->wiggleTimer > 0) {
            int a = r7->wiggleTimer;
            int diff = r7->wiggleFrames - a;
            int sb;
            if (diff <= 0) {
                sb = r6;
            } else if (diff >= r7->wiggleFrames) {
                sb = 0x1000;
            } else {
                sb = cstd::div(diff << 0xc, r7->wiggleFrames);
            }
            {
                int t1 = (sb * (r7->wiggleFrames * 0x48)) >> 0xc;
                int r = cstd::div(t1 << 0x10, 0x168);
                int idx;
                short tblval;
                int inv;
                u16 typeField;
                int r2;
                int mulfactor;
                int result;

                r = (u16)r;
                idx = (r >> 4) * 2;
                tblval = data_02082214[idx];
                inv = 0x1000 - sb;
                typeField = r7->id;
                r2 = FX_MUL(tblval, inv);
                mulfactor = (typeField == 0) ? 4 : 8;
                result = (r2 * mulfactor) >> 0xc;
                view->sprite->rotZ = cstd::div(result << 0x10, 0x168);
            }
        } else {
            view->sprite->rotZ = r6;
        }
    }

    {
        u16 switchVal = r7->layout->kind;
        int var_r0_2;

        switch (switchVal) {
        case 0:
        case 2:
        case 6:
            r5 = 1;
            break;
        case 4:
        {
            s16 v = view->item->state->currentState;
            if ((u32)(u16)(s16)(v - 3) <= 1u) {
                r4 = 1;
            } else {
                r5 = 1;
            }
            break;
        }
        case 8:
            r4 = 1;
            break;
        case 7:
        {
            u16 typeField = r7->id;
            char *obj = *(char **)WORK->settings;
            s16 v = r7->state->currentState;

            if ((v == 2 && (typeField == 9 || (typeField == 0xc && *(u8 *)(obj + 9) != 0) ||
                            (typeField == 0xd && *(u8 *)(obj + 9) == 0))) ||
                (typeField == 0xc && *(u8 *)(obj + 9) == 0) ||
                (typeField == 0xd && *(u8 *)(obj + 9) != 0)) {
                Ov007Sprite *m4 = view->sprite;
                m4->texS = m4->width;
            } else if (v != 2 && (typeField == 9 || (typeField == 0xc && *(u8 *)(obj + 9) != 0) ||
                                  (typeField == 0xd && *(u8 *)(obj + 9) == 0))) {
                view->sprite->texS = 0;
            }
            break;
        }
        case 3:
        {
            s16 v = view->item->state->currentState;
            if ((u32)(u16)(s16)(v - 3) <= 1u) {
                r4 = 1;
            } else {
                r6 = 1;
            }
            break;
        }
        default:
            break;
        }

        var_r0_2 = (r4 != 0) ? (0x1000 - r7->slideOut) : r7->fade;

        if (r5 != 0 || r4 != 0) {
            if (var_r0_2 >= 0x1000) {
                view->sprite->alpha = 0x1f000;
            } else if (var_r0_2 <= 0) {
                view->sprite->alpha = 0;
            } else {
                view->sprite->alpha = FX_MUL(var_r0_2, 0x1f000);
            }
        }

        if (r6 != 0) {
            if (var_r0_2 >= 0x1000) {
                view->sprite->color = 0x7fff;
            } else if (var_r0_2 <= 0) {
                view->sprite->color = 0x4210;
            } else {
                view->sprite->color = func_ov007_020c3be0(var_r0_2, 0x4210, 0x7fff);
            }
        }
    }

    if ((u32)(u16)(r7->id + 0xfff2) <= 2u) {
        func_ov007_020b5f64(view);
    }
    func_ov007_020c8f98((int)view->slideTween);
}
}
