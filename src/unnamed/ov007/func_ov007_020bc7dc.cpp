//cpp
#include "dExtModel_c.h"
#include "Ov007Model.h"
#include "nitro/fx/fx_cp.h"

extern "C" {


extern void func_ov007_020c9110(void *p);
}

extern void func_0204531c(void *model, int x);
extern "C" {


/* Poses the model from blendAnim's current frame and blends it in with
 * weight blendFrames / blendFrameCount, counting blendFrames down on the
 * frames that step the animations. */
void func_ov007_020bc7dc(Ov007Model *m) {
    ModelComponents *model = m->model;
    Ov007AnimPlayer *p = m->blendAnim;
    BCA_File *entry = m->boneAnims[p->anim];
    int quot = FX_DivS32(m->blendFrames << 12, m->blendFrameCount);

    if (m->animTick != 0)
        func_ov007_020c9110(m->blendAnim);

    p = m->blendAnim;
    if (p->anim != -1)
        model->UpdateBones(entry, p->frame);

    func_0204531c(model, quot);

    if (m->animTick == 0)
        return;

    m->blendFrames -= 1;
    if (m->blendFrames <= 0)
        m->blendFrames = 0;
}
}
