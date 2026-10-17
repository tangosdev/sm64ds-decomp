//cpp
/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_wc_mizu_AssetId {
    ASSET_WC_MIZU_BMD = 0x06ca,
    ASSET_WC_MIZU_KCL = 0x06cb
};

#include "Sound.h"
/* PROMOTED translation unit -- ov029/daObjWc_Mizu_c (9 function(s)).
 *
 * WDW water (profile WDW_WATER 101). RTTI ov029:0x02114098 names
 * daObjWc_Mizu_c; g_profile_WC_MIZU is at 0x021140b8. ov029 is WDW
 * (ROTATING_PLATFORM_WDW .. WDW_WATER) -- this is the water plane, not
 * daObjWc_Obj02_c / daObjWc_Obj03_c.
 *
 * This one file is the production source for the whole class: the ROM build
 * takes every one of these functions from a single object, the way the
 * cartridge's own build did. It licenses the contiguous .text run
 * 0x021121a4..0x02112630 in ov029 (config/tu_manifest.d/ov029/daObjWc_Mizu_c.json,
 * config/arm9/overlays/ov029/delinks.txt).
 *
 * WRITTEN IN REVERSE ROM ORDER. mwccarm emits one .text section per function
 * in the reverse of source order, so the highest-address ROM function is
 * written FIRST and the lowest last. Do not reorder these blocks. The inline
 * destructor in include/daObjWc_Mizu_c.h emits the retail D1/D0 pair first
 * and emits no D2 body.
 *
 * common.h FIRST: func_ov029_02112250 assigns a whole Matrix4x3, and the ROM
 * copies it as three 4-word ldm/stm pairs. common.h's flat s32 m[12] is that
 * copy; math/Matrix.h's nested {Matrix3x3 r; Vector3 t;} splits it.
 *
 * deslop leftovers:
 * - TextureTransformer::SetFile / dBgW_KcMbg::SetFile 6az: InitResources
 *   passes Fix12<int> by value; the header method form size-DIFFs.
 * - SharedFilePtr +4: InitResources' TextureTransformer::Prepare reads the
 *   BMD at daObjWc_Obj07_c_ModelFile +4 (words[1]); SharedFilePtr.h has no fields.
 *   The daObjWc_Obj07_c_* names are a stale symbols.txt attribution: this TU
 *   is the only consumer and defines both handles (obj07's own handles live
 *   at 0x021142d4/0x021142dc).
 * - data_ov029_02112b2c / 02112fb8 / 02112fec WDW water-level / BTA / CLPS handles;
 *   this TU consumes them, overlay .data owns them.
 * - S14: g_profile_WC_MIZU stays outside the licensed .text.
 * - common.h first (func_ov029_02112250 twelve-word Matrix4x3 copy).
 * - no Player.h.
 *
 * Absorbed these legacy one-function sources (ROM address order), all deleted
 * by tools/tu_promote.py:
 *   [0] 0x021121a4  src/_ZN14daObjWc_Mizu_cD1Ev.cpp
 *   [1] 0x021121f0  src/_ZN14daObjWc_Mizu_cD0Ev.cpp
 *   [2] 0x02112250  src/func_ov029_02112250.cpp
 *   [3] 0x021122b4  src/func_ov029_021122b4.c
 *   [4] 0x021122dc  src/_ZN14daObjWc_Mizu_c16CleanupResourcesEv.cpp
 *   [5] 0x02112320  src/_ZN14daObjWc_Mizu_c6RenderEv.cpp
 *   [6] 0x02112354  src/_ZN14daObjWc_Mizu_c8BehaviorEv.cpp
 *   [7] 0x021124d0  src/_ZN14daObjWc_Mizu_c13InitResourcesEv.cpp
 *   [8] 0x021125f8  src/daObjWc_Mizu_c_classInit.c
 */

#include "common.h"
#include "daObjWc_Mizu_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

struct BMD_File;
struct BTA_File;
struct KCL_File;
struct CLPS_Block;

/* Retail constructs these 8-byte handles in this order and registers
 * their destructors. The wrapper names are local; the ctor/dtor
 * addresses are the ROM resource-family functions. */
struct WcMizuModelFilePtr : SharedFilePtr {
    u32 words[2];

    WcMizuModelFilePtr(u32 fileID);
    ~WcMizuModelFilePtr();
};

struct WcMizuCollisionFilePtr : SharedFilePtr {
    u32 words[2];

    WcMizuCollisionFilePtr(u32 fileID);
    ~WcMizuCollisionFilePtr();
};

extern "C" {
void func_ov029_02112250(daObjWc_Mizu_c *self);
void func_ov029_021122b4(daObjWc_Mizu_c *self);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
int IsAreaShowing(int idx);
void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void *tt, void *bta, int a, int fix, unsigned b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *mc, void *kcl, void *mtx, int fix, short s, void *clps);

extern WcMizuModelFilePtr daObjWc_Obj07_c_ModelFile;
extern WcMizuCollisionFilePtr daObjWc_Obj07_c_ClsnFile;
extern int data_ov029_02112b2c[];
extern BTA_File data_ov029_02112fb8;
extern CLPS_Block data_ov029_02112fec;
extern u8 data_0209f2c0[];
extern int data_0209f32c;
extern s16 data_02082214[];
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjWc_Mizu_c_classInit
/* Reconstructed source-style name; exact original SM64DS spelling is not
 * preserved. Historical alias: WDW_Water_Spawn. */
extern "C" daObjWc_Mizu_c *daObjWc_Mizu_c_classInit()
{
    return new daObjWc_Mizu_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjWc_Mizu_c13InitResourcesEv
int daObjWc_Mizu_c::InitResources()
{
    void *m;
    void *k;

    m = Model::LoadFile(daObjWc_Obj07_c_ModelFile);
    mModel.SetFile((BMD_File *)m, 1, -1);
    TextureTransformer::Prepare(*(BMD_File *)daObjWc_Obj07_c_ModelFile.words[1], data_ov029_02112fb8);
    /* 6az: header SetFile takes Fix12<int> by value. */
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
        &mTextureTransformer, &data_ov029_02112fb8, 0, 0x1000, 0);

    mUseSpawnPosY = (u8)(param1 & 1);
    if (mUseSpawnPosY == 0) {
        unsigned int idx = data_0209f2c0[0];
        if (idx >= 3) idx = 2;
        mPosY = data_ov029_02112b2c[idx];
    }
    mTargetPosY = mPosY;
    data_0209f32c = mPosY;

    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    k = dBgW_Kc::LoadFile(daObjWc_Obj07_c_ClsnFile);
    /* 6az: header SetFile takes Fix12<int> by value. */
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, k, &mClsnMat, 0x1000, mAngleY, &data_ov029_02112fec);
    mMeshCollider.Enable(this);

    mTrueAreaID = mAreaId;
    mAreaId = -1;
    mPhaseAngle = 0;

    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjWc_Mizu_c8BehaviorEv
int daObjWc_Mizu_c::Behavior()
{
    int t;
    int i;

    mAreaId = -1;

    if (IsAreaShowing(mTrueAreaID) == 0) {
        mAreaId = mTrueAreaID;
        if (mMeshCollider.IsEnabled() != 0) {
            mMeshCollider.Disable();
        }
    } else {
        if (mMeshCollider.IsEnabled() == 0) {
            mMeshCollider.Enable(this);
        }
    }

    t = mTargetPosY;
    if (mPosY != t) {
        if (mPosY < t) {
            /* Plain `mPosY += 0xa000` emits the same .text. The pointer locals
               (these two and the mPhaseAngle one below) feed mwccarm's per-TU
               @NNN counter, one number per named local: without them __sinit's
               registration nodes come out @465/@466, not the @468/@469 the
               manifest renames. */
            s32 *p = &mPosY;
            *p += 0xa000;
            if (mPosY > mTargetPosY)
                mPosY = mTargetPosY;
        } else {
            s32 *p = &mPosY;
            *p -= 0xa000;
            if (mPosY < mTargetPosY)
                mPosY = mTargetPosY;
        }
    }

    if (mPosY != mPrevPosY) {
        mSoundID = Sound::PlayLong(mSoundID, 3, 0x96, *(Vector3 *)&mCamSpacePosX, 0);
    }

    {
        s16 *q = &mPhaseAngle;
        *q += 0x200;
        i = (u16)mPhaseAngle >> 4;
        mWaterHeight = (int)data_02082214[i * 2] * 0xf + mPosY;
    }
    data_0209f32c = mWaterHeight;
    func_ov029_021122b4(this);
    func_ov029_02112250(this);
    mTextureTransformer.speed = 0x1000;
    mTextureTransformer.Advance();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjWc_Mizu_c6RenderEv
int daObjWc_Mizu_c::Render()
{
    mTextureTransformer.Update(mModel.data);
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjWc_Mizu_c16CleanupResourcesEv
int daObjWc_Mizu_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    daObjWc_Obj07_c_ModelFile.Release();
    daObjWc_Obj07_c_ClsnFile.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov029_021122b4
extern "C" void func_ov029_021122b4(daObjWc_Mizu_c *self)
{
    Matrix4x3_FromTranslation(
        &self->mModel.mat4x3,
        self->mPosX >> 3, self->mWaterHeight >> 3, self->mPosZ >> 3);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov029_02112250
extern "C" void func_ov029_02112250(daObjWc_Mizu_c *self)
{
    self->mClsnMat = self->mModel.mat4x3;
    self->mClsnMat.m[9] = self->mPosX;
    self->mClsnMat.m[10] = self->mWaterHeight;
    self->mClsnMat.m[11] = self->mPosZ;
    self->mMeshCollider.Transform(self->mClsnMat, self->mAngleY);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjWc_Mizu_cD1Ev
// @symbol _ZN14daObjWc_Mizu_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjWc_Mizu_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no leaf D2. Writing the
   body out of line here instead flips them to D0-before-D1 and the isolation
   step rejects the object.
 */

/* The static-init globals -- mwcc emits __sinit_d_a_obj_wc_mizu.cpp
   from these: one ctor veneer plus destructor registration per handle. */
WcMizuModelFilePtr daObjWc_Obj07_c_ModelFile(ASSET_WC_MIZU_BMD);
WcMizuCollisionFilePtr daObjWc_Obj07_c_ClsnFile(ASSET_WC_MIZU_KCL);
