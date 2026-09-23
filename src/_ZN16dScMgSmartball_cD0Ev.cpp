//cpp
// @symbol _ZN16dScMgSmartball_cD0Ev
/* Real out-of-line definition, identical body to
   _ZN16dScMgSmartball_cD1Ev.cpp -- see that file's note. */
#include "dScMgSmartball_c.h"
#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dScMgSmartball_c() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" dScMgSmartball_c *_ZN16dScMgSmartball_cD0Ev(dScMgSmartball_c *thiz)
{
    thiz->dScMgSmartball_c::~dScMgSmartball_c();  /* the D1 body, through the one host symbol */
    dScMgSmartball_c::operator delete(thiz);      /* the class-specific delete D0 ends with */
    return thiz;
}
#else
dScMgSmartball_c::~dScMgSmartball_c()
{
    __cxa_vec_cleanup(mArray4, 0x40, 0x24, (void *)func_ov006_0210d894);
    __cxa_vec_cleanup(mArray3, 0x10, 8, (void *)NullDestructor_0203d47c);
    __cxa_vec_cleanup(mArray2, 0x10, 8, (void *)NullDestructor_0203d47c);
    __cxa_vec_cleanup(mArray1, 5, 8, (void *)NullDestructor_0203d47c);
}
#endif
