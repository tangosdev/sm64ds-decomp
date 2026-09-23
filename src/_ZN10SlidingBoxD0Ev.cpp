//cpp
// @symbol _ZN10SlidingBoxD0Ev
/* Real compiler-spelled deleting destructor. The inherited inline
 * dActor_c::operator delete supplies the actor-heap deallocation after the
 * complete SlidingBox destruction sequence. */
#include "SlidingBox.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into one `??1SlidingBox@@QAE@XZ`,
 * which the class's D1 file already defines, so compiling the definition
 * below here as well would define that symbol twice. This arm spells out,
 * in terms of it, what the deleting destructor this file is enrolled for
 * does, the way the Model family's D0 files do: the D1 body, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" SlidingBox *_ZN10SlidingBoxD0Ev(SlidingBox *thiz)
{
    thiz->~SlidingBox();                /* the D1 body, through the one host symbol */
    SlidingBox::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
SlidingBox::~SlidingBox()
{
}
#endif
