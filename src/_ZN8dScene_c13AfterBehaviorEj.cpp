//cpp
/* dScene_c::AfterBehavior(u32) at 0x0202e3c8, 0xc bytes -- vtable slot 8.
 *
 * A tail call to fBase_c::AfterBehavior (0x02043af8); see
 * src/_ZN8dScene_c11AfterRenderEj.cpp for why that is three words and why the
 * parameter type comes from the target's declaration rather than from these bytes. */
#include "dScene_c.h"

void dScene_c::AfterBehavior(u32 vfSuccess)
{
    fBase_c::AfterBehavior(vfSuccess);
}
