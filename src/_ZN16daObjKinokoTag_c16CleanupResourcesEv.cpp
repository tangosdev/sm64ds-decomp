//cpp
// @symbol _ZN16daObjKinokoTag_c16CleanupResourcesEv

#include "daObjKinokoTag_c.h"
#include "SharedFilePtr.h"

extern "C" int data_ov002_0210da30[];

s32 daObjKinokoTag_c::CleanupResources()
{
    ((SharedFilePtr *)data_ov002_0210da30)->Release();
    return 1;
}
