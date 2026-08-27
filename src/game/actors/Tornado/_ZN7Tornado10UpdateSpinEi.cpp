//cpp
// @symbol _ZN7Tornado10UpdateSpinEi
#include "Tornado.h"

extern "C" s16 data_02082214[];

void Tornado::UpdateSpin(s32 scale)
{
    u16 idx = (u16)unk_35a;
    s16 wave = data_02082214[(idx >> 4) * 2 + 1];
    s32 radiusScale = (wave + 0x1000) >> 1;
    s32 frameScale = (s32)(((long long)scale * 0x666 + 0x800) >> 12);
    s32 radius = (s32)(((long long)radiusScale * 0x4cc + 0x800) >> 12);
    radius += 0x1000;
    mScaleX = (s32)(((long long)radius * frameScale + 0x800) >> 12);

    u16 idx2 = (u16)unk_35a;
    s16 wave2 = data_02082214[(idx2 >> 4) * 2 + 1];
    s32 heightScale = ((0x1000 - wave2) >> 2) + 0x800;
    mScaleY = (s32)(((long long)heightScale * frameScale + 0x800) >> 12);

    mScaleZ = mScaleX;
    unk_35a += 0x200;
    mHorzSpeed = 0xe000;
    mAngleY += 0x2c00;
    mdCcAc_c.radius = mScaleX * 0x514;
    mdCcAc_c.height = mScaleY * 0xfa0;
    mScaleX <<= 1;
    mScaleY <<= 2;
    mScaleZ <<= 1;
}
