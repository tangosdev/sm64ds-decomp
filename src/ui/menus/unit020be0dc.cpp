//cpp
/* unit020be0dc -- ov007 unit 0x020be0dc..0x020bed60: 14 functions, .text only.
 *
 * The title menu's figures and the particle-tracking sprites that follow
 * them. ov007 is the title and
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
 * The figures are the two 0x1c-byte Ov007Figure records the work block
 * holds (figure, altFigure). func_ov007_020beaa0 builds one around an
 * Ov007Model and func_ov007_020bea44 frees it; func_ov007_020be9ac poses it
 * each frame and func_ov007_020beb80 then aims its particle system along
 * bone 0's motion; func_ov007_020be980 starts an animation on it and
 * func_ov007_020be964 holds that animation.
 *
 * Edges: the bss word 0x02104bc0 is referenced by 0x020be0dc..0x020be964
 * and nothing else in the overlay. The .rodata tables put the figure
 * functions in the same unit: data_ov007_020d7c5c (func_ov007_020bea44,
 * func_ov007_020beaa0) lies between data_ov007_020d7c58
 * (func_ov007_020be0dc, func_ov007_020be7a8) and data_ov007_020d7c6c
 * (func_ov007_020be50c), and one unit's .rodata is contiguous.
 * func_ov007_020beaa0 alone uses the bss array 0x02104bc4..0x02104bd4,
 * right after 0x02104bc0. func_ov007_020beb80 references no ov007 data; its
 * one caller is func_ov007_020be9ac and it works on the same figure record.
 * The previous function, func_ov007_020be098, is the last of the sound
 * module (bss 0x02104ba4..0x02104bc0); the next one, func_ov007_020bed60,
 * starts unit020bed60 (bss 0x02104bd4).
 *
 * Emission order: mwccarm 2004/b56 emits one .text section per function in
 * the reverse of source order, so the highest-address function is written
 * first here and the functions come out in ROM order.
 *
 * The functions keep their ROM names under C linkage.
 */
#include "Ov007MenuState.h"
#include "Ov007Work.h"
#include "Ov007Display.h"
#include "Ov007SpriteGroup.h"
#include "cstd.h"
#include "Vector3s.h"
#include "dExtModel_c.h"
#include "nitro/fx/fx.h"
#include "types.h"
#include "Ov007Model.h"
#include "Ov007Figure.h"
#include "Particle__System.h"

extern "C" {

/* Vector3 declares a constructor and destructor under C++, which makes its
 * assignment copy word by word. The ROM copies these with C's ldm/stm block
 * move, which C++ keeps for a struct with an array member. */
typedef struct Vec3Words {
    Fix12i w[3];
} Vec3Words;

/* The same for Vector3s's six-byte halfword copy. */
typedef struct Vec3sWords {
    s16 w[3];
} Vec3sWords;

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
extern int data_ov007_02103230;
extern char data_ov007_02103234[]; /* "2DEffect" */
extern int data_ov007_020d7c6c;
extern u16 data_ov007_020d7c58[];

extern int func_02053200(int x);
extern int func_020538b8(int x, int y);
extern void func_0204dab4(void *sys, TrackingRecord *buf, int count, unsigned int mask, void *cb);
extern void func_0204dc84(char *self);
extern void Sprite_SetAnimation(Ov007OamNode *node, int animIndex, int mode, int rev, int frame);
extern void *func_ov007_020b7690(int i);
extern void func_ov007_020bfd70(struct Ov007Camera *cam, void *pos, int *outX, int *outY);
extern void func_ov007_020c3d1c(void *p);
extern void *func_ov007_020c3df4(const char *name, int size);
extern void func_ov007_020c3e7c(Ov007SpriteGroup *grp);
extern void func_ov007_020c3ee8(Ov007SpriteGroup *grp, Ov007OamList *list);
extern Ov007SpriteGroup *func_ov007_020c3f00(int *layout, int extraAnims);
extern void func_ov007_020c3fe4(Ov007SpriteItem *item);

/* the figure and its files */
extern int data_ov007_020d7c5c[2][2]; /* [figureIndex][index]: animation file */
extern void *data_ov007_02104bc4[2][2];
extern int func_02053644(s16 *v);
extern void func_02053380(s16 *in, s16 *out);
extern void func_ov007_020b3edc(int idx);
extern void *func_ov007_020b3f20(int idx);
extern int func_ov007_020aebac(void);
extern void func_ov007_020bc02c(Ov007Model *model, void *viewMatrix);
extern void func_ov007_020bc3dc(void *model, int anim, int a2);
extern void func_ov007_020bc53c(Ov007Model *model);
extern void func_ov007_020bc6b4(Ov007Model *model, void *files, int count);
extern Ov007Model *func_ov007_020bc6d4(void *file, int flags);

#define SPRITES data_ov007_02104bc0

// @symbol func_ov007_020beb80
/* Points the figure's particle system along bone 0's motion: on the frames
 * the model steps, refreshes the figure's yaw and remembered position; while
 * the model is shown and its animation is not finished, moves the system to
 * the bone and aims it along the motion since the last step, faster and
 * spawning more often the faster the bone moves; otherwise hides it. */
void func_ov007_020beb80(Ov007Figure *fig) {
    struct ModelComponents *comps = fig->model->model;
    Matrix4x3 *bone0 = comps->transforms;
    int *row = &bone0->r.m[6];
    int yaw;
    Vector3s motion;
    Particle::System *sys;
    int frames;
    int vel;
    int speed;
    Vector3 *pos = &bone0->t;
    yaw = func_020538b8(row[0], row[2]);
    motion.x = (short)(pos->x - fig->prevPos.x);
    motion.y = (short)(pos->y - fig->prevPos.y);
    motion.z = (short)(pos->z - fig->prevPos.z);
    if (fig->model->animTick != 0) {
        fig->yaw = yaw;
        *(Vec3Words *)&fig->prevPos = *(Vec3Words *)pos;
    }
    sys = (Particle::System *)func_ov007_020b7690(fig->particleIndex);
    {
        Ov007Model *m = fig->model;
        if (m->visible != 0 && (unsigned)m->boneAnim->state <= 1) {
            vel = 0;
            frames = 0x1000;
            if (motion.x != 0 || motion.y != 0 || motion.z != 0) {
                int mag = (short)func_02053644((s16 *)&motion);
                int t;
                if (mag <= 0)
                    speed = 0;
                else if (mag >= 0xfa0)
                    speed = 0x1000;
                else
                    speed = cstd::fdiv(mag, 0xfa0);
                if (mag <= 0x1f4)
                    t = 0;
                else if (mag >= 0x3e8)
                    t = 0x1000;
                else
                    t = cstd::fdiv(mag - 0x1f4, 0x1f4);
                frames -= t;
                func_02053380((s16 *)&motion, (s16 *)&motion);
                *(Vec3sWords *)&sys->direction = *(Vec3sWords *)&motion;
                vel = (short)FX_MUL(speed, 0x460);
            }
            sys->spawnFrames = (u8)(((frames << 2) >> 12) + 1);
            sys->callbackVelocity = vel;
            sys->callbackFlags &= ~2;
            *(Vec3Words *)&sys->position = *(Vec3Words *)pos;
        } else {
            sys->callbackFlags |= 2;
        }
    }
}

// @symbol func_ov007_020beaa0
/* Builds figure `index` (0 the figure, 1 the second) for the work block's
 * figureIndex: makes the model from file 3, then hands func_ov007_020bc6b4
 * two animation files to bind to it -- the figure's own from
 * data_ov007_020d7c5c and a shared one (0xe, or 0xf when
 * func_ov007_020aebac returns 0). */
Ov007Figure *func_ov007_020beaa0(int index, int figureIndex) {
    Ov007Figure *fig;
    void *file;
    int flags;

    fig = (Ov007Figure *)func_ov007_020c3df4(0, sizeof(Ov007Figure));
    fig->index = index;
    fig->figureIndex = figureIndex;
    /* Only a pointer-typed conditional gives the ROM's arm order (moveq #0
     * before movne #1); every int spelling tried swaps the two. */
    fig->particleIndex = (int)((index == 0) ? (char *)0 : (char *)1);

    file = func_ov007_020b3f20(3);
    data_ov007_02104bc4[index][0] = func_ov007_020b3f20(data_ov007_020d7c5c[figureIndex][index]);
    data_ov007_02104bc4[index][1] = func_ov007_020b3f20(func_ov007_020aebac() != 0 ? 0xe : 0xf);

    flags = 2;
    if (index == 1)
        flags |= 8;
    fig->model = func_ov007_020bc6d4(file, flags);
    func_ov007_020bc6b4(fig->model, data_ov007_02104bc4[index], 2);

    fig->yaw = 0;
    fig->prevPos.z = 0;
    fig->prevPos.y = fig->prevPos.z;
    fig->prevPos.x = fig->prevPos.y;
    fig->update = 0;
    return fig;
}

// @symbol func_ov007_020bea44
/* Frees a figure built by func_ov007_020beaa0: releases its files (3, its
 * animation file, 0xe) and its model. It always releases 0xe, even when
 * func_ov007_020beaa0 loaded 0xf instead. */
void func_ov007_020bea44(Ov007Figure *fig) {
    func_ov007_020b3edc(3);
    func_ov007_020b3edc(data_ov007_020d7c5c[fig->figureIndex][fig->index]);
    func_ov007_020b3edc(0xe);
    if (fig->model != 0) {
        func_ov007_020bc53c(fig->model);
        fig->model = 0;
    }
    func_ov007_020c3d1c(fig);
}

// @symbol func_ov007_020be9ac
/* Poses a figure for the frame: hides its model while the main model plays
 * animation 1 or 2, and hides the second figure while figureIndex is 0;
 * then poses the model under viewMatrix, runs the figure's hook and
 * func_ov007_020beb80. */
void func_ov007_020be9ac(Ov007Figure *fig, Ov007Model *owner, void *viewMatrix) {
    switch (owner->boneAnim->anim) {
    case 0:
        break;
    case 1:
    case 2: {
        Ov007Model *m = fig->model;
        if (m->visible != 0)
            m->visible = 0;
        break;
    }
    }

    {
        Ov007Model *m = fig->model;
        if (m->visible != 0 && fig->index == 1 && fig->figureIndex == 0)
            m->visible = 0;
    }

    func_ov007_020bc02c(fig->model, viewMatrix);
    if (fig->update != 0)
        fig->update(fig);
    func_ov007_020beb80(fig);
}

// @symbol func_ov007_020be980
/* Starts animation `anim` on the figure's model and shows it. */
void func_ov007_020be980(Ov007Figure *fig, int anim, int a2) {
    func_ov007_020bc3dc(fig->model, anim, a2);
    fig->model->visible = 1;
    fig->model->animTick = 1;
}

// @symbol func_ov007_020be964
/* Holds the figure's bone animation if it is playing; returns its state. */
int func_ov007_020be964(Ov007Figure *fig) {
    s32 *state = &fig->model->boneAnim->state;
    if (*state == 0) {
        *state = 1;
        return 1;
    }
    return *state;
}

/* from + (to - from) * t, with t a Q12 weight. */
static inline int BlendFx(int from, int to, int t) {
    return (int)(((s64)(0x1000 - t) * from + (s64)t * to) >> 12);
}

// @symbol func_ov007_020be8b8
/* Fills the sixteen affine matrices with uniform scales: matrix i scales by
 * the reciprocal of a Q12 blend from 0x12c (i = 0) to 1 (i = 15). */
void func_ov007_020be8b8(void) {
    int i;
    for (i = 0; i < 16; i++) {
        int *m = SPRITES->affine[i];
        int t = cstd::div(i * 0x1000, 15);
        int scale = func_02053200(BlendFx(0x12c, 0x1000, t));
        m[0] = scale;
        m[1] = 0;
        m[2] = 0;
        m[3] = scale;
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

    func_ov007_020bfd70(WORK->cameras[0], pos, &x, &y);

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
 * particle, at the record's screen point. */
void func_ov007_020be684(void *system, Ov007SpriteGroup *grp) {
    int i;
    int priority;
    Ov007OamNode *extra;
    TrackingRecord *rec;
    Ov007SpriteItem *item;

    extra = grp->extra->node;
    if (extra->visible != 0)
        priority = extra->priority;
    else
        priority = 4;
    func_0204dab4(system, SPRITES->tracking, *grp->layout, (u8)priority,
                  (void *)&func_ov007_020be7a8);

    for (i = 0; i < *grp->layout; i++) {
        item = grp->items[i];
        rec = &SPRITES->tracking[i];
        item->node->visible = (rec->particle != 0) ? 1 : 0;
        item->node->x = rec->screenX << 12;
        item->node->y = rec->screenY << 12;
        item->node->priority = rec->priority;
        item->node->affineIndex = (rec->lifeScale << 4) >> 0xc;
        if (rec->particle != 0)
            func_ov007_020c3fe4(item);
    }
}

// @symbol func_ov007_020be50c
/* Builds the sprite group (and its tracking records) at
 * data_ov007_02104bc0 in the sub OAM list: every item shows sprite bank 2's
 * animation 1 with an 8-pixel margin, the extra record animations 2 and 0
 * with a 0x20-pixel one. */
void func_ov007_020be50c(void) {
    Ov007SpriteBank **banks;
    Ov007SpriteItem *item;
    int i;

    banks = WORK->display->spriteBanks;

    SPRITES = (Ov007ParticleSprites *)func_ov007_020c3df4(0, sizeof(Ov007ParticleSprites));
    SPRITES->group = func_ov007_020c3f00(&data_ov007_020d7c6c, 2);

    SPRITES->tracking = (TrackingRecord *)func_ov007_020c3df4(
        data_ov007_02103234, data_ov007_020d7c6c * sizeof(TrackingRecord));

    func_ov007_020c3ee8(SPRITES->group, WORK->display->oamListSub);

    for (i = 0; i < SPRITES->group->layout[0]; i++) {
        item = SPRITES->group->items[i];
        item->node->anims[0] = &banks[2]->anims[1];
        item->margin = 8;
        Sprite_SetAnimation(item->node, 0, 1, 0, 0);
        func_0204dc84((char *)&SPRITES->tracking[i]);
    }

    item = SPRITES->group->extra;
    item->node->anims[0] = &banks[2]->anims[2];
    item->node->anims[1] = &banks[2]->anims[0];
    item->margin = 0x20;

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
        func_ov007_020c3e7c(SPRITES->group);
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
    for (i = 0; i < 16; i++) {
        int *s = SPRITES->affine[i];
        GXOamAffine *d = (GXOamAffine *)(oam + i * 4);
        d->PA = (short)(s[0] >> 4);
        d->PB = (short)(s[1] >> 4);
        d->PC = (short)(s[2] >> 4);
        d->PD = (short)(s[3] >> 4);
    }
}

/* The figure's bone 0 transform. */
static inline Matrix4x3 *Bone0(Ov007Figure *fig) {
    return fig->model->model->transforms;
}

// @symbol func_ov007_020be0dc
/* One frame: pins the extra node to the shown figure's bone 0 on screen
 * (animation by the figure and its frame, size by the bone's tilt, OAM
 * priority by its depth), runs the figure's particle system through the
 * item nodes, and shows the extra node only while the figure's model is
 * visible. Returns whether it is shown. */
int func_ov007_020be0dc(void) {
    int modelVisible = 0;
    int shown;
    Ov007OamNode *node;
    void *system = 0;
    Ov007Figure *fig;
    int particleIndex;
    int screenX, screenY;

    node = SPRITES->group->extra->node;
    fig = func_ov007_020be850();

    if (fig == 0) {
        particleIndex = data_ov007_02103230;
    } else {
        u8 kind = fig->figureIndex;
        Vector3 *pos;

        switch (kind) {
        case 0:
            if (node->animIndex != 0)
                Sprite_SetAnimation(node, 0, 1, 0, 0);
            break;
        case 1: {
            int frame = fig->model->boneAnim->frame;
            if (frame >= 0x226) {
                if (node->animIndex != 1)
                    Sprite_SetAnimation(node, 1, 1, 0, 0);
            } else {
                if (node->animIndex != 0)
                    Sprite_SetAnimation(node, 0, 1, 0, 0);
            }
        } break;
        }

        pos = &Bone0(fig)->t;
        func_ov007_020bfd70(WORK->cameras[0], pos, &screenX, &screenY);

        if (WORK->cameraPreset == 4)
            screenY += 0x14b;
        else
            screenY += data_ov007_020d7c58[WORK->figureIndex];

        {
            int x = (short)screenX << 0xc;
            int y = (short)screenY << 0xc;
            node->x = x;
            node->y = y;
        }

        {
            int *row = &Bone0(fig)->r.m[6];
            int yaw = func_020538b8(row[0], row[2]);
            int turn = cstd::div(yaw << 0xc, 0xffff);
            unsigned int scale = ((turn * 0x12) >> 0xc) & 0xff;
            if (scale >= 0x12)
                scale = 0x11;
            if (node->animIndex == 0)
                Sprite_SetAnimation(node, 0, 0, 0, (int)scale);
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
        particleIndex = fig->particleIndex;
    }

    data_ov007_02103230 = particleIndex;
    if (particleIndex != -1)
        system = func_ov007_020b7690(particleIndex);

    if (system != 0) {
        Ov007OamNode *q = SPRITES->group->root;
        if (q->visible == 0)
            q->visible = 1;
        func_ov007_020be684(system, SPRITES->group);
    } else {
        Ov007OamNode *q = SPRITES->group->root;
        if (q->visible != 0)
            q->visible = 0;
    }

    func_ov007_020c3fe4(SPRITES->group->extra);

    shown = node->visible;
    if (shown != 0 && modelVisible != 0)
        shown = 1;
    else
        shown = 0;
    node->visible = shown;
    return shown;
}
}
