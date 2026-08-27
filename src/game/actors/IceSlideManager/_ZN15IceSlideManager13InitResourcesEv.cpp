//cpp
// @symbol _ZN15IceSlideManager13InitResourcesEv
#include "IceSlideManager.h"

extern Vector3 data_ov019_021135d8;

int IceSlideManager::InitResources()
{
    mPosX = data_ov019_021135d8.x;
    mPosY = data_ov019_021135d8.y;
    mPosZ = data_ov019_021135d8.z;
    mKillTimer = 0x78;
    return 1;
}
