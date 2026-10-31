//cpp
/* unit020b2764 -- ov007 unit 0x020b2764..0x020b2cdc: 4 functions, .text only.
 *
 * The title menu's figure scene: the work block's model, its two figures
 * and the three full-screen quads drawn with them. ov007
 * is the title and file-select menu program that dScDSMT_c hosts; outside
 * dScDSMT_c it has no ROM RTTI or vtable, so no class name is claimed and
 * the file keeps the unit<addr> name.
 *
 * What the code does. func_ov007_020b2998 builds the scene for the work
 * block's figureIndex: it tears down the previous one
 * (func_ov007_020b283c), loads the model (func_ov007_020b2bd4), builds
 * figure and altFigure around it (func_ov007_020beaa0), and builds the
 * three screenSprites, then sets modelReady. func_ov007_020b2bd4 loads the
 * model's file and its two sets of three animation files through the file
 * cache (func_ov007_020b3f20), builds the 0xa0-byte model record
 * (func_ov007_020bc6d4) and binds the animation sets to it.
 * func_ov007_020b283c releases those files and frees everything
 * func_ov007_020b2998 built. func_ov007_020b2764 sets the corner colours
 * of screenSprites[0] and [1] for a Q12 blend and fades [1]'s alpha with
 * it; the transition functions (func_ov007_020b10dc, func_ov007_020b166c,
 * func_ov007_020b1718) call it.
 *
 * Which files are loaded: data_ov007_020ccc48[figureIndex] is the model's
 * file; data_ov007_020ccc68[figureIndex] and data_ov007_020ccc80[set] hold
 * the two sets of three animation files, and data_ov007_020d7ee8[set] a
 * third set of three already in the overlay. set is figureIndex, except
 * that figure 0 at strokeStage 0 uses set 2.
 *
 * Edges: the .rodata tables of the four functions are interleaved, and one
 * unit's .rodata is contiguous: data_ov007_020ccc48 (func_ov007_020b283c,
 * func_ov007_020b2bd4) lies before data_ov007_020ccc50 and
 * data_ov007_020ccc5c (func_ov007_020b2764), and those before
 * data_ov007_020ccc68 and data_ov007_020ccc80 (func_ov007_020b283c,
 * func_ov007_020b2bd4). func_ov007_020b2998 references no ov007 data; it is
 * the one caller of func_ov007_020b2bd4 and builds what func_ov007_020b283c
 * frees. func_ov007_020b2bd4 alone uses the bss words
 * 0x02103348..0x02103360, which end where unit020b2cdc's (0x02103360)
 * begin. The previous function, func_ov007_020b2728, draws a quad
 * (func_ov007_020c421c) and references no ov007 data, so it is left out;
 * the next one, func_ov007_020b2cdc, starts unit020b2cdc.
 *
 * Emission order: mwccarm 2004/b56 emits one .text section per function in
 * the reverse of source order, so the highest-address function is written
 * first here and the functions come out in ROM order.
 *
 * The callees' declarations keep their definitions' spellings (most are
 * still one-function files taking ints and char *), so the typed values
 * are cast where they are handed over. The functions keep their ROM names
 * under C linkage.
 */
#include "types.h"
#include "Ov007Sprite.h"
#include "Ov007Work.h"
#include "Ov007Model.h"
#include "Ov007Figure.h"

extern "C" {

/* One sprite's corner-colour pair (BGR555): `outer` feeds cornerColors[0]
 * and [3], `inner` cornerColors[1] and [2]. */
typedef struct Ov007CornerPair {
    u16 outer;
    u16 inner;
} Ov007CornerPair;

extern int data_ov007_020ccc48[2];             /* [figureIndex]: model file */
extern Ov007CornerPair data_ov007_020ccc50[3]; /* colours at blend 0 */
extern Ov007CornerPair data_ov007_020ccc5c[3]; /* colours at blend 0x1000 */
extern int data_ov007_020ccc68[][3];           /* [figureIndex]: animation files */
extern int data_ov007_020ccc80[3][3];          /* [set]: animation files */
extern void *data_ov007_020d7ee8[3][3];        /* [set]: animation data in the overlay */
extern void *data_ov007_02103348[3];           /* data_ov007_020ccc80[set], loaded */
extern void *data_ov007_02103354[3];           /* data_ov007_020ccc68[figureIndex], loaded */

extern void func_02044efc(void *bmd);
extern void func_02045ef8(void *model, void *value);
extern void func_02046234(void *model, void *value);
extern void func_ov007_020b3edc(int idx);
extern void *func_ov007_020b3f20(int idx);
extern void func_ov007_020b7708(int idx, void *pos);
extern void func_ov007_020bbf98(void *model, int lights);
extern void func_ov007_020bc53c(Ov007Model *model);
extern void func_ov007_020bc5fc(Ov007Model *model, void *files, int count);
extern void func_ov007_020bc658(Ov007Model *model, void *files, int count);
extern void func_ov007_020bc6b4(Ov007Model *model, void *files, int count);
extern Ov007Model *func_ov007_020bc6d4(void *bmd, int flags);
extern void func_ov007_020bea44(Ov007Figure *fig);
extern Ov007Figure *func_ov007_020beaa0(int index, int figureIndex);
extern unsigned short func_ov007_020c3be0(int t, int a, int b);

void func_ov007_020b283c(void);

// @symbol func_ov007_020b2bd4
/* Loads figure figureIndex's model at the given stroke stage and returns
 * its model record: the model file, the two sets of three animation files
 * bound with func_ov007_020bc6b4 and func_ov007_020bc5fc, and the overlay's
 * set bound with func_ov007_020bc658. func_02045ef8 and func_02046234 then
 * set 0x3def and 2 on every material and func_ov007_020bbf98 sets 3 in each
 * one's low four bits. */
Ov007Model *func_ov007_020b2bd4(int figureIndex, int stage) {
    void *file;
    Ov007Model *model;
    int i, set;
    int *setFiles, *figureFiles;

    if (figureIndex == 0 && stage == 0)
        set = 2;
    else
        set = figureIndex;
    file = func_ov007_020b3f20(data_ov007_020ccc48[figureIndex]);
    setFiles = data_ov007_020ccc80[set];
    figureFiles = data_ov007_020ccc68[figureIndex];
    for (i = 0; i < 3; i++) {
        data_ov007_02103348[i] = func_ov007_020b3f20(setFiles[i]);
        data_ov007_02103354[i] = func_ov007_020b3f20(figureFiles[i]);
    }
    func_02044efc(file);
    model = func_ov007_020bc6d4(file, 6);
    func_ov007_020bc6b4(model, data_ov007_02103348, 3);
    func_ov007_020bc5fc(model, data_ov007_02103354, 3);
    func_ov007_020bc658(model, data_ov007_020d7ee8[set], 3);
    func_02045ef8(model->model, (void *)0x3def);
    func_02046234(model->model, (void *)2);
    func_ov007_020bbf98(model, 3);
    return model;
}

// @symbol func_ov007_020b2998
/* Builds the scene for the work block's figureIndex (see the banner) and
 * sets modelReady. The three quads cover the screen at 256 x 256 pixels on
 * camera 1, each a little nearer than the last: [0] a corner-colour fill,
 * [1] textures[9] tiled four times across, [2] plain white. */
void func_ov007_020b2998(void) {
    int i;
    Ov007Sprite *sprite;
    Vector3 pos;

    func_ov007_020b283c();
    WORK->model = func_ov007_020b2bd4(WORK->figureIndex, WORK->strokeStage[WORK->figureIndex]);
    WORK->figure = func_ov007_020beaa0(0, WORK->figureIndex);
    WORK->altFigure = func_ov007_020beaa0(1, WORK->figureIndex);
    pos.x = pos.y = pos.z = 0;
    func_ov007_020b7708(0, &pos);
    func_ov007_020b7708(1, &pos);
    WORK->modelGrowth = 0x1000;
    for (i = 0; i < 3; i++) {
        WORK->screenSprites[i] = func_ov007_020c44e4(1);
        sprite = WORK->screenSprites[i];
        sprite->screenX = 0x80000;
        sprite->screenY = 0x60000;
        sprite->width = sprite->height = 0x100;
        sprite->cornerColors[0] = sprite->cornerColors[3] = 0;
        sprite->cornerColors[1] = sprite->cornerColors[2] = 0;
        switch (i) {
        case 0:
            sprite->polygonId = 0x14;
            sprite->depth = -0x320ff;
            sprite->drawFlags |= 2;
            break;
        case 1:
            sprite->polygonId = 0x15;
            sprite->depth = -0x32000;
            *sprite->frames = WORK->textures[9];
            sprite->texS = 0x20;
            sprite->repeatU = 4;
            sprite->repeatV = 1;
            sprite->drawFlags |= 2;
            break;
        case 2:
            sprite->polygonId = 0x16;
            sprite->depth = -0x31f01;
            sprite->color = 0x7fff;
            sprite->alpha = 0x1000;
            sprite->drawFlags |= 1;
            break;
        }
        func_ov007_020c43bc(sprite, WORK->cameras[1]);
        func_ov007_020c4388(sprite, WORK->cameras[1]);
    }
    WORK->modelReady = 1;
}

// @symbol func_ov007_020b283c
/* Tears down what func_ov007_020b2998 built, if modelReady is set: releases
 * the files func_ov007_020b2bd4 loaded and frees the model, both figures
 * and the three quads. It indexes both animation tables by set; for set 2
 * that reads data_ov007_020ccc68[2], which is data_ov007_020ccc80[0], where
 * the load used data_ov007_020ccc68[0]. */
void func_ov007_020b283c(void) {
    int figureIndex;
    /* 64-bit only for the register allocation: the ROM keeps set in its
     * own register as a copy of figureIndex, and every int spelling tried
     * (if/else, ?:, re-reading the work block, a work-block local) merges
     * the two. */
    long long set;
    int stage;
    int i;

    if (WORK->modelReady == 0)
        return;
    figureIndex = WORK->figureIndex;
    set = figureIndex;
    stage = WORK->strokeStage[figureIndex];
    if (figureIndex == 0 && stage == 0)
        set = 2;
    func_ov007_020b3edc(data_ov007_020ccc48[figureIndex]);
    for (i = 0; i < 3; i++) {
        func_ov007_020b3edc(data_ov007_020ccc80[set][i]);
        func_ov007_020b3edc(data_ov007_020ccc68[set][i]);
    }

    if (WORK->model != 0) {
        func_ov007_020bc53c(WORK->model);
        WORK->model = 0;
    }
    if (WORK->figure != 0) {
        func_ov007_020bea44(WORK->figure);
        WORK->figure = 0;
    }
    if (WORK->altFigure != 0) {
        func_ov007_020bea44(WORK->altFigure);
        WORK->altFigure = 0;
    }
    for (i = 0; i < 3; i++) {
        if (WORK->screenSprites[i] != 0) {
            func_ov007_020c44c4(WORK->screenSprites[i]);
            WORK->screenSprites[i] = 0;
        }
    }

    WORK->modelReady = 0;
}

/* Q12 product, truncated. */
static inline int MulFx(int a, int b) {
    return (int)(((s64)a * b) >> 12);
}

// @symbol func_ov007_020b2764
/* Blends the corner colours of screenSprites[0] and [1] from
 * data_ov007_020ccc50 (t = 0) to data_ov007_020ccc5c (t = 0x1000), and
 * fades [1]'s alpha along a curve: 31 (of 31) at either end, 16 at
 * t = 0x800. */
void func_ov007_020b2764(int t) {
    int i;
    Ov007Sprite *sprite;
    int falling = 0x1000 - (t - 0x800) * 2;
    int rising = t * 2;
    for (i = 0; i < 2; i++) {
        sprite = WORK->screenSprites[i];
        sprite->cornerColors[3] =
            func_ov007_020c3be0(t, data_ov007_020ccc50[i].outer, data_ov007_020ccc5c[i].outer);
        sprite->cornerColors[0] = sprite->cornerColors[3];
        sprite->cornerColors[2] =
            func_ov007_020c3be0(t, data_ov007_020ccc50[i].inner, data_ov007_020ccc5c[i].inner);
        sprite->cornerColors[1] = sprite->cornerColors[2];
        if (i == 1) {
            int ramp = (t >= 0x800) ? falling : rising;
            int curve = MulFx(ramp, 0x2000 - ramp);
            sprite->alpha = (0x1000 - curve) * 0xf + 0x10000;
        }
    }
}
}
