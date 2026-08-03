//cpp
// @symbol _ZN12MansionSteps16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "MansionSteps.h"
#include "SharedFilePtr.h"

int MansionSteps::CleanupResources()
{
    _ZN16MeshColliderBase7DisableEv((char *)&mMovingMeshCollider);
    int idx = *(int*)((char*)&unk_140);
    ((SharedFilePtr *)((void*)(data_ov063_0211e27c[idx])))->Release();
    idx = *(int*)((char*)&unk_140);
    ((SharedFilePtr *)((void*)(data_ov063_0211e28c[idx])))->Release();
    return 1;
}
