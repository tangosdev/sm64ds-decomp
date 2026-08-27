//cpp
// @symbol _ZN15dScMgRoulette_c16CleanupResourcesEv
#include "decl_common.h"
#include "dScMgRoulette_c.h"
/* dScMgRoulette_c::CleanupResources -- vtable slot 3, ov006 0x0210980c.
 *
 * Named from the table: 0x0210980c is the word slot 3 of
 * _ZTV15dScMgRoulette_c holds where its base's table holds something else, so
 * it is this class's own override of the virtual fBase_c declares.
 *
 * 0x530c falls inside the header's pad_52ac span, which has no named field
 * yet, so the one subobject this tears down is still reached by offset. */

s32 dScMgRoulette_c::CleanupResources()
{
    func_ov006_0210858c((char *)this + 0x530c);
    return 1;
}
