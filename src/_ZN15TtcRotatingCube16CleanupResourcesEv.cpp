//cpp
// @symbol _ZN15TtcRotatingCube16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "TtcRotatingCube.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
struct E { void* p; char pad[8]; };
extern struct E data_ov065_0211cfd0[];
extern struct E data_ov065_0211cfd4[];

int TtcRotatingCube::CleanupResources()
{
    if (((dBgW *)((char*)&mMovingMeshCollider))->IsEnabled()) {
        ((dBgW *)((char*)&mMovingMeshCollider))->Disable();
    }
    ((SharedFilePtr *)(data_ov065_0211c0a8[mVariant]))->Release();
    ((SharedFilePtr *)(data_ov065_0211cfd0[mVariant].p))->Release();
    ((SharedFilePtr *)(data_ov065_0211cfd4[mVariant].p))->Release();
    return 1;
}
