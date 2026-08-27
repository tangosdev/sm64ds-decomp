//cpp
// @symbol _ZN10dScMgBSC_cD1Ev
/* recovered: real C++ destructor. Explicit calls reproduce the ROM's own
   recovered body (func_ov006_02124908, pre-migration) exactly: destroy
   the trivial 2-element array first (reverse of construction order),
   then the shared 0x270-byte table. Everything after -- own vtable
   store, mSysTracker destruction, chain to ~dScMgBase_c() -- is the
   compiler's own inlining of dScMgSingle3DBase_c's now-inline destructor
   (see include/dScMgSingle3DBase_c.h's own note). */
#include "dScMgBSC_c.h"
dScMgBSC_c::~dScMgBSC_c()
{
    __destroy_arr(mArray, 2, 8, (void *)NullDestructor_0203d47c);
    func_ov006_020c1c64((char *)mTable);
}
