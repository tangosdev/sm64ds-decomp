//cpp
// @symbol _ZN12dScMgAmida_c5Unk36Ev
/* dScMgAmida_c::Unk36 -- own new vtable slot 36 (see include/dScMgAmida_c.h's
   class banner). A placeholder name, not tree-wide reconstructed -- picked
   because it is genuinely called virtually from three of this class's own
   real methods (InitResources, Behavior, Render), unlike every other
   dScMgBase_c leaf's slot 18+ helpers. Reads a Scene/ActorBase-level field
   at `this+8`, inherited from further up the hierarchy than dScMgBase_c --
   same "leave it a raw offset, not a named field" precedent dScMgSlot1_c.h
   documents for the same offset. */
#include "dScMgAmida_c.h"

int dScMgAmida_c::Unk36()
{
    return ((*(int *)((char *)this + 8)) & 0xff) == 2;
}
