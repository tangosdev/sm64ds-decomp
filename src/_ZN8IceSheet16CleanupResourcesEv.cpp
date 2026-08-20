//cpp
// @symbol _ZN8IceSheet16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "IceSheet.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int IceSheet_ClsnFile[];
extern int IceSheet_ModelFile[];

int IceSheet::CleanupResources()
{
    if (((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
        ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    }
    ((SharedFilePtr *)(IceSheet_ModelFile))->Release();
    ((SharedFilePtr *)(IceSheet_ClsnFile))->Release();
    return 1;
}
