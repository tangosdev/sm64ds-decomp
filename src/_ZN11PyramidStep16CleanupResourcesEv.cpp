//cpp
// @symbol _ZN11PyramidStep16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PyramidStep.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int data_ov025_02113ab0[];
extern int data_ov025_02113ab8[];

int PyramidStep::CleanupResources()
{
    ((MeshColliderBase *)((char *)&mMovingMeshCollider))->Disable();
    ((SharedFilePtr *)(data_ov025_02113ab8))->Release();
    ((SharedFilePtr *)(data_ov025_02113ab0))->Release();
    return 1;
}
