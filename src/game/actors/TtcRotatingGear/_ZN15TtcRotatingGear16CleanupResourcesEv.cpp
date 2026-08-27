//cpp
// @symbol _ZN15TtcRotatingGear16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "TtcRotatingGear.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int TtcRotatingGear_ClsnFile[];
extern int TtcRotatingGear_ModelFile[];

int TtcRotatingGear::CleanupResources()
{
    if (((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
        ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    }
    ((SharedFilePtr *)(TtcRotatingGear_ModelFile))->Release();
    ((SharedFilePtr *)(TtcRotatingGear_ClsnFile))->Release();
    return 1;
}
