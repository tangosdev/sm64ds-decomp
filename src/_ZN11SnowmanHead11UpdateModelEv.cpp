//cpp
#include "SnowmanHead.h"

extern "C" void Matrix4x3_FromRotationY(void *matrix, int angle);

void SnowmanHead::UpdateModel()
{
    Matrix4x3_FromRotationY((char *)this + 0xf0, mAngleY);
    *(s32 *)((char *)this + 0x114) = mPosX >> 3;
    *(s32 *)((char *)this + 0x118) = (mPosY + 0x96000) >> 3;
    *(s32 *)((char *)this + 0x11c) = mPosZ >> 3;
}
