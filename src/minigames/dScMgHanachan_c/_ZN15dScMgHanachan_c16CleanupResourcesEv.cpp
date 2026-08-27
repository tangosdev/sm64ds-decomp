//cpp
// @symbol _ZN15dScMgHanachan_c16CleanupResourcesEv
/* dScMgHanachan_c::CleanupResources -- slot 3, the fBase_c/dScene_c-level
   virtual (see include/fBase_c.h), not one of dScMgBase_c's usual six
   overrides. */
#include "dScMgHanachan_c.h"

extern "C" void func_ov004_020ad90c(void *);

s32 dScMgHanachan_c::CleanupResources()
{
    func_ov004_020ad90c(this);
    return 1;
}
