//cpp
// @symbol _ZN10PyramidTop16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PyramidTop.h"
#include "SharedFilePtr.h"
extern int data_ov024_02113968[];

int PyramidTop::CleanupResources()
{
    _ZN16MeshColliderBase7DisableEv((char *)&mMeshCollider);
    ((SharedFilePtr *)(data_ov024_02113968))->Release();
    ((SharedFilePtr *)(data_ov024_02113960))->Release();
    return 1;
}
