//cpp
#include "types.h"
#include "dExtModel_c.h"
#include "Ov007Model.h"



extern "C" void func_ov007_020bc968(Ov007Model *m, void *other);
extern void func_ov007_020bc894(Ov007Model *m);
extern void func_ov007_020bc7dc(Ov007Model *m);
void func_02044dc4(struct ModelComponents *model);

void func_ov007_020bc02c(Ov007Model *m, void *other) {
    struct ModelComponents *model = m->model;
    int advance = 0;

    if (m->flags & 2) {
        Ov007AnimPlayer *animPlayer = m->boneAnim;
        if (animPlayer != 0) {
            if (animPlayer->anim != -1) {
                if (animPlayer->state == 0) {
                    if (animPlayer->anim == -1 || animPlayer->frame != animPlayer->frameCount - 1)
                        advance = 1;
                }
            }
        }
    }

    func_ov007_020bc968(m, other);
    if (m->animTick != 0) {
        func_ov007_020bc894(m);
        model->UpdateVertsUsingBones();
    } else if (advance != 0) {
        Ov007AnimPlayer *o = m->boneAnim;
        struct BCA_File **bones = m->boneAnims;
        model->UpdateBones(bones[o->anim], (u16)(o->frame + 1));
        model->transforms = m->nextTransforms;
        model->UpdateVertsUsingBones();
        unsigned int i = 0;
        model->transforms = m->transforms;
        while (i < model->modelFile->numBones) {
            Matrix4x3 *a = &m->transforms[i];
            Matrix4x3 *b = &m->nextTransforms[i];
            a->r.m[0] = (a->r.m[0] + b->r.m[0]) / 2;
            a->r.m[1] = (a->r.m[1] + b->r.m[1]) / 2;
            a->r.m[2] = (a->r.m[2] + b->r.m[2]) / 2;
            a->r.m[3] = (a->r.m[3] + b->r.m[3]) / 2;
            a->r.m[4] = (a->r.m[4] + b->r.m[4]) / 2;
            a->r.m[5] = (a->r.m[5] + b->r.m[5]) / 2;
            a->r.m[6] = (a->r.m[6] + b->r.m[6]) / 2;
            a->r.m[7] = (a->r.m[7] + b->r.m[7]) / 2;
            a->r.m[8] = (a->r.m[8] + b->r.m[8]) / 2;
            a->t.x = (a->t.x + b->t.x) / 2;
            a->t.y = (a->t.y + b->t.y) / 2;
            a->t.z = (a->t.z + b->t.z) / 2;
            i++;
        }
    }

    if (m->animTick != 0 || advance != 0) {
        if (m->blendFrames > 0)
            func_ov007_020bc7dc(m);
        if (model->modelFile->numWeightedMatrices != 0)
            func_02044dc4(model);
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

