//cpp
// @symbol _ZN12dScMgSlot3_cD1Ev
/* recovered: real C++ destructor. The one explicit call reproduces the
   ROM's own recovered body (func_ov006_0210a954, pre-migration): destroy
   the 0x4f38 table, addressed by raw offset from `this` because the header
   keeps it opaque. Everything after -- own vtable store, mSysTracker
   destruction, chain to ~dScMgBase_c() -- is the compiler's own inlining
   of dScMgSingle3DBase_c's now-inline destructor (see
   include/dScMgSingle3DBase_c.h's own note). */
#include "dScMgSlot3_c.h"
dScMgSlot3_c::~dScMgSlot3_c()
{
    func_ov006_020c21e4((char *)this + 0x4f38);
}
