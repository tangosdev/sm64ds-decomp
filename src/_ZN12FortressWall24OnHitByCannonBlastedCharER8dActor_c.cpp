//cpp
// @symbol _ZN12FortressWall24OnHitByCannonBlastedCharER8dActor_c
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "FortressWall.h"
// recovered name: FortressWall_OnHitByCannonBlastedChar
/* recovered: renamed to Class_Method */
/* daObjBk_Kabe_c::OnHitByCannonBlastedChar - recovered from vtable slot
   identity. The ROM body is just a virtual dispatch to slot 31 (Kill) on
   itself -- getting hit by a cannon-blasted character kills the actor --
   so this calls the real virtual Kill() rather than a raw vtable-slot
   shim. Nothing in the tree reads this slot's return value (see
   dActor_c.h's note by the declaration), so the missing explicit return
   matches the ROM, which never sets r0 here either. */
int FortressWall::OnHitByCannonBlastedChar(dActor_c &other)
{
    Kill();
}
