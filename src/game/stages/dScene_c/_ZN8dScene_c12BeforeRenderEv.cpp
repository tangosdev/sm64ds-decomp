//cpp
/* dScene_c::BeforeRender() at 0x0202e3a4, 0x24 bytes -- vtable slot 10.
 *
 * Chains to fBase_c's and narrows the result to 0 or 1. The narrowing is in the
 * ROM, not invented here: after the call the bytes are `cmp r0,#0; movne r0,#1;
 * moveq r0,#0`, which is exactly what a `!= 0` costs and would be absent if the
 * result were returned unchanged.
 *
 * Return type is `int` because fBase_c declares slot 10 as `int`; an override
 * whose return type differs is not an override at all, it is a nineteenth slot. */
#include "dScene_c.h"

int dScene_c::BeforeRender()
{
    return fBase_c::BeforeRender() != 0;
}
