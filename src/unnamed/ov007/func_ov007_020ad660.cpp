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
#include "math/ProgressRatio.h"

extern "C" void func_ov007_020c92d0(Ov007MenuState *state);
extern "C" void func_ov007_020c0638(Ov007BgLayer *layer, int x, int y, u16 tile);
extern "C" void func_ov007_020c02d8(void *a, int b, u16 c);
void RequestScreenFade(int kind, int flags, void (*onEnd)(void));
extern "C" void func_ov007_020c1d8c(int a);
extern void func_ov007_020ada70(void);

/* Per-frame update of one BG layer slot (func_ov007_020aed98 runs all eight). */
void func_ov007_020ad660(Ov007LayerSlot *slot) {
    Ov007MenuState *mainState;
    Ov007MenuState *menuState;
    int ticks;

    mainState = WORK->states[0];
    menuState = WORK->states[1];

    if (slot->layer->visible == 0) {
        return;
    }

    func_ov007_020c92d0(slot->state);

    ticks = slot->state->stateTicks;

    switch (slot->kind) {
    case 1:
    case 6: {
        Ov007Item *obj = WORK->itemViews[8]->item;
        s16 v = obj->state->currentState;
        if (v == 3 || v == 4) {
            func_ov007_020c0638(slot->layer, 0, (obj->posY >> 12) - obj->layout->homeY, 0);
        }
        return;
    }
    case 5:
        if (mainState->currentState != 5) {
            return;
        }
        if (mainState->stateTicks == 0) {
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
        switch (slot->state->currentState) {
        case 0: {
            s16 menu;
            if (mainState->currentState != 0) {
                return;
            }
            menu = menuState->currentState;
            if (menu == 0xb && menuState->stateTicks == 0) {
                slot->layer->priority = 3;
            } else if (menu == 0xa && menuState->stateTicks == 0) {
                slot->layer->priority = 2;
                slot->state->requestedState = 1;
            }
            if (WORK->figureIndex != 1) {
                return;
            }
            if (WORK->model->boneAnim->anim != 0) {
                return;
            }
            {
                u16 frame = WORK->altFigure->model->boneAnim->frame;
                if (frame == 0x11d || frame == 0x12c || frame == 0x13d || frame == 0x163 ||
                    frame == 0x178 || frame == 0x18f) {
                    slot->state->requestedState = 2;
                }
            }
            return;
        }
        case 1: {
            /* ten turns over the step, dying away as (1 - progress)^4 */
            int progress = ProgressRatio(ticks, 0x50);
            u16 angle = FX_DivS32(FX_MUL(progress, 3600) << 16, 360);
            int inv = 0x1000 - progress;
            int amp = (fx32)(((fx64)inv * inv) >> FX32_SHIFT);
            amp = (fx32)(((fx64)amp * amp) >> FX32_SHIFT);
            if (amp < 0x50) {
                amp = 0;
            }
            slot->layer->scrollY = FX_MUL(amp, FX_SinIdx(angle >> 4) * 25);
            if (progress >= 0x1000) {
                slot->state->requestedState = 0;
            }
            return;
        }
        case 2: {
            /* one turn over the step, from full height down to half */
            int progress = ProgressRatio(ticks, 8);
            int inv = 0x1000 - progress;
            int sq = (fx32)(((fx64)inv * inv) >> FX32_SHIFT);
            u16 angle = (u32)(progress * 0xffff) >> 12;
            slot->layer->scrollY = -FX_MUL(sq / 2 + 0x800, FX_SinIdx(angle >> 4));
            if (progress >= 0x1000) {
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
