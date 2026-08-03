//cpp
// @symbol _ZN15FireSeaElevator16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "FireSeaElevator.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int data_ov045_021131b0[];

int FireSeaElevator::CleanupResources()
{
    ((MeshColliderBase *)((char *)&mMovingMeshCollider))->Disable();
    ((SharedFilePtr *)(data_ov045_021131b0))->Release();
    ((SharedFilePtr *)(data_ov045_021131a8))->Release();
    return 1;
}
