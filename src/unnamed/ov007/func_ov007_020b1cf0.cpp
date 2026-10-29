//cpp
#include "types.h"
#include "Ov007MenuState.h"
#include "Ov007Touch.h"
#include "Ov007Work.h"
#include "cstd.h"

extern "C" {

extern void func_ov007_020b2614(int a, int b, int c);
extern int func_ov007_020c1da0(int i);
extern void func_ov007_020aec94(int r4);

/* The 0x2c-byte record func_ov007_020aea6c allocates; only the fields this
 * file touches are spelled. */
typedef struct Ov007Item {
    char pad_00[0x10];
    /* 0x10: Q12; func_ov007_020ae834 eases it in states 5/6,
     * func_ov007_020b5bf0 turns it into the sprite alpha */
    s32 fade;
    char pad_14[4];
    s32 wiggleTimer;  /* 0x18: func_ov007_020ae834 counts it down; func_ov007_020b5bf0
                       * swings the sprite's rotZ while it runs */
    s32 wiggleFrames; /* 0x1c: the wiggle's length; func_ov007_020ae2d0 also arms 0x3c */
} Ov007Item;

/* The 0x1c record func_ov007_020b4c04 returns; only its item here. */
typedef struct Ov007ItemView {
    Ov007Item *item; /* 0x00 */
} Ov007ItemView;

/* The 8-byte record func_ov007_020ade58 returns; only its item here. */
typedef struct Ov007ItemNode {
    Ov007Item *item; /* 0x00 */
} Ov007ItemNode;

/* The 0x70-byte key record func_ov007_020c2304 allocates;
 * func_ov007_020c20b8 copies keys into prevKeys, then reads KEYINPUT. */
typedef struct Ov007Pad {
    u16 keys;     /* 0x00: held this frame */
    u16 prevKeys; /* 0x02: held last frame */
} Ov007Pad;

/* The 0x1c-byte animation player func_ov007_020c92ac allocates; names as
 * in func_ov007_020bc02c, only the slots read here. */
typedef struct Ov007AnimPlayer {
    char pad_00[0xc];
    s16 anim; /* 0x0c: index into the owner's animation table; -1 none */
    char pad_0e[2];
    u16 frame;      /* 0x10: the whole frame func_ov007_020c9110 last reached */
    u16 frameCount; /* 0x12 */
} Ov007AnimPlayer;

/* The 0xa0-byte model record func_ov007_020bc6d4 allocates; names as in
 * func_ov007_020bc02c, only the slot read here. */
typedef struct Ov007Model {
    char pad_00[0x18];
    Ov007AnimPlayer *boneAnim; /* 0x18: func_ov007_020bc894 ticks it */
} Ov007Model;

extern char *data_ov007_0210342c;
/* The extern keeps the tree's char * spelling (the declaration-agreement
 * gate); the typed view is reached through a cast macro. */
#define WORK ((Ov007Work *)data_ov007_0210342c)

void func_ov007_020b1cf0(void) {
    Ov007Model *model = WORK->model;
    Ov007MenuState *menu = WORK->states[1];
    int hi = menu->previousState;
    int r5 = menu->stateTicks;
    int flag = 0;

    if (hi == 8 && r5 == 0) {
        int anim;
        WORK->states[2]->requestedState = 3;
        anim = (*(int *)(data_ov007_0210342c + 0xec) == 1) ? 0x3d : 0x14;
        func_ov007_020b2614(0, anim, 8);
    }

    if (r5 >= 0x28) {
        int m = cstd::mod(r5 - 0x28, 0x154);
        Ov007Item *p;
        switch (m) {
        case 0:
            p = WORK->itemNodes[1]->item;
            p->wiggleFrames = 0x3c;
            p->wiggleTimer = p->wiggleFrames;
            break;
        case 0x46:
            p = WORK->itemViews[1]->item;
            p->wiggleFrames = 0x3c;
            p->wiggleTimer = p->wiggleFrames;
            break;
        case 0x8c:
            p = WORK->itemViews[0]->item;
            p->wiggleFrames = 0x3c;
            p->wiggleTimer = p->wiggleFrames;
            break;
        case 0xd2:
            p = WORK->itemViews[2]->item;
            p->wiggleFrames = 0x3c;
            p->wiggleTimer = p->wiggleFrames;
            break;
        }
    }

    if (WORK->itemViews[7]->item->fade == 0x1000 && func_ov007_020c1da0(0) == 0) {
        Ov007Touch *r1 = WORK->touch;
        if (r1->cur.touch != 0 && r1->prev.touch == 0 && r1->releasedFrames >= 1) {
            flag = 1;
            WORK->transitionMode = 1;
            func_ov007_020b2614(1, 0, 8);
        } else {
            Ov007Pad *r1b = WORK->pad;
            if (r1b->keys & 2) {
                if (!(r1b->prevKeys & 2)) {
                    WORK->transitionMode = 0;
                    flag = 1;
                }
            }
        }
    }

    if (flag != 0) {
        WORK->states[1]->requestedState = 4;
        return;
    }

    {
        Ov007AnimPlayer *anim = model->boneAnim;
        if (anim->anim == -1)
            return;
        if ((int)anim->frame != (int)anim->frameCount - 0x22)
            return;
        func_ov007_020aec94(8);
    }
}
}
