//cpp
// @symbol _ZN11dScMgCard_cD1Ev
/* recovered: real C++ destructor. Explicit calls reproduce the ROM's own
   recovered body (func_ov006_020d95a4, pre-migration) exactly: destroy
   mArray2 first, then mArray1 (reverse of construction order), then the
   shared 0x270-byte table -- addressed by raw offset from `this` since
   the header keeps two of the table's own words named (unk_4f52,
   unk_511e), so there is no single contiguous member spanning it.
   Everything after -- own vtable store, mSysTracker destruction, chain to
   ~dScMgBase_c() -- is the compiler's own inlining of
   dScMgSingle3DBase_c's now-inline destructor (see
   include/dScMgSingle3DBase_c.h's own note). */
#include "dScMgCard_c.h"
dScMgCard_c::~dScMgCard_c()
{
    __destroy_arr(mArray2, 5, 0x30, (void *)func_ov006_020d96f0);
    __destroy_arr(mArray1, 5, 0x30, (void *)func_ov006_020d96e0);
    func_ov006_020c1c64((char *)this + 0x4f38);
}
