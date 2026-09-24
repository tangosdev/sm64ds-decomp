//cpp
// @symbol _ZN8dActor_cD2Ev
/* D2, the base-object destructor. Same definition as the D1 file, and
 * deliberately so: dActor_c has no virtual bases, so mwcc emits D1 and D2 as
 * byte-identical code. Only how the ROM REACHES an address separates them --
 * a vtable slot holds D1, a derived destructor's base-chain `bl` reaches D2. */
#include "dActor_c.h"

extern "C" void func_0203b27c(void *list, void *node);
extern "C" void *data_0209b468;

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D2 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dActor_c() it emits,
 * which the class's D1 file already defines, so compiling the definition
 * below as well would define it twice. The base-object variant the ROM
 * spells _ZN8dActor_cD2Ev is the same body (dActor_c has no virtual base), and
 * host callers tearing down a dActor_c base subobject call it by that flat
 * name. This arm is that name: the one host destructor, called qualified
 * so it is a direct call. Nothing here reaches mwccarm: it builds the
 * `#else` arm and emits the ROM bytes it always emitted, and the object is
 * byte-identical either way. */
extern "C" dActor_c *_ZN8dActor_cD2Ev(dActor_c *thiz)
{
    thiz->dActor_c::~dActor_c();  /* the D1 body, through the one host symbol */
    return thiz;
}
#else
dActor_c::~dActor_c()
{
    func_0203b27c(&data_0209b468, &mActorListNode);
}
#endif
