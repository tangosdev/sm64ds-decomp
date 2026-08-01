//cpp
// @symbol _ZN10PyramidTop16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PyramidTop.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov024_02113968[];

int PyramidTop::CleanupResources()
{
    _ZN16MeshColliderBase7DisableEv((char *)&mMeshCollider);
    _ZN13SharedFilePtr7ReleaseEv(data_ov024_02113968);
    _ZN13SharedFilePtr7ReleaseEv(data_ov024_02113960);
    return 1;
}
