//cpp
// @symbol _ZN10RockPillarD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases --
 * which is why more than one vptr store appears -- then return the object to
 * its heap. Nobody writes that; declaring `~RockPillar()` is enough, because mwcc
 * emits D2, D0 and D1 together and objisolate keeps the one this file is bound
 * to.
 *
 * The deallocation is an inline operator delete, which is why nothing below
 * mentions a heap.
 */
#include "RockPillar.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into one `??1RockPillar@@QAE@XZ`,
 * which the class's D1 file already defines, so compiling the definition
 * below here as well would define that symbol twice. This arm spells out,
 * in terms of it, what the deleting destructor this file is enrolled for
 * does, the way the Model family's D0 files do: the D1 body, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" RockPillar *_ZN10RockPillarD0Ev(RockPillar *thiz)
{
    thiz->~RockPillar();                /* the D1 body, through the one host symbol */
    RockPillar::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
RockPillar::~RockPillar()
{
}
#endif
