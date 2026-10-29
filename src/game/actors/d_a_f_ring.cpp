//cpp
/* daFRing_c: Bowser's shockwave rings (FIRERING), ov060.
 *
 * _ZTS9daFRing_c at 0x0211ab08, _ZTI at 0x0211ab14 (base _ZTI8dActor_c).
 * Out-of-line destructor is the key function: D1 0x02118cfc, D0 0x02118d64,
 * then a deadstripped D2. .text 0x02118cfc..0x02119264 is ROM-ascending
 * under defer_codegen off. daFRing_c_classInit is last. Was BowserShockwaves.
 */
/* Notes:
 * - data_ov060_0211b208 / 0211b1f8 / 0211b200 are the model, animation and
 *   texture-sequence handles. LoadFile takes the handle and SetAnim /
 *   Prepare read its filePtr.
 * - data_ov048_021115e4 / 021115f4 are the BMA and BTA this TU's
 *   dExtAnmMaterial_c::Prepare and dExtAnmTexSRT_c::Prepare load. They sit
 *   in the level overlay; only ov048 has an animation record at both words.
 * - func_02016b24 is the unnamed model-mask call in InitResources.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daFRing_c_AssetId {
    ASSET_KPA_SHWAVE_BCA = 0x0383,
    ASSET_KPA_SHWAVE_BMD = 0x0384,
    ASSET_KPA_SHWAVE_BTP = 0x0385
};

#pragma defer_codegen off

#include "decl_common.h"
#include "daFRing_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"

/* The static-init file handles: the model file constructs through
 * func_02017acc and destroys through func_02017ab4, the animation handle
 * through SharedFilePtr::Construct/SharedFilePtr_Destruct_Anim, and the
 * texture-sequence handle through SharedFilePtr_Construct_TexSeq/
 * SharedFilePtr_Destruct_TexSeq. Each wrapper's ctor and dtor stay
 * undefined here; the manifest aliases them onto those ROM veneers. */
struct FRingModelFilePtr : SharedFilePtr {
    FRingModelFilePtr(u32 fileID);
    ~FRingModelFilePtr();
};
struct FRingAnimationFileHandle : SharedFilePtr {
    FRingAnimationFileHandle(u32 fileID);
    ~FRingAnimationFileHandle();
};
struct FRingTexSequenceFilePtr : SharedFilePtr {
    FRingTexSequenceFilePtr(u32 fileID);
    ~FRingTexSequenceFilePtr();
};

extern FRingAnimationFileHandle data_ov060_0211b1f8;
extern FRingTexSequenceFilePtr data_ov060_0211b200;
extern FRingModelFilePtr data_ov060_0211b208;

extern BMA_File data_ov048_021115e4;
extern BTA_File data_ov048_021115f4;

/* local extern: the model-mask helper has no shared header yet. */
extern "C" void func_02016b24(void *model, unsigned int mask);

// @symbol _ZN9daFRing_cD1Ev
// @symbol _ZN9daFRing_cD0Ev
daFRing_c::~daFRing_c() {}

// @symbol _ZN9daFRing_c16CleanupResourcesEv
s32 daFRing_c::CleanupResources() {
    data_ov060_0211b208.Release();
    data_ov060_0211b1f8.Release();
    data_ov060_0211b200.Release();
    return 1;
}

// @symbol _ZN9daFRing_c6RenderEv
s32 daFRing_c::Render() {
    mTextureSequence1.Update(mAnmModel1.data);
    mTextureSequence2.Update(mAnmModel2.data);
    mMaterialChanger1.Update(mAnmModel1.data);
    mMaterialChanger2.Update(mAnmModel2.data);
    mTextureTransformer1.Update(mAnmModel1.data);
    mTextureTransformer2.Update(mAnmModel2.data);
    mAnmModel1.Render(0);
    Vector3 scale;
    scale.x = 0x800;
    scale.y = 0x800;
    scale.z = 0x800;
    mAnmModel2.Render(&scale);
    return 1;
}

// @symbol _ZN9daFRing_c8BehaviorEv
s32 daFRing_c::Behavior() {
    mFrame++;
    Player *player = ClosestPlayer();
    unsigned int frame = mFrame;
    int reach = frame * 0x22;
    if (player != 0 && frame < 0x46 && player->mIsAirborne == 0) {
        int dist = Vec3_HorzDist(&mPos, &player->mPos);
        int band0 = reach * data_ov060_0211ab20[0];
        int band1 = reach * data_ov060_0211ab20[1];
        int band2 = reach * data_ov060_0211ab20[2];
        int band3 = reach * data_ov060_0211ab20[3];
        if ((band0 < dist && dist < band1) || (band2 < dist && dist < band3))
            player->Shock(1);
    }
    mTextureSequence1.Advance();
    mTextureSequence2.Advance();
    mMaterialChanger1.Advance();
    mMaterialChanger2.Advance();
    mTextureTransformer1.Advance();
    mTextureTransformer2.Advance();
    static_cast<dExtFrameCtrl_c &>(mAnmModel1).Advance();
    static_cast<dExtFrameCtrl_c &>(mAnmModel2).Advance();
    Matrix4x3_FromTranslation(&mAnmModel1.mat4x3, mPosX >> 3, mPosY >> 3, mPosZ >> 3);
    Matrix4x3_FromTranslation(&mAnmModel2.mat4x3, mPosX >> 3, mPosY >> 3, mPosZ >> 3);
    if (static_cast<dExtFrameCtrl_c &>(mAnmModel1).Finished())
        MarkForDestruction();
    return 1;
}

// @symbol _ZN9daFRing_c13InitResourcesEv
s32 daFRing_c::InitResources() {
    mAnmModel1.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(data_ov060_0211b208), 1, 0x13);
    mAnmModel2.SetFile((BMD_File *)data_ov060_0211b208.filePtr, 1, 0x13);

    dExtFrameCtrl_c::LoadFile(data_ov060_0211b1f8);
    dExtAnmTexPat_c::LoadFile(data_ov060_0211b200);

    func_02016b24(&mAnmModel1, 0x4000);
    func_02016b24(&mAnmModel2, 0x4000);

    mAnmModel1.SetAnim((BCA_File *)data_ov060_0211b1f8.filePtr, 0x40000000, 0x1000, 0);
    mAnmModel2.SetAnim((BCA_File *)data_ov060_0211b1f8.filePtr, 0x40000000, 0x1000, 0);

    dExtAnmTexPat_c::Prepare(*(BMD_File *)data_ov060_0211b208.filePtr,
                             *(BTP_File *)data_ov060_0211b200.filePtr);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)data_ov060_0211b208.filePtr,
                             *(BTP_File *)data_ov060_0211b200.filePtr);
    mTextureSequence1.SetFile(*(BTP_File *)data_ov060_0211b200.filePtr, 0x40000000, 0x1000, 0);
    mTextureSequence2.SetFile(*(BTP_File *)data_ov060_0211b200.filePtr, 0x40000000, 0x1000, 0);

    dExtAnmMaterial_c::Prepare(*(BMD_File *)data_ov060_0211b208.filePtr, data_ov048_021115e4);
    dExtAnmMaterial_c::Prepare(*(BMD_File *)data_ov060_0211b208.filePtr, data_ov048_021115e4);
    mMaterialChanger1.SetFile(data_ov048_021115e4, 0x40000000, 0x1000, 0);
    mMaterialChanger2.SetFile(data_ov048_021115e4, 0x40000000, 0x1000, 0);

    dExtAnmTexSRT_c::Prepare(*(BMD_File *)data_ov060_0211b208.filePtr, data_ov048_021115f4);
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)data_ov060_0211b208.filePtr, data_ov048_021115f4);
    mTextureTransformer1.SetFile(data_ov048_021115f4, 0x40000000, 0x1000, 0);
    mTextureTransformer2.SetFile(data_ov048_021115f4, 0x40000000, 0x1000, 0);

    mFrame = 0;
    return 1;
}

/* Definition order is the retail initializer's construction order: the model
 * handle, then the animation handle, then the texture-sequence handle. */
FRingModelFilePtr data_ov060_0211b208(ASSET_KPA_SHWAVE_BMD);
FRingAnimationFileHandle data_ov060_0211b1f8(ASSET_KPA_SHWAVE_BCA);
FRingTexSequenceFilePtr data_ov060_0211b200(ASSET_KPA_SHWAVE_BTP);

/* return new builds the ring and its eight animators. */
// @symbol daFRing_c_classInit
extern "C" daFRing_c *daFRing_c_classInit(void) {
    return new daFRing_c;
}
