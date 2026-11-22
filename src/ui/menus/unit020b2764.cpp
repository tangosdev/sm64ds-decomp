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
 * func_ov007_020b2998 built. FigureScene_SetScreenBlend sets the corner colours
 * of screenSprites[0] and [1] for a Q12 blend and fades [1]'s alpha with
 * it; the transition functions (func_ov007_020b10dc, func_ov007_020b166c,
 * func_ov007_020b1718) call it.
 *
 * Which files are loaded: data_ov007_020ccc48[figureIndex] is the model's
 * file; data_ov007_020ccc68[figureIndex] and data_ov007_020ccc68[set + 2]
 * hold the two sets of three animation files, and data_ov007_020d7ee8[set] a
 * third set of three already in the overlay. set is figureIndex, except
 * that figure 0 at strokeStage 0 uses set 2.
 *
 * Edges: the .rodata tables of the four functions are interleaved, and one
 * unit's .rodata is contiguous: data_ov007_020ccc48 (func_ov007_020b283c,
 * func_ov007_020b2bd4) lies before data_ov007_020ccc50 and
 * data_ov007_020ccc5c (FigureScene_SetScreenBlend), and those before
 * data_ov007_020ccc68 (func_ov007_020b283c,
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
 * are cast where they are handed over.
 * FigureScene_SetScreenBlend's name is coined from its body (the file has
 * no struct of its own; FigureScene_ is coined from this banner); the
 * other three keep their address names for now.
 */
#include "types.h"
#include "Ov007Sprite.h"
#include "Ov007Work.h"
#include "Ov007Model.h"
#include "dExtModel_c.h"
#include "Ov007Figure.h"
#include "private/BTA_File.h"

/* One sprite's corner-colour pair (BGR555): `outer` feeds cornerColors[0]
 * and [3], `inner` cornerColors[1] and [2]. */
typedef struct Ov007CornerPair {
    u16 outer;
    u16 inner;
} Ov007CornerPair;

extern const int data_ov007_020ccc48[2];             /* [figureIndex]: model file */
extern const Ov007CornerPair data_ov007_020ccc50[3]; /* colours at blend 0 */
extern const Ov007CornerPair data_ov007_020ccc5c[3]; /* colours at blend 0x1000 */
extern const int data_ov007_020ccc68[][3]; /* [figureIndex] and [set + 2]: animation files */
/* The three sets of three portable BTA files the model binds directly: the
   record and the arrays its key ranges index. */
extern const BTA_File data_ov007_020cd9b4;
extern const BTA_File data_ov007_020cd9e4;
extern const BTA_File data_ov007_020cda14;
extern const BTA_File data_ov007_020cda2c;
extern const BTA_File data_ov007_020cda5c;
extern const BTA_File data_ov007_020cdabc;
extern const BTA_File data_ov007_020cdb04;
extern "C" fx32 data_ov007_020ccd5c[]; /* 020cd9b4: scales */
extern "C" u16 data_ov007_020ccd68[];  /* 020cd9b4: rotations */
extern "C" fx32 data_ov007_020d042c[]; /* 020cd9b4: translations */
extern "C" BTAMaterialRecord data_ov007_020ce6cc[];
extern "C" fx32 data_ov007_020cfdec[]; /* 020cd9e4: scales */
extern "C" u16 data_ov007_020cccf8[];  /* 020cd9e4: rotations */
extern "C" fx32 data_ov007_020d0194[]; /* 020cd9e4: translations */
extern "C" BTAMaterialRecord data_ov007_020cf0cc[];
extern "C" fx32 data_ov007_020ccd58[]; /* 020cda14: scales */
extern "C" u16 data_ov007_020ccd7c[];  /* 020cda14: rotations */
extern "C" fx32 data_ov007_020ccdb8[]; /* 020cda14: translations */
extern "C" BTAMaterialRecord data_ov007_020ce73c[];
extern "C" fx32 data_ov007_020d072c[]; /* 020cda2c: scales */
extern "C" u16 data_ov007_020ccce8[];  /* 020cda2c: rotations */
extern "C" fx32 data_ov007_020d403c[]; /* 020cda2c: translations */
extern "C" BTAMaterialRecord data_ov007_020cf024[];
extern "C" fx32 data_ov007_020cd03c[]; /* 020cda5c: scales */
extern "C" u16 data_ov007_020cccd4[];  /* 020cda5c: rotations */
extern "C" fx32 data_ov007_020cdb1c[]; /* 020cda5c: translations */
extern "C" BTAMaterialRecord data_ov007_020cf078[];
extern "C" fx32 data_ov007_020ccd60[]; /* 020cdabc: scales */
extern "C" u16 data_ov007_020cccf0[];  /* 020cdabc: rotations */
extern "C" fx32 data_ov007_020ccdac[]; /* 020cdabc: translations */
extern "C" BTAMaterialRecord data_ov007_020ce854[];
extern "C" fx32 data_ov007_020ccd90[]; /* 020cdb04: scales */
extern "C" u16 data_ov007_020cccd8[];  /* 020cdb04: rotations */
extern "C" fx32 data_ov007_020d147c[]; /* 020cdb04: translations */
extern "C" BTAMaterialRecord data_ov007_020ce8c4[];
extern const BTA_File *data_ov007_020d7ee8[3][3]; /* [set]: animation data in the overlay */
extern void *data_ov007_02103348[3];              /* data_ov007_020ccc68[set + 2], loaded */
extern void *data_ov007_02103354[3];              /* data_ov007_020ccc68[figureIndex], loaded */

extern "C" void func_ov007_020b3edc(int idx);
extern "C" void *func_ov007_020b3f20(int idx);
extern void func_ov007_020b7708(int idx, void *pos);
extern "C" void func_ov007_020bbf98(Ov007Model *model, unsigned int lights);
extern void func_ov007_020bc53c(Ov007Model *model);
extern "C" void func_ov007_020bc5fc(Ov007Model *model, void *files, int count);
extern "C" void func_ov007_020bc658(Ov007Model *model, void *files, int count);
extern "C" void func_ov007_020bc6b4(Ov007Model *model, void *files, int count);
extern "C" Ov007Model *func_ov007_020bc6d4(void *bmd, int flags);
extern Ov007Figure *func_ov007_020beaa0(int index, int figureIndex);
extern "C" unsigned short func_ov007_020c3be0(int t, int a, int b);

extern "C" void func_ov007_020b283c(void);

/* Loads figure figureIndex's model at the given stroke stage and returns
 * its model record: the model file, the two sets of three animation files
 * bound with func_ov007_020bc6b4 and func_ov007_020bc5fc, and the overlay's
 * set bound with func_ov007_020bc658. SetAmbient and SetPolygonMode then
 * set 0x3def and 2 (toon) on every material and func_ov007_020bbf98 sets 3 in each
 * one's low four bits. */
Ov007Model *func_ov007_020b2bd4(int figureIndex, int stage) {
    void *file;
    Ov007Model *model;
    int i, set;
    const int *setFiles, *figureFiles;

    if (figureIndex == 0 && stage == 0)
        set = 2;
    else
        set = figureIndex;
    file = func_ov007_020b3f20(data_ov007_020ccc48[figureIndex]);
    setFiles = (&data_ov007_020ccc68[2])[set];
    figureFiles = data_ov007_020ccc68[figureIndex];
    for (i = 0; i < 3; i++) {
        data_ov007_02103348[i] = func_ov007_020b3f20(setFiles[i]);
        data_ov007_02103354[i] = func_ov007_020b3f20(figureFiles[i]);
    }
    ((BMD_File *)file)->func_02044efc();
    model = func_ov007_020bc6d4(file, 6);
    func_ov007_020bc6b4(model, data_ov007_02103348, 3);
    func_ov007_020bc5fc(model, data_ov007_02103354, 3);
    func_ov007_020bc658(model, data_ov007_020d7ee8[set], 3);
    model->model->SetAmbient(0x3def);
    model->model->SetPolygonMode(2);
    func_ov007_020bbf98(model, 3);
    return model;
}

/* Builds the scene for the work block's figureIndex (see the banner) and
 * sets modelReady. The three quads cover the screen at 256 x 256 pixels on
 * camera 1, each a little nearer than the last: [0] a corner-colour fill,
 * [1] textures[9] tiled four times across, [2] plain white. */
extern "C" void func_ov007_020b2998(void) {
    int i;
    Ov007Sprite *sprite;
    Vector3 pos;

    func_ov007_020b283c();
    gMenuWork->model =
        func_ov007_020b2bd4(gMenuWork->figureIndex, gMenuWork->strokeStage[gMenuWork->figureIndex]);
    gMenuWork->figure = func_ov007_020beaa0(0, gMenuWork->figureIndex);
    gMenuWork->altFigure = func_ov007_020beaa0(1, gMenuWork->figureIndex);
    pos.x = pos.y = pos.z = 0;
    func_ov007_020b7708(0, &pos);
    func_ov007_020b7708(1, &pos);
    gMenuWork->modelGrowth = 0x1000;
    for (i = 0; i < 3; i++) {
        gMenuWork->screenSprites[i] = func_ov007_020c44e4(1);
        sprite = gMenuWork->screenSprites[i];
        sprite->screenX = 0x80000;
        sprite->screenY = 0x60000;
        sprite->width = sprite->height = 256;
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
            *sprite->frames = gMenuWork->textures[9];
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
        func_ov007_020c43bc(sprite, gMenuWork->cameras[1]);
        func_ov007_020c4388(sprite, gMenuWork->cameras[1]);
    }
    gMenuWork->modelReady = 1;
}

/* Tears down what func_ov007_020b2998 built, if modelReady is set: releases
 * the files func_ov007_020b2bd4 loaded and frees the model, both figures
 * and the three quads. It indexes both animation sets by set; for set 2
 * data_ov007_020ccc68[2] is the set the load took from (&data_ov007_020ccc68[2])[0],
 * where the load used data_ov007_020ccc68[0]. */
void func_ov007_020b283c(void) {
    int figureIndex;
    int set;
    int stage;
    int i;

    if (gMenuWork->modelReady == 0)
        return;
    figureIndex = gMenuWork->figureIndex;
    stage = gMenuWork->strokeStage[figureIndex];
    set = figureIndex;
    if (figureIndex == 0 && stage == 0)
        set = 2;
    func_ov007_020b3edc(data_ov007_020ccc48[figureIndex]);
    {
        const int *setFiles = (&data_ov007_020ccc68[2])[set];
        const int *figureFiles = data_ov007_020ccc68[set];
        for (i = 0; i < 3; i++) {
            func_ov007_020b3edc(setFiles[i]);
            func_ov007_020b3edc(figureFiles[i]);
        }
    }

    if (gMenuWork->model != 0) {
        func_ov007_020bc53c(gMenuWork->model);
        gMenuWork->model = 0;
    }
    if (gMenuWork->figure != 0) {
        gMenuWork->figure->func_ov007_020bea44();
        gMenuWork->figure = 0;
    }
    if (gMenuWork->altFigure != 0) {
        gMenuWork->altFigure->func_ov007_020bea44();
        gMenuWork->altFigure = 0;
    }
    for (i = 0; i < 3; i++) {
        if (gMenuWork->screenSprites[i] != 0) {
            func_ov007_020c44c4(gMenuWork->screenSprites[i]);
            gMenuWork->screenSprites[i] = 0;
        }
    }

    gMenuWork->modelReady = 0;
}

/* Q12 product, truncated. */
static inline int MulFx(int a, int b) {
    return (int)(((s64)a * b) >> 12);
}

/* Blends the corner colours of screenSprites[0] and [1] from
 * data_ov007_020ccc50 (t = 0) to data_ov007_020ccc5c (t = 0x1000), and
 * fades [1]'s alpha along a curve: 31 (of 31) at either end, 16 at
 * t = 0x800. */
extern "C" void FigureScene_SetScreenBlend(int t) {
    int i;
    Ov007Sprite *sprite;
    int falling = 0x1000 - (t - 0x800) * 2;
    int rising = t * 2;
    for (i = 0; i < 2; i++) {
        sprite = gMenuWork->screenSprites[i];
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

extern const int data_ov007_020ccc48[2] = {
    1,
    2,
};
extern const Ov007CornerPair data_ov007_020ccc50[3] = {
    0x5fbf, 0x6bff, 0x537d, 0x579e, 0x0, 0x0,
};
extern const Ov007CornerPair data_ov007_020ccc5c[3] = {
    0x7d8a, 0x7fd5, 0x69a4, 0x6b2b, 0x0, 0x0,
};
extern const int data_ov007_020ccc68[5][3] = {
    16, 17, 18, 19, 20, 21, 4, 5, 7, 8, 9, 10, 4, 6, 7,
};
extern const BTA_File data_ov007_020cd9b4 = {
    0x30, 0, data_ov007_020ccd5c, data_ov007_020ccd68, data_ov007_020d042c,
    2,    0, data_ov007_020ce6cc,
};
extern const BTA_File data_ov007_020cd9e4 = {
    0x29, 0, data_ov007_020cfdec, data_ov007_020cccf8, data_ov007_020d0194,
    3,    0, data_ov007_020cf0cc,
};
extern const BTA_File data_ov007_020cda14 = {
    0x30, 0, data_ov007_020ccd58, data_ov007_020ccd7c, data_ov007_020ccdb8,
    2,    0, data_ov007_020ce73c,
};
extern const BTA_File data_ov007_020cda2c = {
    0x354, 0, data_ov007_020d072c, data_ov007_020ccce8, data_ov007_020d403c,
    3,     0, data_ov007_020cf024,
};
extern const BTA_File data_ov007_020cda5c = {
    2, 0, data_ov007_020cd03c, data_ov007_020cccd4, data_ov007_020cdb1c, 3, 0, data_ov007_020cf078,
};
extern const BTA_File data_ov007_020cdabc = {
    2, 0, data_ov007_020ccd60, data_ov007_020cccf0, data_ov007_020ccdac, 2, 0, data_ov007_020ce854,
};
extern const BTA_File data_ov007_020cdb04 = {
    0x2bc, 0, data_ov007_020ccd90, data_ov007_020cccd8, data_ov007_020d147c,
    2,     0, data_ov007_020ce8c4,
};

const BTA_File *data_ov007_020d7ee8[3][3] = {
    {&data_ov007_020cdb04, &data_ov007_020cd9b4, &data_ov007_020cdabc},
    {&data_ov007_020cda2c, &data_ov007_020cd9e4, &data_ov007_020cda5c},
    {&data_ov007_020cdb04, &data_ov007_020cda14, &data_ov007_020cdabc},
};
