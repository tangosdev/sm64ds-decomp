//cpp
#include "types.h"
#include "dExtModel_c.h"
#include "Ov007Model.h"

extern "C" {

typedef struct { int f[12]; } Blk;

#define LS(a) (*(short*)(((long long)(int)(a))))

extern void func_ov007_020bc968(Ov007Model *m, void *other);
extern void func_ov007_020bc894(Ov007Model *m);
extern void func_ov007_020bc7dc(Ov007Model *m);
extern void func_02044dc4(void *self);

void func_ov007_020bc02c(Ov007Model *m, void *other) {
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

    func_ov007_020bc968(m, other);
    if (m->animTick != 0) {
        func_ov007_020bc894(m);
        r5->UpdateVertsUsingBones();
    } else if (r4 != 0) {
        Ov007AnimPlayer *o = m->boneAnim;
        struct BCA_File **bones = m->boneAnims;
        r5->UpdateBones(bones[o->anim], (u16)(o->frame + 1));
        r5->transforms = m->nextTransforms;
        r5->UpdateVertsUsingBones();
        unsigned int i = 0;
        r5->transforms = m->transforms;
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
            func_ov007_020bc7dc(m);
        if (r5->modelFile->numWeightedMatrices != 0)
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
