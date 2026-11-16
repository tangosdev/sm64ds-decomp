#ifndef OV007_ITEM_H
#define OV007_ITEM_H

#include "types.h"
#include "Ov007MenuState.h"
#include "Ov007OamNode.h"

/* Reconstructed layouts and names, not recovered class identity: the ov007
 * menu's selectable items, all allocated by func_ov007_020aea6c. Two callers
 * build separate sets: func_ov007_020ade58 builds nine from
 * data_ov007_020ccbc4 with no hit box, each paired with its own OAM node;
 * func_ov007_020b4c04 builds 0x18 item views from data_ov007_020d76e8 with
 * hit boxes, each paired with an Ov007Sprite. func_ov007_020ae834 ticks an
 * item's state. */

/* The 10-byte layout row an item points at (func_ov007_020ade58 passes
 * data_ov007_020ccbc4 + index * 10, func_ov007_020b4c04 data_ov007_020d76e8
 * + index * 10). func_ov007_020ae810 seeds the item position from home;
 * func_ov007_020aeb34 slides it by slide. */
typedef struct Ov007ItemLayout {
    u16 kind;   /* 0x0: func_ov007_020b5bf0 switches on it; func_ov007_020b44ec skips 0xa */
    s16 homeX;  /* 0x2 */
    s16 homeY;  /* 0x4 */
    s16 slideX; /* 0x6 */
    s16 slideY; /* 0x8 */
} Ov007ItemLayout;

/* The 0x14-byte touch rectangles func_ov007_020b4c04 points an item's
 * hitBox at (data_ov007_020d78f8, data_ov007_020d76c0, ...): pixel offsets
 * from the sprite's screen position. No reader of the first word found. */
typedef struct Ov007HitBox {
    char pad_00[4];
    s32 minX; /* 0x04 */
    s32 minY; /* 0x08 */
    s32 maxX; /* 0x0c */
    s32 maxY; /* 0x10 */
} Ov007HitBox;

/* The 0x2c-byte item func_ov007_020aea6c allocates. */
typedef struct Ov007Item {
    u16 id;     /* 0x00: func_ov007_020b4c04's index (func_ov007_020aea6c's first argument) */
    s16 half02; /* 0x02: cleared by func_ov007_020aea6c; no reader found */
    Ov007MenuState *state; /* 0x04: func_ov007_020c9388(1); ticked by func_ov007_020ae834 */
    s32 posX;              /* 0x08: Q12; func_ov007_020ae810 seeds it from the layout's home */
    s32 posY;              /* 0x0c: Q12 */
    /* 0x10: Q12; func_ov007_020ae834 eases it in states 5/6, and the item is
     * selectable once it is fully faded in (>= 0x1000) */
    s32 fade;
    /* 0x14: Q12; func_ov007_020ae834 eases it in states 3/4/7, and
     * func_ov007_020aeb34 slides the item by its square */
    s32 slideOut;
    s32 wiggleTimer;  /* 0x18: func_ov007_020ae834 counts it down */
    s32 wiggleFrames; /* 0x1c: the wiggle's length; func_ov007_020ae2d0 arms 0x3c */
    /* 0x20: shown. func_ov007_020add3c copies it to the OAM node's
     * visible. Only while it is set does func_ov007_020b44ec run
     * func_ov007_020b5bf0, func_ov007_020b4464 draw the sprite and
     * func_ov007_020beeb0 step the swing. func_ov007_020aea6c seeds it,
     * func_ov007_020ae834 sets it on entry to states 0 and 3-7 and clears
     * it on entry to state 1, and func_ov007_020ae2d0 toggles it for item 7 */
    s32 visible;
    Ov007ItemLayout *layout; /* 0x24: func_ov007_020aea6c's second argument */
    Ov007HitBox *hitBox;     /* 0x28: its third */
} Ov007Item;

/* The 8-byte record func_ov007_020ade58 builds: an item and the OAM node
 * drawn for it. func_ov007_020b6d40 keeps nine in Ov007Work::itemNodes. */
typedef struct Ov007ItemNode {
    Ov007Item *item;    /* 0x00: func_ov007_020aea6c */
    Ov007OamNode *node; /* 0x04: func_ov007_020c11ac; Sprite_SetAnimation's target */
} Ov007ItemNode;

struct Ov007Sprite;
struct Ov007Tween;

/* The 0x1c-byte record func_ov007_020b4c04 returns (freed by
 * func_ov007_020b4bd4, stored in Ov007Work's itemViews by
 * func_ov007_020b693c): an item, its sprite, and the slide
 * func_ov007_020b5f64 moves it along, from slideFrom to slideTo by slideT. */
typedef struct Ov007ItemView {
    Ov007Item *item;               /* 0x00 */
    struct Ov007Sprite *sprite;    /* 0x04: func_ov007_020c44e4 */
    s32 pad_08;                    /* 0x08: cleared by func_ov007_020b4c04; no reader found */
    s32 slideT;                    /* 0x0c: slideTween output, 0..0x1000 */
    struct Ov007Tween *slideTween; /* 0x10: func_ov007_020c908c, bound to slideT */
    s16 slideFromX;                /* 0x14 */
    s16 slideFromY;                /* 0x16 */
    s16 slideToX;                  /* 0x18 */
    s16 slideToY;                  /* 0x1a */
#ifdef __cplusplus
    void ItemView_HandleTouch(void *arg1);
#endif
} Ov007ItemView;

#ifndef SM64DS_PLATFORM_PC
typedef char Ov007ItemLayout_size_must_be_0xa[sizeof(Ov007ItemLayout) == 0xa ? 1 : -1];
typedef char Ov007HitBox_size_must_be_0x14[sizeof(Ov007HitBox) == 0x14 ? 1 : -1];
typedef char Ov007Item_size_must_be_0x2c[sizeof(Ov007Item) == 0x2c ? 1 : -1];
typedef char Ov007ItemNode_size_must_be_0x8[sizeof(Ov007ItemNode) == 0x8 ? 1 : -1];
typedef char Ov007ItemView_size_must_be_0x1c[sizeof(Ov007ItemView) == 0x1c ? 1 : -1];
#endif

#endif
