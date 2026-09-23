//cpp
// @symbol _ZN8dActor_cD0Ev
/* D0, the DELETING destructor: run the destructor body, then hand the object
 * to operator delete. Longer than the D1/D2 pair, but built from the same one
 * `dActor_c::~dActor_c()` definition. */
#include "dActor_c.h"

extern "C" void func_0203b27c(void *list, void *node);
extern "C" void *data_0209b468;

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dActor_c() it
 * emits, which the class's D1 file already defines, so compiling the
 * definition below as well would define that symbol twice. This arm
 * spells out, in terms of it, what the deleting destructor this file is
 * enrolled for does: the D1 body, called qualified so it is a direct call
 * even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" dActor_c *_ZN8dActor_cD0Ev(dActor_c *thiz)
{
    thiz->dActor_c::~dActor_c();      /* the D1 body, through the one host symbol */
    dActor_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
dActor_c::~dActor_c()
{
    func_0203b27c(&data_0209b468, &mActorListNode);
}
#endif
