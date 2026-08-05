//cpp
// @symbol _ZN15TtcRotatingGear16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "TtcRotatingGear.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int TtcRotatingGear_ClsnFile[];
extern int TtcRotatingGear_ModelFile[];

int TtcRotatingGear::CleanupResources()
{
    if (((MeshColliderBase *)((char *)&mMeshCollider))->IsEnabled()) {
        ((MeshColliderBase *)((char *)&mMeshCollider))->Disable();
    }
    ((SharedFilePtr *)(TtcRotatingGear_ModelFile))->Release();
    ((SharedFilePtr *)(TtcRotatingGear_ClsnFile))->Release();
    return 1;
}
