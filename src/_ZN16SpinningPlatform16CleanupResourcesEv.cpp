//cpp
// @symbol _ZN17RotatingClockHand16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RotatingClockHand.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int RotatingClockHand_ClsnFile[];
extern int RotatingClockHand_ModelFile[];

int RotatingClockHand::CleanupResources()
{
    if (((MeshColliderBase *)((char *)&mMeshCollider))->IsEnabled()) {
        ((MeshColliderBase *)((char *)&mMeshCollider))->Disable();
    }
    ((SharedFilePtr *)(RotatingClockHand_ModelFile))->Release();
    ((SharedFilePtr *)(RotatingClockHand_ClsnFile))->Release();
    return 1;
}
