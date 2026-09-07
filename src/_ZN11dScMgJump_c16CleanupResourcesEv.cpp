//cpp
// @symbol _ZN11dScMgJump_c16CleanupResourcesEv
#include "decl_common.h"
#include "dScMgJump_c.h"
/* dScMgJump_c::CleanupResources -- vtable slot 3.
 *
 * Attributed by the ROM's vtable: slot 3 is one of the three where this class's
 * table differs from dScMgD3DBase_c's. The old file's `recovered name:` comment
 * agreed, and here it is right.
 *
 * IT IGNORES `this` ENTIRELY -- the only state it releases is the file handle in
 * the ov006 global at 0x02142184, not anything on the object. That is why the
 * pre-migration file could be spelled `int func_ov006_020edffc(void)` and still
 * match: the ROM leaves r0 untouched, so a receiver it never reads costs nothing.
 * The signature has to be fBase_c::CleanupResources's, and it now is. */

extern "C" void _ZN13SharedFilePtr7ReleaseEv(void *p);
extern "C" void func_ov004_020ad90c(void);

s32 dScMgJump_c::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov006_02142184);
    data_ov006_02142184 = 0;
    func_ov004_020ad90c();
    return 1;
}
