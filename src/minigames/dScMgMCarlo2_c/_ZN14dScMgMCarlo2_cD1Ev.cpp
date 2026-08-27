//cpp
// @symbol _ZN14dScMgMCarlo2_cD1Ev
/* recovered: real C++ destructor. Explicit calls reproduce the ROM's own
   recovered body (func_ov006_020f8ef4, pre-migration) exactly: destroy
   the 40-element array first (reverse of construction order), then the
   shared 0x270-byte table. Both are addressed by raw offset from `this`
   rather than a member name -- the header keeps the table's own two
   matched fields (unk_4f52, unk_511e) named, so there is no single
   contiguous member spanning either range. Everything after -- own
   vtable store, mSysTracker destruction, chain to ~dScMgBase_c() -- is
   the compiler's own inlining of dScMgSingle3DBase_c's now-inline
   destructor (see include/dScMgSingle3DBase_c.h's own note). */
#include "dScMgMCarlo2_c.h"
dScMgMCarlo2_c::~dScMgMCarlo2_c()
{
    __destroy_arr((char *)this + 0x51a8, 0x28, 0x30, (void *)func_ov006_020f8ff0);
    func_ov006_020c1c64((char *)this + 0x4f38);
}
