//cpp
// @symbol _ZN11dScMgCoin_cD0Ev
#include "dScMgCoin_c.h"
/* dScMgCoin_c::~dScMgCoin_c (D0, deleting destructor) -- dScMgBase_c's own
   operator delete (its immediate base) covers this; no per-class copy
   needed. The tree-wide OnYoshiTryEat mislabel on this class landed HERE
   (its source comment said "recovered name: dScMgCoin_c_OnYoshiTryEat")
   rather than on the real OnYoshiTryEat at slot 18 -- this body is the
   textbook D0 shape (vtable write, base D2, Deallocate), not gameplay
   logic. */
#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dScMgCoin_c() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" dScMgCoin_c *_ZN11dScMgCoin_cD0Ev(dScMgCoin_c *thiz)
{
    thiz->dScMgCoin_c::~dScMgCoin_c();   /* the D1 body, through the one host symbol */
    dScMgCoin_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
dScMgCoin_c::~dScMgCoin_c()
{
}
#endif
