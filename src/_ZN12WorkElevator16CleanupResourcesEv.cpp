//cpp
// @symbol _ZN12WorkElevator16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "WorkElevator.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int data_ov021_021149b8[];

int WorkElevator::CleanupResources()
{
    int i;
    char *p = ((char *)this);
    ((dBgW *)(p + 0x124))->Disable();
    p += 0x520;
    for (i = 0; i < 4; i++) {
        ((dBgW *)(p))->Disable();
        p += 0x1c8;
    }
    ((SharedFilePtr *)(data_ov021_021149b0))->Release();
    ((SharedFilePtr *)(data_ov021_021149b8))->Release();
    ((SharedFilePtr *)(data_ov021_021149a0))->Release();
    ((SharedFilePtr *)(data_ov021_021149a8))->Release();
    return 1;
}
