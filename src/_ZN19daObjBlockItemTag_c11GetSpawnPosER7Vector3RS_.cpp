//cpp
// @symbol _ZN19daObjBlockItemTag_c11GetSpawnPosER7Vector3RS_
#include "daObjBlockItemTag_c.h"
#include "common.h"

extern s32 data_ov002_020ff090[];

void daObjBlockItemTag_c::GetSpawnPos(Vector3 &destination, daObjBlockItemTag_c &tag)
{
    u8 index = tag.mActionIndex;
    s32 z = tag.mPosZ;
    s32 y = tag.mPosY + data_ov002_020ff090[index];
    s32 x = tag.mPosX;

    destination.x = x;
    destination.y = y;
    destination.z = z;
}
