//cpp
// @symbol _ZN8PoleLift16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PoleLift.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int PoleLift_ClsnFile[];
extern int PoleLift_ModelFile[];

int PoleLift::CleanupResources()
{
    ((MeshColliderBase *)((char *)&mCollider))->Disable();
    ((SharedFilePtr *)(PoleLift_ModelFile))->Release();
    ((SharedFilePtr *)(PoleLift_ClsnFile))->Release();
    return 1;
}
