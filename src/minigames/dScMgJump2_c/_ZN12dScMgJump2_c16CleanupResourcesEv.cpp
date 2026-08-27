//cpp
// @symbol _ZN12dScMgJump2_c16CleanupResourcesEv
#include "dScMgJump2_c.h"
#include "SharedFilePtr.h"
/* dScMgJump2_c::CleanupResources -- vtable slot 3, ov006 0x020ef110.
 *
 * Attributed by the vtable: dScMgJump2_c's own table is ov006 0x0213ccfc (the
 * address include/dScMgJump2_c.h's banner records) and its slot 3 relocates
 * here, so this is this class's own override and not dScMgD3DBase_c's. The
 * signature is include/fBase_c.h's own slot 3, `virtual s32
 * CleanupResources()`.
 *
 * data_ov006_021421b8 IS A POINTER TO the handle, not the handle: the ROM
 * loads the word and passes its VALUE, then stores 0 back. That is why it is
 * spelled `void *` here and cast at the call, unlike dScMg3DEsp_c's own
 * CleanupResources where the two globals ARE the handles and their addresses
 * are the arguments.
 *
 * Release() comes from include/SharedFilePtr.h rather than from a bare
 * `extern void _ZN13SharedFilePtr7ReleaseEv(void *)`: in a //cpp translation
 * unit that spelling mangles a SECOND time and emits a reference nothing
 * defines, which is the defect that header's own banner records. */

extern "C" {
extern void *data_ov006_021421b8;
void func_ov004_020ad90c(void);
}

s32 dScMgJump2_c::CleanupResources()
{
    ((SharedFilePtr *)data_ov006_021421b8)->Release();
    data_ov006_021421b8 = 0;
    func_ov004_020ad90c();
    return 1;
}
