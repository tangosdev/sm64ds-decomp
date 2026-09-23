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
 * Leftover: TextureTransformer::Prepare keeps its mangled spelling: the
 *   real signature takes references, and the double-dereference expression
 *   that would bind them is worse slop than the call.
 * Leftover: dBgW_KcMbg::SetFile and TextureTransformer::SetFile keep
 *   mangled spellings (by-value Fix12<int> parameters, wall 6az).
 * Leftover: data_ov033_02111bc8 (BTA) and data_ov033_02111c1c
 *   (CLPS) are unnamed homes this TU feeds Prepare/SetFile.
 */

#include "daObjTtWater_c.h"
#include "SharedFilePtr.h"
#include "dMap_c.h"
#include "Sound.h"

/* BSS file homes; Init loads them, Cleanup releases them. */
extern SharedFilePtr data_ov033_021124f0;
extern SharedFilePtr data_ov033_021124e8;

extern "C" {
int _ZN5Event6GetBitEj(u32 bit);
extern void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(void* bmd, void* bta);
extern int data_ov033_02111bc8[];
extern int data_ov033_02111c1c[];
extern void _ZN6dMap_c19UpdateLevelSpecificEv(void);
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
 * mangled free functions. dMap_c::UpdateLevelSpecific is declared in
 * include/dMap_c.h since #2899; the call stays mangled here to keep the
 * text-verified shadow's instruction selection (member spelling needs the
 * build box). Animation::Advance and fBase_c::MarkForDestruction are
 * declared, and are reached as members.
 */
int daObjTtWater_c::Behavior()
{
    if (_ZN5Event6GetBitEj(0xe)) {
        if (mPosY <= mMinPosY) {
            if (Sound::PlaySecretSound((dActor_c *)this, (u16 *)&mSoundTimer))
                MarkForDestruction();
        } else {
            mPosY -= 0x1000;
            mSoundID = Sound::PlayLong(mSoundID, 3, 0x96, *(const Vector3 *)&mCamSpacePosX, 0);
            if (mPosY <= mMinPosY) {
                mPosY = mMinPosY;
                _ZN6dMap_c19UpdateLevelSpecificEv();
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
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(*(void**)((char*)&data_ov033_021124f0 + 4), data_ov033_02111bc8);
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
