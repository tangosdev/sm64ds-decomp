//cpp
// @symbol _ZN11PyramidStep16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PyramidStep.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int G0[];

int PyramidStep::CleanupResources()
{
    ((MeshColliderBase *)((char *)&mMovingMeshCollider))->Disable();
    ((SharedFilePtr *)(G0))->Release();
    ((SharedFilePtr *)(G1))->Release();
    return 1;
}
