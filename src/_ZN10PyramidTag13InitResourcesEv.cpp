//cpp
// @symbol _ZN10PyramidTag13InitResourcesEv

#include "PyramidTag.h"

/* dCcAc_c::Init takes Fix12<int> values by value. Spelling those as the real
 * class type makes mwccarm home the register arguments and grows this caller,
 * so retain the measured scalar ABI view at this one call boundary. */
extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    void *, dActor_c *, Fix12i, Fix12i, u32, u32);

s32 PyramidTag::InitResources()
{
    dActor_c *top = dActor_c::FindWithActorID(0x55, 0);
    if (top == 0) {
        MarkForDestruction();
        return 1;
    }

    mPyramidTopID = top->uniqueID;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mCylinder, this, 0x7d000, 0x28000, 2, 0x400000);
    return 1;
}
