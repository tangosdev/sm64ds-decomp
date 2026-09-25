//cpp
/* Production translation unit for ov096/daTor_c.
 * 10 function(s), .text 0x02136db0..0x021376bc. The sand tornado (registry
 * profile TORNADO).
 *
 * NAME: _ZTS7daTor_c is "7daTor_c" at ov096 0x02137a54; _ZTI at 0x02137a60
 * reads [__si_class_type_info, that string, _ZTI8dActor_c]. The tree
 * previously called the class Tornado (coined).
 *
 * The out-of-line destructor is the key function, so this TU emits _ZTV/_ZTI/
 * _ZTS. Under `#pragma defer_codegen off` it comes out D1 (0x02136db0), D0
 * (0x02136df8), then a D2 the cartridge has no home for (manifest: deadstrip);
 * the same pragma lays .text down in source order, so this file is
 * ROM-ascending. The factory daTor_c_classInit (0x021376bc) stays in its own
 * source, src/d_a_tor.c.
 */

#pragma defer_codegen off

#include "decl_common.h"
#include "daTor_c.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "SurfaceInfo.h"

/* Leftover: cstd::atan2, Particle::System::New, ModelAnim::SetAnim,
 * TextureTransformer::SetFile, dCcAc_c::Init and dBgCh_Actr::Init take
 * Fix12<int> by value, so they stay mangled. dBgCh_Actr::GetWallResult stays
 * mangled because include/dBgCh_Actr.h does not declare it yet. */
extern "C" {
/* The texture animation InitResources hands to mTextureTransformer lives in
 * the level overlay, not here. ov096 relocs.txt lists the load at 0x021376b0
 * as ambiguous between ten level overlays; tools/overlay_residency.py settles
 * it to ov024, the only level whose tables load ov096. */
extern int data_ov024_02112968[];
s16 data_02082214[];
s32 Vec3_HorzDist(const void *a, const void *b);
s16 Vec3_HorzAngle(const void *a, const void *b);
s32 Vec3_Dist(const void *a, const void *b);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void dBgCh_Actr_UpdateContinuous_Veneer(void *collision);
void *_ZNK10dBgCh_Actr13GetWallResultEv(void *collision);
s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 handle, u32 effect, s32 x, s32 y, s32 z, const void *direction,
    void *callback);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    void *self, void *file, int flags, int speed, unsigned int start);
void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
    void *self, void *file, int flags, int speed, unsigned int start);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    void *self, void *actor, int radius, int height, unsigned int flags, unsigned int vuln);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void *self, void *actor, int radius, int height, void *a, int b);
}

void ApproachLinear(s16 &value, s16 target, s16 step);

// @symbol _ZN7daTor_cD1Ev
// @symbol _ZN7daTor_cD0Ev
daTor_c::~daTor_c()
{
}

// @symbol _ZN7daTor_c10UpdateSpinEi
void daTor_c::UpdateSpin(s32 scale)
{
    u16 idx = (u16)unk_35a;
    s16 wave = data_02082214[(idx >> 4) * 2 + 1];
    s32 radiusScale = (wave + 0x1000) >> 1;
    s32 frameScale = (s32)(((long long)scale * 0x666 + 0x800) >> 12);
    s32 radius = (s32)(((long long)radiusScale * 0x4cc + 0x800) >> 12);
    radius += 0x1000;
    mScaleX = (s32)(((long long)radius * frameScale + 0x800) >> 12);

    u16 idx2 = (u16)unk_35a;
    s16 wave2 = data_02082214[(idx2 >> 4) * 2 + 1];
    s32 heightScale = ((0x1000 - wave2) >> 2) + 0x800;
    mScaleY = (s32)(((long long)heightScale * frameScale + 0x800) >> 12);

    mScaleZ = mScaleX;
    unk_35a += 0x200;
    mHorzSpeed = 0xe000;
    mAngleY += 0x2c00;
    mdCcAc_c.radius = mScaleX * 0x514;
    mdCcAc_c.height = mScaleY * 0xfa0;
    mScaleX <<= 1;
    mScaleY <<= 2;
    mScaleZ <<= 1;
}

// @symbol _ZN7daTor_c6State2Ev
void daTor_c::State2()
{
    s32 scale = (0x3c - mStateTimer) << 12;
    if (scale < 0)
        goto finished;

    UpdateSpin(scale / 0x3c);
    return;

finished:
    mdCcAc_c.flags |= 1;
    mCaughtActor = 0;

    Player *player = ClosestPlayer();
    if (player) {
        dActor_c *actor = (dActor_c *)player;
        if (Vec3_HorzDist(&mHomePosX, &actor->mPosX) > 0x9c4000)
            mState = 0;
        if (mStateTimer > 0x168)
            mState = 0;
        return;
    }
    mState = 0;
}

// @symbol _ZN7daTor_c6State1Ev
void daTor_c::State1()
{
    Vector3 playerPos;
    s16 angle;
    u16 *chaseTimer = &mChaseTimer;
    *chaseTimer = (u16)(*chaseTimer + 1);

    angle = Vec3_HorzAngle(&mPosX, &mHomePosX);
    mAngleToHome = angle;

    mSoundHandle = Sound::PlayLong(mSoundHandle, 3, 0x85,
                                   *(Vector3 *)&mCamSpacePosX, 0);
    mHorzSpeed = 0x14000;

    Player *player = ClosestPlayer();
    if (player == 0)
        goto null_player;

    /* Leftover: the player position is copied through a raw pointer at
       player+0x5c, because named mPosX/Y/Z loads come out one instruction
       shorter than the ROM's pointer copy. */
    {
        s32 *pos = (s32 *)((char *)player + 0x5c);
        playerPos.x = pos[0];
        playerPos.y = pos[1];
        playerPos.z = pos[2];
    }

    if (Vec3_HorzDist(&mHomePosX, &playerPos) < mChaseRange
        && mTriggerCount == 0
        && mChaseTimer < 0x384) {
        angle = Vec3_HorzAngle(&mPosX, &playerPos);
        mAngleToPlayer = angle;
        ApproachLinear(mPrevAngleY, mAngleToPlayer, 0x200);
        if (mCaughtActor != 0 && func_ov002_020de328(mCaughtActor) != 0)
            ++mTriggerCount;
    } else {
        ApproachLinear(mPrevAngleY, mAngleToHome, 0x200);
        if (Vec3_HorzDist(&mHomePosX, &mPosX) < 0xc8000)
            mState = 2;
    }
    goto cont;

null_player:
    mState = 2;
    return;

cont:
    if (Vec3_Dist(&mPosX, &playerPos) > 0xbb8000 || mChaseTimer >= 0x384)
        mState = 2;

    UpdatePos(0);
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
    if (mWithMeshClsn.IsOnWall() != 0) {
        s32 normal[3];
        void *wall = _ZNK10dBgCh_Actr13GetWallResultEv(&mWithMeshClsn);
        ((SurfaceInfo *)((char *)wall + 4))->CopyNormalTo(*(Vector3 *)normal);
        mPrevAngleY = _ZN4cstd5atan2E5Fix12IiES1_(normal[0], normal[2]);
    }

    UpdateSpin(0x1000);

    {
        s32 z = mPosZ;
        mParticleHandle0 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            mParticleHandle0, 0x11f, mPosX, mPosY, z, 0, 0);
        z = mPosZ;
        mParticleHandle1 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            mParticleHandle1, 0x120, mPosX, mPosY, z, 0, 0);
    }
}

// @symbol _ZN7daTor_c6State0Ev
void daTor_c::State0()
{
    if (mChaseTimer >= 0x384) {
        mPosX = mHomePosX;
        mPosY = mHomePosY;
        mPosZ = mHomePosZ;
    }

    if (mTriggerCount == 0) {
        mParticleHandle1 = 0;
        mParticleHandle0 = mParticleHandle1;
        mScaleX = 0;
        mScaleY = 0;
        mScaleZ = 0;
        unk_352 = 0;
        if (DistToCPlayer() < 0x5dc000) {
            u8 *trigger = &mTriggerCount;
            *trigger = *trigger + 1;
        }
        mStateTimer = 0;
        mdCcAc_c.flags &= ~1;
        mChaseTimer = 0;
    } else {
        mSoundHandle = Sound::PlayLong(mSoundHandle, 3, 0x85,
                                       *(Vector3 *)&mCamSpacePosX, 0);
        s32 timer = mStateTimer;
        UpdateSpin((timer << 12) / 60);
        if (mStateTimer >= 0x3c)
            mState = 1;
    }
}

// @symbol _ZN7daTor_c16CleanupResourcesEv
int daTor_c::CleanupResources()
{
    ((SharedFilePtr *)data_ov096_02137ba8)->Release();
    ((SharedFilePtr *)data_ov096_02137bb0)->Release();
    return 1;
}

// @symbol _ZN7daTor_c6RenderEv
int daTor_c::Render()
{
    mTextureTransformer.Update(mModelAnim.data);
    mModelAnim.Render((const Vector3 *)&mScaleX);
    return 1;
}

// @symbol _ZN7daTor_c8BehaviorEv
int daTor_c::Behavior()
{
    int s = mState;
    switch (s) {
    case 0: State0(); break;
    case 1: State1(); break;
    case 2: State2(); break;
    }
    ++mStateTimer;
    if (s != mState) {
        mStateTimer = 0;
        mTriggerCount = 0;
    }
    unsigned int id = mdCcAc_c.otherOwner;
    if (id != 0 && (mdCcAc_c.hitFlags & 0x400000) != 0) {
        dActor_c *o = dActor_c::FindWithID(id);
        if (o != 0) {
            dActor_c *closest = ClosestWithActorID(0x135);
            if (closest == 0 || Vec3_Dist(&o->mPosX, &closest->mPosX) > 0x118000) {
                if (func_ov002_020de33c((char *)o, (int)((char *)this)) != 0)
                    mCaughtActor = o;
            }
        }
    }
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    Matrix4x3_FromTranslation(&mModelAnim.mat4x3, mPosX >> 3, mPosY >> 3, mPosZ >> 3);
    mModelAnim.Animation::Advance();
    mTextureTransformer.Advance();
    return 1;
}

// @symbol _ZN7daTor_c13InitResourcesEv
int daTor_c::InitResources()
{
    /* Leftover: SetAnim, TextureTransformer::SetFile and
       dCcAc_c::Init take Fix12<int> by value; calling them as methods homes
       the arguments and the function grows from 0x158 to 0x18c. */
    mModelAnim.SetFile((BMD_File *)Model::LoadFile(*(SharedFilePtr *)data_ov096_02137ba8),
                       1, 0x15);
    Animation::LoadFile(*(SharedFilePtr *)data_ov096_02137bb0);
    func_02016aac(&mModelAnim, 0x16, 1);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, (void *)data_ov096_02137bb0[1], 0, 0x1000, 0);
    TextureTransformer::Prepare(*(BMD_File *)data_ov096_02137ba8[1], *(BTA_File *)data_ov024_02112968);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
        &mTextureTransformer, data_ov024_02112968, 0, 0x1000, 0);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0, 0, 0x200002, 0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, this, 0x50000, 0x50000, 0, 0);

    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    unk_35a = 0;
    mState = 0;
    mTriggerCount = 0;
    mVertAccel = -0x1000;
    mTerminalVelocity = -0x1e000;
    {
        unsigned int t = param1 & 0xff;
        if (t == 0xff)
            mChaseRange = 0x5dc000;
        else
            mChaseRange = t * 0x64000;
    }
    mParticleHandle0 = 0;
    mParticleHandle1 = 0;
    return 1;
}
