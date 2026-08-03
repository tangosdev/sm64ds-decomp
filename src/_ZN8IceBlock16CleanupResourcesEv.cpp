//cpp
// @symbol _ZN8IceBlock16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "IceBlock.h"
#include "SharedFilePtr.h"
extern int data_ov081_02128fd8[];

int IceBlock::CleanupResources()
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)&mMeshCollider)) {
        _ZN16MeshColliderBase7DisableEv((char *)&mMeshCollider);
    }
    ((SharedFilePtr *)(data_ov081_02128fd8))->Release();
    ((SharedFilePtr *)(data_ov081_02128fd0))->Release();
    return 1;
}
