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
 * What stays:
 * - data_ov033_02111bc8 (BTA) and data_ov033_02111c1c (CLPS) are
 *   unnamed homes this TU feeds Prepare/SetFile.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjTtWater_c_AssetId { ASSET_TT_OBJ_WATER_BMD = 0x06c7, ASSET_TT_OBJ_WATER_KCL = 0x06c8 };

#include "daObjTtWater_c.h"
#include "SharedFilePtr.h"
#include "dMap_c.h"
#include "Sound.h"
#include "Event.h"

/* 8-byte file handles. The model uses func_02017acc / func_02017ab4 and the
 * collision file func_02017b4c / SharedFilePtr_Destruct_Clsn. The spellings
 * are local; the manifest aliases the generated names to those ROM symbols. */
struct TtWaterModelFilePtr : SharedFilePtr {
    TtWaterModelFilePtr(u32 fileID);
    ~TtWaterModelFilePtr();
};

struct TtWaterCollisionFilePtr : SharedFilePtr {
    TtWaterCollisionFilePtr(u32 fileID);
    ~TtWaterCollisionFilePtr();
};

/* BSS file homes; Init loads them, Cleanup releases them. Defined at the
   end of this file so the constructors do not enter .text. */
extern TtWaterModelFilePtr data_ov033_021124f0;
extern TtWaterCollisionFilePtr data_ov033_021124e8;

extern int data_ov033_02111bc8[];
extern int data_ov033_02111c1c[];

/* Emission order is ROM order: the destructor pair must stay first.
 * Do not reorder. */
#pragma defer_codegen off

// @symbol _ZN14daObjTtWater_cD1Ev
// @symbol _ZN14daObjTtWater_cD0Ev
/* One out-of-line definition; mwccarm emits D1 and D0 from it, in that
 * order: its own vptr, then dBgActor_c's inlined, then dBgActor_c's
 * dExtSimpleModel_c and dBgW_KcMbg, then dActor_c. */
daObjTtWater_c::~daObjTtWater_c() {}

// @symbol _ZN14daObjTtWater_c16CleanupResourcesEv
int daObjTtWater_c::CleanupResources() {
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    data_ov033_021124f0.Release();
    data_ov033_021124e8.Release();
    return 1;
}

// @symbol _ZN14daObjTtWater_c6RenderEv
int daObjTtWater_c::Render() {
    mTextureTransformer.Update(mModel.data);
    mModel.Render(0);
    return 1;
}

// @symbol _ZN14daObjTtWater_c8BehaviorEv
int daObjTtWater_c::Behavior() {
    if (Event::GetBit(0xe)) {
        if (mPosY <= mMinPosY) {
            if (Sound::PlaySecretSound(this, &mSoundTimer))
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
int daObjTtWater_c::InitResources() {
    void *m = dExtSimpleModel_c::LoadFile(data_ov033_021124f0);
    mModel.SetFile((BMD_File *)m, 1, 0x14);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)data_ov033_021124f0.filePtr,
                             *(BTA_File *)data_ov033_02111bc8);
    mTextureTransformer.SetFile(*(BTA_File *)data_ov033_02111bc8, 0, 0x1000, 0);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    void *mc = dBgW_Kc::LoadFile(data_ov033_021124e8);
    mMeshCollider.SetFile((KCL_File *)mc, mClsnMat, 0x1000, mAngleY,
                          *(CLPS_Block *)data_ov033_02111c1c);
    mMeshCollider.Enable(this);
    mMinPosY = mPosY - 0x3c000;
    return Event::GetBit(0xe) == 0;
}

/* Reconstructed source-style name: SM64DS proves daObjTtWater_c through
 * RTTI, allocation size, vtable identity, and the TT_WATER registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical alias: TinyWater_Spawn. */
// @symbol daObjTtWater_c_classInit
extern "C" daObjTtWater_c *daObjTtWater_c_classInit() {
    return new daObjTtWater_c();
}

/* Source order is construction order: model file 1735, collision file 1736.
 * __sinit_d_a_obj_tt_water.cpp emits both constructions and registers the
 * destructors; the registration nodes are compiler temporaries. */
// @symbol __sinit_d_a_obj_tt_water.cpp
TtWaterModelFilePtr data_ov033_021124f0(ASSET_TT_OBJ_WATER_BMD);
TtWaterCollisionFilePtr data_ov033_021124e8(ASSET_TT_OBJ_WATER_KCL);
