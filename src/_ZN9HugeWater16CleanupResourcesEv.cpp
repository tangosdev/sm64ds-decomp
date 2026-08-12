//cpp
// @symbol _ZN9HugeCover16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "HugeCover.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int data_ov032_02113af4[];
extern int data_ov032_02113afc[];

int HugeCover::CleanupResources()
{
    if (((MeshColliderBase *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
        ((MeshColliderBase *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    }
    ((SharedFilePtr *)(data_ov032_02113afc))->Release();
    ((SharedFilePtr *)(data_ov032_02113af4))->Release();
    return 1;
}
