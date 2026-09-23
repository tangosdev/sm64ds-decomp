//cpp
// @symbol _ZN7daKrb_cD0Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The five typed sub-objects in include/daKrb_c.h (dCcAc_c, dBgCh_Actr, ModelAnim,
 * ShadowModel, MaterialChanger) are torn down in reverse declaration order and then
 * the dCapEnemy_c base chain runs, all of it compiler-generated.
 */
#include "daKrb_c.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~daKrb_c() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" daKrb_c *_ZN7daKrb_cD0Ev(daKrb_c *thiz)
{
    thiz->daKrb_c::~daKrb_c();       /* the D1 body, through the one host symbol */
    daKrb_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
daKrb_c::~daKrb_c()
{
}
#endif
