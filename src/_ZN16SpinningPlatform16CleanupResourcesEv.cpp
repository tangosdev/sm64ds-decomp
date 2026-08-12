//cpp
// @symbol _ZN16SpinningPlatform16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SpinningPlatform.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int RotatingClockHand_ClsnFile[];
extern int RotatingClockHand_ModelFile[];

int SpinningPlatform::CleanupResources()
{
    if (((MeshColliderBase *)((char *)&mMeshCollider))->IsEnabled()) {
        ((MeshColliderBase *)((char *)&mMeshCollider))->Disable();
    }
    ((SharedFilePtr *)(RotatingClockHand_ModelFile))->Release();
    ((SharedFilePtr *)(RotatingClockHand_ClsnFile))->Release();
    return 1;
}
