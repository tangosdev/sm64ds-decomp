//cpp
// @symbol _ZN12dScMg3DEsp_cD0Ev
/* Real out-of-line definition, identical body to
   _ZN12dScMg3DEsp_cD1Ev.cpp -- see that file's note. */
#include "dScMg3DEsp_c.h"
#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dScMg3DEsp_c() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" dScMg3DEsp_c *_ZN12dScMg3DEsp_cD0Ev(dScMg3DEsp_c *thiz)
{
    thiz->dScMg3DEsp_c::~dScMg3DEsp_c();  /* the D1 body, through the one host symbol */
    dScMg3DEsp_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
dScMg3DEsp_c::~dScMg3DEsp_c()
{
    _ZN18TextureTransformerD1Ev(mTextureTransformer);
    _ZN15dMg3DEspModel_cD1Ev((char *)this + 0x4fd8);
    _ZN5ModelD1Ev(mModel2);
    _ZN5ModelD1Ev(mModel1);
}
#endif
