//cpp
// @symbol _ZN8PoleLift16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PoleLift.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"

int PoleLift::CleanupResources()
{
    ((MeshColliderBase *)((char *)&mCollider))->Disable();
    ((SharedFilePtr *)(data_ov045_021131d8))->Release();
    ((SharedFilePtr *)(data_ov045_021131d0))->Release();
    return 1;
}
