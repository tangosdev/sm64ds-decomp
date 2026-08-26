//cpp
// @symbol _ZN16daObjKinokoTag_c13InitResourcesEv

#include "MegaMushroomCreateTag.h"
#include "Model.h"
#include "SharedFilePtr.h"

/* dCcAc_c::Init takes Fix12<int> values by value. Spelling those as the real
 * class type makes mwccarm home the register arguments and grows this caller,
 * so retain the measured scalar ABI view at this one call boundary. */
extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    void *, dActor_c *, Fix12i, Fix12i, u32, u32);
extern "C" int data_ov002_0210da30[];

s32 daObjKinokoTag_c::InitResources()
{
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mMovingCylinderClsn, this, 0x12c000, 0x32000, 0x800002, 0x20);
    mGroupId = param1;
    Model::LoadFile(*(SharedFilePtr *)data_ov002_0210da30);
    return 1;
}
