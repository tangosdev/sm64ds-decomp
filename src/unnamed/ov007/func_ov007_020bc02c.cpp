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
            a->m[0] = (a->m[0] + b->m[0]) / 2;
            a->m[1] = (a->m[1] + b->m[1]) / 2;
            a->m[2] = (a->m[2] + b->m[2]) / 2;
            a->m[3] = (a->m[3] + b->m[3]) / 2;
            a->m[4] = (a->m[4] + b->m[4]) / 2;
            a->m[5] = (a->m[5] + b->m[5]) / 2;
            a->m[6] = (a->m[6] + b->m[6]) / 2;
            a->m[7] = (a->m[7] + b->m[7]) / 2;
            a->m[8] = (a->m[8] + b->m[8]) / 2;
            a->m[9] = (a->m[9] + b->m[9]) / 2;
            a->m[10] = (a->m[10] + b->m[10]) / 2;
            a->m[11] = (a->m[11] + b->m[11]) / 2;
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
