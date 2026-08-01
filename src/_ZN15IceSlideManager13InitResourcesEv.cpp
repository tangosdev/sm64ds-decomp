//cpp
// @symbol _ZN15IceSlideManager13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "IceSlideManager.h"
struct S3 { int w0; int w1; int w2; };
extern struct S3 data_ov019_021135d8;

int IceSlideManager::InitResources()
{
    *(int*)((char*)&unk_05c) = data_ov019_021135d8.w0;
    *(int*)((char*)&unk_060) = data_ov019_021135d8.w1;
    *(int*)((char*)&unk_064) = data_ov019_021135d8.w2;
    *(short*)((char*)&unk_0d4) = 0x78;
    return 1;
}
