//cpp
// @symbol _ZN5Pokey16OnPendingDestroyEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Pokey.h"

void Pokey::OnPendingDestroy()
{
    int r1 = *(unsigned short *)((char *)&actorID);
    r1 = (r1 == 0xf1);
    if (r1) return;
    void *p = *(void **)((char *)&mNextSegment);
    if (!p) return;
    do {
        func_ov096_0213585c(p);
        p = *(void **)((char *)p + 0x394);
    } while (p);
}
