//cpp
// @symbol _ZN5StageD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * Destroy through Stage and dScene_c (see _ZN5StageD1Ev.cpp), then hand the
 * object back through dScene_c's inline operator delete -- declared there
 * rather than on dBase_c, because mwcc only inlines it when found on
 * the class itself or its IMMEDIATE base, which for Stage is dScene_c.
 */
#include "Stage.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~Stage() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" Stage *_ZN5StageD0Ev(Stage *thiz)
{
    thiz->Stage::~Stage();         /* the D1 body, through the one host symbol */
    Stage::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
Stage::~Stage()
{
}
#endif
