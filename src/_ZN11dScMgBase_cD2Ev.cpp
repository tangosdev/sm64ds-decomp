//cpp
// @symbol _ZN11dScMgBase_cD2Ev
/* recovered: real C++ base-subobject destructor
 *
 * The ordinary destructor body clears the shared minigame pointer and tears
 * down the touch-icon array. CodeWarrior supplies this class's vptr restore
 * and the dScene_c/fBase_c base-subobject teardown for the D2 variant.
 */
#include "dScMgBase_c.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D2 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dScMgBase_c() it emits,
 * which the class's D1 file already defines, so compiling the definition
 * below as well would define it twice. The base-object variant the ROM
 * spells _ZN11dScMgBase_cD2Ev is the same body (dScMgBase_c has no virtual base), and
 * host callers tearing down a dScMgBase_c base subobject call it by that flat
 * name. This arm is that name: the one host destructor, called qualified
 * so it is a direct call. Nothing here reaches mwccarm: it builds the
 * `#else` arm and emits the ROM bytes it always emitted, and the object is
 * byte-identical either way. */
extern "C" dScMgBase_c *_ZN11dScMgBase_cD2Ev(dScMgBase_c *thiz)
{
    thiz->dScMgBase_c::~dScMgBase_c();  /* the D1 body, through the one host symbol */
    return thiz;
}
#else
dScMgBase_c::~dScMgBase_c()
{
    data_ov004_020beb68 = 0;
}
#endif
