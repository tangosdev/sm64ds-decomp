//cpp
// @symbol _ZN13dScMgMCarlo_cD1Ev
/* recovered: real C++ destructor. The two explicit calls reproduce the
   ROM's own recovered body (func_ov006_020f7634, pre-migration) exactly:
   destroy mArray, then the shared 0x270-byte table -- addressed by raw
   offset from `this` since the header keeps two of the table's own words
   named (unk_4f52, unk_511e), so there is no single contiguous member
   spanning it. Everything after -- own vtable store, mSysTracker
   destruction, chain to ~dScMgBase_c() -- is the compiler's own inlining
   of dScMgSingle3DBase_c's now-inline destructor (see
   include/dScMgSingle3DBase_c.h's own note). */
#include "dScMgMCarlo_c.h"
dScMgMCarlo_c::~dScMgMCarlo_c()
{
    __destroy_arr(mArray, 0x50, 0x30, (void *)func_ov006_020f7730);
    func_ov006_020c1c64((char *)this + 0x4f38);
}
