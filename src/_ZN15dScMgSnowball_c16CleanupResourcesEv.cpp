//cpp
// @symbol _ZN15dScMgSnowball_c16CleanupResourcesEv
#include "dScMgSnowball_c.h"
/* dScMgSnowball_c::CleanupResources -- vtable slot 3, ov006 0x021291f8.
 *
 * Named from the table: 0x021291f8 is the word slot 3 of
 * _ZTV15dScMgSnowball_c holds where its base's table holds something else, so
 * it is this class's own override of the virtual fBase_c declares.
 *
 * It frees the BMD this class's own InitResources (slot 0, 0x02129268) parked
 * in unk_abf4 -- the header's note that 0xabf4 is "the constructor's own `= 0`
 * write" and this teardown are the two ends of that field's lifetime. It is a
 * file pointer, so it is read back through a cast rather than retyped here:
 * retyping the field is the model slice's job, not this rename's. */

extern "C" void Ov004_Deallocate(void *p);

s32 dScMgSnowball_c::CleanupResources()
{
    Ov004_Deallocate(*(void **)&unk_abf4);
    return 1;
}
