//cpp
// @symbol _ZN5Unagi6RenderEv

#include "Unagi.h"

struct UnagiRenderStep {
    s32 unk_00;
    s32 unk_04;
    s32 angleScale;
};

extern "C" {
void func_020167a4(BlendModelAnim *model);
void func_0204531c(ModelComponents *data, s32 weight);
extern UnagiRenderStep data_ov016_02114908[];
}

#pragma opt_strength_reduction off

s32 Unagi::Render()
{
    int i;
    UnagiRenderStep *step;
    char *bone;

    func_020167a4(&mBlendModelAnim);
    bone = (char *)mBlendModelAnim.data.bones + 0x34;
    step = data_ov016_02114908;
    for (i = 1; i < 7; i++) {
        s16 movement = mSegmentAngle[i];
        u16 *angle = (u16 *)(bone + 0x1e);
        *angle = *angle + (u16)(s16)(movement * step->angleScale);
        step++;
        bone += 0x34;
    }
    func_0204531c(&mBlendModelAnim.data, mBlendModelAnim.blendWeight);
    mBlendModelAnim.Model::Render(0);
    return 1;
}
