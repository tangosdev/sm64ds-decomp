//cpp
// @symbol _ZN18BowserFireSeaArena16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BowserFireSeaArena.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int BowserFireSeaArena_ClsnFile[];
extern int BowserFireSeaArena_ModelFile[];

int BowserFireSeaArena::CleanupResources()
{
    if (((dBgW *)((char *)&mMovingMeshCollider2))->IsEnabled()) {
        ((dBgW *)((char *)&mMovingMeshCollider2))->Disable();
    }
    ((SharedFilePtr *)(BowserFireSeaArena_ModelFile))->Release();
    ((SharedFilePtr *)(BowserFireSeaArena_ClsnFile))->Release();
    return 1;
}
