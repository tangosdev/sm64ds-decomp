//cpp
// @symbol _ZN8ShipWing13InitResourcesEv

#include "Model.h"
#include "SharedFilePtr.h"
#include "ShipWing.h"

extern SharedFilePtr data_ov036_02114070;

int ShipWing::InitResources()
{
    BMD_File *file = (BMD_File *)Model::LoadFile(data_ov036_02114070);
    mModel.SetFile(file, 1, -1);
    mReverseMotion = param1 & 1;
    mPlaySound = (param1 >> 8) & 1;
    if (mReverseMotion != 0) {
        mAngleX += 0x2400;
        mAngleY += 0x8000;
        mAngleZ += 0x8000;
    }
    mBaseAngleX = mAngleX;
    mBaseAngleY = mAngleY;
    mBaseAngleZ = mAngleZ;
    mFrame = 0;
    return 1;
}
