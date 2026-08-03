//cpp
// @symbol _ZN6Thwomp16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Thwomp.h"
#include "SharedFilePtr.h"

int Thwomp::CleanupResources()
{
    void **fp;
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)&mMovingMeshCollider)) {
        _ZN16MeshColliderBase7DisableEv((char *)&mMovingMeshCollider);
    }
    fp = *(void***)((char *)&mFileTable);
    ((SharedFilePtr *)(fp[0]))->Release();
    fp = *(void***)((char *)&mFileTable);
    ((SharedFilePtr *)(fp[1]))->Release();
    fp = *(void***)((char *)&mFileTable);
    if (fp[3] != 0) {
        ((SharedFilePtr *)(fp[3]))->Release();
    }
    return 1;
}
