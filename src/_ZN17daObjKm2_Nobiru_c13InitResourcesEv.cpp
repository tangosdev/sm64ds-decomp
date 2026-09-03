//cpp
// @symbol _ZN17daObjKm2_Nobiru_c13InitResourcesEv
#include "daObjKm2_Nobiru_c.h"
#include "SharedFilePtr.h"

extern SharedFilePtr PoleLift_ClsnFile;
extern SharedFilePtr PoleLift_ModelFile;
extern "C" CLPS_Block data_ov045_021125b0;

/* SetFile's by-value Fix12<int> is the same measured definition-side ABI wall
 * as SetScaleY. Keep the cartridge spelling at this call boundary. */
extern "C" void _ZN14dBgW_KcMbgSclY7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbgSclY *, KCL_File *, const Matrix4x3 &, s32, s16, CLPS_Block &);
extern "C" void func_020396c0(void *, s32);

int daObjKm2_Nobiru_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(PoleLift_ModelFile), 1, -1);
    UpdateModelTransform();
    UpdateColliderTransform();

    _ZN14dBgW_KcMbgSclY7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mCollider, (KCL_File *)dBgW_Kc::LoadFile(PoleLift_ClsnFile),
        mColliderTransform, 0x1000, mAngleY, data_ov045_021125b0);
    func_020396c0(&mCollider, 4);
    mCollider.unk_4d = 1;
    mCollider.Enable(this);
    mGrowing = 1;
    return 1;
}
