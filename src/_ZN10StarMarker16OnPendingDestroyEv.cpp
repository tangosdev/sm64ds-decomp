//cpp
// @symbol _ZN10StarMarker16OnPendingDestroyEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "StarMarker.h"
extern "C" {
extern void* _ZN8dActor_c10FindWithIDEj(unsigned int id);
}

void StarMarker::OnPendingDestroy()
{
    char* a = (char*)_ZN8dActor_c10FindWithIDEj(mSpawnedActorID);
    if (a == 0) return;
    if (*(short*)(a + 0xce) >= 0) return;
    DeathTable_ClearBit(mSpawnedDeathTableID);
}
