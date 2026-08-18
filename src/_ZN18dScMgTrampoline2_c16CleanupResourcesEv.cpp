//cpp
// @symbol _ZN18dScMgTrampoline2_c16CleanupResourcesEv
#include "decl_common.h"
#include "dScMgTrampoline2_c.h"
/* dScMgTrampoline2_c::CleanupResources -- vtable slot 3, ov006 0x0212318c.
 *
 * Attributed by the vtable: dScMgTrampoline2_c's own table is ov006 0x0213fc7c
 * (the address include/dScMgTrampoline2_c.h's banner records) and its slot 3
 * relocates here, so this is this class's own override and not
 * dScMgD3DBase_c's. The signature is include/fBase_c.h's own slot 3,
 * `virtual s32 CleanupResources()`.
 *
 * Takes no argument in the ROM's own terms and reads nothing off the scene --
 * two teardown calls and a constant. func_ov004_020ad90c is spelled with no
 * parameter here and with one in dScMgMCarlo2_c's own CleanupResources;
 * `this` is already in r0 either way, so neither spelling is evidence about
 * its real arity. */

extern "C" void func_ov004_020ad90c(void);

s32 dScMgTrampoline2_c::CleanupResources()
{
    func_ov006_020ceedc();
    func_ov004_020ad90c();
    return 1;
}
