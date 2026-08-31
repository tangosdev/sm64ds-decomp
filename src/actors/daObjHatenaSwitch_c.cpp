//cpp
/* Genuine production translation unit for ov002/daObjHatenaSwitch_c.
 *
 * The class identity comes from retail RTTI. QuestionSwitch_Spawn and
 * QuestionSwitch_SpawnInfo remain evidence-bounded C ABI aliases because the
 * original factory spelling does not survive. The private helper spellings
 * are inferred; their class ownership, bodies, calls, and ordering are proven.
 *
 * mwccarm emits ordinary function sections in reverse source order. Keep the
 * ROM-high factory first and OnGroundPounded last. InitResources is the key
 * function; together with the inline destructor in the real header it
 * naturally emits retail D1 then D0 and the complete class data group.
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
    daObjHatenaSwitch_c *(*spawn)();
    s16 behaviorPriority;
    s16 renderPriority;
    u32 flags;
    Fix12i rangeOffsetY;
    Fix12i range;
    Fix12i drawDistance;
    u32 unk_18;
};

typedef char HatenaSwitchSpawnInfo_size_must_be_0x1c[
    sizeof(HatenaSwitchSpawnInfo) == 0x1c ? 1 : -1];

/* The factory allocator/constructor sequence, callback registration, and
 * Fix12-by-value calls below are measured narrow ABI seams. Other declarations
 * are genuine free functions or ROM-address globals. */
extern "C" {
extern void *_ZN7fBase_cnwEj(u32 size);
extern void _ZN10dBgActor_cC2Ev(dBgActor_c *actor);
extern void _ZN10dBgW_KcMbgC1Ev(dBgW_KcMbg *collider);
extern void _ZN9ModelAnimC1Ev(ModelAnim *model);

extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    ModelAnim *model, BCA_File *file, s32 mode, Fix12i speed, u32 flags);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *collider, KCL_File *file, Matrix4x3 *matrix,
    Fix12i scale, s16 angleY, CLPS_Block *clps);
extern void func_020393c4(int *collider, int callback);

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

extern "C" daObjHatenaSwitch_c *QuestionSwitch_Spawn();

/* ROM ordinal 12 -- QuestionSwitch_Spawn, 0x020b56d8, size 0x5c. Natural new
 * targets _Znwm instead of the retail actor allocator, so retain the measured
 * base/member construction and vptr sequence. */
extern "C" daObjHatenaSwitch_c *QuestionSwitch_Spawn()
{
    daObjHatenaSwitch_c *actor =
        (daObjHatenaSwitch_c *)_ZN7fBase_cnwEj(
            sizeof(daObjHatenaSwitch_c));
    if (actor) {
        _ZN10dBgActor_cC2Ev(actor);
        *(int *)actor = (int)&_ZTV19daObjHatenaSwitch_c[2];
        _ZN10dBgW_KcMbgC1Ev(&actor->mStaticMeshCollider);
        _ZN10dBgW_KcMbgC1Ev(&actor->mMovingMeshCollider);
        _ZN9ModelAnimC1Ev(&actor->mModelAnim);
    }
    return actor;
}

extern "C" HatenaSwitchSpawnInfo QuestionSwitch_SpawnInfo = {
    QuestionSwitch_Spawn,
    0x001a,
    0x0126,
    0x00000002,
    0x00100000,
    0x00200000,
    0x01000000,
    0
};

/* ROM ordinal 11 -- inferred static callback, 0x020b56c4, size 0x14. */
void daObjHatenaSwitch_c::AfterClsnCallback(
    dBgW *collider, dActor_c *owner, dActor_c *other)
{
    ((daObjHatenaSwitch_c *)owner)->HandleClsn(*other);
}

/* ROM ordinal 10 -- inferred collision handler, 0x020b567c, size 0x48. */
void daObjHatenaSwitch_c::HandleClsn(dActor_c &other)
{
    u16 actorID = other.actorID;
    int isPlayer = (int)(actorID == 0xbf);
    if (isPlayer == 0) {
        int isYoshi = (int)(actorID == 0xc2);
        if (isYoshi == 0)
            return;
    }

    if (other.mPosY > mPosY + 0x64000)
        mPressedThisFrame = 1;
}

/* ROM ordinal 9 -- key function, 0x020b5500, size 0x17c. */
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
        (int *)&mStaticMeshCollider,
        (int)&daObjHatenaSwitch_c::AfterClsnCallback);

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

/* ROM ordinal 8 -- Behavior, 0x020b51dc, size 0x324. */
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

/* ROM ordinal 7 -- Render, 0x020b51ac, size 0x30. */
s32 daObjHatenaSwitch_c::Render()
{
    mModelAnim.Render(0);
    return 1;
}

/* ROM ordinal 6 -- CleanupResources, 0x020b512c, size 0x80. */
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

/* ROM ordinal 5 -- inferred model transform, 0x020b50a0, size 0x8c. */
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

/* ROM ordinal 4 -- inferred collision transform, 0x020b503c, size 0x64. */
void daObjHatenaSwitch_c::UpdateClsnTransform()
{
    *(HatenaMatrixWords *)&mClsnMat =
        *(HatenaMatrixWords *)&mModelAnim.mat4x3;
    mClsnMat.t.x = mPosX;
    mClsnMat.t.y = mPosY;
    mClsnMat.t.z = mPosZ;
    mActiveMeshCollider->Transform(mClsnMat, mAngleY);
}

/* ROM ordinal 3 -- inferred collision state, 0x020b4fd0, size 0x6c. */
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

/* ROM ordinal 2 -- vtable slot 21, 0x020b4fc4, size 0x0c. */
void daObjHatenaSwitch_c::OnGroundPounded(dActor_c &other)
{
    mPressTimer = 0;
}

/* ROM ordinals 0/1 -- D1 at 0x020b4ed8 and D0 at 0x020b4f44 are emitted
 * naturally by the inline destructor and InitResources vtable instantiation. */
