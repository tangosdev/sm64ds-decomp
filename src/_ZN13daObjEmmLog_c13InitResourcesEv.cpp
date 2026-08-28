//cpp
// @symbol _ZN13daObjEmmLog_c13InitResourcesEv
#include "daObjEmmLog_c.h"

struct daObjEmmLog_c_Resources {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};

extern daObjEmmLog_c_Resources data_ov052_021124d4;

/* 2004/b56 homes the real Fix12<int>-by-value member form through a literal
 * pool instead of passing the immediate found in the ROM. Keep this one ABI
 * seam until the shared fixed-point compiler wall is solved. */
extern "C" void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *, KCL_File *, const Matrix4x3 *, Fix12i, s16, CLPS_Block *);

int daObjEmmLog_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(*data_ov052_021124d4.model), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider,
        (KCL_File *)dBgW_Kc::LoadFile(*data_ov052_021124d4.collision),
        &mClsnMat, 0x1000, mAngleY, data_ov052_021124d4.clps);

    mBasePosY = mPosY;
    u8 amplitude = param1 & 0xff;
    if (amplitude == 0xff || param1 == 0)
        mBobAmplitude = 0x64000;
    else
        mBobAmplitude = amplitude * 0xa000;

    mSpinAngle = mAngleX;
    return 1;
}
