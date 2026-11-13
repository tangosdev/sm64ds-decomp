//cpp
#include "types.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "Ov007MenuState.h"
#include "Ov007Sprite.h"
#include "Ov007Work.h"
#include "Ov007Settings.h"
#include "Ov007Item.h"
#include "nitro/fx/fx_cp.h"
#include "math/ProgressRatio.h"

extern "C" unsigned short func_ov007_020c3be0(int t, int a, int b);
extern "C" void func_ov007_020b5f64(Ov007ItemView *view);
extern "C" int func_ov007_020c8f98(struct Ov007Tween *tw);

/* One frame of a menu item's look. Kind 0 items wiggle: rotZ follows a
 * sine at 72 degrees per frame, decaying over wiggleFrames, 4 degrees deep
 * for item 0 and 8 for the rest. The kind then picks how the item's level
 * shows as the sprite's alpha: fade for kinds 0, 2 and 6; 1 - slideOut for
 * kind 8 in any state and for kinds 3 and 4 in states 3 and 4; otherwise
 * kind 4 fades and kind 3 shows fade as its colour, from grey (0x4210) to
 * white. Kind 7 moves the texture origin by width for the backlight
 * buttons: the one that would change the setting (item 0xc, turn on, while
 * backlightOn is clear; 0xd, turn off, while it is set) always, and item 9
 * and the button for the current setting only in state 2. Items 0xe..0x10
 * also run func_ov007_020b5f64, and the slide tween steps. */
void func_ov007_020b5bf0(Ov007ItemView *view) {
    Ov007Item *item = view->item;
    int dims = 0;
    int fades = 0;
    int slides = 0;

    if (item->layout->kind == 0) {
        if (item->wiggleTimer > 0) {
            int timer = item->wiggleTimer;
            int elapsed = item->wiggleFrames - timer;
            int progress;
            progress = ProgressRatio(elapsed, item->wiggleFrames);
            {
                int degrees = (progress * (item->wiggleFrames * 0x48)) >> 0xc;
                int angle = FX_DivS32(degrees << 0x10, 0x168);
                fx16 sine;
                int decay;
                u16 id;
                int wave;
                int amplitude;
                int tilt;

                angle = (u16)angle;
                sine = FX_SinIdx(angle >> 4);
                decay = 0x1000 - progress;
                id = item->id;
                wave = FX_MUL(sine, decay);
                amplitude = (id == 0) ? 4 : 8;
                tilt = (wave * amplitude) >> 0xc;
                view->sprite->rotZ = FX_DivS32(tilt << 0x10, 0x168);
            }
        } else {
            view->sprite->rotZ = 0;
        }
    }

    {
        u16 kind = item->layout->kind;
        int level;

        switch (kind) {
        case 0:
        case 2:
        case 6:
            fades = 1;
            break;
        case 4: {
            s16 v = view->item->state->currentState;
            if (v == 3 || v == 4) {
                slides = 1;
            } else {
                fades = 1;
            }
            break;
        }
        case 8:
            slides = 1;
            break;
        case 7: {
            u16 id = item->id;
            Ov007BootArgs *bootArgs = WORK->settings->bootArgs;
            s16 v = item->state->currentState;

            if ((v == 2 && (id == 9 || (id == 0xc && bootArgs->backlightOn != 0) ||
                            (id == 0xd && bootArgs->backlightOn == 0))) ||
                (id == 0xc && bootArgs->backlightOn == 0) ||
                (id == 0xd && bootArgs->backlightOn != 0)) {
                Ov007Sprite *sprite = view->sprite;
                sprite->texS = sprite->width;
            } else if (v != 2 && (id == 9 || (id == 0xc && bootArgs->backlightOn != 0) ||
                                  (id == 0xd && bootArgs->backlightOn == 0))) {
                view->sprite->texS = 0;
            }
            break;
        }
        case 3: {
            s16 v = view->item->state->currentState;
            if (v == 3 || v == 4) {
                slides = 1;
            } else {
                dims = 1;
            }
            break;
        }
        default:
            break;
        }

        level = (slides != 0) ? (0x1000 - item->slideOut) : item->fade;

        if (fades != 0 || slides != 0) {
            if (level >= 0x1000) {
                view->sprite->alpha = 0x1f000;
            } else if (level <= 0) {
                view->sprite->alpha = 0;
            } else {
                view->sprite->alpha = FX_MUL(level, 0x1f000);
            }
        }

        if (dims != 0) {
            if (level >= 0x1000) {
                view->sprite->color = 0x7fff;
            } else if (level <= 0) {
                view->sprite->color = 0x4210;
            } else {
                view->sprite->color = func_ov007_020c3be0(level, 0x4210, 0x7fff);
            }
        }
    }

    if ((u32)(u16)(item->id + 0xfff2) <= 2u) {
        func_ov007_020b5f64(view);
    }
    func_ov007_020c8f98(view->slideTween);
}
