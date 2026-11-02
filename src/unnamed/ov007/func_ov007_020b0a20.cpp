//cpp
#include "types.h"
#include "nitro/fx/fx.h"
#include "cstd.h"
#include "Ov007MenuState.h"
#include "Ov007Touch.h"
#include "Ov007Work.h"
#include "Ov007Item.h"
#include "Ov007LayerSlot.h"
#include "Ov007Pad.h"
#include "Ov007Figure.h"
#include "Ov007Model.h"

extern "C" {


extern int func_ov007_020aebac(void);
extern void RequestScreenFade(int kind, int flags, void (*onEnd)(void));
extern void func_ov007_020b1fa4(int preset);
extern void func_ov007_020b0da0(Ov007Figure *figure);
extern void func_ov007_020be980(Ov007Figure *fig, int anim, int a2);
extern int func_ov007_020be964(Ov007Figure *fig);
extern void func_ov007_020c0638(void *layer, int x, int y, unsigned short tile);
extern void func_ov007_020b797c(void);
extern int func_ov007_020bdeb0(int idx);
extern void func_ov007_020bde70(int idx, int arg);
extern void func_ov007_020b131c(void);

/* states[1]'s state 11 (func_ov007_020b0548), the figure's opening
 * animation. On its first tick it requests screen fade 4, applies camera
 * preset 4, installs func_ov007_020b0da0 as the figure's hook and starts
 * animation 1; with no save file in use the figure's scale follows the
 * animation's frames 0x10..0x14. Once save files exist, a new button press
 * or stylus tap from tick 4 skips to the end. 0x1f frames before the end it
 * requests fade 3, 6 before it hands over to state 10. */
void func_ov007_020b0a20(void) {
    Ov007Model *model;
    int ticks;
    int done;
    int startFade;
    int noSaveFiles;
    Ov007Work *g;

    done = 0;
    startFade = 0;
    g = WORK;
    model = g->figure->model;
    ticks = g->states[1]->stateTicks;
    noSaveFiles = func_ov007_020aebac();

    if (ticks == 0) {
        RequestScreenFade(4, 6, 0);
        func_ov007_020b1fa4(4);
        WORK->figure->update = func_ov007_020b0da0;
        func_ov007_020be980(WORK->figure, 1, 0);
        func_ov007_020be964(WORK->figure);
        func_ov007_020c0638(WORK->layerSlots[6]->layer, 0, 0, 0);
        func_ov007_020b797c();
        model->scale.z = (func_ov007_020aebac() == 0) ? 0xDAC : 0x1770;
        model->scale.y = model->scale.z;
        model->scale.x = model->scale.y;
    }
    if (ticks == 1) {
        if (noSaveFiles != 0) {
            func_ov007_020bdeb0(0x32);
            WORK->itemNodes[8]->item->state->requestedState = 0;
        } else {
            func_ov007_020bdeb0(0x33);
        }
    }
    if (ticks == 0x1E) {
        RequestScreenFade(7, 0x18, 0);
    }
    if (WORK->figure->update == 0) {
        if (model->boneAnim->state != 0) {
            func_ov007_020be980(WORK->figure, 1, 0);
        } else if (noSaveFiles != 0) {
            int v = model->boneAnim->frame;
            if (v <= 0x10) {
                v = 0;
            } else if (v >= 0x14) {
                v = 0x1000;
            } else {
                v = cstd::div((v - 0x10) << 12, 4);
            }
            model->scale.z = 0x1770 - FX_MUL(v, 0x9C4);
            model->scale.y = model->scale.z;
            model->scale.x = model->scale.y;
        }
    }
    if (model->boneAnim->anim != -1 &&
        model->boneAnim->frame == model->boneAnim->frameCount - 0x1F) {
        startFade = 1;
    } else if (model->boneAnim->anim != -1 &&
               model->boneAnim->frame == model->boneAnim->frameCount - 6) {
        done = 1;
    }
    if (((WORK->pad->keys & ~WORK->pad->prevKeys) ||
         (WORK->touch->cur.touch != 0 && WORK->touch->prev.touch == 0)) &&
        ticks >= 4 && noSaveFiles == 0 && model->boneAnim->anim != -1 &&
        model->boneAnim->frame <= model->boneAnim->frameCount - 0x1F) {
        done = 1;
        startFade = 1;
        if (noSaveFiles != 0) {
            func_ov007_020bde70(0x32, 0x1E);
        } else {
            func_ov007_020bde70(0x33, 0x1E);
        }
        func_ov007_020be964(WORK->figure);
        model->boneAnim->framePos = (model->boneAnim->frameCount - 1) << 12;
    }
    if (startFade != 0) {
        RequestScreenFade(3, 0xA, 0);
    }
    if (done == 0) {
        return;
    }
    func_ov007_020b131c();
    WORK->states[1]->requestedState = 0xA;
    WORK->figure->update = 0;
    model->scale.z = 0x1000;
    model->scale.y = model->scale.z;
    model->scale.x = model->scale.y;
    WORK->itemNodes[8]->item->state->requestedState = 1;
}
}
