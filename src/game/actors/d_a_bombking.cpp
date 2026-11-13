//cpp
/* daBombking_c -- King Bob-omb (BOMBKING), Bob-omb Battlefield.
 * The destructor is out of line and first. defer_codegen off emits D1 then
 * D0 at the head, so source order is ROM order. common.h is first so the
 * shadow and hold copies are flat 12-word moves.
 */
/* leftovers: the held matrix pointer at actor+0xc8 is dActor_c::mCarryMatrix.
 * UpdateHandMatrix stores the king's own as the member; the stores into
 * the held actor (there and in the throw helper) and the reads in Render and
 * Behavior still use the raw offset on that other actor.
 * dBgCh_Actr::SetWaterTrackFlag is the call
 * that ORs 0x4000 into dBgCh_Actr::mFlags; inlining the or deletes the call.
 * func_02012694 plays bank 3 at mCamSpacePos. It is not Sound::PlayBank3
 * (that body is 0x02012664). dCamera_c::func_0200d8c8 shakes the camera.
 * func_ov002_020db54c launches the grabbed player; func_ov002_020db5f4
 * starts the hold state. func_ov102_0214b384 arms a bob-omb fuse;
 * func_ov102_0214ad14 makes it chase. GetHealth in TickGrabPlayer is
 * called with no receiver: r0 is whatever the previous call left.
 * The resource handles, the carry offsets and the eighteen state records
 * are this TU's statics; mwcc emits their initializer,
 * __sinit_d_a_bombking.cpp (0x02126660). The records keep their
 * data_ov078_* address names, as their handlers keep func_ov078_*.
 * Sound::ChangeMusicVolume keeps its mangled decl (Fix12<int> by value, S13 wall); the Layer3 pair stays on decl_common.h / the local mangled decl because Sound.h declares only Layer1/Layer2.
 */

/* common.h FIRST: daBombking_c.h reaches math/Matrix.h through dExtBlendAnmModel_c.h,
 * and that header spells Matrix4x3 as {Matrix3x3 r; Vector3 t;} where common.h
 * spells it flat as s32 m[12]. Helpers whole-struct-assign the shadow/hold
 * matrices; only the flat spelling reproduces the block move. */
#include "common.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "daBombking_c.h"
#include "SpawnInfo.h"
#include "daBmb_c.h"
#include "SharedFilePtr.h"
#include "dBgCh_Gnd.h"
#include "decl_common.h"
#include "decl_Message.h"
#include "daPly_c.h"
#include "dCamera_c.h"
#include "Message.h"
#include "nitro/fx/fx.h"
#include "Sound.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "LevelRng.h"
namespace cstd {
s16 atan2(s32, s32);
}

/* Remaining reconstruction views. Reconciled by hand against include/: every type
 * the real headers already define (Vector3, Matrix4x3, Fix12<int>, u8/u16/s16,
 * dActor_c, fBase_c, daPly_c, BMD_File, dExtBlendAnmModel_c) was DROPPED here -- the
 * generated preamble redefined all of them. The shapes below are surviving
 * views from the legacy shards, not evidence of original local classes. */

/* Whole-struct 3-word copies go through an array member: mwccarm block-moves
 * (ldm/stm) a POD struct copy under -lang c99 but scalarises the same copy
 * under -lang c++, and an array member forces the block move back. Two sites
 * still need it; the snapshots in TickKnockback and TickChase
 * initialize the POD VecFx32 directly instead. */
struct M3 { int w[3]; };
struct BCA_File;
typedef daBombking_c::StateFunction PMF;

/* Resource handles owned by this TU. The static initializer constructs the
 * model (id 0x2c7) and twelve animations (ids 0x2bb..0x2c6) and registers
 * each destructor. Construct/Load/Release use a u16 fileID at +0, a u8
 * reference count at +2 and the loaded file at +4. The wrappers' constructor
 * and destructor resolve to the retail veneers through the manifest. Names
 * follow the runtime asset paths; they are reconstructed identifiers. */
struct BombkingModelFile : SharedFilePtr {
    BombkingModelFile(u32);
    ~BombkingModelFile();
};
struct BombkingAnimFile : SharedFilePtr {
    BombkingAnimFile(u32);
    ~BombkingAnimFile();
};

/* Runtime asset handles resolved through the overlay 0 file table. */
enum BombkingAssetId {
    ASSET_BOMBKING_DAMAGE_BCA = 0x02bb,
    ASSET_BOMBKING_GETUP_BCA = 0x02bc,
    ASSET_BOMBKING_JUMP_BCA = 0x02bd,
    ASSET_BOMBKING_JUMPED_BCA = 0x02be,
    ASSET_BOMBKING_LIFT_BCA = 0x02bf,
    ASSET_BOMBKING_START_BCA = 0x02c0,
    ASSET_BOMBKING_STRUGGLE_BCA = 0x02c1,
    ASSET_BOMBKING_THROW_BCA = 0x02c2,
    ASSET_BOMBKING_WAIT1_BCA = 0x02c3,
    ASSET_BOMBKING_WAIT2_BCA = 0x02c4,
    ASSET_BOMBKING_WALK1_BCA = 0x02c5,
    ASSET_BOMBKING_WALK2_BCA = 0x02c6,
    ASSET_BOMB_KING_BMD = 0x02c7
};

/* This TU's statics, in the order the retail initializer (0x02126660)
 * builds them. */
BombkingModelFile gBombkingModelFile(ASSET_BOMB_KING_BMD);
BombkingAnimFile gBombkingGetupAnimationFile(ASSET_BOMBKING_GETUP_BCA);
BombkingAnimFile gBombkingDamageAnimationFile(ASSET_BOMBKING_DAMAGE_BCA);
BombkingAnimFile gBombkingJumpAnimationFile(ASSET_BOMBKING_JUMP_BCA);
BombkingAnimFile gBombkingJumpedAnimationFile(ASSET_BOMBKING_JUMPED_BCA);
BombkingAnimFile gBombkingLiftAnimationFile(ASSET_BOMBKING_LIFT_BCA);
BombkingAnimFile gBombkingStruggleAnimationFile(ASSET_BOMBKING_STRUGGLE_BCA);
BombkingAnimFile gBombkingThrowAnimationFile(ASSET_BOMBKING_THROW_BCA);
BombkingAnimFile gBombkingWait1AnimationFile(ASSET_BOMBKING_WAIT1_BCA);
BombkingAnimFile gBombkingWait2AnimationFile(ASSET_BOMBKING_WAIT2_BCA);
BombkingAnimFile gBombkingWalk1AnimationFile(ASSET_BOMBKING_WALK1_BCA);
BombkingAnimFile gBombkingWalk2AnimationFile(ASSET_BOMBKING_WALK2_BCA);
BombkingAnimFile gBombkingStartAnimationFile(ASSET_BOMBKING_START_BCA);

/* Carry offsets for the held player: AttachHeldPlayer uses the second
 * when daPly_c::param1 is 2. */
Vector3 data_ov078_0212711c[2] = {
    Vector3(0x2c000, -0x19000, 0x34000),
    Vector3(0x44000, -0x19000, 0x34000),
};

/* Offset of both collision spheres from the king's position, read
 * field by field in InitResources and Behavior. Initialized data, not
 * built by the initializer. */
VecFx32 data_ov078_02126e00 = {0, 0x3c000, 0};

/* State records: {enter, tick}. KingBobOmb_SetState runs the first and
 * Behavior the second. The handlers keep their address names, so the
 * records keep theirs. */
PMF data_ov078_0212710c[2] = {&daBombking_c::EnterInactive, &daBombking_c::TickInactive};
PMF data_ov078_0212700c[2] = {&daBombking_c::EnterIntroTalk, &daBombking_c::TickIntroTalk};
PMF data_ov078_0212701c[2] = {&daBombking_c::EnterActivate, &daBombking_c::TickActivate};
PMF data_ov078_0212703c[2] = {&daBombking_c::EnterChase, &daBombking_c::TickChase};
PMF data_ov078_0212704c[2] = {&daBombking_c::EnterThrowBomb, &daBombking_c::TickThrowBomb};
PMF data_ov078_0212707c[2] = {&daBombking_c::EnterHeldByPlayer, &daBombking_c::TickHeldByPlayer};
PMF data_ov078_0212708c[2] = {&daBombking_c::EnterThrown, &daBombking_c::TickThrown};
PMF data_ov078_021270ac[2] = {&daBombking_c::EnterBounceOffHill, &daBombking_c::TickBounceOffHill};
PMF data_ov078_021270bc[2] = {&daBombking_c::EnterReturnHome, &daBombking_c::TickReturnHome};
PMF data_ov078_021270dc[2] = {&daBombking_c::EnterHomeTalk, &daBombking_c::TickHomeTalk};
PMF data_ov078_021270ec[2] = {&daBombking_c::EnterDamaged, &daBombking_c::TickDamaged};
PMF data_ov078_02126ffc[2] = {&daBombking_c::EnterKnockback, &daBombking_c::TickKnockback};
PMF data_ov078_0212702c[2] = {&daBombking_c::EnterGetUp, &daBombking_c::TickGetUp};
PMF data_ov078_0212706c[2] = {&daBombking_c::EnterGrabPlayer, &daBombking_c::TickGrabPlayer};
PMF data_ov078_0212709c[2] = {&daBombking_c::EnterCarryPlayer, &daBombking_c::TickCarryPlayer};
PMF data_ov078_021270cc[2] = {&daBombking_c::EnterThrowPlayer, &daBombking_c::TickThrowPlayer};
PMF data_ov078_021270fc[2] = {&daBombking_c::EnterWalkHome, &daBombking_c::TickWalkHome};
PMF data_ov078_0212705c[2] = {&daBombking_c::EnterDefeated, &daBombking_c::TickDefeated};

extern "C" {

/* Canonical single declaration per ROM data symbol -- the generated
     * preamble carried up to three contradictory spellings of each. */
extern unsigned char gCurrentStarId;
extern dCamera_c *gCamera;
extern Matrix4x3 data_020a0e68;
extern int data_02092138;
}

extern void func_02012694(unsigned int a, void *p);
extern void func_02011cfc(void);


// local extern: CalcLib.h does not declare Vec3_VertAngle yet.
extern short Vec3_VertAngle(const Vector3 *a, const Vector3 *b);
extern void func_0200fa8c(void *c, int a);
extern "C" {

extern int atan2_hold_d_a_bombking_1(int a, int b);
extern int func_ov002_020db5f4(daPly_c *c, dActor_c *arg);
extern Matrix4x3 IDENTITY_MATRIX4X3;
}


/* `defer_codegen off` flips .text emission from reversed to source order,
 * which is why this file is written ROM-ASCENDING.
 */
#pragma defer_codegen off

/* Out of line, and FIRST in this ROM-ascending file: that is what puts D1 and
 * D0 at 0x02123740 / 0x02123798, the two lowest addresses in the run. */
daBombking_c::~daBombking_c() {}

int daBombking_c::CheckKnockbackHit() {
    unsigned int v = this->mdCcAcPos_c2.otherOwner;
    if (v == 0) return 0;
    if (dActor_c::FindWithID(v) == 0) return 0;
    if ((this->mdCcAcPos_c2.hitFlags & 0x4000) == 0) return 0;
    KingBobOmb_SetState(data_ov078_02126ffc);
    return 1;
}

/* -------------------------------------------------------------------------- */

void daBombking_c::DetonateSpawnedBombs() {
    int i = 0;
    do {
        daBmb_c *bmb = (daBmb_c *)dActor_c::FindWithID(this->mSpawnedId[i]);
        if (bmb) {
            bmb->mHurtDamage = 0;
            bmb->mDetonate = 1;
        }
        i++;
    } while (i < 2);
}

/* -------------------------------------------------------------------------- */

int daBombking_c::TickDefeated() {
    dCamera_c *cam = gCamera;
    Vector3 v;
    Vector3 t;
    Vector3 u;

    if (this->mTalkingPlayer->GetTalkState() != -1) {
        cam->SetFlag_3();
        return 1;
    }

    cam->mFlags &= ~8;
    v.x = 0;
    v.y = 0;
    v.z = 0;
    data_020a0e68 = this->mBlendAnmModel.mat4x3;
    MTX_Concat43(&this->mBlendAnmModel.data.transforms[1], &data_020a0e68, &data_020a0e68);

    v.x = data_020a0e68.m[9];
    v.y = data_020a0e68.m[10];
    v.z = data_020a0e68.m[11];
    Vec3_Lsl(&t, &v, 3);
    v.x = t.x;
    v.y = t.y;
    v.z = t.z;
    func_02012694(0x130, &this->mCamSpacePosX);

    this->UntrackAndSpawnStar(*(s8 *)&this->mStarTracked, this->mStarID, v, 4);
    Particle::System::NewSimple(kPtclKingBobOmbDefeat, v.x, v.y, v.z);
    u.x = v.x;
    u.y = v.y;
    u.z = v.z;
    this->TriplePoofDustAt(u);

    if (this->mMusicLayer3 == 1) {
        this->mMusicLayer3 = 0;
        Sound::StopLoadedMusic_Layer3();
        func_02011cfc();
        Sound::ChangeMusicVolume(0x7f, 0x15666);
    }
    this->fBase_c::MarkForDestruction();
    return 1;
}

/* -------------------------------------------------------------------------- */

int daBombking_c::EnterDefeated() {
    this->mAnimSpeed = 2;
    this->mHorzSpeed = 0;
    this->mStateTimer = 0x32;
    DetonateSpawnedBombs();
    this->mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingDamageAnimationFile.filePtr, 0, 0, 0x1000,
                                 0);
    return 1;
}

/* -------------------------------------------------------------------------- */

int daBombking_c::TickWalkHome() {
    short ang = Vec3_HorzAngle(&this->mPos, &this->mHomePos);
    if (CheckKnockbackHit() == 1)
        return 1;
    daPly_c *p = this->ClosestPlayer();
    if (p != 0) {
        VecFx32 v = *(VecFx32 *)&p->mPosX;
        if (Vec3_Dist(&this->mArenaPos, (const Vector3 *)&v) < 0x640000) {
            if (this->mArenaPos.y - 0x64000 < v.y) {
                KingBobOmb_SetState(data_ov078_0212703c);
                return 1;
            }
        }
    }
    if (this->mSoundCooldown == 0) {
        func_02012694(0x12d, &this->mCamSpacePosX);
        this->mSoundCooldown = 5;
    }
    ApproachAngle(&this->mPrevAngleY, ang, 5, 0x1000, 0x100);
    this->mAngleY = this->mPrevAngleY;
    if (Vec3_Dist(&this->mPos, &this->mHomePos) < 0x32000) {
        KingBobOmb_SetState(data_ov078_0212710c);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* This caller retains the reconstructed SetAnim ABI declaration above.
   The callee's Fix12 definition experiment in notes/mwccarm-codegen.md 6az
   does not establish that a typed call here would fail to match. */
int daBombking_c::EnterWalkHome() {
    this->mVertAccel = -0x2000;
    this->mAnimSpeed = 2;
    this->mHorzSpeed = 0xa000;
    this->mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingWalk1AnimationFile.filePtr, 4, 0, 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */

int daBombking_c::TickThrowPlayer() {
    if (CheckKnockbackHit() == 1) {
        DetonateSpawnedBombs();
        int v = *(int*)&this->mHeldActor;
        if (v != 0) {
            func_ov002_020db54c((char*)v, 0, 0x50000, this->mAngleY);
            this->mTalkingPlayer = this->mHeldActor;
            this->mHeldActor = 0;
        }
        return 1;
    }
    if (((dExtFrameCtrl_c *)&this->mBlendAnmModel)->WillHitFrame(0x14)) {
        int v = *(int*)&this->mHeldActor;
        if (v != 0) {
            func_ov002_020db54c((char*)v, 0x28000, 0x50000, this->mAngleY);
            this->mTalkingPlayer = this->mHeldActor;
            this->mHeldActor = 0;
            func_02012694(0x131, &this->mCamSpacePosX);
        }
    }
    if (((dExtFrameCtrl_c *)&this->mBlendAnmModel)->Finished()) {
        KingBobOmb_SetState(data_ov078_021270fc);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
struct BCA_File;

int daBombking_c::EnterThrowPlayer() {
    this->mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingThrowAnimationFile.filePtr, 0, 0x40000000,
                                 0x1000, 0);
    this->mHorzSpeed = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */

int daBombking_c::TickCarryPlayer() {
    if (CheckKnockbackHit() == 1) {
        DetonateSpawnedBombs();
        if (this->mHeldActor != 0) {
            func_ov002_020db54c((char *)this->mHeldActor, 0, 0x50000, this->mAngleY);
            this->mTalkingPlayer = this->mHeldActor;
            this->mHeldActor = 0;
        }
        return 1;
    }

    Vector3 v;
    v.x = this->mArenaPos.x;
    v.y = this->mArenaPos.y;
    v.z = this->mArenaPos.z;

    if (gCurrentStarId != 1) {
        v.x = this->mPosX;
        v.y = this->mPosY;
        v.z = this->mPosZ;
        this->mPrevAngleY += 0x1000;
        this->mHorzSpeed = 0;
    } else {
        s16 a = Vec3_HorzAngle(&this->mPos, &v);
        ApproachAngle(&this->mPrevAngleY, a, 5, 0x1000, 0x100);
    }

    this->mAngleY = this->mPrevAngleY;

    if (Vec3_Dist(&this->mPos, &v) > 0x1e000) {
        if (this->mSoundCooldown == 0) {
            func_02012694(0x12d, &this->mCamSpacePosX);
            this->mSoundCooldown = 0xf;
        }
    } else {
        if (this->mSoundCooldown == 0) {
            this->mSoundCooldown = 0x14;
            func_02012694(0x12f, &this->mCamSpacePosX);
        }
    }

    if ((u16)this->mStateTimer == 0)
        KingBobOmb_SetState(&data_ov078_021270cc);
    return 1;
}

/* -------------------------------------------------------------------------- */
int daBombking_c::EnterCarryPlayer() {
    mVertAccel = -0x2000;
    mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingWalk2AnimationFile.filePtr, 0, 0, 0x1000, 0);
    mStateTimer = 0x32;
    mHorzSpeed = 0xa000;
    return 1;
}

/* -------------------------------------------------------------------------- */
int daBombking_c::TickGrabPlayer() {
    if (CheckKnockbackHit() != 1) {
        if (this->mHeldActor == 0) goto L6c;
        if (this->mHeldActor->GetHealth() != 0)
            goto L6c;
    }
    DetonateSpawnedBombs();
    if (this->mHeldActor != 0) {
        func_ov002_020db54c((char *)this->mHeldActor, 0, 0x50000, this->mAngleY);
        this->mTalkingPlayer = this->mHeldActor;
        this->mHeldActor = 0;
    }
    return 1;
L6c:
    if (((dExtFrameCtrl_c *)&this->mBlendAnmModel)->Finished()) {
        KingBobOmb_SetState(data_ov078_0212709c);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */

int daBombking_c::EnterGrabPlayer() {
    this->mVertAccel = -0x2000;
    this->mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingLiftAnimationFile.filePtr, 0, 0x40000000,
                                 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */

int daBombking_c::TickGetUp() {
    s16 ang = this->HorzAngleToCPlayer();
    ApproachAngle(&this->mPrevAngleY, ang, 1, 0x500, 0x500);
    this->mAngleY = this->mPrevAngleY;
    if (((dExtFrameCtrl_c *)&this->mBlendAnmModel)->Finished()) {
        KingBobOmb_SetState(data_ov078_0212703c);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */

int daBombking_c::EnterGetUp() {
    this->mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingGetupAnimationFile.filePtr, 0, 0x40000000,
                                 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
int daBombking_c::TickKnockback() {
    short msg;

    if (mWithMeshClsn.IsOnGround() != 0) {
        if (mHealth <= 0) {
            if (mHorzSpeed != 0) {
                mHorzSpeed = 0;
                ShakeScreen(0x7d0000);
            }
            if ((unsigned short)(mTalkingPlayer->mStateFlags & 0x800) != 0)
                return 1;

            ApproachAngle(&mPrevAngleY, HorzAngleToCPlayer(), 5, 0x1000, 0x200);
            mAngleY = mPrevAngleY;
            if (AngleDiff(HorzAngleToCPlayer(), mAngleY) < 0x1000) {
                daPly_c *pl = mTalkingPlayer;
                if (pl->StartTalk(*this, 1) != 0) {
                    msg = 0;
                    if (gCurrentStarId == 1) {
                        msg += (short)(pl->param1 + 0x9a);
                    } else {
                        msg = 0x95;
                    }
                    Sound::ChangeMusicVolume(0x14, 0x15666);
                    if (pl->ShowMessage(*this, (int)msg, &mPos, 0, 0) != 0) {
                        func_02012694(0x12a, &mCamSpacePosX);
                        KingBobOmb_SetState(&data_ov078_0212705c);
                    }
                }
            }
            return 1;
        }

        if (mActionStep == 0) {
            func_02012694(0x128, &mCamSpacePosX);
            ShakeScreen(0x7d0000);
            HugeLandingDust(1);
            mActionStep = 1;
        }
        if ((unsigned short)mStateTimer == 0) {
            mSpawnSlot = 0;
            KingBobOmb_SetState(&data_ov078_0212702c);
            return 1;
        }
        if (Vec3_Dist(&mArenaPos, &mPos) < 0x258000) {
            short d = (((unsigned)RandomIntInternal(&data_0209e650.value) >> 8) & 0xf) << 12;
            if (d < -0x4000)
                d = -0x4000;
            else if (d > 0x4000)
                d = 0x4000;
            mTargetAngY += d;
        } else {
            mTargetAngY = Vec3_HorzAngle(&mPos, &mArenaPos);
        }
        mActionStep = 0;
        mVertSpeed = 0x1e000;
        mHorzSpeed = 0xa000;
    } else {
        if (mVertSpeed < 0) {
            unsigned int id = mdCcAcPos_c.otherOwner;
            if (id != 0) {
                daPly_c *a = static_cast<daPly_c *>(dActor_c::FindWithID(id));
                if (a != 0) {
                    int b = (int)(a->actorID == 0xbf);
                    if (b != 0) {
                        VecFx32 v = *(const VecFx32 *)&a->mPosX;
                        if (mPosY > v.y)
                            a->Unk_020c6a10(1);
                    }
                }
            }
        }
    }

    mAngleY = mPrevAngleY;
    ApproachAngle(&mPrevAngleY, mTargetAngY, 5, 0x1000, 0x200);
    return 1;
}

/* -------------------------------------------------------------------------- */

int daBombking_c::EnterKnockback() {
    this->mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingDamageAnimationFile.filePtr, 0, 0, 0x1000,
                                 0);
    this->mStateTimer = 0xc8;
    this->mTargetAngY = Vec3_HorzAngle(&this->mPos, &this->mArenaPos);
    this->mHealth -= 1;
    this->mActionStep = 0;
    this->mVertSpeed = 0x28000;
    this->mHorzSpeed = 0x5000;
    if (this->mHealth <= 0)
        this->mFlags = 0x10000000;
    this->mVertAccel = -0x2000;
    return 1;
}

/* -------------------------------------------------------------------------- */

int daBombking_c::TickDamaged() {
    s16 ang = this->HorzAngleToCPlayer();
    ApproachAngle(&this->mPrevAngleY, ang, 1, 0x500, 0x500);
    this->mAngleY = this->mPrevAngleY;
    if ((u16)this->mStateTimer == 0)
        KingBobOmb_SetState(&data_ov078_0212702c);
    return 1;
}

/* -------------------------------------------------------------------------- */

int daBombking_c::EnterDamaged() {
    this->mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingDamageAnimationFile.filePtr, 0, 0, 0x1000,
                                 0);
    this->mStateTimer = 0x32;
    return 1;
}

/* -------------------------------------------------------------------------- */

int daBombking_c::TickHomeTalk() {
    Vector3 a;
    Vector3 look;
    Vector3 pos;
    Vector3 in;
    Vector3 out;
    Vector3 ppos;
    Vector3 tmp;
    Vector3 dust;
    dCamera_c *cam;
    daPly_c *player;
    int *pp;
    s16 ang;

    cam = gCamera;
    if (((dExtFrameCtrl_c *)&this->mBlendAnmModel)->WillHitFrame(0x46)) {
        ShakeScreen(0x7d0000);
        func_02012694(0x12c, &this->mCamSpacePosX);
        a.x = 0;
        a.y = 0;
        a.z = 0;
        data_020a0e68 = this->mBlendAnmModel.mat4x3;
        MTX_Concat43(&this->mBlendAnmModel.data.transforms[6], &data_020a0e68, &data_020a0e68);
        a.x = data_020a0e68.m[9];
        a.y = data_020a0e68.m[10];
        a.z = data_020a0e68.m[11];
        Vec3_Lsl(&tmp, &a, 3);
        a.x = tmp.x;
        dust.x = tmp.x;
        a.y = tmp.y;
        dust.y = tmp.y;
        a.z = tmp.z;
        dust.z = tmp.z;
        this->HugeLandingDustAt(dust, 1);
    }
    player = this->mTalkingPlayer;
    if (player->GetTalkState() != -1) {
        pp = &player->mPosX;
        in.x = 0;
        in.y = 0;
        in.z = 0;
        out.x = 0;
        out.y = 0;
        out.z = 0;
        ppos.x = pp[0];
        ppos.y = pp[1];
        ppos.z = pp[2];
        ang = Vec3_HorzAngle(&this->mPos, &ppos);
        look.x = this->mPosX;
        look.y = this->mPosY;
        look.z = this->mPosZ;
        pos.x = this->mPosX;
        pos.y = this->mPosY;
        pos.z = this->mPosZ;
        look.y += 0x100000;
        in.z = 0x800000;
        Matrix4x3_FromRotationY(&data_020a0e68, ang);
        MTX_MultVec43(&in, &data_020a0e68, &out);
        pos.y += 0x100000;
        pos.x += out.x;
        pos.z += out.z;
        cam->SetLookAt(look);
        cam->SetPos(pos);
        ApproachLinear(this->mAngleY, ang, 0x800);
        this->mPrevAngleY = this->mAngleY;
        return 1;
    }
    cam->mFlags &= ~8;
    if (((dExtFrameCtrl_c *)&this->mBlendAnmModel)->Finished()) {
        KingBobOmb_SetState(&data_ov078_0212703c);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */

int daBombking_c::EnterHomeTalk() {
    this->mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingStartAnimationFile.filePtr, 8, 0x40000000,
                                 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */

int daBombking_c::TickReturnHome() {
    short horz;
    short vert;
    Vector3 vA;
    Vector3 vB;
    Vector3 vC;
    Vector3 D;
    Vector3 E;

    if (this->mActionStep != 2) {
        horz = Vec3_HorzAngle(&this->mPos, &this->mArenaPos);
        vert = Vec3_VertAngle(&this->mPos, &this->mArenaPos);
        Vec3_Dist(&this->mPos, &this->mArenaPos);
        vA.x = 0;
        vA.y = 0;
        vA.z = 0;
        ApproachAngle(&this->mPrevAngleX, vert, 5, 0x1000, 0x300);
        ApproachAngle(&this->mPrevAngleY, horz, 5, 0x1000, 0x300);
        this->mAngleY = this->mPrevAngleY;
        if (((dExtFrameCtrl_c *)&this->mBlendAnmModel)->Finished() == 0)
            return 1;
        vB.x = 0;
        vB.y = 0x3c000;
        vB.z = 0x28000;
        vC.x = 0;
        vC.y = 0;
        vC.z = 0;
        Matrix4x3_FromRotationY(&data_020a0e68, this->mPrevAngleY);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, this->mPrevAngleX);
        MTX_MultVec43(&vB, &data_020a0e68, &vC);
        this->mSpeedX = vC.x;
        if (this->mActionStep == 0) {
            this->mVertSpeed = vC.y;
            if (this->mVertSpeed > 0x3c000)
                this->mVertSpeed = 0x3c000;
            else if (this->mVertSpeed < 0x1e000)
                this->mVertSpeed = 0x1e000;
        }
        this->mSpeedZ = vC.z;
        this->mVertSpeed += this->mVertAccel;
        vA.x = this->mPosX;
        vA.y = this->mPosY;
        vA.z = this->mPosZ;
        vA.y = this->mArenaPos.y;
        if (Vec3_Dist(&vA, &this->mArenaPos) > 0x640000)
            return 1;
        this->mActionStep = 1;
    }

    if (this->mWithMeshClsn.IsOnGround() != 0) {
        daPly_c *pl = this->ClosestPlayer();
        if (pl != 0) {
            *(struct M3 *)&D = *(struct M3 *)&pl->mPosX;
            if (Vec3_Dist(&this->mArenaPos, &D) > 0x640000 || this->mArenaPos.y - 0x64000 > D.y) {
                this->mSpeedX = 0;
                this->mVertSpeed = 0;
                this->mSpeedZ = 0;
                KingBobOmb_SetState(&data_ov078_021270fc);
                return 1;
            }
        }

        if (this->mActionStep == 1) {
            this->mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingJumpedAnimationFile.filePtr, 0,
                                         0x40000000, 0x1000, 0);
            this->mVertAccel = -0x2000;
            this->mFlags = 0x10000002;
            func_02012694(0x12c, &this->mCamSpacePosX);
            ShakeScreen(0xfa0000);
            func_0200fa8c(this, 1);
            this->mActionStep = 2;
            this->mSpeedX = 0;
            this->mVertSpeed = 0;
            this->mSpeedZ = 0;
            return 1;
        }

        if (((dExtFrameCtrl_c *)&this->mBlendAnmModel)->Finished() != 0) {
            daPly_c *pl2 = this->mTalkingPlayer;
            unsigned short m;
            E.x = this->mPosX;
            E.y = this->mPosY;
            E.z = this->mPosZ;
            E.y += 0xc8000;
            m = pl2->mStateFlags & 0x800;
            if (m == 0) {
                if (pl2->ShowMessage(*this, 0x94, &E, 0, 0) != 0) {
                    gCamera->SetFlag_3();
                    func_02012694(0x12a, &this->mCamSpacePosX);
                    KingBobOmb_SetState(&data_ov078_021270dc);
                }
            }
        }
    }

    return 1;
}

/* -------------------------------------------------------------------------- */

int daBombking_c::EnterReturnHome() {
    this->mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingJumpAnimationFile.filePtr, 0, 0x40000000,
                                 0x1000, 0);
    func_02012694(0x129, &this->mCamSpacePosX);
    this->mWithMeshClsn.ClearGroundFlag();
    this->mPrevAngleX = 0;
    this->mHorzSpeed = 0;
    this->mSpeedX = 0;
    this->mVertSpeed = 0;
    this->mSpeedZ = 0;
    this->mActionStep = 0;
    this->mVertAccel = -0x6000;
    return 1;
}

/* -------------------------------------------------------------------------- */

int daBombking_c::TickBounceOffHill() {
    if ((unsigned short)this->mStateTimer != 0) return 1;
    if (*(int *)&data_02092138 > this->mPosY) {
        KingBobOmb_SetState(data_ov078_021270bc);
        return 1;
    }
    if (this->mWithMeshClsn.IsOnGround() != 0) {
        if (this->mActionStep == 0) {
            ShakeScreen(0x7d0000);
            func_0200fa8c(this, 1);
            func_02012694(0x128, &this->mCamSpacePosX);
            this->mStateTimer = 5;
            this->mVertSpeed = 0x14000;
            this->mHorzSpeed = 0xa000;
            this->mActionStep = 1;
        } else {
            KingBobOmb_SetState(data_ov078_021270bc);
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */

int daBombking_c::EnterBounceOffHill() {
    this->mVertAccel = -0x2000;
    this->mVertSpeed = 0x1e000;
    this->mHorzSpeed = 0x14000;
    this->mActionStep = 0;
    this->mStateTimer = 5;
    this->mFlags = 0x10000000;
    ShakeScreen(0xfa0000);
    func_0200fa8c(this, 1);
    return 1;
}

/* -------------------------------------------------------------------------- */
int daBombking_c::TickThrown() {
    if (this->mWithMeshClsn.IsOnGround() == 0) goto done;
    this->mHorzSpeed = 0;
    if ((int)(this->mArenaPos.y - 0x28000) > this->mPosY) {
        func_02012694(0x128, &this->mCamSpacePosX);
        KingBobOmb_SetState(&data_ov078_021270ac);
        goto done;
    }
    if (this->mActionStep == 0) {
        func_02012694(0x128, &this->mCamSpacePosX);
        func_02012694(0x12b, &this->mCamSpacePosX);
        ShakeScreen(0x7d0000);
        func_0200fa8c(this, 1);
        this->mActionStep = 1;
        this->mHealth -= 1;
    }
    if (this->mHealth > 0) {
        KingBobOmb_SetState(&data_ov078_021270ec);
        goto done;
    }
    daPly_c *other = this->mTalkingPlayer;
    if (other == 0) goto done;
    if ((unsigned short)(other->mStateFlags & 0x800) != 0) goto done;
    if (other->StartTalk(*this, 1) == 0)
        goto done;

    short msg = 0;
    Sound::ChangeMusicVolume(0x14, 0x15666);
    if (gCurrentStarId == 1) {
        msg += (short)(other->param1 + 0x9a);
    } else {
        msg = 0x95;
    }

    if (other->ShowMessage(*this, (unsigned)(int)msg, &this->mPos, 0, 0) == 0)
        goto done;
    func_02012694(0x12a, &this->mCamSpacePosX);
    KingBobOmb_SetState(&data_ov078_0212705c);
done:
    return 1;
}

/* -------------------------------------------------------------------------- */

int daBombking_c::EnterThrown() {
    this->mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingDamageAnimationFile.filePtr, 0, 0, 0x1000,
                                 0);
    if (this->mHealth <= 0)
        this->mFlags = 0x10000000;
    this->mWithMeshClsn.ClearGroundFlag();
    this->mActionStep = 0;
    this->mVertAccel = -0x2000;
    this->mVertSpeed = 0x28000;
    this->mHorzSpeed = 0x14000;
    this->mTerminalVelocity = -0x3c000;
    return 1;
}

/* -------------------------------------------------------------------------- */

int daBombking_c::TickHeldByPlayer() {
    if (this->mActionStep == 0) {
        int b = (this->mFlags & 0x4000) != 0;
        if (b != 0) {
            this->mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingStruggleAnimationFile.filePtr, 0, 0,
                                         0x1000, 0);
            this->mActionStep = 1;
        }
    }

    {
        daPly_c *obj = this->mHeldActor;
        if (obj == 0) {
            KingBobOmb_SetState(&data_ov078_0212708c);
            return 1;
        }
        {
            VecFx32 copy = *(VecFx32 *)&obj->mPosX;
            if (Vec3_Dist(&this->mArenaPos, (const Vector3 *)&copy) > 0x640000)
                goto drop;
            if (this->mArenaPos.y - 0xa000 <= copy.y)
                goto flags;
        }
    }

drop:
    this->mHeldActor->DropActor();
    this->mHeldActor = 0;
    KingBobOmb_SetState(&data_ov078_0212708c);
    return 1;

flags:
    {
        int flags = this->mFlags;
        int b0 = (flags & 0x400) != 0;
        if (b0 == 0) {
            int b1 = (flags & 0x2000) != 0;
            if (b1 == 0) {
                int b2 = (flags & 0x100) != 0;
                if (b2 != 0)
                    goto done;
            }
        }
        if (b0 != 0) {
            this->mPrevAngleY = this->mHeldActor->mAngleY;
        }
        {
            short t = this->mPrevAngleY;
            this->mAngleY = t;
        }
        this->mdCcAcPos_c.flags &= ~2;
        KingBobOmb_SetState(&data_ov078_0212708c);
        {
            daPly_c *p = this->mHeldActor;
            if (p != 0) {
                if (p->param1 == 2) {
                    this->mVertSpeed = 0x32000;
                    this->mHorzSpeed = 0x1e000;
                }
            }
        }
        this->mHeldActor = 0;
    }
done:
    return 1;
}

/* -------------------------------------------------------------------------- */

int daBombking_c::EnterHeldByPlayer() {
    this->mdCcAcPos_c.flags |= 2;
    this->mVertAccel = 0;
    this->mHorzSpeed = 0;
    this->mActionStep = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */

int daBombking_c::TickThrowBomb() {
    struct Vector3 in, out, v[2];
    daBmb_c *target;
    daPly_c *player;

    if (CheckKnockbackHit() == 1) {
        DetonateSpawnedBombs();
        return 1;
    }

    if (this->mSpawnedThrown[this->mSpawnSlot] == 0) {
        target = (daBmb_c *)dActor_c::FindWithID(this->mSpawnedId[this->mSpawnSlot]);
        if (target != 0) {
            target->mPosX = this->mThrowPos.x;
            target->mPosY = this->mThrowPos.y;
            target->mPosZ = this->mThrowPos.z;
            target->mPrevAngleX = this->mPrevAngleX;
            target->mPrevAngleY = this->mPrevAngleY;
            target->mPrevAngleZ = this->mPrevAngleZ;
            target->mAngleX = this->mAngleX;
            target->mAngleY = this->mAngleY;
            target->mAngleZ = this->mAngleZ;
            target->func_ov102_0214b384(0x78);
            if (((dExtFrameCtrl_c *)&this->mBlendAnmModel)->WillHitFrame(0x13) != 0 ||
                CheckKnockbackHit() == 1) {
                in.x = 0; in.y = 0; in.z = 0x28000;
                out.x = 0; out.y = 0; out.z = 0;
                v[0].x = 0; v[0].y = 0; v[0].z = 0;
                player = this->ClosestPlayer();
                if (player != 0) {
                    int *q = &player->mPosX;
                    v[1].x = q[0];
                    v[1].y = q[1];
                    v[1].z = q[2];
                    v[0].x = v[1].x - this->mThrowPos.x;
                    v[0].y = v[1].y - this->mThrowPos.y;
                    v[0].z = v[1].z - this->mThrowPos.z;
                    Matrix4x3_FromRotationY(&data_020a0e68, cstd::atan2(v[0].x, v[0].z));
                    Matrix4x3_ApplyInPlaceToRotationX(
                        &data_020a0e68, (short)(-cstd::atan2(v[0].y, Vec3_HorzLen(&v[0]))));
                    out.y += 0x14000;
                    target->func_ov102_0214ad14();
                    target->mSpeedX = out.x;
                    target->mVertSpeed = out.y;
                    target->mSpeedZ = out.z;
                    target->mHorzSpeed = 0x14000;
                    target->mVertAccel = -0x2000;
                    MTX_MultVec43(&in, &data_020a0e68, &out);
                }
                this->mSpawnedThrown[this->mSpawnSlot] = 1;
                target->mCarryMatrix = 0;
            }
        }
    } else {
        if (CheckKnockbackHit() == 1) {
            return 1;
        }
    }

    if (((dExtFrameCtrl_c *)&this->mBlendAnmModel)->Finished() != 0) {
        player = this->ClosestPlayer();
        if (player != 0) {
            if (player->param1 != 3) {
                this->mThrowCooldown = 0x64;
            }
        }
        KingBobOmb_SetState(&data_ov078_0212703c);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */

int daBombking_c::EnterThrowBomb() {
    int i;
    int r;

    this->mAnimSpeed = 1;
    this->mVertAccel = -0x2000;
    this->mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingThrowAnimationFile.filePtr, 0, 0x40000000,
                                 0x1000, 0);

    this->mHorzSpeed = 0;

    for (i = 0; i < this->mPhase; i++) {
        if (this->mSpawnedId[i] == 0) {
            r = (int)dActor_c::Spawn(0xce, 2, this->mThrowPos, 0, this->mAreaId, -1);
            if (r != 0) {
                this->mSpawnedId[i] = ((dActor_c *)r)->uniqueID;
                this->mSpawnSlot = i;
                return 1;
            }
        }
    }

    this->mActionStep = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */

int daBombking_c::TickChase() {
    daPly_c *p;
    dActor_c *a;
    int isType;
    int index;
    int empty;
    int z;
    unsigned int id;
    int v4a0;
    int slot;

    if (this->mSoundCooldown == 0) {
        func_02012694(0x12d, &this->mCamSpacePosX);
        this->mSoundCooldown = 0xf;
    }
    if (CheckKnockbackHit() == 1) {
        DetonateSpawnedBombs();
        return 1;
    }

    p = this->ClosestPlayer();
    if (p != 0) {
        VecFx32 v = *(const VecFx32 *)&p->mPosX;
        if (Vec3_Dist(&this->mArenaPos, (const Vector3 *)&v) > 0x640000 ||
            (this->mArenaPos.y - 0xa000) > v.y) {
            this->mHorzSpeed = 0;
            KingBobOmb_SetState(data_ov078_021270fc);
            return 1;
        }

        ApproachAngle(&this->mPrevAngleY, this->HorzAngleToCPlayer(), 0xa, 0x200, 0x100);
        {
            s16 ang = this->mPrevAngleY;
            this->mAngleY = ang;
            this->mFlags &= ~0x80;
        }

        id = this->mdCcAcPos_c2.otherOwner;
        if (id != 0) {
            a = dActor_c::FindWithID(id);
            if (a != 0) {
                isType = (int)(a->actorID == 0xbf);
                if (isType != 0) {
                    daPly_c *player = static_cast<daPly_c *>(a);
                    if (AngleDiff(this->HorzAngleToCPlayer(), this->mAngleY) > 0x2800) {
                        if ((this->mdCcAcPos_c2.hitFlags & 0x1000) != 0) {
                            this->mFlags |= 0x80;
                            if (player->TryGrab(*this) != 0) {
                                this->mHeldActor = player;
                                this->mHorzSpeed = 0;
                                KingBobOmb_SetState(data_ov078_0212707c);
                            }
                        }
                    } else if (func_ov002_020db5f4(player, this) != 0) {
                        this->mHeldActor = player;
                        this->mHorzSpeed = 0;
                        KingBobOmb_SetState(data_ov078_0212706c);
                    }
                }
            }
            return 1;
        }

        if (this->mThrowCooldown == 0) {
            if (gCurrentStarId == 1) {
                if (this->DistToCPlayer() < 0x3e8000) {
                    if (AngleDiff(this->HorzAngleToCPlayer(), this->mAngleY) < 0x2800) {
                        index = 0;
                        empty = index;
                        z = index;
                        for (; index < 2; index++) {
                            slot = this->mSpawnedId[index];
                            if (slot != 0) {
                                if (dActor_c::FindWithID((unsigned int)slot) == 0) {
                                    this->mSpawnedId[index] = z;
                                    this->mSpawnedThrown[index] = (unsigned char)z;
                                }
                            }
                            if (this->mSpawnedId[index] == 0) {
                                empty++;
                                if (empty == 2)
                                    this->mPhase ^= 3;
                            }
                        }
                        v4a0 = this->mPhase;
                        if ((v4a0 == 2 && this->mSpawnedId[1] == 0)
                            || (v4a0 == 1 && empty == 2)) {
                            KingBobOmb_SetState(data_ov078_0212704c);
                            return 1;
                        }
                    }
                }
            }
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */

int daBombking_c::EnterChase() {
    this->mAnimSpeed = 1;
    this->mHorzSpeed = 0x5000;
    this->mVertAccel = -0x2000;
    this->mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingWalk1AnimationFile.filePtr, 8, 0, 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
int daBombking_c::TickActivate() {
    Vector3 s;
    Vector3 d;
    Vector3 v;
    if (CheckKnockbackHit() == 1)
        return 1;
    ApproachAngle(&this->mPrevAngleY, this->HorzAngleToCPlayer(), 1, 0x500, 0x500);
    this->mAngleY = this->mPrevAngleY;
    if (((dExtFrameCtrl_c *)&this->mBlendAnmModel)->WillHitFrame(0x46)) {
        ShakeScreen(0x7d0000);
        func_02012694(0x12c, &this->mCamSpacePosX);
        s.x = 0;
        s.y = 0;
        s.z = 0;
        data_020a0e68 = this->mBlendAnmModel.mat4x3;
        MTX_Concat43(&this->mBlendAnmModel.data.transforms[6], &data_020a0e68, &data_020a0e68);
        s.x = data_020a0e68.m[9];
        s.y = data_020a0e68.m[10];
        s.z = data_020a0e68.m[11];
        Vec3_Lsl(&d, &s, 3);
        s.x = d.x;
        v.x = d.x;
        s.y = d.y;
        v.y = d.y;
        s.z = d.z;
        v.z = d.z;
        this->HugeLandingDustAt(v, 1);
    }
    if (((dExtFrameCtrl_c *)&this->mBlendAnmModel)->Finished()) {
        KingBobOmb_SetState(&data_ov078_0212703c);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */

int daBombking_c::EnterActivate() {
    this->mVertAccel = -0x2000;
    this->mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingStartAnimationFile.filePtr, 8, 0x40000000,
                                 0x1000, 0);
    if (this->mMusicLayer3 == 0) {
        this->mMusicLayer3 = 1;
        this->mFlags = 0x10000002;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
int daBombking_c::TickIntroTalk() {
    daPly_c *target = this->mTalkingPlayer;
    int *src = &target->mPosX;
    Vector3 v;
    int t = src[0];
    v.x = t;
    v.y = src[1];
    v.z = src[2];
    short ang = Vec3_HorzAngle(&this->mPos, &v);
    ApproachLinear(this->mAngleY, ang, 0x800);
    this->mPrevAngleY = this->mAngleY;
    if (target->GetTalkState() == -1) {
        Message::EndTalk();
        func_02011d44();
        Sound::LoadAndSetMusic_Layer3(0x2d);
        KingBobOmb_SetState(&data_ov078_0212701c);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */

int daBombking_c::EnterIntroTalk() {
    return 1;
}

/* -------------------------------------------------------------------------- */

int daBombking_c::TickInactive() {
    int dist;
    struct Vector3 ppos;
    struct Vector3 v;
    daPly_c *player;
    unsigned int b;

    if (this->mMusicLayer3 == 1) {
        unsigned char *p = &this->mMusicLayer3Timer;
        *p += 1;
        if (this->mMusicLayer3Timer > 0xc8) {
            this->mFlags = 0x10000003;
            this->mMusicLayer3 = 0;
            this->mMusicLayer3Timer = 0;
            this->mHealth = 3;
            Sound::StopLoadedMusic_Layer3();
            func_02011cfc();
            this->mIntroTalked = 0;
        }
    }

    ApproachLinear(this->mAngleY, this->mInitAngleY, 0x800);
    this->mPrevAngleY = this->mAngleY;

    if (CheckKnockbackHit() == 1) {
        return 1;
    }

    player = this->ClosestPlayer();
    if (player != 0) {
        *(struct M3 *)&ppos = *(struct M3 *)&player->mPosX;

        if (this->mIntroTalked == 0) {
            dist = Vec3_Dist(&this->mPos, &ppos);
            if (dist < 0x12c000) {
                this->mTalkingPlayer = player;
                v.x = this->mPosX;
                v.y = this->mPosY;
                v.z = this->mPosZ;
                v.y = v.y + 0xc8000;

                if (this->mTalkingPlayer->StartTalk(*this, 1)) {
                    Message::PrepareTalk();
                    b = (gCurrentStarId != 1) ? 0x93 : (unsigned int)(short)(player->param1 + 0x96);
                    if (player->ShowMessage(*this, b, &v, 0, 0)) {
                        func_02012694(0x12a, &this->mCamSpacePosX);
                        this->mIntroTalked = 1;
                        KingBobOmb_SetState(&data_ov078_0212700c);
                    }
                }
            }
        } else {
            dist = Vec3_Dist(&this->mPos, &ppos);
            if (dist < 0x258000) {
                if (this->mArenaPos.y - 0xa000 < ppos.y) {
                    KingBobOmb_SetState(&data_ov078_0212701c);
                }
            }
        }
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
int daBombking_c::EnterInactive() {
    this->mHorzSpeed = 0;
    this->mAnimSpeed = 1;
    this->mVertAccel = -0x2000;
    this->mMusicLayer3Timer = 0;
    this->mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingWait2AnimationFile.filePtr, 8, 0, 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
void daBombking_c::ShakeScreen(int strength) {
    gCamera->func_0200d8c8(&mPos, strength);
}

/* -------------------------------------------------------------------------- */
int daBombking_c::KingBobOmb_SetState(void *pv) {
    mState = (PMF *)pv;
    PMF *q = mState;
    if (*q == 0) return 1;
    return (this->**q)();
}

/* -------------------------------------------------------------------------- */
void daBombking_c::UpdateShadow() {
    int h = mPosY;
    if (this->mWithMeshClsn.IsOnGround() == 0) {
        dBgCh_Gnd rg;
        rg.SetObjAndPos(this->mPos, 0);
        if (rg.DetectClsn() != 0)
            h = rg.clsnY;
    }
    int b = (this->mFlags & 0x4000) != 0;
    if (b) {
        daPly_c *p = this->mHeldActor;
        if (p != 0)
            h = p->mPosY;
    }
    int ip = this->mPosY - h;
    if (ip <= 0x1000)
        ip = 0x1000;
    int scale = 0x15e000 - FX_MUL(ip, 0x180);
    if (scale < 0xa000)
        scale = 0xa000;
    *(Matrix4x3 *)this->mShadowMtx = IDENTITY_MATRIX4X3;
    this->mShadowMtx[9] = this->mPosX >> 3;
    this->mShadowMtx[10] = this->mPosY >> 3;
    this->mShadowMtx[11] = this->mPosZ >> 3;
    DropShadowRadHeight(this->mShadowModel, *(Matrix4x3 *)this->mShadowMtx, scale, ip + 0x28000,
                        0xf);
}

/* -------------------------------------------------------------------------- */
void daBombking_c::UpdateHandMatrix() {
    struct Vector3 lv;
    int i;

    Matrix4x3_FromRotationY(&this->mBlendAnmModel.mat4x3, this->mAngleY);
    this->mBlendAnmModel.mat4x3.m[9] = this->mPosX >> 3;
    this->mBlendAnmModel.mat4x3.m[10] = this->mPosY >> 3;
    this->mBlendAnmModel.mat4x3.m[11] = this->mPosZ >> 3;
    MTX_Concat43(&this->mBlendAnmModel.data.transforms[10], &this->mBlendAnmModel.mat4x3,
                 &this->mHoldMtx);

    /* Held-matrix pointer: dActor_c's mCarryMatrix. */
    this->mCarryMatrix = &this->mHoldMtx;
    this->mThrowPos.x = 0;
    this->mThrowPos.y = 0;
    this->mThrowPos.z = 0;
    this->mThrowPos.x = this->mHoldMtx.m[9];
    this->mThrowPos.y = this->mHoldMtx.m[10];
    this->mThrowPos.z = this->mHoldMtx.m[11];
    Vec3_Lsl(&lv, &this->mThrowPos, 3);
    this->mThrowPos.x = lv.x;
    this->mThrowPos.y = lv.y;
    this->mThrowPos.z = lv.z;

    for (i = 0; i < 2; i++) {
        unsigned int id;
        void *actor;

        if (this->mSpawnedThrown[i] != 0)
            continue;
        id = (unsigned int)this->mSpawnedId[i];
        if (id == 0)
            continue;
        actor = (void *)dActor_c::FindWithID(id);
        if (actor == 0)
            continue;

        data_020a0e68 = this->mHoldMtx;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0xb000, -0x7000, -0x6000);
        Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, 0x4000, -0x1000, 0x2000);
        this->mHoldMtx = data_020a0e68;
        ((dActor_c *)actor)->mCarryMatrix = &this->mHoldMtx;
    }
}

/* -------------------------------------------------------------------------- */

void daBombking_c::AttachHeldPlayer() {
    daPly_c *player = mHeldActor;
    int idx = 0;
    if (player == 0) return;
    if (player->param1 == 2) idx = 1;
    Matrix4x3 *res = this->UpdateCarry(*player, data_ov078_0212711c[idx]);
    this->mBlendAnmModel.mat4x3 = *res;
}

/* -------------------------------------------------------------------------- */
int daBombking_c::CleanupResources()
{
    gBombkingModelFile.Release();
    gBombkingGetupAnimationFile.Release();
    gBombkingDamageAnimationFile.Release();
    gBombkingJumpAnimationFile.Release();
    gBombkingJumpedAnimationFile.Release();
    gBombkingLiftAnimationFile.Release();
    gBombkingStruggleAnimationFile.Release();
    gBombkingThrowAnimationFile.Release();
    gBombkingWait1AnimationFile.Release();
    gBombkingWait2AnimationFile.Release();
    gBombkingWalk1AnimationFile.Release();
    gBombkingWalk2AnimationFile.Release();
    gBombkingStartAnimationFile.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
void daBombking_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
int daBombking_c::Render()
{
    void *held = mHeldActor;
    if (held != 0) {
        int flags = mFlags;
        int flag = (flags & 0x4000) ? 1 : 0;
        if (flag != 0) {
            if (((dActor_c *)held)->mCarryMatrix != 0) {
                AttachHeldPlayer();
            }
        }
    }
    mBlendAnmModel.dExtSimpleModel_c::Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */

int daBombking_c::Behavior()
{
    if (DistToCPlayer() < 0x1770000) {
        gCamera->mFramedActor = this;
    }

    if (mState[1] != 0) {
        PMF *p = mState + 1;
        (this->**p)();
    }

    mBlendAnmModel.speed = mAnimSpeed << 0xc;
    mBlendAnmModel.UpdateVerts();
    mBlendAnmModel.Advance();

    if ((char *)mState == (char *)data_ov078_0212707c) {
        void *held = mHeldActor;
        int flag;
        if (held != 0) {
            flag = (mFlags & 0x4000) != 0;
            if (flag != 0 && ((dActor_c *)held)->mCarryMatrix != 0) {
                goto skip_de0;
            }
        }
        UpdateHandMatrix();
    skip_de0:
        UpdateShadow();
        return 1;
    }

    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    DecIfAbove0_Byte(&mSoundCooldown);
    DecIfAbove0_Byte(&mThrowCooldown);

    if ((char *)mState != (char *)data_ov078_021270bc) {
        UpdatePos(&mdCcAcPos_c);
    } else {
        UpdatePosWithOnlySpeed(&mdCcAcPos_c);
    }

    if ((char *)mState != (char *)data_ov078_021270bc || mActionStep == 1) {
        UpdateWMClsn(mWithMeshClsn, 0);
    }

    if ((char *)mState == (char *)data_ov078_0212703c || (char *)mState == (char *)data_ov078_021270fc) {
        if (mWithMeshClsn.IsOnWall() != 0 || mWithMeshClsn.IsOnGround() == 0 ||
            (mArenaPos.y - 0x28000) > mPosY) {
            KingBobOmb_SetState(data_ov078_021270bc);
        }
    }

    {
        Vector3 v;
        v.x = data_ov078_02126e00.x;
        v.y = data_ov078_02126e00.y;
        v.z = data_ov078_02126e00.z;
        mdCcAcPos_c.SetPosRelativeToActor(v);
    }
    {
        Vector3 v;
        v.x = data_ov078_02126e00.x;
        v.y = data_ov078_02126e00.y;
        v.z = data_ov078_02126e00.z;
        mdCcAcPos_c2.SetPosRelativeToActor(v);
    }
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.Update();
    mdCcAcPos_c2.Clear();
    mdCcAcPos_c2.Update();

    UpdateHandMatrix();
    UpdateShadow();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* SharedFilePtr's complete declaration is included above. The file handles
   below still use their existing ROM-backed data declarations. */
int daBombking_c::InitResources()
{
    BMD_File *f;
    Vector3 v0;
    Vector3 v1;
    int i;
    f = dExtSimpleModel_c::LoadFile(gBombkingModelFile);
    mBlendAnmModel.SetFile(f, 1, 1);
    mShadowModel.InitCylinder();
    dExtFrameCtrl_c::LoadFile(gBombkingGetupAnimationFile);
    dExtFrameCtrl_c::LoadFile(gBombkingDamageAnimationFile);
    dExtFrameCtrl_c::LoadFile(gBombkingJumpAnimationFile);
    dExtFrameCtrl_c::LoadFile(gBombkingJumpedAnimationFile);
    dExtFrameCtrl_c::LoadFile(gBombkingLiftAnimationFile);
    dExtFrameCtrl_c::LoadFile(gBombkingStruggleAnimationFile);
    dExtFrameCtrl_c::LoadFile(gBombkingThrowAnimationFile);
    dExtFrameCtrl_c::LoadFile(gBombkingWait1AnimationFile);
    dExtFrameCtrl_c::LoadFile(gBombkingWait2AnimationFile);
    dExtFrameCtrl_c::LoadFile(gBombkingWalk1AnimationFile);
    dExtFrameCtrl_c::LoadFile(gBombkingWalk2AnimationFile);
    dExtFrameCtrl_c::LoadFile(gBombkingStartAnimationFile);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    v0.x = data_ov078_02126e00.x;
    v0.y = data_ov078_02126e00.y;
    v0.z = data_ov078_02126e00.z;
    mdCcAcPos_c.Init(this, v0, 0x78000, 0xc8000, 0x200004, 0x206000);
    v1.x = data_ov078_02126e00.x;
    v1.y = data_ov078_02126e00.y;
    v1.z = data_ov078_02126e00.z;
    mdCcAcPos_c2.Init(this, v1, 0xc8000, 0xc8000, 0x200000, 0x207000);
    unk_498 = 0x1f;
    mHomePos.x = mPosX;
    mHomePos.y = mPosY;
    mHomePos.z = mPosZ;
    mArenaPos.x = 0xb1d000;
    mArenaPos.y = 0x1060000;
    mArenaPos.z = 0xfee15000;
    mWithMeshClsn.Init(this, 0x190000, 0x190000, 0, 0);
    mWithMeshClsn.SetWaterTrackFlag();
    mAnimSpeed = 1;
    mHealth = 3;
    mStarID = param1 & 0xf;
    mStarTracked = TrackStar(mStarID, 2);
    {
    int z = 0;
    for (i = 0; i < 2; i++) {
        mSpawnedId[i] = z;
        mSpawnedThrown[i] = (unsigned char)z;
    }
    }
    mPhase = ((unsigned int)RandomIntInternal(&data_0209e650.value) >> 0x1e) & 1;
    mPhase = mPhase + 1;
    mInitAngleY = mAngleY;
    KingBobOmb_SetState(&data_ov078_0212710c);
    return 1;
}


/* -------------------------------------------------------------------------- */
/* Egg-aim query. Returns the fixed distance the slot stores. */
s32 daBombking_c::OnAimedAtWithEgg() {
    return 1024000;
}

/* -------------------------------------------------------------------------- */
/* Registry factory for the BOMBKING profile. */
extern "C" daBombking_c *daBombking_c_classInit(void)
{
    return new daBombking_c();
}

extern "C" SpawnInfo<daBombking_c> g_profile_BOMBKING = {
    daBombking_c_classInit,
    0x00bd, /* BOMBKING registry id 189 */
    0x002c,
    0x10000003,
    0x00000000,
    0x00190000,
    0x01fa0000,
    0x01fa0000,
};
