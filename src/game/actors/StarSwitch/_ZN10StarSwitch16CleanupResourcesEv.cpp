//cpp
// @symbol _ZN10StarSwitch16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "StarSwitch.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
// _ZN10StarSwitch16CleanupResourcesEv at 0x020ba568
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
void UnloadSilverStarAndNumber(void);
}

extern char data_ov002_021098e8;
extern char data_ov002_021098ec;
extern char data_ov002_0211092c;

int StarSwitch::CleanupResources()
{
    int t;
    if (((dBgW *)&mMeshCollider)->IsEnabled()) {
        ((dBgW *)&mMeshCollider)->Disable();
    }
    ((SharedFilePtr *)(*(void**)(&data_ov002_021098e8 + mResourceIdx * 0xc)))->Release();
    ((SharedFilePtr *)(*(void**)(&data_ov002_021098ec + mResourceIdx * 0xc)))->Release();
    t = actorID == 0xc;
    if (t != false) {
        UnloadSilverStarAndNumber();
        ((SharedFilePtr *)(&data_ov002_0211092c))->Release();
    }
    return 1;
}
