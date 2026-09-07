//cpp
// @symbol _ZN10daKpa2Bg_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daKpa2Bg_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int daKpa2Bg_c_ClsnFile[];
extern int daKpa2Bg_c_ModelFile[];

int daKpa2Bg_c::CleanupResources()
{
    if (((dBgW *)((char *)&mMovingMeshCollider2))->IsEnabled()) {
        ((dBgW *)((char *)&mMovingMeshCollider2))->Disable();
    }
    ((SharedFilePtr *)(daKpa2Bg_c_ModelFile))->Release();
    ((SharedFilePtr *)(daKpa2Bg_c_ClsnFile))->Release();
    return 1;
}
