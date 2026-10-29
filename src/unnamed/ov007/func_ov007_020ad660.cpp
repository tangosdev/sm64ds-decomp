//cpp
#include "types.h"
#include "nitro/fx/fx.h"
#include "Ov007MenuState.h"
#include "Ov007Work.h"
#include "cstd.h"

extern "C" {

extern u8 data_ov007_0210342c[];
extern short data_02082214[];

extern void func_ov007_020c92d0(void *a);
extern void func_ov007_020c0638(void *a, int b, int c, int d);
extern void func_ov007_020c02d8(void *a, int b, u16 c);
extern void RequestScreenFade(int a, int b, int c);
extern void func_ov007_020c1d8c(int a);
extern void func_ov007_020ada70(void);

/* The 0x38-byte BG layer record func_ov007_020c0aa8 allocates; screen as in
 * func_ov007_020adc08, scrollY as in func_ov007_020b131c, only the slots this
 * file touches. */
typedef struct Ov007BgLayer {
    char pad_00[4];
    /* 0x04: the BG priority func_ov007_020c03b0 writes into BGnCNT bits 0-1 */
    int priority;
    /* 0x08: func_ov007_020c03b0 uploads and shows the layer only while it is
     * 1 (func_ov007_020c0a44 resets it to 1) */
    int visible;
    /* 0x0c: func_ov007_020c0638 sets it after rebuilding screen;
     * func_ov007_020c076c re-uploads screen while it is set, then clears it */
    int dirty;
    void *screen; /* 0x10: func_ov007_020c076c uploads it to BG screen VRAM */
    char pad_14[0x18];
    int scrollY; /* 0x2c: Q12 */
} Ov007BgLayer;

/* The 0xc-byte record func_ov007_020adc08 returns; names as there. */
typedef struct Ov007LayerSlot {
    Ov007MenuState *state; /* 0x00: func_ov007_020c9388(0) */
    Ov007BgLayer *layer;   /* 0x04 */
    u8 kind;               /* 0x08: func_ov007_020b6d40's slot index, 0..7 */
} Ov007LayerSlot;

/* The 10-byte layout row an item points at (names as in
 * func_ov007_020ae834); only the home row is read here. */
typedef struct Ov007ItemLayout {
    u16 kind;  /* 0x0 */
    s16 homeX; /* 0x2 */
    s16 homeY; /* 0x4 */
} Ov007ItemLayout;

/* The 0x2c-byte record func_ov007_020aea6c allocates; names as in
 * func_ov007_020b5bf0 and func_ov007_020ae2d0, only the slots this file
 * reads. */
typedef struct Ov007Item {
    char pad_00[4];
    Ov007MenuState *state; /* 0x04: func_ov007_020c9388(1) */
    char pad_08[4];
    s32 posY; /* 0x0c: Q12 */
    char pad_10[0x14];
    Ov007ItemLayout *layout; /* 0x24 */
} Ov007Item;

/* The 0x1c record func_ov007_020b4c04 returns; only its item here. */
typedef struct Ov007ItemView {
    Ov007Item *item; /* 0x00 */
} Ov007ItemView;

/* func_ov007_020c9110's animation player; names as in func_ov007_020bc02c. */
typedef struct Ov007AnimPlayer {
    char pad_00[0xc];
    s16 anim; /* 0x0c: index into the owner's animation table; -1 none */
    char pad_0e[2];
    u16 frame; /* 0x10: the whole frame func_ov007_020c9110 last reached */
} Ov007AnimPlayer;

/* The 0xa0-byte model record func_ov007_020bc6d4 allocates; names as in
 * func_ov007_020bc02c, only the slot this file reads. */
typedef struct Ov007Model {
    char pad_00[0x18];
    Ov007AnimPlayer *boneAnim; /* 0x18: func_ov007_020bc894 ticks it */
} Ov007Model;

/* The 0x1c-byte record func_ov007_020beaa0 builds around an Ov007Model;
 * names as in func_ov007_020b0da0. */
typedef struct Ov007Figure {
    char pad_00[4];
    Ov007Model *model; /* 0x04: func_ov007_020bc6d4 */
} Ov007Figure;

typedef char Ov007BgLayer_size_must_be_0x30[sizeof(Ov007BgLayer) == 0x30 ? 1 : -1];
typedef char Ov007LayerSlot_size_must_be_0xc[sizeof(Ov007LayerSlot) == 0xc ? 1 : -1];
typedef char Ov007Item_size_must_be_0x28[sizeof(Ov007Item) == 0x28 ? 1 : -1];

/* The extern keeps this file's u8[] spelling; the manager pointer is stored there. */
#define WORK (*(Ov007Work **)data_ov007_0210342c)

/* Per-frame update of one BG layer slot (func_ov007_020aed98 runs all eight).
 * The parameter keeps the tree's char * spelling; `slot` is the typed local. */
void func_ov007_020ad660(char *arg0)
{
    Ov007LayerSlot *slot = (Ov007LayerSlot *)arg0;
    Ov007Work *mgr;
    Ov007MenuState *temp_r5;
    Ov007MenuState *temp_r4;
    Ov007MenuState *temp_r2;
    int temp_r1;
    u8 state;

    mgr = WORK;
    temp_r5 = mgr->states[0];
    temp_r4 = mgr->states[1];

    if (slot->layer->visible == 0) {
        return;
    }

    func_ov007_020c92d0(slot->state);

    state = slot->kind;
    temp_r2 = slot->state;
    temp_r1 = temp_r2->stateTicks;

    switch (state) {
    case 1:
    case 6:
    {
        Ov007ItemView *m134 = WORK->itemViews[8];
        Ov007Item *obj = m134->item;
        s16 v = obj->state->currentState;
        if ((u32)(u16)(s16)(v - 3) > 1u) {
            return;
        }
        {
            s16 c2 = obj->layout->homeY;
            int c1 = obj->posY >> 0xc;
            func_ov007_020c0638(slot->layer, 0, c1 - c2, 0);
        }
        return;
    }
    case 5:
        if (temp_r5->currentState != 5) {
            return;
        }
        if (temp_r5->stateTicks == 0) {
            func_ov007_020c02d8(slot->layer->screen, 0x18, 0x300);
        }
        if (WORK->paletteState->stateTicks != 0) {
            return;
        }
        RequestScreenFade(9, 0, (int)func_ov007_020ada70);
        func_ov007_020c02d8(slot->layer->screen, 0x18, 0x301);
        slot->layer->dirty = 1;
        func_ov007_020c1d8c(0);
        return;
    case 2:
        switch (temp_r2->currentState) {
        case 0:
        {
            s16 v5;
            s16 t4;
            v5 = temp_r5->currentState;
            if (v5 != 0) {
                return;
            }
            t4 = temp_r4->currentState;
            if (t4 == 0xb && temp_r4->stateTicks == 0) {
                slot->layer->priority = 3;
            } else if (t4 == 0xa && temp_r4->stateTicks == 0) {
                slot->layer->priority = 2;
                slot->state->requestedState = 1;
            }
            if (*(int *)(*(char **)data_ov007_0210342c + 0xec) != 1) {
                return;
            }
            {
                Ov007Model *a1 = WORK->model;
                Ov007AnimPlayer *a2 = a1->boneAnim;
                if (a2->anim != 0) {
                    return;
                }
            }
            {
                Ov007Figure *b1 = WORK->altFigure;
                Ov007Model *b2 = b1->model;
                Ov007AnimPlayer *b3 = b2->boneAnim;
                u16 v = b3->frame;
                if (v == 0x11d || v == 0x12c || v == 0x13d || v == 0x163 || v == 0x178 || v == 0x18f) {
                    slot->state->requestedState = 2;
                }
            }
            return;
        }
        case 1:
        {
            int var_r4;
            if (temp_r1 <= 0) {
                var_r4 = 0;
            } else if (temp_r1 >= 0x50) {
                var_r4 = 0x1000;
            } else {
                var_r4 = cstd::div(temp_r1 << 0xc, 0x50);
            }
            {
                int t = FX_MUL(var_r4, 0xe10);
                int r = cstd::div(t << 0x10, 0x168);
                int inv, inv2, prod, result;
                r = (u16)r;
                inv = 0x1000 - var_r4;
                inv2 = (int)(((long long)inv * inv) >> 0xc);
                inv2 = (int)(((long long)inv2 * inv2) >> 0xc);
                if (inv2 < 0x50) {
                    inv2 = 0;
                }
                prod = data_02082214[(r >> 4) * 2] * 0x19;
                result = FX_MUL(inv2, prod);
                slot->layer->scrollY = result;
            }
            if (var_r4 >= 0x1000) {
                slot->state->requestedState = 0;
            }
            return;
        }
        case 2:
        {
            int var_r0;
            if (temp_r1 <= 0) {
                var_r0 = 0;
            } else if (temp_r1 >= 8) {
                var_r0 = 0x1000;
            } else {
                var_r0 = cstd::div(temp_r1 << 0xc, 8);
            }
            {
                int inv = 0x1000 - var_r0;
                int sq = (int)(((long long)inv * inv) >> 0xc);
                u32 t = (u32)(var_r0 * 0xffff);
                int idx;
                int half;
                short tblval;
                int temp_a;
                int result;
                t = t << 4;
                t = t >> 0x10;
                idx = (int)t >> 4;
                half = sq / 2;
                tblval = data_02082214[idx * 2];
                temp_a = half + 0x800;
                result = FX_MUL(temp_a, tblval);
                slot->layer->scrollY = -result;
            }
            if (var_r0 >= 0x1000) {
                slot->state->requestedState = 0;
            }
            return;
        }
        default:
            return;
        }
    default:
        return;
    }
}
}
