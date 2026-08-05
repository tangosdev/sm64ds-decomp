//cpp
// @symbol _ZN20SwitchActivatedPlank16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SwitchActivatedPlank.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int data_ov029_02114324[];
extern int data_ov029_0211432c[];

int SwitchActivatedPlank::CleanupResources()
{
    if (((MeshColliderBase *)((char *)&mMovingMeshCollider))->IsEnabled()) {
        ((MeshColliderBase *)((char *)&mMovingMeshCollider))->Disable();
    }
    ((SharedFilePtr *)(data_ov029_0211432c))->Release();
    ((SharedFilePtr *)(data_ov029_02114324))->Release();
    return 1;
}
