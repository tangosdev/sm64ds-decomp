//cpp
// @symbol _ZN15dScMgRoulette_cD1Ev
/* recovered: real C++ destructor. Explicit calls reproduce the ROM's own
   recovered body (func_ov006_0210788c, pre-migration) exactly: mModel2
   first, then mModel1, then mArray, then the shared 0x270-byte table --
   see include/dScMgRoulette_c.h's own note on why this can't be typed
   auto-destruction. Everything after -- own vtable store, mSysTracker
   destruction, chain to ~dScMgBase_c() -- is the compiler's own inlining
   of dScMgSingle3DBase_c's now-inline destructor. */
#include "dScMgRoulette_c.h"
dScMgRoulette_c::~dScMgRoulette_c()
{
    _ZN5ModelD1Ev(mModel2);
    _ZN5ModelD1Ev(mModel1);
    __destroy_arr(mArray, 5, 0x34, (void *)func_ov006_021079c8);
    func_ov006_020c1c64((char *)mTable);
}
