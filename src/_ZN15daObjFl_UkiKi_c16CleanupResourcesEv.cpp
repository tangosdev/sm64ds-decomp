//cpp
// @symbol _ZN15daObjFl_UkiKi_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daObjFl_UkiKi_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int FloatingFloorLllBig_ClsnFile[];
extern int FloatingFloorLllBig_ModelFile[];

int daObjFl_UkiKi_c::CleanupResources()
{
    if (((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
        ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    }
    ((SharedFilePtr *)(FloatingFloorLllBig_ModelFile))->Release();
    ((SharedFilePtr *)(FloatingFloorLllBig_ClsnFile))->Release();
    return 1;
}
