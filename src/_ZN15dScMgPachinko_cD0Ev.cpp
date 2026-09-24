//cpp
// @symbol _ZN15dScMgPachinko_cD0Ev
#include "dScMgPachinko_c.h"
#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC ONLY MAKES ONE DESTRUCTOR.
 * It folds the Itanium D1/D0 pair into the single ??1dScMgPachinko_c@@QAE@XZ
 * that src/_ZN15dScMgPachinko_cD1Ev.cpp defines out of line
 * (include/dScMgPachinko_c.h declares ~dScMgPachinko_c() with no slot under
 * _MSC_VER), so this file cannot carry the same definition: the host link
 * refuses the pair (LNK2005). It spells out, in terms of that one host symbol,
 * what the variant this file is enrolled for does: the D1 body, then the
 * class-specific operator delete (Memory::Deallocate with the game heap, the
 * word at 0x020a0eac, from dScMgBase_c's inline operator delete). The
 * qualified call is direct. The port's slot 17 for this class
 * (hal/scene_mg.cpp's pch_d0, through port/unmatched/MgPachinko_Dtor.cpp)
 * calls this name. Nothing here reaches mwccarm: it builds the `#else` arm and
 * emits the ROM bytes it always emitted, and the object is byte-identical
 * either way. */
extern "C" dScMgPachinko_c *_ZN15dScMgPachinko_cD0Ev(dScMgPachinko_c *thiz)
{
    thiz->dScMgPachinko_c::~dScMgPachinko_c();          /* the D1 body, through the one host symbol */
    dScMgPachinko_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
/* dScMgPachinko_c::~dScMgPachinko_c (D0, deleting destructor) --
   dScMgBase_c's own operator delete (its immediate base) covers this; no
   per-class copy needed. */
dScMgPachinko_c::~dScMgPachinko_c()
{
}
#endif
