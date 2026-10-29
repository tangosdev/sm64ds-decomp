// cpp
/* unit020be0dc -- ov007 unit 0x020be0dc..0x020be964: 8 functions, .text only.
 *
 * The title menu's particle-tracking sprites. ov007 is the title and
 * file-select menu program that dScDSMT_c hosts; outside dScDSMT_c it has no
 * ROM RTTI or vtable, so no class name is claimed and the file keeps the
 * unit<addr> name.
 *
 * What the code does. The module owns one 0x108-byte record,
 * data_ov007_02104bc0 (Ov007ParticleSprites): a sprite group on the sub
 * screen, the tracking records func_0204dab4 fills for it, and sixteen OBJ
 * affine matrices. func_ov007_020be50c (called from the init dispatcher,
 * func_ov007_020b7138) builds the group and fills the matrices
 * (func_ov007_020be8b8); func_ov007_020be47c (called from
 * func_ov007_020b6f4c, which tears every menu module down) frees it.
 * func_ov007_020be0dc runs each frame: it pins the group's extra node to
 * the shown figure's bone 0 on screen (func_ov007_020be850 picks the
 * figure), looks up that figure's particle system, and moves each item onto
 * a particle (func_ov007_020be684, whose callback func_ov007_020be7a8
 * projects a particle into screen pixels). func_ov007_020be3bc copies the
 * affine matrices into the sub screen's OAM buffer.
 *
 * Edges: the bss word 0x02104bc0 is referenced by these functions and
 * nothing else in the overlay. The previous function, func_ov007_020be098,
 * is the last of the sound module (bss 0x02104ba4..0x02104bc0); the next
 * one, func_ov007_020be964, references no ov007 data and is called only
 * from func_ov007_020b0a20 and func_ov007_020b2614.
 *
 * Emission order: mwccarm 2004/b56 emits one .text section per function in
 * the reverse of source order, so the highest-address function is written
 * first here and the functions come out in ROM order.
 *
 * The functions keep their ROM names under C linkage.
 */
#include "Ov007MenuState.h"
#include "Ov007Work.h"
#include "cstd.h"
#include "dExtModel_c.h"
#include "types.h"

extern "C" {

/* func_ov007_020c9110's animation player; names as in func_ov007_020bc02c,
 * only the slots this file reads. */
typedef struct Ov007AnimPlayer {
    char pad_00[0xc];
    s16 anim; /* 0x0c: index into the owner's animation table; -1 none */
    char pad_0e[2];
    u16 frame; /* 0x10: the whole frame func_ov007_020c9110 last reached */
} Ov007AnimPlayer;

/* The model record func_ov007_020bc6d4 allocates; names as in
 * func_ov007_020bc02c, only the slots this file reads. */
typedef struct Ov007Model {
    struct ModelComponents *model; /* 0x00 */
    char pad_04[0x14];
    Ov007AnimPlayer *boneAnim; /* 0x18: func_ov007_020bc894 ticks it */
    char pad_1c[0x78];
    int visible; /* 0x94: func_ov007_020bbff0 renders only while it is nonzero */
} Ov007Model;

/* func_ov007_020beaa0's 0x1c-byte figure; func_ov007_020be850 returns one. */
typedef struct Ov007Figure {
    char pad_00;
    u8 figureIndex; /* 0x01: a copy of Ov007Work's figureIndex */
    /* 0x02: func_ov007_020beaa0 seeds it (figure index != 0); this file
   * hands it to func_ov007_020b7690, the particle system lookup */
    u8 particleIndex;
    char pad_03;
    Ov007Model *model; /* 0x04: func_ov007_020bc6d4 */
} Ov007Figure;

/* The 0x44-byte OAM sprite node func_ov007_020c11ac allocates; names as in
 * func_ov007_020c10ec and func_ov007_020ade58, only the slots this file
 * touches. */
typedef struct Ov007OamNode {
    int x;       /* 0x00: Q12 screen x */
    int y;       /* 0x04: Q12 screen y */
    int visible; /* 0x08: func_ov007_020c0b78 draws this node and its children
                  only when set */
    char pad_0c[8];
    u8 priority;    /* 0x14: OAM priority, used when the caller passes none (> 3) */
    s8 affineIndex; /* 0x15: OAM affine matrix, -1 = none */
    char pad_16[2];
    void **anims; /* 0x18: the node's animations (func_ov007_020c11ac) */
    char pad_1c;
    u8 animIndex; /* 0x1d: Sprite_SetAnimation's first argument */
} Ov007OamNode;

typedef char
    Ov007OamNode_animIndex_must_be_at_0x1d[(int)&((Ov007OamNode *)0)->animIndex == 0x1d ? 1 : -1];

/* A func_ov007_020c40d4 record: its first word is its node (as
 * func_ov007_020c3f00 and func_ov007_020c3fe4 read it); margin as in
 * func_ov007_020c3fe4, which shows the node only while it lies within
 * margin pixels of the screen. */
typedef struct Ov007SpriteItem {
    Ov007OamNode *node; /* 0x00 */
    char pad_04[6];
    u8 margin; /* 0x0a */
} Ov007SpriteItem;

typedef char
    Ov007SpriteItem_margin_must_be_at_0xa[(int)&((Ov007SpriteItem *)0)->margin == 0xa ? 1 : -1];

/* The 0x18-byte sprite group func_ov007_020c3f00 builds; names as there. */
typedef struct Ov007SpriteGroup {
    int *layout;             /* 0x00: [0] is the item count */
    Ov007OamNode *root;      /* 0x04: the items' parent */
    Ov007SpriteItem **items; /* 0x08: layout[0] records */
    Ov007SpriteItem *extra;  /* 0x0c: the extra record func_ov007_020c40d4 builds */
} Ov007SpriteGroup;

/* One 0xc-byte particle tracking record of the kind func_0204dab4 fills (as
 * dPa_c's edStarKiraCallback_c::trackingRecords). func_0204dab4 claims a free
 * record for each particle of the system, tagging it with its mask argument
 * (only when that is < 4), and calls the callback, here
 * func_ov007_020be7a8, which writes the particle's screen point. */
typedef struct TrackingRecord TrackingRecord;
struct TrackingRecord {
    short screenX; /* 0x00: func_ov007_020be7a8's first output, in pixels */
    short screenY; /* 0x02: its second output */
    /* 0x04: the callback's 0xfff, scaled down by the particle's age over its
   * lifetime (func_0204dab4); this file turns it into an affine index */
    short lifeScale;
    /* 0x06 bits 0-1: the mask the record was claimed with; this file passes
   * an OAM priority and copies it back into the node */
    unsigned char priority : 2;
    /* 0x06 bits 2-7: bit 2 is set while func_0204dab4 sees the particle */
    unsigned char flags : 6;
    unsigned char pad_07;
    void *particle; /* 0x08: the tracked particle, 0 = free */
};

typedef char TrackingRecord_size_must_be_0xc[sizeof(TrackingRecord) == 0xc ? 1 : -1];

/* The manager's +0x74: the 0x48-byte 2D display record func_ov007_020b7594
 * allocates; names as in func_ov007_020ade58 and func_ov007_020adc08, only
 * the slots this file reads. A bank's +0xc (its array of 0x10-byte
 * animations) stays raw. */
typedef struct Ov007Display {
    void *oamListMain; /* 0x00 */
    void *oamListSub;  /* 0x04 */
    char pad_08[0xc];
    /* 0x14: the sub screen's 0x400-byte OAM buffer; func_ov007_020b7384
   * copies it to 0x07000400 (func_02056674), and +0x10 to the main
   * screen's OAM */
    char *oamSub;
    char pad_18[0x24];
    char **spriteBanks; /* 0x3c: the six func_ov007_020c9688 sprite banks */
} Ov007Display;

typedef char Ov007Display_spriteBanks_must_be_at_0x3c[(int)&((Ov007Display *)0)->spriteBanks == 0x3c
                                                          ? 1
                                                          : -1];

/* NitroSDK's GXOamAttr: one 8-byte OAM entry, its fourth halfword an
 * affine parameter. */
typedef struct GXOamAttr {
    u16 attr0;
    u16 attr1;
    u16 attr2;
    s16 affine;
} GXOamAttr;

/* NitroSDK's GXOamAffine: one OBJ affine matrix, its four Q8 parameters in
 * the fourth halfword of four consecutive OAM entries. */
typedef struct GXOamAffine {
    unsigned short _0[3];
    short PA;
    unsigned short _1[3];
    short PB;
    unsigned short _2[3];
    short PC;
    unsigned short _3[3];
    short PD;
} GXOamAffine;

typedef char GXOamAffine_size_must_be_0x20[sizeof(GXOamAffine) == 0x20 ? 1 : -1];

/* data_ov007_02104bc0: the 0x108-byte record func_ov007_020be50c
 * allocates; medium-confidence name. */
typedef struct Ov007ParticleSprites {
    Ov007SpriteGroup *group;  /* 0x00 */
    TrackingRecord *tracking; /* 0x04: one per item */
    /* 0x08: sixteen OBJ affine matrices (PA, PB, PC, PD in Q12);
   * func_ov007_020be8b8 fills each with a uniform scale */
    int affine[16][4];
} Ov007ParticleSprites;

typedef char
    Ov007ParticleSprites_size_must_be_0x108[sizeof(Ov007ParticleSprites) == 0x108 ? 1 : -1];

extern Ov007ParticleSprites *data_ov007_02104bc0;
extern char *data_ov007_0210342c;
extern int data_ov007_02103230;
extern int data_ov007_02103234;
extern int data_ov007_020d7c6c;
extern u16 data_ov007_020d7c58[];

extern int func_02053200(int x);
extern int func_020538b8(int x, int y);
extern void func_0204dab4(void *sys, TrackingRecord *buf, int count, unsigned int mask, void *cb);
extern void func_0204dc84(char *self);
extern void Sprite_SetAnimation(char *c, int r1, int r2, int r3, int sp0);
extern void *func_ov007_020b7690(int i);
extern void func_ov007_020bfd70(char *actor, void *pos, int *outX, int *outY);
extern void func_ov007_020c3d1c(void *p);
extern int func_ov007_020c3df4(int a, void *b);
extern void func_ov007_020c3e7c(char *c);
extern void func_ov007_020c3ee8(int *c, int x);
extern void *func_ov007_020c3f00(int *r6, void *r4);
extern void func_ov007_020c3fe4(void *self);

/* The extern keeps the tree's char * spelling of the work block; the typed
 * view is reached through a cast macro. */
#define WORK ((Ov007Work *)data_ov007_0210342c)
#define SPRITES data_ov007_02104bc0

// @symbol func_ov007_020be8b8
/* Fills the sixteen affine matrices with uniform scales: matrix i scales by
 * the reciprocal of a Q12 blend from 0x12c (i = 0) to 1 (i = 15). */
void func_ov007_020be8b8(void) {
    int sl;
    for (sl = 0; sl < 0x10; sl++) {
        char *base = (char *)SPRITES->affine;
        int *e = (int *)(base + sl * 0x10);
        int x = cstd::div(sl * 0x1000, 0xf);
        int r3 = 0x1000 - x;
        long long m = (long long)r3 * 0x12c;
        int v = (int)((m + ((long long)x << 12)) >> 12);
        v = func_02053200(v);
        *(int *)(base + sl * 0x10) = v;
        e[1] = 0;
        e[2] = 0;
        e[3] = v;
    }
}

// @symbol func_ov007_020be850
/* Picks a figure: none while the model is not ready; while the model's
 * bone animation is animation 0, the figure (or, from frame 0x7c on with
 * figureIndex 1, the second figure); otherwise the figure only in main
 * state 0xb. */
Ov007Figure *func_ov007_020be850(void) {
    Ov007Work *work = WORK;
    Ov007AnimPlayer *r2;
    Ov007Figure *res = 0;
    if (work->modelReady == 0)
        return res;
    r2 = work->model->boneAnim;
    if (r2->anim == 0) {
        if (r2->frame >= 0x7c) {
            if (work->figureIndex == 1)
                return work->altFigure;
        }
        return work->figure;
    }
    if (work->states[1]->currentState == 0xb)
        res = work->figure;
    return res;
}

// @symbol func_ov007_020be7a8
/* func_0204dab4's callback: projects a particle at pos through camera 0 into
 * screen pixels, lifted by the figure's row (or 0x14b for camera preset
 * 4), with a full life scale. */
void func_ov007_020be7a8(void *pos, unsigned short *outX, unsigned short *outY, int *outScale) {
    int x;
    int y;

    func_ov007_020bfd70((char *)WORK->cameras[0], pos, &x, &y);

    if (WORK->cameraPreset == 4) {
        y = y + 0x14b;
    } else {
        y = y + data_ov007_020d7c58[WORK->figureIndex];
    }

    *outX = (unsigned short)x;
    *outY = (unsigned short)y;
    *outScale = 0xfff;
}

// @symbol func_ov007_020be684
/* Moves each item's node onto the particle it tracks: func_0204dab4 updates
 * the tracking records (claiming new ones at the extra node's priority while
 * that node is visible), then every item shows while its record holds a
 * particle, at the record's screen point. ctx keeps the char * spelling of
 * the definition; `grp` is the typed view. */
void func_ov007_020be684(void *a0, char *ctx) {
    Ov007SpriteGroup *grp = (Ov007SpriteGroup *)ctx;
    int i;
    int r3;
    Ov007OamNode *q;
    TrackingRecord *e;
    Ov007SpriteItem *o;

    q = grp->extra->node;
    if (q->visible != 0)
        r3 = q->priority;
    else
        r3 = 4;
    r3 = r3 & 0xff;
    func_0204dab4(a0, SPRITES->tracking, *grp->layout, r3, (void *)&func_ov007_020be7a8);

    for (i = 0; i < *grp->layout; i++) {
        o = grp->items[i];
        e = SPRITES->tracking + i;
        o->node->visible = (e->particle != 0) ? 1 : 0;
        o->node->x = e->screenX << 12;
        o->node->y = e->screenY << 12;
        o->node->priority = e->priority;
        o->node->affineIndex = (e->lifeScale << 4) >> 0xc;
        if (e->particle != 0)
            func_ov007_020c3fe4(o);
    }
}

// @symbol func_ov007_020be50c
/* Builds the sprite group (and its tracking records) at
 * data_ov007_02104bc0 in the sub OAM list: every item shows sprite bank 2's
 * animation 1 with an 8-pixel margin, the extra record animations 2 and 0
 * with a 0x20-pixel one. */
void func_ov007_020be50c(void) {
    int *t;
    Ov007SpriteGroup *n;
    Ov007SpriteItem *e;
    Ov007OamNode *ef0;
    int *t2;
    int count;
    int i;
    int off;

    t = (int *)WORK->display->spriteBanks;

    SPRITES = (Ov007ParticleSprites *)func_ov007_020c3df4(0, (void *)sizeof(Ov007ParticleSprites));
    SPRITES->group = (Ov007SpriteGroup *)func_ov007_020c3f00(&data_ov007_020d7c6c, (void *)2);

    SPRITES->tracking = (TrackingRecord *)func_ov007_020c3df4(
        (int)&data_ov007_02103234, (void *)(data_ov007_020d7c6c * sizeof(TrackingRecord)));

    func_ov007_020c3ee8((int *)SPRITES->group, (int)WORK->display->oamListSub);

    n = SPRITES->group;
    count = n->layout[0];
    i = 0;
    if (count > 0) {
        off = i;
        do {
            e = n->items[i];
            t2 = (int *)t[2];
            ef0 = e->node;
            ef0->anims[0] = (void *)(t2[3] + 0x10);
            e->margin = 8;
            Sprite_SetAnimation((char *)e->node, 0, 1, 0, 0);
            func_0204dc84((char *)SPRITES->tracking + off);
            n = SPRITES->group;
            i++;
            off += sizeof(TrackingRecord);
            count = n->layout[0];
        } while (i < count);
    }

    e = n->extra;
    e->node->anims[0] = (void *)(((int *)t[2])[3] + 0x20);
    ef0 = e->node;
    ef0->anims[1] = (void *)((int *)t[2])[3];
    e->margin = 0x20;

    func_ov007_020be8b8();
}

// @symbol func_ov007_020be47c
void func_ov007_020be47c(void) {
    if (SPRITES == 0)
        return;
    if (SPRITES->tracking != 0) {
        func_ov007_020c3d1c(SPRITES->tracking);
        SPRITES->tracking = 0;
    }
    if (SPRITES->group != 0) {
        func_ov007_020c3e7c((char *)SPRITES->group);
        SPRITES->group = 0;
    }
    func_ov007_020c3d1c(SPRITES);
    SPRITES = 0;
}

// @symbol func_ov007_020be3bc
/* Once func_ov007_020b7690 has particle systems 0 and 1, copies the sixteen
 * affine matrices into the sub screen's OAM buffer, Q12 down to Q8. */
void func_ov007_020be3bc(void) {
    GXOamAttr *oam = (GXOamAttr *)WORK->display->oamSub;
    int i;
    if (func_ov007_020b7690(0) == 0)
        return;
    if (func_ov007_020b7690(1) == 0)
        return;
    for (i = 0; i < 0x10; i++) {
        int *s = SPRITES->affine[i];
        GXOamAffine *d = (GXOamAffine *)(oam + i * 4);
        d->PA = (short)(s[0] >> 4);
        d->PB = (short)(s[1] >> 4);
        d->PC = (short)(s[2] >> 4);
        d->PD = (short)(s[3] >> 4);
    }
}

/* bone 0's transform */
#define BONE0 (fig->model->model->transforms)

// @symbol func_ov007_020be0dc
/* One frame: pins the extra node to the shown figure's bone 0 on screen
 * (animation by the figure and its frame, size by the bone's tilt, OAM
 * priority by its depth), runs the figure's particle system through the
 * item nodes, and shows the extra node only while the figure's model is
 * visible. Returns whether it is shown. */
int func_ov007_020be0dc(void) {
    int modelVisible = 0;
    Ov007OamNode *node;
    void *system = 0;
    Ov007Figure *fig;
    int val;
    int sp4, sp8;

    node = SPRITES->group->extra->node;
    fig = func_ov007_020be850();

    if (fig == 0) {
        val = data_ov007_02103230;
    } else {
        u8 kind = fig->figureIndex;
        Vector3 *pos;

        switch (kind) {
        case 0:
            if (node->animIndex != 0)
                Sprite_SetAnimation((char *)node, 0, 1, 0, 0);
            break;
        case 1: {
            Ov007AnimPlayer *y = fig->model->boneAnim;
            int idv = y->frame;
            if (idv >= 0x226) {
                if (node->animIndex != 1)
                    Sprite_SetAnimation((char *)node, 1, 1, 0, 0);
            } else {
                if (node->animIndex != 0)
                    Sprite_SetAnimation((char *)node, 0, 1, 0, 0);
            }
        } break;
        }

        pos = &BONE0->t;
        func_ov007_020bfd70((char *)WORK->cameras[0], pos, &sp4, &sp8);

        {
            Ov007Work *P = WORK;
            if (P->cameraPreset == 4) {
                sp8 += 0x14b;
            } else {
                sp8 += data_ov007_020d7c58[P->figureIndex];
            }
        }

        {
            int t0 = (short)sp4 << 0xc;
            int t1 = (short)sp8 << 0xc;
            node->x = t0;
            node->y = t1;
        }

        {
            int *pp = &BONE0->r.m[6];
            int d = func_020538b8(pp[0], pp[2]);
            int e = cstd::div(d << 0xc, 0xffff);
            unsigned int scale = ((e * 0x12) >> 0xc) & 0xff;
            if (scale >= 0x12)
                scale = 0x11;
            if (node->animIndex == 0)
                Sprite_SetAnimation((char *)node, 0, 0, 0, (int)scale);
        }

        if (node->animIndex == 0)
            node->affineIndex = 0xf;
        else
            node->affineIndex = -1;

        if (pos->z > 0)
            node->priority = 0;
        else
            node->priority = 3;

        modelVisible = fig->model->visible;
        val = fig->particleIndex;
    }

    data_ov007_02103230 = val;
    if (val != -1)
        system = func_ov007_020b7690(val);

    if (system != 0) {
        Ov007OamNode *q = SPRITES->group->root;
        if (q->visible == 0)
            q->visible = 1;
        func_ov007_020be684(system, (char *)SPRITES->group);
    } else {
        Ov007OamNode *q = SPRITES->group->root;
        if (q->visible != 0)
            q->visible = 0;
    }

    func_ov007_020c3fe4(SPRITES->group->extra);

    {
        int r0 = node->visible;
        if (r0 != 0 && modelVisible != 0)
            r0 = 1;
        else
            r0 = 0;
        node->visible = r0;
        return r0;
    }
}
}
