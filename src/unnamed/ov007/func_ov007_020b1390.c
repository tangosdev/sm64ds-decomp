#include "types.h"
#include "Ov007Sprite.h"
#include "Ov007Work.h"

/* The 0x38-byte BG layer record (func_ov007_020c0aa8); names as in
 * func_ov007_020adc08, only the buffer this file recolours. */
typedef struct Ov007BgLayer {
    char pad_00[0x10];
    void *screen; /* 0x10: func_ov007_020c076c uploads it to BG screen VRAM */
    void *source; /* 0x14: func_ov007_020c0638 rebuilds screen from it */
} Ov007BgLayer;

/* func_ov007_020adc08's 0xc-byte layer slot; names as in that file. */
typedef struct Ov007LayerSlot {
    int state;           /* 0x00: an Ov007MenuState (func_ov007_020c9388) */
    Ov007BgLayer *layer; /* 0x04 */
} Ov007LayerSlot;

/* Names as in func_ov007_020b5bf0 and func_ov007_020ae558. */
typedef struct Ov007ItemLayout {
    u16 kind;  /* 0x0 */
    s16 homeX; /* 0x2: func_ov007_020ae810 seeds posX from it */
} Ov007ItemLayout;

typedef struct Ov007Item {
    char pad_00[8];
    s32 posX; /* 0x08: Q12 */
    s32 posY; /* 0x0c */
    char pad_10[0x14];
    Ov007ItemLayout *layout; /* 0x24 */
} Ov007Item;

/* func_ov007_020ade58's record; names as in func_ov007_020ae558. */
typedef struct Ov007ItemNode {
    Ov007Item *item; /* 0x00: func_ov007_020aea6c */
    void *node;      /* 0x04: func_ov007_020c11ac */
} Ov007ItemNode;

/* func_ov007_020b4c04's record; names as in func_ov007_020b3360. */
typedef struct Ov007ItemView {
    Ov007Item *item;     /* 0x00 */
    Ov007Sprite *sprite; /* 0x04: func_ov007_020c44e4 */
} Ov007ItemView;

extern char *data_ov007_0210342c;
extern u8 data_ov007_020ccc44[];

/* The extern keeps the tree's char * spelling (the declaration-agreement
 * gate); the typed view is reached through a cast macro. */
#define WORK ((Ov007Work *)data_ov007_0210342c)

extern void func_ov007_020c0288(u16 *p, int count, int val);
extern void func_ov007_020c0638(void *a, int b, int c, int d);
void func_ov007_020b1390(int mode) {
    Ov007Work *g = WORK;
    Ov007BgLayer *p6 = g->layerSlots[1]->layer;
    Ov007Item *p5 = g->itemNodes[7]->item;
    int v;
    char *base;
    /* The byte offsets below are rows 4-5, 10-11 and 14-15 of the 32-tile-wide
     * source map, each from column 3. */
    v = data_ov007_020ccc44[(mode == 0) ? (1) : (0)];
    base = (char *)p6->source;
    func_ov007_020c0288((u16 *)(base + 0x106), 0x20, v);
    base = (char *)p6->source;
    func_ov007_020c0288((u16 *)(base + 0x146), 0x20, v);
    v = data_ov007_020ccc44[(mode == 1) ? (1) : (0)];
    base = (char *)p6->source;
    func_ov007_020c0288((u16 *)(base + 0x286), 0x20, v);
    base = (char *)p6->source;
    func_ov007_020c0288((u16 *)(base + 0x2c6), 0x20, v);
    v = data_ov007_020ccc44[(mode == 2) ? (1) : (0)];
    base = (char *)p6->source;
    func_ov007_020c0288((u16 *)(base + 0x386), 0x20, v);
    base = (char *)p6->source;
    func_ov007_020c0288((u16 *)(base + 0x3c6), 0x20, v);
    func_ov007_020c0638(p6, 0, 0, 0);
    switch (mode) {
    case 0: {
        Ov007ItemLayout *q = p5->layout;
        p5->posX = q->homeX << 0xc;
        p5->posY = 0x28000;
        break;
    }

    case 1: {
        Ov007ItemLayout *q = p5->layout;
        p5->posX = q->homeX << 0xc;
        p5->posY = 0x58000;
        break;
    }

    case 3: {
        Ov007ItemView *r2 = WORK->itemViews[8];
        unsigned int o = r2->sprite->width;
        o = o >> 1;
        o = o + 0xc;
        p5->posX = r2->item->posX - (o << 12);
        p5->posY = 0xa2000;
        break;
    }
    }
}
