//cpp
// @symbol _ZN11MirrorLuigi16CleanupResourcesEv
/* MirrorLuigi::CleanupResources -- vtable slot 3. Releases the three shared
 * files the class holds; it never touches `this`. */
#include "MirrorLuigi.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr data_ov002_0210ebb8;
extern SharedFilePtr data_ov002_0210eb20;
extern SharedFilePtr data_ov002_0210eaa0;
}

int MirrorLuigi::CleanupResources()
{
    data_ov002_0210ebb8.Release();
    data_ov002_0210eb20.Release();
    data_ov002_0210eaa0.Release();
    return 1;
}
