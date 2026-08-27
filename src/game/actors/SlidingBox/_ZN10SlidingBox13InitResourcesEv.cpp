//cpp
// @symbol _ZN10SlidingBox13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SlidingBox.h"
#include "SharedFilePtr.h"
extern "C" {
/* The two SetFile/Init names carry Fix12<int> by value. Defining or calling
 * them as ordinary methods changes mwccarm's ABI, so these stay raw at the
 * measured by-value-class wall documented in dBgActor_c.h. */
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* thiz, void* kcl, void* mtx, int fix, short s, void* clps);
extern void func_020393d4(void* p, void* v);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* thiz, void* act, int fix, int t, void* vr, int t2);
extern void _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_(void);
}

int SlidingBox::InitResources()
{
    BMD_File *modelFile = (BMD_File *)Model::LoadFile(
        *(SharedFilePtr *)data_ov016_02114e74);
    mModel.SetFile(modelFile, 1, -1);
    UpdateModel();
    UpdateClsnPosAndRot();
    KCL_File *clsnFile = (KCL_File *)dBgW_Kc::LoadFile(
        *(SharedFilePtr *)data_ov016_02114e6c);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, clsnFile, &mClsnMat, 0x199, mAngleY, data_ov016_02113bac);
    func_020393d4(&mMeshCollider, (void*)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x14000, 0x14000, 0, 0);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x14000;
    mShip = 0;
    mState = 0;
    mSoundID = 0;
    return 1;
}
