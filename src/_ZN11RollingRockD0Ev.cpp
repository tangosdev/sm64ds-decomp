//cpp
// @symbol _ZN11RollingRockD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases, then
 * return the object to its heap. Declaring `~RollingRock()` is enough -- mwcc emits
 * D2, D0 and D1 together and objisolate keeps the one this file is bound to.
 *
 * The deallocation is an inline operator delete -- dEnemyBase_c's, reachable because
 * dEnemyBase_c is this class's IMMEDIATE base.
 */
#include "RollingRock.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into one `??1RollingRock@@QAE@XZ`,
 * which the class's D1 file already defines, so compiling the definition
 * below here as well would define that symbol twice. This arm spells out,
 * in terms of it, what the deleting destructor this file is enrolled for
 * does, the way the Model family's D0 files do: the D1 body, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" RollingRock *_ZN11RollingRockD0Ev(RollingRock *thiz)
{
    thiz->~RollingRock();                /* the D1 body, through the one host symbol */
    RollingRock::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
RollingRock::~RollingRock()
{
}
#endif
