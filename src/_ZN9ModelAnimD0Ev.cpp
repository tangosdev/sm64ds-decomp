//cpp
// @symbol _ZN9ModelAnimD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases --
 * which is why both vptr stores appear, the primary over _ZTV9ModelAnim and
 * the Animation subobject's onto VTable_Animation_ModelAnimThunk at +0x50 --
 * then return the object to the heap. Nobody writes that; the same empty
 * `~ModelAnim()` the D1 file defines emits D2, D0 and D1 together, and
 * objisolate keeps the one this file is bound to.
 *
 * The deallocation is Memory::operator_delete2, the family's own inline
 * operator delete -- the same reason the D1 file needs no heap mention.
 */
#include "ModelAnim.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~ModelAnim() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" ModelAnim *_ZN9ModelAnimD0Ev(ModelAnim *thiz)
{
    thiz->ModelAnim::~ModelAnim();     /* the D1 body, through the one host symbol */
    ModelAnim::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
ModelAnim::~ModelAnim()
{
}
#endif
