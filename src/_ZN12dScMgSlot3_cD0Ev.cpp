//cpp
// @symbol _ZN12dScMgSlot3_cD0Ev
/* Real out-of-line definition, identical body to
   _ZN12dScMgSlot3_cD1Ev.cpp -- see that file's note. */
#include "dScMgSlot3_c.h"
#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dScMgSlot3_c() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" dScMgSlot3_c *_ZN12dScMgSlot3_cD0Ev(dScMgSlot3_c *thiz)
{
    thiz->dScMgSlot3_c::~dScMgSlot3_c();  /* the D1 body, through the one host symbol */
    dScMgSlot3_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
dScMgSlot3_c::~dScMgSlot3_c()
{
    func_ov006_020c21e4((char *)this + 0x4f38);
}
#endif
