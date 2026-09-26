//cpp
// @symbol _ZN12dScMg3DEsp_cD0Ev
/* dScMg3DEsp_c's destructor; the build keeps D0 from this file. The
   definition is the same as in the D1 file. */
#include "dScMg3DEsp_c.h"
#include "dMg3DEspModel_c.h"
#include "TextureTransformer.h"
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
    ((TextureTransformer *)mTextureTransformer)->TextureTransformer::~TextureTransformer();
    ((dMg3DEspModel_c *)pad_4fd8)->~dMg3DEspModel_c();
    ((Model *)mModel2)->Model::~Model();
    ((Model *)mModel1)->Model::~Model();
}
#endif
