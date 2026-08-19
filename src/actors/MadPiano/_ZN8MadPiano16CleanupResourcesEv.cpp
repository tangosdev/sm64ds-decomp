//cpp
// @symbol _ZN8MadPiano16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "MadPiano.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int MadPiano_AnimFile[];
extern int MadPiano_ClsnFile[];
extern int MadPiano_ModelFile[];

int MadPiano::CleanupResources()
{
    if (((dBgW *)((char *)&mMeshCollider))->IsEnabled()) {
        ((dBgW *)((char *)&mMeshCollider))->Disable();
    }
    ((SharedFilePtr *)(MadPiano_ModelFile))->Release();
    ((SharedFilePtr *)(MadPiano_AnimFile))->Release();
    ((SharedFilePtr *)(MadPiano_ClsnFile))->Release();
    return 1;
}
