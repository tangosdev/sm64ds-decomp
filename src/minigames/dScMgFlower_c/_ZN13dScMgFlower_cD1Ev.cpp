//cpp
// @symbol _ZN13dScMgFlower_cD1Ev
/* recovered: real C++ destructor. The two explicit calls reproduce the
   ROM's own recovered body (func_ov006_0212a554, pre-migration) exactly:
   destroy the 0x51f8 object first, then mArray -- reverse of the factory's
   construction order. The 0x51f8 object is addressed by raw offset from
   `this` because the header keeps it opaque. Everything after -- own
   vtable store, mSysTracker destruction, chain to ~dScMgBase_c() -- is the
   compiler's own inlining of dScMgSingle3DBase_c's now-inline destructor
   (see include/dScMgSingle3DBase_c.h's own note). */
#include "dScMgFlower_c.h"
dScMgFlower_c::~dScMgFlower_c()
{
    func_ov006_020c3e70((char *)this + 0x51f8);
    __destroy_arr(mArray, 0x16, 0x20, (void *)func_ov006_0212a650);
}
