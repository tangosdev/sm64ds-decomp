//cpp
// @symbol _ZN6Coffin16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Coffin.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int data_ov071_021230d8[];
extern int data_ov071_021230d0[];

int Coffin::CleanupResources()
{
    if (((dBgW *)((char *)&mMeshCollider))->IsEnabled()) {
        ((dBgW *)((char *)&mMeshCollider))->Disable();
    }
    ((SharedFilePtr *)(data_ov071_021230d0))->Release();
    ((SharedFilePtr *)(data_ov071_021230d8))->Release();
    return 1;
}
