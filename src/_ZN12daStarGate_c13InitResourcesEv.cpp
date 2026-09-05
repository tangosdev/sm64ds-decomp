//cpp
// @symbol _ZN12daStarGate_c13InitResourcesEv

#include "Model.h"
#include "SharedFilePtr.h"
#include "daStarGate_c.h"

extern "C" void Matrix4x3_FromRotationY(Matrix4x3 *matrix, int angle);

extern SharedFilePtr data_ov100_02148934;
extern u8 data_0209f250;
extern int data_0209f394[];

int daStarGate_c::InitResources()
{
    param1 = (u32)param1 >> 16;

    BMD_File *file = (BMD_File *)Model::LoadFile(data_ov100_02148934);
    if (!mModel.SetFile(file, 1, 1))
        return 0;

    Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
    mModel.mat4x3.t.x = mPosX >> 3;
    mModel.mat4x3.t.y = mPosY >> 3;
    mModel.mat4x3.t.z = mPosZ >> 3;
    ChangeState(&ST_WAIT,
                (Player *)data_0209f394[data_0209f250]);
    return 1;
}
