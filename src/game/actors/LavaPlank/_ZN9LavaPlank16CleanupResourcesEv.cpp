//cpp
// @symbol _ZN9LavaPlank16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "LavaPlank.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int FloatingFloorLllBig_ClsnFile[];
extern int FloatingFloorLllBig_ModelFile[];

int LavaPlank::CleanupResources()
{
    if (((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
        ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    }
    ((SharedFilePtr *)(FloatingFloorLllBig_ModelFile))->Release();
    ((SharedFilePtr *)(FloatingFloorLllBig_ClsnFile))->Release();
    return 1;
}
