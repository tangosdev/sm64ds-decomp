//cpp
// @symbol _ZN9HugeCover13InitResourcesEv
#include "HugeCover.h"
#include "SharedFilePtr.h"

namespace Event { s32 GetBit(u32 bit); }

/* dBgW_KcMbg::SetFile takes Fix12<int> by value. An ordinary member call
 * triggers mwccarm's by-value-class parameter homing and changes the ROM ABI,
 * so this one call deliberately retains the measured register-level view. */
extern "C" {
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, void *mtx, int scale, short angleY, void *clps);
extern SharedFilePtr data_ov032_02113acc;
extern SharedFilePtr data_ov032_02113ad4;
extern char data_ov032_02112f98;
}

s32 HugeCover::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov032_02113ad4), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    void *kcl = dBgW_Kc::LoadFile(data_ov032_02113acc);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY,
        &data_ov032_02112f98);

    return Event::GetBit(0xe) == 0;
}
