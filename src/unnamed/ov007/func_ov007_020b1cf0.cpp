//cpp
#include "types.h"
#include "Ov007MenuState.h"
#include "Ov007Touch.h"
#include "Ov007Work.h"
#include "Ov007Item.h"
#include "Ov007Model.h"
#include "Ov007Pad.h"
#include "nitro/fx/fx_cp.h"

extern "C" void func_ov007_020b2614(int a, int b, int c);
extern int func_ov007_020c1da0(int i);
extern "C" void func_ov007_020aec94(int r4);

extern "C" void func_ov007_020b1cf0(void) {
    Ov007Model *model = gMenuWork->model;
    Ov007MenuState *menu = gMenuWork->states[1];
    int hi = menu->previousState;
    int r5 = menu->stateTicks;
    int flag = 0;

    if (hi == 8 && r5 == 0) {
        int anim;
        gMenuWork->states[2]->requestedState = 3;
        anim = (gMenuWork->figureIndex == 1) ? 0x3d : 0x14;
        func_ov007_020b2614(0, anim, 8);
    }

    if (r5 >= 40) {
        int m = FX_ModS32(r5 - 40, 340);
        Ov007Item *p;
        switch (m) {
        case 0:
            p = gMenuWork->itemNodes[1]->item;
            p->wiggleFrames = 60;
            p->wiggleTimer = p->wiggleFrames;
            break;
        case 70:
            p = gMenuWork->itemViews[1]->item;
            p->wiggleFrames = 60;
            p->wiggleTimer = p->wiggleFrames;
            break;
        case 140:
            p = gMenuWork->itemViews[0]->item;
            p->wiggleFrames = 60;
            p->wiggleTimer = p->wiggleFrames;
            break;
        case 210:
            p = gMenuWork->itemViews[2]->item;
            p->wiggleFrames = 60;
            p->wiggleTimer = p->wiggleFrames;
            break;
        }
    }

    if (gMenuWork->itemViews[7]->item->fade == 0x1000 && func_ov007_020c1da0(0) == 0) {
        Ov007Touch *r1 = gMenuWork->touch;
        if (r1->cur.touch != 0 && r1->prev.touch == 0 && r1->releasedFrames >= 1) {
            flag = 1;
            gMenuWork->transitionMode = 1;
            func_ov007_020b2614(1, 0, 8);
        } else {
            Ov007Pad *r1b = gMenuWork->pad;
            if (r1b->keys & 2) {
                if (!(r1b->prevKeys & 2)) {
                    gMenuWork->transitionMode = 0;
                    flag = 1;
                }
            }
        }
    }

    if (flag != 0) {
        gMenuWork->states[1]->requestedState = 4;
        return;
    }

    {
        Ov007AnimPlayer *anim = model->boneAnim;
        if (anim->anim == -1)
            return;
        if (anim->frame != anim->frameCount - 0x22)
            return;
        func_ov007_020aec94(8);
    }
}
