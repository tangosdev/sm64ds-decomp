//cpp
// @symbol _ZN12daBakubaku_c16CleanupResourcesEv
/* Vtable slot 3, previously func_ov032_02112124.
 *
 * Gives back the three shared files InitResources took: the model at 0x02113a40 and
 * the two animations at 0x02113a50 and 0x02113a48 -- released in that order, which is
 * NOT the order they were loaded, and not the order of their addresses either.
 *
 * The ROM's function reads no field of the object at all, and as a method `this`
 * simply arrives in r0 unused. Bytes verified unchanged by the promotion.
 */
#include "daBakubaku_c.h"

extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(void *sfp);
extern int data_ov032_02113a40[];
extern int data_ov032_02113a48[];
extern int data_ov032_02113a50[];
}

s32 daBakubaku_c::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov032_02113a40);
    _ZN13SharedFilePtr7ReleaseEv(data_ov032_02113a50);
    _ZN13SharedFilePtr7ReleaseEv(data_ov032_02113a48);
    return 1;
}
