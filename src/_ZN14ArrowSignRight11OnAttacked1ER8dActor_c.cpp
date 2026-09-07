//cpp
// @symbol _ZN14ArrowSignRight11OnAttacked1ER8dActor_c
#include "ArrowSignRight.h"
// recovered name: ArrowSignRight_OnAttacked1
/* recovered: renamed to Class_Method */
/* daObjYajirusi_c::OnAttacked1 - recovered from vtable slot identity. The
   draft indexed a raw 64-entry function-pointer vtable at word 0x7c/4 =
   31; that slot is Kill(), already a named virtual on this class, so
   this calls it directly. `other`'s actorID (offset 0xc) has no public
   C++ name on dActor_c (only the C-fallback flat struct names it), so it
   stays a raw offset read, matching every other sibling override in this
   tree that reads another actor's fields this way. */
int ArrowSignRight::OnAttacked1(dActor_c &other)
{
    unsigned r = (*(unsigned short*)((char*)&other + 0xc) == 0xce) ? 1u : 0u;
    if (r == 0) return;
    Kill();
}
