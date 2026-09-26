//cpp
// @symbol _ZN14UnchainedChompD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * Nothing below spells the teardown. The three `__cxa_vec_cleanup` calls, the six
 * Model and six ShadowModel array elements, the sub-object destructors and the
 * dEnemyBase_c chain are all consequences of include/UnchainedChomp.h's typed
 * members; the deallocation is dEnemyBase_c's inline `operator delete`.
 *
 * The identical body stands in _ZN14UnchainedChompD1Ev.cpp: one
 * `~UnchainedChomp() {}` emits D2, D0 and D1 together and objisolate keeps the
 * variant each file is bound to.
 */
#include "UnchainedChomp.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~UnchainedChomp() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" UnchainedChomp *_ZN14UnchainedChompD0Ev(UnchainedChomp *thiz)
{
    thiz->UnchainedChomp::~UnchainedChomp();  /* the D1 body, through the one host symbol */
    UnchainedChomp::operator delete(thiz);    /* the class-specific delete D0 ends with */
    return thiz;
}
#else
UnchainedChomp::~UnchainedChomp()
{
}
#endif
