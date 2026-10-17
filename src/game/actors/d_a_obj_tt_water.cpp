//cpp
/**
 * Tiny-Huge Island's drainable water pool (TT_WATER).
 *
 * Once event bit 0xe is set, Behavior sinks the surface one unit a
 * frame while looping the drain sound; at mMinPosY it plays the
 * secret jingle and destroys the actor when it finishes. InitResources
 * loads the model and collision files, prepares the texture
 * transformer, and records the drain floor from the spawn height.
 *
 * deslop
 * Leftover: Event::GetBit stays mangled (no Event header in this tree).
 * Leftover: dBgW_KcMbg::SetFile and TextureTransformer::SetFile keep
 *   mangled spellings (by-value Fix12<int> parameters, wall 6az).
 * Leftover: data_ov033_02111bc8 (BTA) and data_ov033_02111c1c
 *   (CLPS) are unnamed homes this TU feeds Prepare/SetFile.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjTtWater_c_AssetId {
    ASSET_TT_OBJ_WATER_BMD = 0x06c7,
    ASSET_TT_OBJ_WATER_KCL = 0x06c8
};

#include "daObjTtWater_c.h"
#include "SharedFilePtr.h"
#include "dMap_c.h"
#include "Sound.h"

/* 8-byte file handles. The model uses func_02017acc / func_02017ab4 and the
 * collision file func_02017b4c / SharedFilePtr_Destruct_Clsn. The spellings
 * are local; the manifest aliases the generated names to those ROM symbols. */
struct TtWaterModelFilePtr : SharedFilePtr {
    u32 words[2];

    TtWaterModelFilePtr(u32 fileID);
    ~TtWaterModelFilePtr();
};

struct TtWaterCollisionFilePtr : SharedFilePtr {
    u32 words[2];

    TtWaterCollisionFilePtr(u32 fileID);
    ~TtWaterCollisionFilePtr();
};

/* BSS file homes; Init loads them, Cleanup releases them. Defined at the
   end of this file so the constructors do not enter .text. */
extern TtWaterModelFilePtr data_ov033_021124f0;
extern TtWaterCollisionFilePtr data_ov033_021124e8;

extern "C" {
int _ZN5Event6GetBitEj(u32 bit);
extern int data_ov033_02111bc8[];
extern int data_ov033_02111c1c[];
extern void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void* thiz, void* bta, int a, int b, unsigned int e);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* thiz, void* kcl, void* mtx, int fix, short s, void* clps);
}

/* Emission order is ROM order: the destructor pair must stay first.
 * Do not reorder. */
#pragma defer_codegen off

// @symbol _ZN14daObjTtWater_cD1Ev
// @symbol _ZN14daObjTtWater_cD0Ev
/* One out-of-line definition; mwccarm emits D1 and D0 from it, in that
 * order: its own vptr, then dBgActor_c's inlined, then dBgActor_c's
 * Model and dBgW_KcMbg, then dActor_c. */
daObjTtWater_c::~daObjTtWater_c()
{
}

// @symbol _ZN14daObjTtWater_c16CleanupResourcesEv
int daObjTtWater_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    data_ov033_021124f0.Release();
    data_ov033_021124e8.Release();
    return 1;
}

// @symbol _ZN14daObjTtWater_c6RenderEv
int daObjTtWater_c::Render()
{
  mTextureTransformer.Update(mModel.data); mModel.Render(0); return 1;
}

// @symbol _ZN14daObjTtWater_c8BehaviorEv
/* Sound and Event have no headers in this tree, so those two stay extern-C
 * mangled free functions. dMap_c::UpdateLevelSpecific, dExtFrameCtrl_c::Advance
 * and fBase_c::MarkForDestruction are declared, and are reached as members.
 */
int daObjTtWater_c::Behavior()
{
    if (_ZN5Event6GetBitEj(0xe)) {
        if (mPosY <= mMinPosY) {
            if (Sound::PlaySecretSound((dActor_c *)this, (u16 *)&mSoundTimer))
                MarkForDestruction();
        } else {
            mPosY -= 0x1000;
            mSoundID = Sound::PlayLong(mSoundID, 3, 0x96, mCamSpacePos, 0);
            if (mPosY <= mMinPosY) {
                mPosY = mMinPosY;
                dMap_c::UpdateLevelSpecific();
            }
        }
    }
    mTextureTransformer.speed = 0x1000;
    mTextureTransformer.Advance();
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    return 1;
}

// @symbol _ZN14daObjTtWater_c13InitResourcesEv
int daObjTtWater_c::InitResources()
{
    void* m = Model::LoadFile(data_ov033_021124f0);
    mModel.SetFile((BMD_File*)m, 1, 0x14);
    TextureTransformer::Prepare(**(BMD_File**)((char*)&data_ov033_021124f0 + 4), *(BTA_File*)data_ov033_02111bc8);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(&mTextureTransformer, data_ov033_02111bc8, 0, 0x1000, 0);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    void* mc = dBgW_Kc::LoadFile(data_ov033_021124e8);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, mc, &mClsnMat, 0x1000, mAngleY, data_ov033_02111c1c);
    mMeshCollider.Enable(this);
    mMinPosY = mPosY - 0x3c000;
    return _ZN5Event6GetBitEj(0xe) == 0;
}

/* Reconstructed source-style name: SM64DS proves daObjTtWater_c through
 * RTTI, allocation size, vtable identity, and the TT_WATER registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical alias: TinyWater_Spawn. */
// @symbol daObjTtWater_c_classInit
extern "C" daObjTtWater_c *daObjTtWater_c_classInit()
{
    return new daObjTtWater_c();
}

/* Source order is construction order: model file 1735, collision file 1736.
 * __sinit_d_a_obj_tt_water.cpp emits both constructions and registers the
 * destructors; the registration nodes are compiler temporaries. */
// @symbol __sinit_d_a_obj_tt_water.cpp
TtWaterModelFilePtr data_ov033_021124f0(ASSET_TT_OBJ_WATER_BMD);
TtWaterCollisionFilePtr data_ov033_021124e8(ASSET_TT_OBJ_WATER_KCL);
