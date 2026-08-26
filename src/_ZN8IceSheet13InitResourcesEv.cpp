//cpp
// @symbol _ZN8IceSheet13InitResourcesEv
#include "IceSheet.h"
#include "SharedFilePtr.h"

extern "C" {
/* The true member signature takes Fix12<int> by value. A faithful C++ call
 * homes that argument differently under mwccarm and does not reproduce this
 * call site, so retain the verified scalar ABI spelling. */
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, Matrix4x3 *mat, int scale,
    short angY, void *clps);
extern char data_ov002_0210d754;
}
extern SharedFilePtr IceSheet_ClsnFile;
extern SharedFilePtr IceSheet_ModelFile;

int IceSheet::InitResources()
{
    void *modelFile = Model::LoadFile(IceSheet_ModelFile);
    mModel.SetFile((BMD_File *)modelFile, 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    KCL_File *clsnFile = (KCL_File *)dBgW_Kc::LoadFile(IceSheet_ClsnFile);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, clsnFile, &mClsnMat, 0x199, mAngleY,
        &data_ov002_0210d754);
    return 1;
}
