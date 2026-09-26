//cpp
// @symbol _ZN12dScMgSlot1_cD0Ev
/* Deleting destructor for the same compiler-owned member teardown documented
   by src/_ZN12dScMgSlot1_cD1Ev.cpp. dScMgBase_c's own operator delete
   (its immediate base) covers the Memory::Deallocate call the
   pre-migration source made explicitly; no per-class copy needed, same
   precedent as every other dScMgBase_c leaf's D0. The tree-wide
   OnYoshiTryEat mislabel on this class landed HERE (its source comment
   said "recovered name: dScMgSlot1_c_OnYoshiTryEat") rather than on the
   real OnYoshiTryEat-shaped slot 18 helper -- this body is the textbook D0
   shape (member teardown, base D2, Deallocate), not gameplay logic. */
#include "dScMgSlot1_c.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dScMgSlot1_c() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" dScMgSlot1_c *_ZN12dScMgSlot1_cD0Ev(dScMgSlot1_c *thiz)
{
    thiz->dScMgSlot1_c::~dScMgSlot1_c();  /* the D1 body, through the one host symbol */
    dScMgSlot1_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
dScMgSlot1_c::~dScMgSlot1_c()
{
}
#endif
