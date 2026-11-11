//cpp
#include "types.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "Ov007MenuState.h"
#include "Ov007Work.h"
#include "Ov007BgLayer.h"
#include "Ov007Item.h"
#include "Ov007Model.h"
#include "Ov007LayerSlot.h"
#include "Ov007Figure.h"
#include "nitro/fx/fx_cp.h"

extern "C" {

extern void func_ov007_020c92d0(Ov007MenuState *state);
extern void func_ov007_020c0638(Ov007BgLayer *layer, int x, int y, u16 tile);
extern void func_ov007_020c02d8(void *a, int b, u16 c);
extern void RequestScreenFade(int kind, int flags, void (*onEnd)(void));
extern void func_ov007_020c1d8c(int a);
extern void func_ov007_020ada70(void);

/* Per-frame update of one BG layer slot (func_ov007_020aed98 runs all eight). */
void func_ov007_020ad660(Ov007LayerSlot *slot) {
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
    case 6: {
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
        RequestScreenFade(9, 0, func_ov007_020ada70);
        func_ov007_020c02d8(slot->layer->screen, 0x18, 0x301);
        slot->layer->dirty = 1;
        func_ov007_020c1d8c(0);
        return;
    case 2:
        switch (temp_r2->currentState) {
        case 0: {
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
            if (WORK->figureIndex != 1) {
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
                if (v == 0x11d || v == 0x12c || v == 0x13d || v == 0x163 || v == 0x178 ||
                    v == 0x18f) {
                    slot->state->requestedState = 2;
                }
            }
            return;
        }
        case 1: {
            int var_r4;
            if (temp_r1 <= 0) {
                var_r4 = 0;
            } else if (temp_r1 >= 0x50) {
                var_r4 = 0x1000;
            } else {
                var_r4 = FX_DivS32(temp_r1 << 0xc, 0x50);
            }
            {
                int t = FX_MUL(var_r4, 0xe10);
                int r = FX_DivS32(t << 0x10, 0x168);
                int inv, inv2, prod, result;
                r = (u16)r;
                inv = 0x1000 - var_r4;
                inv2 = (int)(((long long)inv * inv) >> 0xc);
                inv2 = (int)(((long long)inv2 * inv2) >> 0xc);
                if (inv2 < 0x50) {
                    inv2 = 0;
                }
                prod = FX_SinIdx(r >> 4) * 0x19;
                result = FX_MUL(inv2, prod);
                slot->layer->scrollY = result;
            }
            if (var_r4 >= 0x1000) {
                slot->state->requestedState = 0;
            }
            return;
        }
        case 2: {
            int var_r0;
            if (temp_r1 <= 0) {
                var_r0 = 0;
            } else if (temp_r1 >= 8) {
                var_r0 = 0x1000;
            } else {
                var_r0 = FX_DivS32(temp_r1 << 0xc, 8);
            }
            {
                int inv = 0x1000 - var_r0;
                int sq = (int)(((long long)inv * inv) >> 0xc);
                u32 t = (u32)(var_r0 * 0xffff);
                int half;
                fx16 tblval;
                int temp_a;
                int result;
                t = t << 4;
                t = t >> 0x10;
                half = sq / 2;
                tblval = FX_SinIdx((int)t >> 4);
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
