//cpp
// @symbol _ZN12WorkElevator16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "WorkElevator.h"
#include "SharedFilePtr.h"
extern int data_ov021_021149b8[];

int WorkElevator::CleanupResources()
{
    int i;
    char *p = ((char *)this);
    _ZN16MeshColliderBase7DisableEv(p + 0x124);
    p += 0x520;
    for (i = 0; i < 4; i++) {
        _ZN16MeshColliderBase7DisableEv(p);
        p += 0x1c8;
    }
    ((SharedFilePtr *)(data_ov021_021149b0))->Release();
    ((SharedFilePtr *)(data_ov021_021149b8))->Release();
    ((SharedFilePtr *)(data_ov021_021149a0))->Release();
    ((SharedFilePtr *)(data_ov021_021149a8))->Release();
    return 1;
}
