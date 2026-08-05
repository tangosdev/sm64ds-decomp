//cpp
// @symbol _ZN8PoleLift16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PoleLift.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int G0[];

int PoleLift::CleanupResources()
{
    ((MeshColliderBase *)((char *)&mCollider))->Disable();
    ((SharedFilePtr *)(G0))->Release();
    ((SharedFilePtr *)(G1))->Release();
    return 1;
}
