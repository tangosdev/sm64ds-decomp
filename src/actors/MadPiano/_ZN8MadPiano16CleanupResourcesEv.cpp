//cpp
// @symbol _ZN8MadPiano16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "MadPiano.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"

int MadPiano::CleanupResources()
{
    if (((MeshColliderBase *)((char *)&mMeshCollider))->IsEnabled()) {
        ((MeshColliderBase *)((char *)&mMeshCollider))->Disable();
    }
    ((SharedFilePtr *)(&data_ov063_0211ef80))->Release();
    ((SharedFilePtr *)(&data_ov063_0211ef90))->Release();
    ((SharedFilePtr *)(&data_ov063_0211ef88))->Release();
    return 1;
}
