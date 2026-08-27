//cpp
// @symbol _ZN10BrickBlock11GetSpawnPosER7Vector3RS_
#include "BrickBlock.h"
#include "common.h"

extern s32 data_ov002_020ff090[];

void BrickBlock::GetSpawnPos(Vector3 &destination, BrickBlock &tag)
{
    u8 index = tag.mActionIndex;
    s32 z = tag.mPosZ;
    s32 y = tag.mPosY + data_ov002_020ff090[index];
    s32 x = tag.mPosX;

    destination.x = x;
    destination.y = y;
    destination.z = z;
}
