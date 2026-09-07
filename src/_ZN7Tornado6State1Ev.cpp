//cpp
// @symbol _ZN7Tornado6State1Ev
#include "Tornado.h"
#include "common.h"

extern "C" {
int func_ov002_020de328(void *actor);
s16 Vec3_HorzAngle(const void *a, const void *b);
s32 Vec3_HorzDist(const void *a, const void *b);
s32 Vec3_Dist(const void *a, const void *b);
u32 _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 handle, u32 bank, u32 sound,
                                      const void *pos, u32 flags);
void _Z14ApproachLinearRsss(s16 *value, s16 target, s16 step);
void dBgCh_Actr_UpdateContinuous_Veneer(void *collision);
void *_ZNK10dBgCh_Actr13GetWallResultEv(void *collision);
void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *surface, s32 *out);
s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 handle, u32 effect, s32 x, s32 y, s32 z, const void *direction,
    void *callback);
}

void Tornado::State1()
{
    Vector3 playerPos;
    s16 angle;
    u16 *chaseTimer = &mChaseTimer;
    *chaseTimer = (u16)(*chaseTimer + 1);

    angle = Vec3_HorzAngle(&mPosX, &mHomePosX);
    mAngleToHome = angle;

    mSoundHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(
        mSoundHandle, 3, 0x85, &mCamSpacePosX, 0);
    mHorzSpeed = 0x14000;

    Player *player = ClosestPlayer();
    if (player == 0)
        goto null_player;

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
        _Z14ApproachLinearRsss(&mPrevAngleY, mAngleToPlayer, 0x200);
        if (mCaughtActor != 0 && func_ov002_020de328(mCaughtActor) != 0)
            ++mTriggerCount;
    } else {
        _Z14ApproachLinearRsss(&mPrevAngleY, mAngleToHome, 0x200);
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
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char *)wall + 4, normal);
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
