//cpp
// @symbol _ZN12WaterDiamond13InitResourcesEv
#include "WaterDiamond.h"
#include "SharedFilePtr.h"

/* This one call deliberately retains the ABI-spelled entry point. Its ROM
 * signature takes Fix12<int> by value; asking this compiler to pass the real
 * class type homes the register arguments to the stack and grows the caller.
 * The scalar view below is the measured 2004/b56 codegen wall, not a guessed
 * replacement API. */
extern "C" {
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    void *, dActor_c *, Fix12i, Fix12i, u32, u32);
}

extern SharedFilePtr data_ov029_02114270;

int WaterDiamond::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov029_02114270), 1, -1);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mCylinder, this, 0x32000, 0x64000, 0x800002, 0);
    mWaterID = 0;
    mWaterParam = param1 & 1;
    mActive = 0;
    mAngleY = 0;
    return 1;
}
