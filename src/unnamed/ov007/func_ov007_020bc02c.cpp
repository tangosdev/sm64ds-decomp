//cpp
#include "types.h"
#include "dExtModel_c.h"

extern "C" {

typedef struct { int f[12]; } Blk;

#define LS(a) (*(short*)(((long long)(int)(a))))

extern void func_ov007_020bc968(char *c, void *other);
extern void func_ov007_020bc894(char *c);
extern void func_ov007_020bc7dc(char *c);
extern void func_02044dc4(void *self);

/* The 0x1c-byte animation player func_ov007_020c92ac allocates and
 * func_ov007_020c9110 ticks; only the slots this file reads. */
typedef struct Ov007AnimPlayer {
    s32 state; /* 0x00: 0 playing; func_ov007_020c9110 sets 2 when a one-shot ends */
    char pad_04[8];
    s16 anim; /* 0x0c: index into the owner's animation table; -1 none */
    char pad_0e[2];
    u16 frame;      /* 0x10: the whole frame func_ov007_020c9110 last reached */
    u16 frameCount; /* 0x12 */
} Ov007AnimPlayer;

/* The 0xa0-byte model record func_ov007_020bc6d4 allocates; only the slots
 * this file touches. */
typedef struct Ov007Model {
    struct ModelComponents *model; /* 0x00 */
    void *transforms;              /* 0x04: the pose model->transforms points at
                             * (func_ov007_020bc47c binds it) */
    /* 0x08: flags & 2 only: the next frame's pose; on in-between frames this
     * file averages it into transforms */
    void *nextTransforms;
    char pad_0c[8];
    struct BCA_File **boneAnims; /* 0x14: boneAnim->anim indexes it */
    Ov007AnimPlayer *boneAnim;   /* 0x18: func_ov007_020bc894 ticks it */
    char pad_1c[0x70];
    s16 blendFrames; /* 0x8c: frames left of func_ov007_020bc7dc's cross-fade */
    char pad_8e[0xa];
    /* 0x98: 1 on the frames that step the animations; with flags & 3 it
     * alternates, so they run at half rate (func_ov007_020bc47c seeds 1) */
    int animTick;
    /* 0x9c: func_ov007_020bc6d4's second argument. 1/2 half-rate animation
     * (2 also fills the in-between frames), 4 cross-fade buffers, 8 skips
     * dExtSimpleModel_c::LoadTexAndPal (func_ov007_020bc47c) */
    int flags;
} Ov007Model;

/* c keeps the char * spelling of the callers' declarations; `m` is the
 * typed view. */
void func_ov007_020bc02c(char *c, void *other) {
    Ov007Model *m = (Ov007Model *)c;
    struct ModelComponents *r5 = m->model;
    int r4 = 0;

    if (m->flags & 2) {
        Ov007AnimPlayer *r3 = m->boneAnim;
        if (r3 != 0) {
            if (LS(&r3->anim) != -1) {
                if (r3->state == 0) {
                    if (r3->anim == -1 || r3->frame != r3->frameCount - 1)
                        r4 = 1;
                }
            }
        }
    }

    func_ov007_020bc968(c, other);
    if (m->animTick != 0) {
        func_ov007_020bc894(c);
        r5->UpdateVertsUsingBones();
    } else if (r4 != 0) {
        Ov007AnimPlayer *o = m->boneAnim;
        struct BCA_File **bones = m->boneAnims;
        r5->UpdateBones(bones[o->anim], (u16)(o->frame + 1));
        r5->transforms = (Matrix4x3 *)m->nextTransforms;
        r5->UpdateVertsUsingBones();
        unsigned int i = 0;
        r5->transforms = (Matrix4x3 *)m->transforms;
        while (i < r5->modelFile->numBones) {
            Blk *a = (Blk *)((char *)m->transforms + i * 0x30);
            Blk *b = (Blk *)((char *)m->nextTransforms + i * 0x30);
            a->f[0] = (a->f[0] + b->f[0]) / 2;
            a->f[1] = (a->f[1] + b->f[1]) / 2;
            a->f[2] = (a->f[2] + b->f[2]) / 2;
            a->f[3] = (a->f[3] + b->f[3]) / 2;
            a->f[4] = (a->f[4] + b->f[4]) / 2;
            a->f[5] = (a->f[5] + b->f[5]) / 2;
            a->f[6] = (a->f[6] + b->f[6]) / 2;
            a->f[7] = (a->f[7] + b->f[7]) / 2;
            a->f[8] = (a->f[8] + b->f[8]) / 2;
            a->f[9] = (a->f[9] + b->f[9]) / 2;
            a->f[10] = (a->f[10] + b->f[10]) / 2;
            a->f[11] = (a->f[11] + b->f[11]) / 2;
            i++;
        }
    }

    if (m->animTick != 0 || r4 != 0) {
        if (m->blendFrames > 0)
            func_ov007_020bc7dc(c);
        if (*(int *)((char *)r5->modelFile + 0x30) != 0)
            func_02044dc4(r5);
    }

    {
        int flags = m->flags;
        if ((flags & 1) == 0) {
            if ((flags & 2) == 0)
                return;
        }
        m->animTick = (m->animTick == 0) ? 1 : 0;
    }
}
}
