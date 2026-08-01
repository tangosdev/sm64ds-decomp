//cpp
// @symbol _ZN12MansionSteps16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "MansionSteps.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void *);

int MansionSteps::CleanupResources()
{
    _ZN16MeshColliderBase7DisableEv((char *)&mMovingMeshCollider);
    int idx = *(int*)((char*)&unk_140);
    _ZN13SharedFilePtr7ReleaseEv((void*)(data_ov063_0211e27c[idx]));
    idx = *(int*)((char*)&unk_140);
    _ZN13SharedFilePtr7ReleaseEv((void*)(data_ov063_0211e28c[idx]));
    return 1;
}
