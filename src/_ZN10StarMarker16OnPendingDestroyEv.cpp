//cpp
// @symbol _ZN10StarMarker16OnPendingDestroyEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "StarMarker.h"
extern void* _ZN5Actor10FindWithIDEj(unsigned int id);

void StarMarker::OnPendingDestroy()
{
    char* a = (char*)_ZN5Actor10FindWithIDEj(unk_1cc);
    if (a == 0) return;
    if (*(short*)(a + 0xce) >= 0) return;
    DeathTable_ClearBit(unk_1d6);
}
