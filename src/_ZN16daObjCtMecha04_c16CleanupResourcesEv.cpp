//cpp
// @symbol _ZN16daObjCtMecha04_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daObjCtMecha04_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern char data_ov065_0211d194[];
extern char data_ov065_0211d198[];

int daObjCtMecha04_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    ((SharedFilePtr *)(*(void **)(data_ov065_0211d194 + mVariant * 0xc)))->Release();
    ((SharedFilePtr *)(*(void **)(data_ov065_0211d198 + mVariant * 0xc)))->Release();
    return 1;
}
