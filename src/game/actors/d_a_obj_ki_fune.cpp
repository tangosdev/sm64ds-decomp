//cpp
/* daObjKi_Fune_c -- the Jolly Roger Bay ship (KI_FUNE / KI_FUNE_UP), ov016.
 *
 * One class drives both halves of the pair: actor 0x39 takes model index 0,
 * the ship that rocks on a sine of mBobAngle and loops a long sound while
 * the player is within 3000.0; anything else takes index 1, the still hull,
 * whose Behavior only keeps its collider live. InitResources succeeds for
 * index 0 only once star 1 of SublevelToLevel(8) is collected (with
 * data_0209f220 > 1), and for index 1 only until then, so the two never
 * coexist.
 *
 * This TU owns the nine-function linker run 0x0211260c..0x02112a00: the
 * seven class members plus the two daObjKi_Fune_c_classInit_* registry
 * factories (KI_FUNE_UP at 0x021129a0, KI_FUNE at 0x021129d0). mwccarm lays
 * object sections out in reverse source order, so the file is written
 * last-ROM-first: the factories, then InitResources down to
 * func_ov016_021126a8. The destructor is defined in the class body in
 * include/daObjKi_Fune_c.h: that emits D1 (0x0211260c) ahead of D0
 * (0x02112650) with no D2, and makes InitResources the key function, so
 * this TU also emits the class vtable and the inherited RTTI chain.
 *
 * comment leftovers:
 * - dBgW_KcMbg::SetFile (InitResources) is still called through its mangled
 *   symbol on void* arguments: it takes Fix12<int> by value, and the header
 *   method form changes the code size (notes/mwccarm-codegen.md 6az).
 * - func_020393a4 / func_020393d4 are still the linker names of two arm9
 *   dBgW hook helpers, data_02082214 the arm9 sine table Behavior indexes
 *   with mBobAngle, and data_0209f220 the star index this mission was
 *   entered for. Naming those belongs in arm9.
 * - data_ov016_021136dc, data_ov016_021136e4 and data_ov016_021149d4 are
 *   still the linker names of this overlay's per-model file and CLPS
 *   tables, indexed by mModelIndex; this TU does not own them.
 * - mBobAngle += 0xda is written through an s16 pun: the u16 member
 *   spelling fails ov016.
 * - func_ov016_021126a8 takes the object and recasts it, so it is a member
 *   now; it keeps its address-derived name because the ROM records none.
 */

#include "daObjKi_Fune_c.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "dBgW.h"

/* Two model handles construct through func_02017acc and destroy through
 * func_02017ab4. Two collision handles construct through func_02017b4c and
 * destroy through SharedFilePtr_Destruct_Clsn. The file tables stay ROM data. */
struct KiFuneModelFilePtr : SharedFilePtr {
    unsigned int words[2];
    KiFuneModelFilePtr(unsigned int fileId);
    ~KiFuneModelFilePtr();
};
struct KiFuneClsnFileHandle : SharedFilePtr {
    unsigned int words[2];
    KiFuneClsnFileHandle(unsigned int fileId);
    ~KiFuneClsnFileHandle();
};
typedef char KiFuneModelFilePtr_size_must_be_8[
    sizeof(KiFuneModelFilePtr) == 8 ? 1 : -1];
typedef char KiFuneClsnFileHandle_size_must_be_8[
    sizeof(KiFuneClsnFileHandle) == 8 ? 1 : -1];

extern "C" {
extern void func_020393a4(int* p, int v);
extern void func_020393d4(int* p, int v);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*, void*, void*, int, short, void*);
extern int IsStarCollected(int a, int b);
extern short data_02082214[];
extern unsigned char data_0209f220;
}

// @symbol daObjKi_Fune_c_classInit_KI_FUNE
extern "C" daObjKi_Fune_c *daObjKi_Fune_c_classInit_KI_FUNE()
{
    return new daObjKi_Fune_c();
}

// @symbol daObjKi_Fune_c_classInit_KI_FUNE_UP
extern "C" daObjKi_Fune_c *daObjKi_Fune_c_classInit_KI_FUNE_UP()
{
    return new daObjKi_Fune_c();
}

// @symbol _ZN14daObjKi_Fune_c13InitResourcesEv
int daObjKi_Fune_c::InitResources()
{
    void* clpsBlocks[2];
    unsigned int idx;
    void* file;
    int isModel0;
    clpsBlocks[0] = data_ov016_021149d4[0];
    clpsBlocks[1] = data_ov016_021149d4[1];
    isModel0 = (int)(actorID == 0x39);
    if (isModel0 != 0) mModelIndex = 0;
    else mModelIndex = 1;
    idx = mModelIndex;
    file = Model::LoadFile(*(SharedFilePtr *)data_ov016_021136e4[idx]);
    mModel.SetFile((BMD_File *)file, 1, -1);
    func_ov016_021126a8();
    UpdateClsnPosAndRot();
    idx = mModelIndex;
    file = dBgW_Kc::LoadFile(*(SharedFilePtr *)data_ov016_021136dc[idx]);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(&mMeshCollider, file, &mClsnMat, 0x1000, mAngleY, clpsBlocks[idx]);
    if (mModelIndex == 0) {
        func_020393d4((int*)&mMeshCollider, (int)&dBgW::UpdatePosWithTransform);
    }
    mMeshCollider.Enable(this);
    mSoundHandle = 0;
    unk_328 = 0;
    if (data_0209f220 > 1) {
        if (IsStarCollected(SublevelToLevel(8), 1) != 0) {
            if (mModelIndex == 0) goto ret1;
            return 0;
        }
    }
    if (mModelIndex == 0) return 0;
ret1:
    return 1;
}

// @symbol _ZN14daObjKi_Fune_c8BehaviorEv
int daObjKi_Fune_c::Behavior()
{
    if (mMeshCollider.IsEnabled() == 0) {
        mMeshCollider.Enable(this);
    }
    func_020393a4((int*)&mMeshCollider, 0x2000000);
    if (mModelIndex == 0) {
        *(s16 *)&mBobAngle += 0xda;
        mAngleX = (s16)((data_02082214[(mBobAngle >> 4) * 2] << 0xa) >> 0xc);
        if (DistToCPlayer() < 0xbb8000) {
            mSoundHandle = Sound::PlayLong(mSoundHandle, 3, 0x8b, *(Vector3*)&mCamSpacePosX, 0);
        }
        func_ov016_021126a8();
        UpdateClsnPosAndRot();
    }
    return 1;
}

// @symbol _ZN14daObjKi_Fune_c6RenderEv
int daObjKi_Fune_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN14daObjKi_Fune_c16CleanupResourcesEv
int daObjKi_Fune_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    ((SharedFilePtr *)(data_ov016_021136e4[mModelIndex]))->Release();
    ((SharedFilePtr *)(data_ov016_021136dc[mModelIndex]))->Release();
    return 1;
}

// @symbol _ZN14daObjKi_Fune_c19func_ov016_021126a8Ev
void daObjKi_Fune_c::func_ov016_021126a8()
{
    Matrix4x3_FromRotationXYZExt(&mModel.mat4x3, mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.m[9]  = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
}

/* Retail construction order. mwcc emits __sinit_d_a_obj_ki_fune.cpp. */
KiFuneModelFilePtr data_ov016_02114dd4(1595);
KiFuneModelFilePtr data_ov016_02114de4(1593);
KiFuneClsnFileHandle data_ov016_02114ddc(1596);
KiFuneClsnFileHandle data_ov016_02114dcc(1594);
