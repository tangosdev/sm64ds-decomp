//cpp
/**
 * Question Switch (profile HATENA_SWITCH / QUESTION_SWITCH 26).
 *
 * Ground-pound or walk-on switch: after an 8-frame press latch it swaps the
 * static KCL for the depressed one, sets the save bit, talks, and plays the
 * press animation.
 *
 * daObjHatenaSwitch_c is the ROM RTTI name (ov002 0x02108e14). ov002 also
 * has EXCLAMATION_SWITCH(11), STAR_SWITCH(12), BLUE_COIN_SWITCH(10); this
 * class is QUESTION_SWITCH, not those.
 *
 * daObjHatenaSwitch_c_classInit is reconstructed (RTTI daObjHatenaSwitch_c,
 * HATENA_SWITCH registry). Retail does not store that spelling. Historical
 * alias QuestionSwitch_Spawn.
 *
 * deslop
 * Leftover: ModelAnim::SetAnim and dBgW_KcMbg::SetFile stay mangled in this
 *   TU -- both take Fix12<int> by value (wall 6az); a method call homes the
 *   argument and size-DIFFs InitResources.
 * Leftover: Sound::PlaySub and Particle::System::NewSimple stay mangled --
 *   Fix12<int> by value (wall 6az). Sound.h still only has PlayBank3;
 *   Particle.h has no System::NewSimple.
 * Leftover: func_020393c4 is a 4-byte store into dBgW+0x1c (unk_1c). This
 *   TU calls it; naming belongs with dBgW in arm9.
 * Leftover: HatenaMatrixWords 12-word copies -- structured Matrix4x3
 *   assignment scalarizes; common.h-first would drop mClsnMat.t used in
 *   UpdateClsnTransform.
 * Leftover: data_ov002_0210dd60/dd68/dd58/dd50 are this overlay's BMD/BCA/KCL
 *   handles (sinit-owned BSS). data_ov002_0210d8b4 / 0210d774 are CLPS
 *   blocks this TU does not own.
 * Leftover: data_0209caa0 / data_0209d684 / data_0209d660 / data_020a0e68
 *   are arm9 scratch/save globals.
 */

#include "daObjHatenaSwitch_c.h"
#include "Message.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "Sound.h"

/* Plain stack values avoid pulling unrelated Vector3 and Matrix4x3 special
 * members into this TU while preserving their measured three/twelve-word ABI. */
struct HatenaVector3Words { s32 x, y, z; };
struct HatenaMatrixWords { s32 words[12]; };

struct HatenaSwitchSpawnInfo {
    daObjHatenaSwitch_c *(*classInit)();
    s16 profileIDAndExecuteOrder;
    s16 drawOrder;
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};

typedef char HatenaSwitchSpawnInfo_size_must_be_0x1c[
    sizeof(HatenaSwitchSpawnInfo) == 0x1c ? 1 : -1];

/* SetAnim / SetFile / PlaySub / NewSimple are measured Fix12-by-value seams
 * (wall 6az). func_020393c4 is the dBgW+0x1c store; naming belongs with dBgW. */
extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    ModelAnim *model, BCA_File *file, s32 mode, Fix12i speed, u32 flags);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *collider, KCL_File *file, Matrix4x3 *matrix,
    Fix12i scale, s16 angleY, CLPS_Block *clps);
extern void func_020393c4(dBgW *collider, void *callback);

extern u16 DecIfAbove0_Short(u16 *value);
extern u8 DecIfAbove0_Byte(u8 *value);
extern void _ZN5Sound7PlaySubEjjj5Fix12IiEb(
    u32 bank, u32 soundID, u32 param, Fix12i volume, bool loop);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    u32 effectID, Fix12i x, Fix12i y, Fix12i z);

extern void Vec3_Asr(void *destination, const void *source, s32 shift);
extern void Matrix4x3_FromTranslation(
    Matrix4x3 *matrix, s32 x, s32 y, s32 z);
extern void Matrix4x3_ApplyInPlaceToRotationY(
    Matrix4x3 *matrix, s16 angleY);
}

extern SharedFilePtr data_ov002_0210dd60;
extern SharedFilePtr data_ov002_0210dd68;
extern SharedFilePtr data_ov002_0210dd58;
extern SharedFilePtr data_ov002_0210dd50;
extern CLPS_Block data_ov002_0210d8b4;
extern CLPS_Block data_ov002_0210d774;
extern Matrix4x3 data_020a0e68;
extern u8 data_0209d684;
extern u8 data_0209d660;
extern u32 data_0209caa0[];

/*
 * Every instruction the cartridge has here falls out of the one `new`.
 * Reconstructed spelling from ROM RTTI plus later EAD lineage; historical
 * alias QuestionSwitch_Spawn. */
extern "C" daObjHatenaSwitch_c *daObjHatenaSwitch_c_classInit()
{
    return new daObjHatenaSwitch_c();
}

/* HATENA_SWITCH is the literal ROM registry ID. The g_profile spelling is a
 * lineage-supported reconstruction; historical alias QuestionSwitch_SpawnInfo. */
extern "C" HatenaSwitchSpawnInfo g_profile_HATENA_SWITCH = {
    daObjHatenaSwitch_c_classInit,
    0x001a,
    0x0126,
    0x00000002,
    0x00100000,
    0x00200000,
    0x01000000,
    0
};


// @symbol _ZN19daObjHatenaSwitch_c17AfterClsnCallbackEP4dBgWP8dActor_cS3_
void daObjHatenaSwitch_c::AfterClsnCallback(
    dBgW *collider, dActor_c *owner, dActor_c *other)
{
    ((daObjHatenaSwitch_c *)owner)->HandleClsn(*other);
}


// @symbol _ZN19daObjHatenaSwitch_c10HandleClsnER8dActor_c
void daObjHatenaSwitch_c::HandleClsn(dActor_c &other)
{
    u16 actorID = other.actorID;
    int isPlayer = (int)(actorID == 0xbf);
    if (isPlayer == 0) {
        int isCrate = (int)(actorID == 0xc2);
        if (isCrate == 0)
            return;
    }

    if (other.mPosY > mPosY + 0x64000)
        mPressedThisFrame = 1;
}


// @symbol _ZN19daObjHatenaSwitch_c13InitResourcesEv
s32 daObjHatenaSwitch_c::InitResources()
{
    void *file;

    file = Model::LoadFile(data_ov002_0210dd60);
    mModelAnim.SetFile((BMD_File *)file, 1, -1);

    file = Animation::LoadFile(data_ov002_0210dd68);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, (BCA_File *)file, 0, 0x1000, 0);
    mModelAnim.SetFlags(0x40000000);
    mModelAnim.speed = 0x1000;
    UpdateModelTransform();

    file = dBgW_Kc::LoadFile(data_ov002_0210dd58);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mStaticMeshCollider, (KCL_File *)file, &mClsnMat,
        0x199, mAngleY, &data_ov002_0210d8b4);

    file = dBgW_Kc::LoadFile(data_ov002_0210dd50);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMovingMeshCollider, (KCL_File *)file, &mClsnMat,
        0x199, mAngleY, &data_ov002_0210d774);

    func_020393c4(
        &mStaticMeshCollider,
        (void *)&daObjHatenaSwitch_c::AfterClsnCallback);

    if (data_0209caa0[1] & 0x80000000) {
        mActiveMeshCollider = &mMovingMeshCollider;
        mModelAnim.currFrame = mModelAnim.GetFrameCount() << 12;
        mModelAnim.Advance();
    } else {
        mActiveMeshCollider = &mStaticMeshCollider;
        mModelAnim.currFrame = 0;
    }

    UpdateClsnTransform();
    mPressTimer = 8;
    mTalkingPlayer = 0;
    return 1;
}


// @symbol _ZN19daObjHatenaSwitch_c8BehaviorEv
s32 daObjHatenaSwitch_c::Behavior()
{
    if (mSoundDelay != 0) {
        if (DecIfAbove0_Short(&mSoundDelay) == 0)
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(
                0x20, 0x7f, 0, 0x8777, false);
    }

    if (mTalking != 0) {
        Player *player = mTalkingPlayer;
        if (player != 0) {
            switch (player->GetTalkState()) {
            case 0:
                _ZN5Sound7PlaySubEjjj5Fix12IiEb(
                    0x20, 0x14, 0x7f, 0x6b000, false);
                mSoundDelay = 0x4b;
                if (mModelAnim.Finished() != 0) {
                    Message::PrepareTalk();
                    player->ShowMessage(*this, 0x183, 0, 1, 2);
                }
                break;
            case 1:
                break;
            default:
                if (data_0209d684 == 1) {
                    Message::DisplaySaving(0x295);
                } else if (data_0209d684 == 2) {
                    if (player->HasFinishedTalking() != 0) {
                        Message::EndTalk();
                        mTalkingPlayer = 0;
                        mTalking = 0;
                    }
                } else if (data_0209d660 == 0) {
                    if (player->HasFinishedTalking() != 0) {
                        Message::EndTalk();
                        mTalkingPlayer = 0;
                        mTalking = 0;
                    }
                }
                break;
            }
        } else {
            Player *player = ClosestPlayer();
            if (player->mIsAirborne == 0) {
                if (player->StartTalk(*this, true) != 0)
                    mTalkingPlayer = player;
            }
        }
    }

    {
        u8 pressed = mPressedThisFrame;
        if (pressed != 0) {
            if (DecIfAbove0_Byte(&mPressTimer) == 0) {
                if (mActiveMeshCollider->IsEnabled() != 0)
                    mActiveMeshCollider->Disable();
                mActiveMeshCollider = &mMovingMeshCollider;
                data_0209caa0[1] |= 0x80000000;
                {
                    volatile HatenaVector3Words position;
                    s32 y = mPosY + 0x64000;
                    s32 z = mPosZ;
                    s32 x = mPosX;
                    position.x = x;
                    position.y = y;
                    position.z = z;
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
                        0x6d, x, y, z);
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
                        0x6e, position.x, position.y, position.z);
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
                        0x6f, position.x, position.y, position.z);
                }
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
                    0x70, mPosX, mPosY, mPosZ);
                mTalking = 1;
            }
        } else if (pressed == 0) {
            mPressTimer = 8;
        }
    }

    if ((data_0209caa0[1] & 0x80000000) == 0)
        mActiveMeshCollider = &mStaticMeshCollider;

    if (UpdateClsnState() == 0) {
        if (mActiveMeshCollider == &mStaticMeshCollider) {
            mModelAnim.currFrame = 0;
        } else if (mModelAnim.Finished() == 0) {
            mModelAnim.Advance();
            if (mModelAnim.Finished() != 0)
                Sound::PlayBank3(0, *(Vector3 *)&mCamSpacePosX);
        }
        UpdateModelTransform();
        UpdateClsnTransform();
    }

    mPressedThisFrame = 0;
    return 1;
}


// @symbol _ZN19daObjHatenaSwitch_c6RenderEv
s32 daObjHatenaSwitch_c::Render()
{
    mModelAnim.Render(0);
    return 1;
}


// @symbol _ZN19daObjHatenaSwitch_c16CleanupResourcesEv
s32 daObjHatenaSwitch_c::CleanupResources()
{
    if (mStaticMeshCollider.IsEnabled())
        mStaticMeshCollider.Disable();
    if (mMovingMeshCollider.IsEnabled())
        mMovingMeshCollider.Disable();
    data_ov002_0210dd60.Release();
    data_ov002_0210dd68.Release();
    data_ov002_0210dd58.Release();
    data_ov002_0210dd50.Release();
    return 1;
}


// @symbol _ZN19daObjHatenaSwitch_c20UpdateModelTransformEv
void daObjHatenaSwitch_c::UpdateModelTransform()
{
    s32 shifted[4];
    Vec3_Asr(&shifted, &mPosX, 3);
    Matrix4x3_FromTranslation(
        &data_020a0e68, shifted[0], shifted[1], shifted[2]);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    *(HatenaMatrixWords *)&mModelAnim.mat4x3 =
        *(HatenaMatrixWords *)&data_020a0e68;
    *(HatenaMatrixWords *)&mModel.mat4x3 =
        *(HatenaMatrixWords *)&mModelAnim.mat4x3;
}


// @symbol _ZN19daObjHatenaSwitch_c19UpdateClsnTransformEv
void daObjHatenaSwitch_c::UpdateClsnTransform()
{
    *(HatenaMatrixWords *)&mClsnMat =
        *(HatenaMatrixWords *)&mModelAnim.mat4x3;
    mClsnMat.t.x = mPosX;
    mClsnMat.t.y = mPosY;
    mClsnMat.t.z = mPosZ;
    mActiveMeshCollider->Transform(mClsnMat, mAngleY);
}


// @symbol _ZN19daObjHatenaSwitch_c15UpdateClsnStateEv
s32 daObjHatenaSwitch_c::UpdateClsnState()
{
    int shouldDisable = (int)((mFlags & 8) != 0);

    if (shouldDisable) {
        if (mActiveMeshCollider->IsEnabled())
            mActiveMeshCollider->Disable();
        return 1;
    }

    if (!mActiveMeshCollider->IsEnabled())
        mActiveMeshCollider->Enable(this);
    return 0;
}


// @symbol _ZN19daObjHatenaSwitch_c15OnGroundPoundedER8dActor_c
void daObjHatenaSwitch_c::OnGroundPounded(dActor_c &other)
{
    mPressTimer = 0;
}

/*
 * naturally by the inline destructor and InitResources vtable instantiation. */
