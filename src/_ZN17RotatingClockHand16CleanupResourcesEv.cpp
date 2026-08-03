//cpp
// @symbol _ZN17RotatingClockHand16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RotatingClockHand.h"
#include "SharedFilePtr.h"
extern int G0[];

int RotatingClockHand::CleanupResources()
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)&mMeshCollider)) {
        _ZN16MeshColliderBase7DisableEv((char *)&mMeshCollider);
    }
    ((SharedFilePtr *)(G0))->Release();
    ((SharedFilePtr *)(G1))->Release();
    return 1;
}
