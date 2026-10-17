//cpp
/* daFRing_c: Bowser's shockwave rings (FIRERING), ov060.
 *
 * _ZTS9daFRing_c at 0x0211ab08, _ZTI at 0x0211ab14 (base _ZTI8dActor_c).
 * Out-of-line destructor is the key function: D1 0x02118cfc, D0 0x02118d64,
 * then a deadstripped D2. .text 0x02118cfc..0x02119264 is ROM-ascending
 * under defer_codegen off. daFRing_c_classInit is last. Was BowserShockwaves.
 */
/* leftovers:
 * - ModelAnim::SetAnim, TextureSequence::SetFile, MaterialChanger::SetFile
 *   and TextureTransformer::SetFile stay mangled. Each takes Fix12<int> by
 *   value; the member spelling spills the 0x1000 and InitResources stops
 *   matching.
 * - data_ov060_0211b208 / 0211b1f8 / 0211b200 are the model, animation and
 *   texture-sequence handles. SharedFilePtr has no fields, so LoadFile takes
 *   the handle and SetAnim / Prepare read words[1].
 * - data_ov048_021115e4 / 021115f4 are the BMA and BTA this TU's
 *   MaterialChanger::Prepare and TextureTransformer::Prepare load. They sit
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

/* The static-init file handles: the model file constructs through
 * func_02017acc and destroys through func_02017ab4, the animation handle
 * through SharedFilePtr::Construct/SharedFilePtr_Destruct_Anim, and the
 * texture-sequence handle through SharedFilePtr_Construct_TexSeq/
 * SharedFilePtr_Destruct_TexSeq. Each wrapper's ctor and dtor stay
 * undefined here; the manifest aliases them onto those ROM veneers. */
struct FRingModelFilePtr : SharedFilePtr {
    u32 words[2];
    FRingModelFilePtr(u32 fileID);
    ~FRingModelFilePtr();
};
struct FRingAnimationFileHandle : SharedFilePtr {
    u32 words[2];
    FRingAnimationFileHandle(u32 fileID);
    ~FRingAnimationFileHandle();
};
struct FRingTexSequenceFilePtr : SharedFilePtr {
    u32 words[2];
    FRingTexSequenceFilePtr(u32 fileID);
    ~FRingTexSequenceFilePtr();
};

extern FRingAnimationFileHandle data_ov060_0211b1f8;
extern FRingTexSequenceFilePtr data_ov060_0211b200;
extern FRingModelFilePtr data_ov060_0211b208;

extern "C" {
extern BMA_File data_ov048_021115e4;
extern BTA_File data_ov048_021115f4;
void func_02016b24(void *model, unsigned int mask);
Fix12i Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
/* Leftover: these four take Fix12<int> by value, so they stay mangled; the
 * member spelling spills the 0x1000 and InitResources stops matching. */
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(ModelAnim *self, BCA_File *file, int flags, int speed, unsigned int start);
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(TextureSequence *self, BTP_File *file, int flags, int speed, unsigned int start);
void _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(MaterialChanger *self, BMA_File *file, int flags, int speed, unsigned int start);
void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(TextureTransformer *self, BTA_File *file, int flags, int speed, unsigned int start);
}

// @symbol _ZN9daFRing_cD1Ev
// @symbol _ZN9daFRing_cD0Ev
daFRing_c::~daFRing_c()
{
}

// @symbol _ZN9daFRing_c16CleanupResourcesEv
s32 daFRing_c::CleanupResources()
{
    ((SharedFilePtr *)&data_ov060_0211b208)->Release();
    ((SharedFilePtr *)&data_ov060_0211b1f8)->Release();
    ((SharedFilePtr *)&data_ov060_0211b200)->Release();
    return 1;
}

// @symbol _ZN9daFRing_c6RenderEv
s32 daFRing_c::Render()
{
    mTextureSequence1.Update(mModelAnim1.data);
    mTextureSequence2.Update(mModelAnim2.data);
    mMaterialChanger1.Update(mModelAnim1.data);
    mMaterialChanger2.Update(mModelAnim2.data);
    mTextureTransformer1.Update(mModelAnim1.data);
    mTextureTransformer2.Update(mModelAnim2.data);
    mModelAnim1.Render(0);
    Vector3 scale;
    scale.x = 0x800;
    scale.y = 0x800;
    scale.z = 0x800;
    mModelAnim2.Render(&scale);
    return 1;
}

// @symbol _ZN9daFRing_c8BehaviorEv
s32 daFRing_c::Behavior()
{
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
    static_cast<dExtFrameCtrl_c &>(mModelAnim1).Advance();
    static_cast<dExtFrameCtrl_c &>(mModelAnim2).Advance();
    Matrix4x3_FromTranslation(&mModelAnim1.mat4x3, mPosX >> 3, mPosY >> 3, mPosZ >> 3);
    Matrix4x3_FromTranslation(&mModelAnim2.mat4x3, mPosX >> 3, mPosY >> 3, mPosZ >> 3);
    if (static_cast<dExtFrameCtrl_c &>(mModelAnim1).Finished())
        MarkForDestruction();
    return 1;
}

// @symbol _ZN9daFRing_c13InitResourcesEv
s32 daFRing_c::InitResources()
{
    mModelAnim1.SetFile((BMD_File *)Model::LoadFile(*(SharedFilePtr *)&data_ov060_0211b208), 1, 0x13);
    mModelAnim2.SetFile((BMD_File *)data_ov060_0211b208.words[1], 1, 0x13);

    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov060_0211b1f8);
    TextureSequence::LoadFile(*(SharedFilePtr *)&data_ov060_0211b200);

    func_02016b24(&mModelAnim1, 0x4000);
    func_02016b24(&mModelAnim2, 0x4000);

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim1, (BCA_File *)data_ov060_0211b1f8.words[1], 0x40000000, 0x1000, 0);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim2, (BCA_File *)data_ov060_0211b1f8.words[1], 0x40000000, 0x1000, 0);

    TextureSequence::Prepare(*(BMD_File *)data_ov060_0211b208.words[1], *(BTP_File *)data_ov060_0211b200.words[1]);
    TextureSequence::Prepare(*(BMD_File *)data_ov060_0211b208.words[1], *(BTP_File *)data_ov060_0211b200.words[1]);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequence1, (BTP_File *)data_ov060_0211b200.words[1], 0x40000000, 0x1000, 0);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequence2, (BTP_File *)data_ov060_0211b200.words[1], 0x40000000, 0x1000, 0);

    MaterialChanger::Prepare(*(BMD_File *)data_ov060_0211b208.words[1], data_ov048_021115e4);
    MaterialChanger::Prepare(*(BMD_File *)data_ov060_0211b208.words[1], data_ov048_021115e4);
    _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(&mMaterialChanger1, &data_ov048_021115e4, 0x40000000, 0x1000, 0);
    _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(&mMaterialChanger2, &data_ov048_021115e4, 0x40000000, 0x1000, 0);

    TextureTransformer::Prepare(*(BMD_File *)data_ov060_0211b208.words[1], data_ov048_021115f4);
    TextureTransformer::Prepare(*(BMD_File *)data_ov060_0211b208.words[1], data_ov048_021115f4);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(&mTextureTransformer1, &data_ov048_021115f4, 0x40000000, 0x1000, 0);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(&mTextureTransformer2, &data_ov048_021115f4, 0x40000000, 0x1000, 0);

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
extern "C" daFRing_c *daFRing_c_classInit(void)
{
    return new daFRing_c;
}
