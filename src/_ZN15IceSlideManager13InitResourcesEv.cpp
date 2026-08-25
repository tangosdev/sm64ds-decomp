//cpp
// @symbol _ZN15IceSlideManager13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "IceSlideManager.h"
struct S3 { int w0; int w1; int w2; };
extern struct S3 data_ov019_021135d8;

int IceSlideManager::InitResources()
{
    *(int*)((char*)&mPosX) = data_ov019_021135d8.w0;
    *(int*)((char*)&mPosY) = data_ov019_021135d8.w1;
    *(int*)((char*)&mPosZ) = data_ov019_021135d8.w2;
    *(short*)((char*)&mKillTimer) = 0x78;
    return 1;
}
