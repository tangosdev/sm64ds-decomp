//cpp
/* daGrock_c -- Hazy Maze Cave rolling rock (GORO_ROCK 221), ov021.
 *
 * param1's low nibble is the kind. 0 and 1 are invisible spawners: a rock
 * is born only while the player is farther than 1500.0, faster inside
 * 6000.0 than beyond it. Kind 1's fifth rock is the star rock. 2 rolls,
 * 3 rolls with a star, 4 is placed still and uses a wider cylinder. The
 * next nibble is the star index. Spawned rocks are actor 0xdd, this class.
 * 0xbf is the player.
 *
 * The travel heading lives in mPrevAngleY. Lateral velocity is mSpeedX /
 * mSpeedZ around mVertSpeed.
 * The rolling sound handle (mRollSoundID) and the rolling dust handle
 * (mDustParticleID) are renewed every frame the rock rolls. A star rock
 * keeps its spawner's uniqueID in mSpawnerID and clears the spawner's
 * mStarRockOut when it falls. mTrackStarID is the signed TrackStar slot,
 * mStarID the star index (0xff when there is none).
 *
 * #pragma defer_codegen off stays above the includes. The out-of-line
 * destructor is the key function, so this TU emits _ZTV/_ZTI/_ZTS, and the
 * pragma lays .text down D1, D0, then a D2 the cartridge does not keep.
 * daGrock_c_classInit (historical alias RollingRock_Spawn, folded in from
 * fold-lane-c-0929) is now the last function in the file, appended after
 * InitResources in ROM order; it hand-called fBase_c::operator new(968),
 * dEnemyBase_c::dEnemyBase_c(), stored _ZTV9daGrock_c, then dExtSimpleModel_c, then
 * dExtShadowModel_c, then dCcAcPos_c, then dBgCh_Actr in field order. daGrock_c
 * declares no constructor of its own, so the compiler-synthesized default
 * constructor emits exactly that sequence, and the factory is now
 * `return new daGrock_c();`. g_profile_GORO_ROCK is still not in this TU.
 *
 * Notes (measured spellings that do not match):
 * - SpawnStar: one UntrackAndSpawnStar with a chosen mode size-DIFFs
 *   (999 words). The two call sites, mode 2 and mode 4, match.
 * - HandleMeshCollision: ReflectAngle(Fix12<int>, Fix12<int>, s16)
 *   size-DIFFs. The wall angle is held in a short so GetSubtraction gets
 *   it without a re-extension.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daGrock_c_AssetId { ASSET_CV_GORO_ROCK_BMD = 0x05d4 };

#pragma defer_codegen off

#include "decl_common.h"
#include "daGrock_c.h"
#include "daPly_c.h"
#include "Sound.h"
#include "dBgPi.h"
#include "dBgCh_Actr.h"
#include "SharedFilePtr.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "cstd.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "LevelRng.h"

/* Kind in param1's low nibble. */
enum {
    kGrockSpawner = 0,
    kGrockStarSpawner = 1,
    kGrockRolling = 2,
    kGrockStar = 3,
    kGrockStill = 4
};

/* The GORO_ROCK model handle, constructed with file ID 1492 by the
 * compiler's __sinit_d_a_grock.cpp at overlay load. The wrapper name is
 * local; the constructor and destructor are the ROM resource-family
 * functions func_02017acc / func_02017ab4, aliased in the manifest. */
struct GrockModelFilePtr : SharedFilePtr {
    GrockModelFilePtr(u32 fileID);
    ~GrockModelFilePtr();
};

/* arm9 C helper with no shared header. func_02012694 has the same body as
 * Sound::PlayBank3 (0x02012664); the ROM calls this copy. */
extern "C" {
extern void func_02012694(unsigned int id, void *pos);
}


extern GrockModelFilePtr data_ov021_02114a50;

/* D1 stores this vtable, destroys the members in reverse declaration order,
 * then dEnemyBase_c::~dEnemyBase_c. D0 adds dEnemyBase_c's operator delete. */
daGrock_c::~daGrock_c() {}

s32 daGrock_c::OnAimedAtWithEgg() {
    return 0;
}

/* Retire the marker and spawn the star. Still rocks pass spawn mode 2;
 * rolling rocks pass 4. No star (index 0xff) does nothing. */
void daGrock_c::SpawnStar() {
    if (mStarID == 0xff)
        return;
    if (mType == kGrockStill) {
        UntrackAndSpawnStar(mTrackStarID, mStarID, mPos, 2);
    } else {
        UntrackAndSpawnStar(mTrackStarID, mStarID, mPos, 4);
    }
}

/* Shift the position down 3, build the scratch matrix, keep the unrotated
 * copy in pad_188 for the shadow, then rotate it into the model. */
void daGrock_c::UpdateModelTransform() {
    int shifted[3];

    Vec3_Asr(reinterpret_cast<Vector3 *>(shifted), &mPos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, shifted[0], shifted[1], shifted[2]);
    *(Matrix4x3 *)pad_188 = data_020a0e68;
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3 = data_020a0e68;
    DropShadowRadHeight(mShadowModel, *(Matrix4x3 *)pad_188, 0x1f4000, 0x1f4000, 0xf);
}

/* Cylinder hit. Mega always breaks the rock. Punch/kick/breakdance/slide
 * (0x3c0) breaks it only for character 2 (Wario; SaveData's unlock bit 2)
 * and otherwise just plays the bonk. A still rock never hurts the player. */
void daGrock_c::HandleCylinderHit() {
    dActor_c *other;
    u32 hit;
    int isPlayer;

    if (mdCcAcPos_c.otherOwner == 0)
        return;
    other = dActor_c::FindWithID(mdCcAcPos_c.otherOwner);
    if (other == 0)
        return;

    hit = mdCcAcPos_c.hitFlags;
    if ((hit & 0x10) != 0) {
        func_02012694(0x17a, &mCamSpacePos);
        ((daPly_c *)other)->IncMegaKillCount();
        TriplePoofDust();
        Particle::System::NewSimple(kPtclBigSteelieBreak, mPosX, mPosY, mPosZ);
        SpawnStar();
        MarkForDestruction();
        return;
    }

    isPlayer = (int)(other->actorID == 0xbf);
    if (isPlayer == 0)
        return;
    if (((daPly_c *)other)->mIsVanish != 0)
        return;

    if ((hit & 0x3c0) != 0) {
        if (other->param1 == 2) {
            func_02012694(0x17a, &mCamSpacePos);
            MarkForDestruction();
            TriplePoofDust();
            Particle::System::NewSimple(kPtclBigSteelieBreak, mPosX, mPosY, mPosZ);
            SpawnStar();
            return;
        }
        Sound::PlayBank0(0xb5, mCamSpacePos);
        return;
    }

    if (mType == kGrockStill)
        return;
    {
        Vector3 pos;
        pos.x = mPosX;
        pos.y = mPosY;
        pos.z = mPosZ;
        ((daPly_c *)other)->Hurt(pos, 3, 0xc000, 1, 0, 1);
    }
}

/* Floor: bounce, dust, and steer the lateral velocity along the normal,
 * then cap it. Wall: reflect the heading when the approach is steep. */
void daGrock_c::HandleMeshCollision() {
    Vector3 normal;
    Vector3 pos;
    Vector3 wallNormal;
    dBgPi *hit;

    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
    if (mWithMeshClsn.IsOnGround() != 0) {
        hit = mWithMeshClsn.GetFloorResult();
        hit->surface.CopyNormalTo(normal);

        pos = mPos;
        pos.x -= normal.x * 0x12c;
        pos.y -= normal.y * 0x12c;
        pos.z -= normal.z * 0x12c;

        if (mWithMeshClsn.JustHitGround() != 0) {
            if (mVertSpeed < (mVertAccel << 1) && normal.y >= 0xc00)
                mVertSpeed = -mVertSpeed * 6 / 10;
            pos.y += 0x3c000;
            Particle::System::NewSimple(kPtclBigSteelieBounce, pos.x, pos.y, pos.z);
            func_0201267c(0x48, &mCamSpacePos);
        } else {
            mDustParticleID = (int)Particle::System::New(mDustParticleID, kPtclBigSteelieRollDust,
                                                         pos.x, pos.y, pos.z, 0, 0);
            mRollSoundID = Sound::PlayLong(mRollSoundID, 3, 0x8a, mCamSpacePos, 0);
        }
        mSpeedX += normal.x * 5;
        mSpeedZ += normal.z * 5;
        mHorzSpeed = Vec3_HorzLen(reinterpret_cast<const Vector3 *>(&mSpeedX));
        mPrevAngleY = (s16)cstd::atan2(mSpeedX, mSpeedZ);
        if (mHorzSpeed >= 0x22000) {
            mHorzSpeed = 0x22000;
            mSpeedX = FX_SinIdx((u16)mPrevAngleY >> 4) * 0x22;
            mSpeedZ = FX_CosIdx((u16)mPrevAngleY >> 4) * 0x22;
        }
    }

    if (mWithMeshClsn.IsOnWall() == 0)
        return;

    hit = mWithMeshClsn.GetWallResult();
    hit->surface.CopyNormalTo(wallNormal);
    {
        short wallAngle = cstd::atan2(wallNormal.x, wallNormal.z);
        if (GetSubtraction(mPrevAngleY, wallAngle) <= 0x1000)
            return;
        mPrevAngleY = ReflectAngle(wallNormal.x, wallNormal.z, mPrevAngleY);
        UpdatePosWithHorzSpeedAndAng();
    }
}

/* Gravity, clamped at the terminal velocity, then step by the velocity. */
void daGrock_c::ApplyGravityAndMove() {
    int sum = mVertSpeed + mVertAccel;
    int clamped = mTerminalVelocity;

    if (sum >= clamped)
        clamped = sum;
    mVertSpeed = clamped;
    VEC_Add(&mPos, (Vector3 *)&mSpeedX, &mPos);
}

int daGrock_c::CleanupResources() {
    data_ov021_02114a50.Release();
    return 1;
}

int daGrock_c::Render() {
    if (mType >= kGrockRolling)
        mModel.Render(0);
    return 1;
}

int daGrock_c::Behavior() {
    if (mType >= kGrockRolling) {
        func_0200f760(&mdCcAcPos_c);
        HandleCylinderHit();
        if (mDeathState == 8)
            return 1;

        if (mType != kGrockStill) {
            s16 *roll;

            ApplyGravityAndMove();
            roll = (s16 *)&mAngleX;
            *roll = (s16)(*roll + (mHorzSpeed >> 12) * 0x43);
            mAngleY = mPrevAngleY;
            HandleMeshCollision();
        }
        UpdateModelTransform();
        mdCcAcPos_c.Clear();

        if (mPosZ >= (int)0xfe82c000) {
            Vector3 offset;
            offset.x = 0;
            offset.y = (int)0xffebb000;
            offset.z = 0;
            mdCcAcPos_c.SetPosRelativeToActor(offset);
        } else {
            Vector3 offset;
            offset.x = 0;
            offset.y = -0xe1000;
            offset.z = 0;
            mdCcAcPos_c.SetPosRelativeToActor(offset);
        }
        mdCcAcPos_c.dCc_c::Update();

        if (mType != kGrockStill) {
            if (mPosY < -0x3e8000) {
                daGrock_c *spawner;

                MarkForDestruction();
                if (mSpawnerID != 0) {
                    spawner = (daGrock_c *)dActor_c::FindWithID(mSpawnerID);
                    if (spawner)
                        spawner->mStarRockOut = 0;
                }
                UntrackStar(mTrackStarID);
                TriplePoofDust();
                Particle::System::NewSimple(kPtclBigSteelieBreak, mPosX, mPosY, mPosZ);
                func_02012694(0x17a, &mCamSpacePos);
            }
        }
    } else {
        int dist = DistToCPlayer();
        if (dist > 0x5dc000) {
            u32 threshold = (dist < 0x1770000) ? 0x70 : 0xe0;
            if ((u16)mStateTimer >= threshold) {
                u32 rnd;

                mSpawnCount++;
                mStateTimer = 0;
                Vector3_16 rot = mPrevAngle;
                rnd = (u32)RandomIntInternal(&data_0209e650.value);
                rot.y = rot.y + (rnd >> 16) % 0xc00;
                if (mType == kGrockStarSpawner && mSpawnCount >= 5 && mStarRockOut == 0) {
                    daGrock_c *child;

                    mStarRockOut = 1;
                    child = (daGrock_c *)dActor_c::Spawn(0xdd, (mStarID << 8) | kGrockStar, mPos,
                                                         &rot, mAreaId, -1);
                    mSpawnCount = 0;
                    if (child)
                        child->mSpawnerID = uniqueID;
                } else {
                    dActor_c::Spawn(0xdd, kGrockRolling, mPos, &rot, mAreaId, -1);
                }
            }
        }
        {
            u16 *timer = (u16 *)&mStateTimer;
            *timer = (u16)(*timer + 1);
        }
    }
    return 1;
}

int daGrock_c::InitResources() {

    mType = (u8)(param1 & 0xf);
    mStarID = 0xff;
    mTrackStarID = -1;
    mSpawnCount = 0;

    dExtSimpleModel_c::LoadFile(data_ov021_02114a50);

    if (mType >= kGrockRolling) {
        if (mModel.SetFile((BMD_File *)data_ov021_02114a50.filePtr, 1, 1) == 0)
            return 0;
        if (mShadowModel.InitCylinder() == 0)
            return 0;

        mWithMeshClsn.Init(this, 0x12c000, 0, 0, 0);
        mWithMeshClsn.SetLimMovFlag();
        mRollSoundID = 0;

        if (mType == kGrockStill) {
            Vector3 offset;
            mStarID = (u8)((param1 >> 8) & 0xf);
            offset.x = 0;
            offset.y = 0;
            offset.z = 0;
            mdCcAcPos_c.Init(this, offset, 0x10e000, 0x226000, 0x200004, 0x3c0);
        } else {
            Vector3 offset;
            offset.x = 0;
            offset.y = 0;
            offset.z = 0;
            mdCcAcPos_c.Init(this, offset, 0xf3000, 0x226000, 0x200004, 0x3c0);

            if (mType == kGrockStar)
                mStarID = (u8)((param1 >> 8) & 0xf);
            mVertAccel = -0x4000;
            mTerminalVelocity = -0x28000;
            mHorzSpeed = 0x1e000;
            mSpeedX = FX_SinIdx((u16)mPrevAngleY >> 4) * 0x1e;
            mSpeedZ = FX_CosIdx((u16)mPrevAngleY >> 4) * 0x1e;
        }
    } else if (mType == kGrockStarSpawner) {
        mStarID = (u8)((param1 >> 8) & 0xf);
    }

    if (mType != kGrockStarSpawner && mStarID != 0xff)
        mTrackStarID = TrackStar(mStarID, 2);
    mDustParticleID = 0;
    mDeathState = 0;
    mSpawnerID = 0;
    mStarRockOut = 0;
    return 1;
}

/* Reconstructed source-style name: SM64DS proves daGrock_c through RTTI,
 * allocation size, vtable identity, and the GORO_ROCK registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: RollingRock_Spawn. */
daGrock_c *daGrock_c_classInit() {
    return new daGrock_c();
}


/* File-scope model handle. __sinit_d_a_grock.cpp emits the construction
 * and registers the destructor; the registration node is a compiler
 * temporary. */
GrockModelFilePtr data_ov021_02114a50(ASSET_CV_GORO_ROCK_BMD);
