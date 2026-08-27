//cpp
// @symbol _ZN12dScMgSlot1_cD0Ev
/* Real out-of-line definition, identical body to
   src/_ZN12dScMgSlot1_cD1Ev.cpp -- see that file's note and
   include/dScMgSlot1_c.h's own note. dScMgBase_c's own operator delete
   (its immediate base) covers the Memory::Deallocate call the
   pre-migration source made explicitly; no per-class copy needed, same
   precedent as every other dScMgBase_c leaf's D0. The tree-wide
   OnYoshiTryEat mislabel on this class landed HERE (its source comment
   said "recovered name: dScMgSlot1_c_OnYoshiTryEat") rather than on the
   real OnYoshiTryEat-shaped slot 18 helper -- this body is the textbook D0
   shape (vtable writes, base D2, Deallocate), not gameplay logic. */
#include "dScMgSlot1_c.h"

dScMgSlot1_c::~dScMgSlot1_c()
{
    *(void **)((char *)this + 0x4660) = &data_ov006_0213e5d4;
    *(void **)((char *)this + 0x4660) = (void *)&data_ov001_020ad494;
}
