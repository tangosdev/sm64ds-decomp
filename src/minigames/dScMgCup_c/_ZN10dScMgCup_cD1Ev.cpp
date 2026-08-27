//cpp
// @symbol _ZN10dScMgCup_cD1Ev
/* recovered: real C++ destructor. The three explicit calls reproduce the
   ROM's own recovered body (func_ov006_020de988, pre-migration) exactly:
   destroy mArray2 first, then mArray1 (reverse of the factory's
   construction order), then the 0x4f38 table -- addressed by raw offset
   from `this` because the header keeps it opaque. Everything after -- own
   vtable store, mSysTracker destruction, chain to ~dScMgBase_c() -- is the
   compiler's own inlining of dScMgSingle3DBase_c's now-inline destructor
   (see include/dScMgSingle3DBase_c.h's own note). */
#include "dScMgCup_c.h"
dScMgCup_c::~dScMgCup_c()
{
    __destroy_arr(mArray2, 3, 8, (void *)NullDestructor_0203d47c);
    __destroy_arr(mArray1, 0x20, 0x18, (void *)func_ov006_020deac4);
    func_ov006_020c3288((char *)this + 0x4f38);
}
