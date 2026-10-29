//cpp
/* daBombking_c -- King Bob-omb (BOMBKING), Bob-omb Battlefield.
 * The destructor is out of line and first. defer_codegen off emits D1 then
 * D0 at the head, so source order is ROM order. common.h is first so the
 * shadow and hold copies stay flat 12-word moves through M12.
 * opt_propagation is off around func_ov078_021240a0. opt_strength_reduction
 * and opt_common_subs are off around func_ov078_02125448. InitResources
 * has opt_strength_reduction off. Each pragma is push/pop on that member.
 */
/* leftovers: the held matrix pointer at actor+0xc8 is dActor_c::mCarryMatrix.
 * func_ov078_02125de0 stores the king's own as the member; the stores into
 * the held actor (there and in the throw helper) and the reads in Render and
 * Behavior still use the raw offset on that other actor.
 * func_02035550 is the call
 * that ORs 0x4000 into dBgCh_Actr::mFlags; inlining the or deletes the call.
 * func_02012694 plays bank 3 at mCamSpacePos. It is not Sound::PlayBank3
 * (that body is 0x02012664). func_0200d8c8 shakes the camera.
 * func_ov002_020db54c launches the grabbed player; func_ov002_020db5f4
 * starts the hold state. func_ov102_0214b384 arms a bob-omb fuse;
 * func_ov102_0214ad14 makes it chase. GetHealth in func_ov078_02123f1c is
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
 * matrices through M12; only the flat spelling reproduces the block move. */
#include "common.h"
#include "daBombking_c.h"
#include "SpawnInfo.h"
#include "daBmb_c.h"
#include "SharedFilePtr.h"
#include "dBgCh_Gnd.h"
#include "decl_common.h"
#include "decl_Message.h"
#include "Player.h"
#include "dCamera_c.h"
#include "Message.h"
#include "nitro/fx/fx.h"
#include "Sound.h"
#include "math/Approach.h"
namespace cstd {
s16 atan2(s32, s32);
}


/* Remaining reconstruction views. Reconciled by hand against include/: every type
 * the real headers already define (Vector3, Matrix4x3, Fix12<int>, u8/u16/s16,
 * dActor_c, fBase_c, Player, BMD_File, dExtBlendAnmModel_c) was DROPPED here -- the
 * generated preamble redefined all of them. The shapes below are surviving
 * views from the legacy shards, not evidence of original local classes. */

/* File-local flat matrix. math/Matrix.h spells Matrix4x3 as {Matrix3x3 r;
 * Vector3 t;} while common.h spells it flat; whole-matrix assignment must stay
 * flat or mwcc splits it into ldm/stm + a CSE'd tail. */
struct M12 { int w[12]; };

struct Vec3 { int x, y, z; };

/* Whole-struct 3-word copies go through an array member: mwccarm block-moves
 * (ldm/stm) a POD struct copy under -lang c99 but scalarises the same copy
 * under -lang c++, and an array member forces the block move back. Two sites
 * still need it; the snapshots in func_ov078_021240a0 and func_ov078_02125448
 * initialize the POD Vec3 directly instead. */
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

/* Carry offsets for the held player: func_ov078_02125f8c uses the second
 * when Player::param1 is 2. */
Vector3 data_ov078_0212711c[2] = {
    Vector3(0x2c000, -0x19000, 0x34000),
    Vector3(0x44000, -0x19000, 0x34000),
};

/* Offset of both collision spheres from the king's position, read
 * field by field in InitResources and Behavior. Initialized data, not
 * built by the initializer. */
Vec3 data_ov078_02126e00 = {0, 0x3c000, 0};

/* State records: {enter, tick}. KingBobOmb_SetState runs the first and
 * Behavior the second. The handlers keep their address names, so the
 * records keep theirs. */
PMF data_ov078_0212710c[2] = {&daBombking_c::func_ov078_02125bc8,
                              &daBombking_c::func_ov078_021259ec};
PMF data_ov078_0212700c[2] = {&daBombking_c::func_ov078_021259e4,
                              &daBombking_c::func_ov078_02125950};
PMF data_ov078_0212701c[2] = {&daBombking_c::func_ov078_021258e4,
                              &daBombking_c::func_ov078_02125790};
PMF data_ov078_0212703c[2] = {&daBombking_c::func_ov078_02125734,
                              &daBombking_c::func_ov078_02125448};
PMF data_ov078_0212704c[2] = {&daBombking_c::func_ov078_02125350,
                              &daBombking_c::func_ov078_021250f8};
PMF data_ov078_0212707c[2] = {&daBombking_c::func_ov078_021250d0,
                              &daBombking_c::func_ov078_02124f28};
PMF data_ov078_0212708c[2] = {&daBombking_c::func_ov078_02124e9c,
                              &daBombking_c::func_ov078_02124cf4};
PMF data_ov078_021270ac[2] = {&daBombking_c::func_ov078_02124c94,
                              &daBombking_c::func_ov078_02124bc4};
PMF data_ov078_021270bc[2] = {&daBombking_c::func_ov078_02124b40,
                              &daBombking_c::func_ov078_021247bc};
PMF data_ov078_021270dc[2] = {&daBombking_c::func_ov078_02124778,
                              &daBombking_c::func_ov078_02124520};
PMF data_ov078_021270ec[2] = {&daBombking_c::func_ov078_021244d0,
                              &daBombking_c::func_ov078_02124470};
PMF data_ov078_02126ffc[2] = {&daBombking_c::func_ov078_021243c0,
                              &daBombking_c::func_ov078_021240a0};
PMF data_ov078_0212702c[2] = {&daBombking_c::func_ov078_02124060,
                              &daBombking_c::func_ov078_02124000};
PMF data_ov078_0212706c[2] = {&daBombking_c::func_ov078_02123fb4,
                              &daBombking_c::func_ov078_02123f1c};
PMF data_ov078_0212709c[2] = {&daBombking_c::func_ov078_02123eb8,
                              &daBombking_c::func_ov078_02123d3c};
PMF data_ov078_021270cc[2] = {&daBombking_c::func_ov078_02123cf0,
                              &daBombking_c::func_ov078_02123c20};
PMF data_ov078_021270fc[2] = {&daBombking_c::func_ov078_02123bc4,
                              &daBombking_c::func_ov078_02123aa0};
PMF data_ov078_0212705c[2] = {&daBombking_c::func_ov078_02123a3c,
                              &daBombking_c::func_ov078_021238ac};

extern "C" {
/* Canonical single declaration per ROM data symbol -- the generated
     * preamble carried up to three contradictory spellings of each. */
extern unsigned char data_0209f220[];
extern void *data_0209f318;
extern struct M12 data_020a0e68;
extern int data_02092138;
extern void MulMat4x3Mat4x3(void *dst, void *a, void *b);
extern void Vec3_Lsl(void *d, void *s, int sh);
extern void func_02012694(int a, void *p);
namespace Particle {
struct System {
    static void NewSimple(unsigned int id, int x, int y, int z);
};
}
extern void func_02011cfc(void);
}
extern "C" {
extern short Vec3_HorzAngle(const void* a, const void* b);
extern int Vec3_Dist(const void* a, const void* b);
extern void ApproachAngle(void *p, int target, int a, int b, int c);
extern int RandomIntInternal(int* seed);
extern int data_0209e650;
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void MulVec3Mat4x3(Vector3 *v, void *m, Vector3 *out);
extern short Vec3_VertAngle(const void *a, const void *b);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, int angle);
extern void func_0200fa8c(void *c, int a);
extern void func_ov102_0214b384(void* a, int b);
extern int atan2_hold_d_a_bombking_1(int a, int b);
extern int Vec3_HorzLen(void* v);
extern int func_ov002_020db5f4(Player *c, dActor_c *arg);
extern void func_0200d8c8(void* cam, void* v, int strength);
extern Matrix4x3 IDENTITY_MATRIX4X3;
extern void Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
}

/* `opt_strength_reduction`, `opt_common_subs` and `opt_propagation` are
 * file-global last-wins in mwccarm 2004/b56, so the three members that need
 * them would otherwise recompile the other 48. `defer_codegen off` makes the
 * positional push/pop brackets below actually bind -- and it also flips .text
 * emission from reversed to source order, which is why this file is written
 * ROM-ASCENDING. The two are one decision: linkcheck [4b/8] refuses the mix.
 */
#pragma defer_codegen off

/* Out of line, and FIRST in this ROM-ascending file: that is what puts D1 and
 * D0 at 0x02123740 / 0x02123798, the two lowest addresses in the run. */
daBombking_c::~daBombking_c() {}

// @symbol _ZN12daBombking_cD1Ev
// @symbol _ZN12daBombking_cD0Ev
// @symbol _ZN12daBombking_c19func_ov078_02123804Ev

int daBombking_c::func_ov078_02123804()
{
    unsigned int v = this->mdCcAcPos_c2.otherOwner;
    if (v == 0) return 0;
    if (dActor_c::FindWithID(v) == 0) return 0;
    if ((this->mdCcAcPos_c2.hitFlags & 0x4000) == 0) return 0;
    KingBobOmb_SetState(data_ov078_02126ffc);
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02123864Ev

void daBombking_c::func_ov078_02123864() {
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
// @symbol _ZN12daBombking_c19func_ov078_021238acEv

int daBombking_c::func_ov078_021238ac()
{
    dCamera_c *cam = (dCamera_c *)data_0209f318;
    struct Vec3 v;
    struct Vec3 t;
    struct Vec3 u;

    if (this->mTalkingPlayer->GetTalkState() != -1) {
        cam->SetFlag_3();
        return 1;
    }

    cam->mFlags &= ~8;
    v.x = 0;
    v.y = 0;
    v.z = 0;
    data_020a0e68 = *(struct M12 *)&this->mBlendAnmModel.mat4x3;
    MulMat4x3Mat4x3(&this->mBlendAnmModel.data.transforms[1], &data_020a0e68, &data_020a0e68);

    v.x = data_020a0e68.w[9];
    v.y = data_020a0e68.w[10];
    v.z = data_020a0e68.w[11];
    Vec3_Lsl(&t, &v, 3);
    v.x = t.x;
    v.y = t.y;
    v.z = t.z;
    func_02012694(0x130, &this->mCamSpacePosX);

    this->UntrackAndSpawnStar(*(s8 *)&this->mStarTracked, this->mStarID, *(Vector3 *)&v, 4);
    Particle::System::NewSimple(0x1a, v.x, v.y, v.z);
    u.x = v.x;
    u.y = v.y;
    u.z = v.z;
    this->TriplePoofDustAt(*(Vector3 *)&u);

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
// @symbol _ZN12daBombking_c19func_ov078_02123a3cEv

int daBombking_c::func_ov078_02123a3c()
{
    this->mAnimSpeed = 2;
    this->mHorzSpeed = 0;
    this->mStateTimer = 0x32;
    func_ov078_02123864();
    this->mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingDamageAnimationFile.filePtr, 0, 0, 0x1000,
                                 0);
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02123aa0Ev

int daBombking_c::func_ov078_02123aa0()
{
    short ang = Vec3_HorzAngle(&this->mPosX, &this->mHomePosX);
    if (func_ov078_02123804() == 1) return 1;
    Player *p = this->ClosestPlayer();
    if (p != 0) {
        struct Vec3 v = *(struct Vec3 *)&p->mPosX;
        if (Vec3_Dist(&this->mArenaPosX, &v) < 0x640000) {
            if (this->mArenaPosY - 0x64000 < v.y) {
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
    if (Vec3_Dist(&this->mPosX, &this->mHomePosX) < 0x32000) {
        KingBobOmb_SetState(data_ov078_0212710c);
    }
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02123bc4Ev
/* This caller retains the reconstructed SetAnim ABI declaration above.
   The callee's Fix12 definition experiment in notes/mwccarm-codegen.md 6az
   does not establish that a typed call here would fail to match. */
int daBombking_c::func_ov078_02123bc4()
{
    this->mVertAccel = -0x2000;
    this->mAnimSpeed = 2;
    this->mHorzSpeed = 0xa000;
    this->mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingWalk1AnimationFile.filePtr, 4, 0, 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02123c20Ev


int daBombking_c::func_ov078_02123c20()
{
    if (func_ov078_02123804() == 1) {
        func_ov078_02123864();
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
// @symbol _ZN12daBombking_c19func_ov078_02123cf0Ev
struct BCA_File;

int daBombking_c::func_ov078_02123cf0()
{
    this->mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingThrowAnimationFile.filePtr, 0, 0x40000000,
                                 0x1000, 0);
    this->mHorzSpeed = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02123d3cEv

int daBombking_c::func_ov078_02123d3c()
{
    if (func_ov078_02123804() == 1) {
        func_ov078_02123864();
        if (this->mHeldActor != 0) {
            func_ov002_020db54c((char *)this->mHeldActor, 0, 0x50000, this->mAngleY);
            this->mTalkingPlayer = this->mHeldActor;
            this->mHeldActor = 0;
        }
        return 1;
    }

    Vector3 v;
    v.x = this->mArenaPosX;
    v.y = this->mArenaPosY;
    v.z = this->mArenaPosZ;

    if (data_0209f220[0] != 1) {
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
// @symbol _ZN12daBombking_c19func_ov078_02123eb8Ev
int daBombking_c::func_ov078_02123eb8()
{
    mVertAccel = -0x2000;
    mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingWalk2AnimationFile.filePtr, 0, 0, 0x1000, 0);
    mStateTimer = 0x32;
    mHorzSpeed = 0xa000;
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02123f1cEv
int daBombking_c::func_ov078_02123f1c()
{
    if (func_ov078_02123804() != 1) {
        if (this->mHeldActor == 0) goto L6c;
        if (this->mHeldActor->GetHealth() != 0)
            goto L6c;
    }
    func_ov078_02123864();
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
// @symbol _ZN12daBombking_c19func_ov078_02123fb4Ev

int daBombking_c::func_ov078_02123fb4()
{
    this->mVertAccel = -0x2000;
    this->mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingLiftAnimationFile.filePtr, 0, 0x40000000,
                                 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02124000Ev

int daBombking_c::func_ov078_02124000()
{
    int ang = this->HorzAngleToCPlayer();
    ApproachAngle(&this->mPrevAngleY, ang, 1, 0x500, 0x500);
    this->mAngleY = this->mPrevAngleY;
    if (((dExtFrameCtrl_c *)&this->mBlendAnmModel)->Finished()) {
        KingBobOmb_SetState(data_ov078_0212703c);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02124060Ev

int daBombking_c::func_ov078_02124060()
{
    this->mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingGetupAnimationFile.filePtr, 0, 0x40000000,
                                 0x1000, 0);
    return 1;
}


#pragma push
#pragma opt_propagation off
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_021240a0Ev
int daBombking_c::func_ov078_021240a0() {
    char *c = (char *)this;
    daBombking_c *k = (daBombking_c *)c;
    short msg;
    int lim;

    if (k->mWithMeshClsn.IsOnGround() != 0) {
        if (k->mHealth <= 0) {
            if (k->mHorzSpeed != 0) {
                k->mHorzSpeed = 0;
                k->func_ov078_02125c24(0x7d0000);
            }
            if ((unsigned short)(k->mTalkingPlayer->mStateFlags & 0x800) != 0)
                return 1;

            ApproachAngle(&k->mPrevAngleY, k->HorzAngleToCPlayer(), 5, 0x1000, 0x200);
            k->mAngleY = k->mPrevAngleY;
            if (AngleDiff(k->HorzAngleToCPlayer(), k->mAngleY) < 0x1000) {
                Player *pl = k->mTalkingPlayer;
                if (pl->StartTalk(*(fBase_c *)c, 1) != 0) {
                    msg = 0;
                    if (data_0209f220[0] == 1) {
                        msg += (short)(pl->param1 + 0x9a);
                    } else {
                        msg = 0x95;
                    }
                    Sound::ChangeMusicVolume(0x14, 0x15666);
                    if (pl->ShowMessage(*(fBase_c *)c, (int)msg, &k->mPos, 0, 0) != 0) {
                        func_02012694(0x12a, &k->mCamSpacePosX);
                        k->KingBobOmb_SetState(&data_ov078_0212705c);
                    }
                }
            }
            return 1;
        }

        if (k->mActionStep == 0) {
            func_02012694(0x128, &k->mCamSpacePosX);
            k->func_ov078_02125c24(0x7d0000);
            k->HugeLandingDust(1);
            k->mActionStep = 1;
        }
        if ((unsigned short)k->mStateTimer == 0) {
            k->mSpawnSlot = 0;
            k->KingBobOmb_SetState(&data_ov078_0212702c);
            return 1;
        }
        if (Vec3_Dist(&k->mArenaPosX, &k->mPosX) < 0x258000) {
            int r = RandomIntInternal(&data_0209e650);
            int d;
            r = ((unsigned)r >> 8) & 0xf;
            r <<= 0x1c;
            lim = 0x4000;
            d = r >> 0x10;
            r = -lim;
            if (d < r)
                d = r;
            else if (d > 0x4000)
                d = lim;
            {
                short *p = &k->mTargetAngY;
                *p = (short)(*p + d);
            }
        } else {
            k->mTargetAngY = Vec3_HorzAngle(&k->mPosX, &k->mArenaPosX);
        }
        k->mActionStep = 0;
        k->mVertSpeed = 0x1e000;
        k->mHorzSpeed = 0xa000;
    } else {
        if (k->mVertSpeed < 0) {
            unsigned int id = k->mdCcAcPos_c.otherOwner;
            if (id != 0) {
                Player *a = static_cast<Player *>(dActor_c::FindWithID(id));
                if (a != 0) {
                    int b = (int)(a->actorID == 0xbf);
                    if (b != 0) {
                        Vec3 v = *(const Vec3 *)&a->mPosX;
                        if (k->mPosY > v.y)
                            a->Unk_020c6a10(1);
                    }
                }
            }
        }
    }

    k->mAngleY = k->mPrevAngleY;
    ApproachAngle(&k->mPrevAngleY, k->mTargetAngY, 5, 0x1000, 0x200);
    return 1;
}

#pragma pop

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_021243c0Ev

int daBombking_c::func_ov078_021243c0()
{
    this->mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingDamageAnimationFile.filePtr, 0, 0, 0x1000,
                                 0);
    this->mStateTimer = 0xc8;
    this->mTargetAngY = Vec3_HorzAngle(&this->mPosX, &this->mArenaPosX);
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
// @symbol _ZN12daBombking_c19func_ov078_02124470Ev

int daBombking_c::func_ov078_02124470()
{
    s16 ang = this->HorzAngleToCPlayer();
    ApproachAngle(&this->mPrevAngleY, ang, 1, 0x500, 0x500);
    this->mAngleY = this->mPrevAngleY;
    if ((u16)this->mStateTimer == 0)
        KingBobOmb_SetState(&data_ov078_0212702c);
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_021244d0Ev

int daBombking_c::func_ov078_021244d0()
{
    this->mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingDamageAnimationFile.filePtr, 0, 0, 0x1000,
                                 0);
    this->mStateTimer = 0x32;
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02124520Ev

int daBombking_c::func_ov078_02124520()
{
    Vector3 a;
    Vector3 look;
    Vector3 pos;
    Vector3 in;
    Vector3 out;
    Vector3 ppos;
    Vector3 tmp;
    Vector3 dust;
    dCamera_c *cam;
    Player *player;
    int *pp;
    s16 ang;

    cam = (dCamera_c *)data_0209f318;
    if (((dExtFrameCtrl_c *)&this->mBlendAnmModel)->WillHitFrame(0x46)) {
        func_ov078_02125c24(0x7d0000);
        func_02012694(0x12c, &this->mCamSpacePosX);
        a.x = 0;
        a.y = 0;
        a.z = 0;
        data_020a0e68 = *(struct M12 *)&this->mBlendAnmModel.mat4x3;
        MulMat4x3Mat4x3(&this->mBlendAnmModel.data.transforms[6], &data_020a0e68, &data_020a0e68);
        a.x = data_020a0e68.w[9];
        a.y = data_020a0e68.w[10];
        a.z = data_020a0e68.w[11];
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
        MulVec3Mat4x3(&in, &data_020a0e68, &out);
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
// @symbol _ZN12daBombking_c19func_ov078_02124778Ev

int daBombking_c::func_ov078_02124778()
{
    this->mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingStartAnimationFile.filePtr, 8, 0x40000000,
                                 0x1000, 0);
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_021247bcEv

int daBombking_c::func_ov078_021247bc()
{
    short horz;
    short vert;
    Vec3 vA;
    Vec3 vB;
    Vec3 vC;
    Vec3 D;
    Vec3 E;

    if (this->mActionStep != 2) {
        horz = Vec3_HorzAngle(&this->mPosX, &this->mArenaPosX);
        vert = Vec3_VertAngle(&this->mPosX, &this->mArenaPosX);
        Vec3_Dist(&this->mPosX, &this->mArenaPosX);
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
        MulVec3Mat4x3((Vector3 *)&vB, &data_020a0e68, (Vector3 *)&vC);
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
        vA.y = this->mArenaPosY;
        if (Vec3_Dist(&vA, &this->mArenaPosX) > 0x640000)
            return 1;
        this->mActionStep = 1;
    }

    if (this->mWithMeshClsn.IsOnGround() != 0) {
        Player *pl = this->ClosestPlayer();
        if (pl != 0) {
            *(struct M3 *)&D = *(struct M3 *)&pl->mPosX;
            if (Vec3_Dist(&this->mArenaPosX, &D) > 0x640000 ||
                this->mArenaPosY - 0x64000 > D.y) {
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
            func_ov078_02125c24(0xfa0000);
            func_0200fa8c(this, 1);
            this->mActionStep = 2;
            this->mSpeedX = 0;
            this->mVertSpeed = 0;
            this->mSpeedZ = 0;
            return 1;
        }

        if (((dExtFrameCtrl_c *)&this->mBlendAnmModel)->Finished() != 0) {
            Player *pl2 = this->mTalkingPlayer;
            unsigned short m;
            E.x = this->mPosX;
            E.y = this->mPosY;
            E.z = this->mPosZ;
            E.y += 0xc8000;
            m = pl2->mStateFlags & 0x800;
            if (m == 0) {
                if (pl2->ShowMessage(*(fBase_c *)this, 0x94, (Vector3 *)(&E), 0, 0) != 0) {
                    ((dCamera_c *)data_0209f318)->SetFlag_3();
                    func_02012694(0x12a, &this->mCamSpacePosX);
                    KingBobOmb_SetState(&data_ov078_021270dc);
                }
            }
        }
    }

    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02124b40Ev

int daBombking_c::func_ov078_02124b40()
{
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
// @symbol _ZN12daBombking_c19func_ov078_02124bc4Ev

int daBombking_c::func_ov078_02124bc4()
{
    if ((unsigned short)this->mStateTimer != 0) return 1;
    if (*(int *)&data_02092138 > this->mPosY) {
        KingBobOmb_SetState(data_ov078_021270bc);
        return 1;
    }
    if (this->mWithMeshClsn.IsOnGround() != 0) {
        if (this->mActionStep == 0) {
            func_ov078_02125c24(0x7d0000);
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
// @symbol _ZN12daBombking_c19func_ov078_02124c94Ev

int daBombking_c::func_ov078_02124c94()
{
    this->mVertAccel = -0x2000;
    this->mVertSpeed = 0x1e000;
    this->mHorzSpeed = 0x14000;
    this->mActionStep = 0;
    this->mStateTimer = 5;
    this->mFlags = 0x10000000;
    func_ov078_02125c24(0xfa0000);
    func_0200fa8c(this, 1);
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02124cf4Ev
int daBombking_c::func_ov078_02124cf4()
{
    if (this->mWithMeshClsn.IsOnGround() == 0) goto done;
    this->mHorzSpeed = 0;
    if ((int)(this->mArenaPosY - 0x28000) > this->mPosY) {
        func_02012694(0x128, &this->mCamSpacePosX);
        KingBobOmb_SetState(&data_ov078_021270ac);
        goto done;
    }
    if (this->mActionStep == 0) {
        func_02012694(0x128, &this->mCamSpacePosX);
        func_02012694(0x12b, &this->mCamSpacePosX);
        func_ov078_02125c24(0x7d0000);
        func_0200fa8c(this, 1);
        this->mActionStep = 1;
        this->mHealth -= 1;
    }
    if (this->mHealth > 0) {
        KingBobOmb_SetState(&data_ov078_021270ec);
        goto done;
    }
    Player *other = this->mTalkingPlayer;
    if (other == 0) goto done;
    if ((unsigned short)(other->mStateFlags & 0x800) != 0) goto done;
    if (other->StartTalk(*(fBase_c *)this, 1) == 0) goto done;

    short msg = 0;
    Sound::ChangeMusicVolume(0x14, 0x15666);
    if (data_0209f220[0] == 1) {
        msg += (short)(other->param1 + 0x9a);
    } else {
        msg = 0x95;
    }

    if (other->ShowMessage(*(fBase_c *)this, (unsigned)(int)msg, &this->mPos, 0, 0) == 0)
        goto done;
    func_02012694(0x12a, &this->mCamSpacePosX);
    KingBobOmb_SetState(&data_ov078_0212705c);
done:
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02124e9cEv

int daBombking_c::func_ov078_02124e9c()
{
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
// @symbol _ZN12daBombking_c19func_ov078_02124f28Ev



int daBombking_c::func_ov078_02124f28()
{
    if (this->mActionStep == 0) {
        int b = (this->mFlags & 0x4000) != 0;
        if (b != 0) {
            this->mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingStruggleAnimationFile.filePtr, 0, 0,
                                         0x1000, 0);
            this->mActionStep = 1;
        }
    }

    {
        Player *obj = this->mHeldActor;
        if (obj == 0) {
            KingBobOmb_SetState(&data_ov078_0212708c);
            return 1;
        }
        {
            struct Vec3 copy = *(struct Vec3 *)&obj->mPosX;
            if (Vec3_Dist(&this->mArenaPosX, &copy) > 0x640000)
                goto drop;
            if (this->mArenaPosY - 0xa000 <= copy.y)
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
            Player *p = this->mHeldActor;
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
// @symbol _ZN12daBombking_c19func_ov078_021250d0Ev

int daBombking_c::func_ov078_021250d0()
{
    this->mdCcAcPos_c.flags |= 2;
    this->mVertAccel = 0;
    this->mHorzSpeed = 0;
    this->mActionStep = 0;
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_021250f8Ev

int daBombking_c::func_ov078_021250f8()
{
    struct Vector3 in, out, v[2];
    dActor_c *target;
    Player *player;

    if (func_ov078_02123804() == 1) {
        func_ov078_02123864();
        return 1;
    }

    if (this->mSpawnedThrown[this->mSpawnSlot] == 0) {
        target = dActor_c::FindWithID(this->mSpawnedId[this->mSpawnSlot]);
        if (target != 0) {
            target->mPosX = this->mThrowPosX;
            target->mPosY = this->mThrowPosY;
            target->mPosZ = this->mThrowPosZ;
            target->mPrevAngleX = this->mPrevAngleX;
            target->mPrevAngleY = this->mPrevAngleY;
            target->mPrevAngleZ = this->mPrevAngleZ;
            target->mAngleX = this->mAngleX;
            target->mAngleY = this->mAngleY;
            target->mAngleZ = this->mAngleZ;
            func_ov102_0214b384(target, 0x78);
            if (((dExtFrameCtrl_c *)&this->mBlendAnmModel)->WillHitFrame(0x13) != 0 ||
                func_ov078_02123804() == 1) {
                in.x = 0; in.y = 0; in.z = 0x28000;
                out.x = 0; out.y = 0; out.z = 0;
                v[0].x = 0; v[0].y = 0; v[0].z = 0;
                player = this->ClosestPlayer();
                if (player != 0) {
                    int *q = &player->mPosX;
                    v[1].x = q[0];
                    v[1].y = q[1];
                    v[1].z = q[2];
                    v[0].x = v[1].x - this->mThrowPosX;
                    v[0].y = v[1].y - this->mThrowPosY;
                    v[0].z = v[1].z - this->mThrowPosZ;
                    Matrix4x3_FromRotationY(&data_020a0e68, cstd::atan2(v[0].x, v[0].z));
                    Matrix4x3_ApplyInPlaceToRotationX(
                        &data_020a0e68, (short)(-cstd::atan2(v[0].y, Vec3_HorzLen(&v[0]))));
                    out.y += 0x14000;
                    func_ov102_0214ad14(target);
                    target->mSpeedX = out.x;
                    target->mVertSpeed = out.y;
                    target->mSpeedZ = out.z;
                    target->mHorzSpeed = 0x14000;
                    target->mVertAccel = -0x2000;
                    MulVec3Mat4x3(&in, &data_020a0e68, &out);
                }
                this->mSpawnedThrown[this->mSpawnSlot] = 1;
                target->mCarryMatrix = 0;
            }
        }
    } else {
        if (func_ov078_02123804() == 1) {
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
// @symbol _ZN12daBombking_c19func_ov078_02125350Ev

int daBombking_c::func_ov078_02125350()
{
    int i;
    int r;

    this->mAnimSpeed = 1;
    this->mVertAccel = -0x2000;
    this->mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingThrowAnimationFile.filePtr, 0, 0x40000000,
                                 0x1000, 0);

    this->mHorzSpeed = 0;

    for (i = 0; i < this->mPhase; i++) {
        if (this->mSpawnedId[i] == 0) {
            r = (int)dActor_c::Spawn(0xce, 2, *(Vector3 *)(&this->mThrowPosX), 0, this->mAreaId, -1);
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


#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02125448Ev

int daBombking_c::func_ov078_02125448()
{
    Player *p;
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
    if (func_ov078_02123804() == 1) {
        func_ov078_02123864();
        return 1;
    }

    p = this->ClosestPlayer();
    if (p != 0) {
        Vec3 v = *(const Vec3 *)&p->mPosX;
        if (Vec3_Dist(&this->mArenaPosX, &v) > 0x640000
            || (this->mArenaPosY - 0xa000) > v.y) {
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
                    Player *player = static_cast<Player *>(a);
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
            if (data_0209f220[0] == 1) {
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


#pragma pop

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02125734Ev

int daBombking_c::func_ov078_02125734()
{
    this->mAnimSpeed = 1;
    this->mHorzSpeed = 0x5000;
    this->mVertAccel = -0x2000;
    this->mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingWalk1AnimationFile.filePtr, 8, 0, 0x1000, 0);
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02125790Ev
int daBombking_c::func_ov078_02125790()
{
    Vector3 s;
    Vector3 d;
    Vector3 v;
    if (func_ov078_02123804() == 1)
        return 1;
    ApproachAngle(&this->mPrevAngleY, this->HorzAngleToCPlayer(), 1, 0x500, 0x500);
    this->mAngleY = this->mPrevAngleY;
    if (((dExtFrameCtrl_c *)&this->mBlendAnmModel)->WillHitFrame(0x46)) {
        func_ov078_02125c24(0x7d0000);
        func_02012694(0x12c, &this->mCamSpacePosX);
        s.x = 0;
        s.y = 0;
        s.z = 0;
        data_020a0e68 = *(struct M12 *)&this->mBlendAnmModel.mat4x3;
        MulMat4x3Mat4x3(&this->mBlendAnmModel.data.transforms[6], &data_020a0e68, &data_020a0e68);
        s.x = data_020a0e68.w[9];
        s.y = data_020a0e68.w[10];
        s.z = data_020a0e68.w[11];
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
// @symbol _ZN12daBombking_c19func_ov078_021258e4Ev

int daBombking_c::func_ov078_021258e4()
{
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
// @symbol _ZN12daBombking_c19func_ov078_02125950Ev
int daBombking_c::func_ov078_02125950()
{
    Player *target = this->mTalkingPlayer;
    int *src = &target->mPosX;
    Vector3 v;
    int t = src[0];
    v.x = t;
    v.y = src[1];
    v.z = src[2];
    short ang = Vec3_HorzAngle(&this->mPosX, &v);
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
// @symbol _ZN12daBombking_c19func_ov078_021259e4Ev

int daBombking_c::func_ov078_021259e4()
{
    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_021259ecEv

int daBombking_c::func_ov078_021259ec()
{
    int dist;
    struct Vector3 ppos;
    struct Vector3 v;
    Player *player;
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

    if (func_ov078_02123804() == 1) {
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

                if (this->mTalkingPlayer->StartTalk(*(fBase_c *)this, 1)) {
                    Message::PrepareTalk();
                    b = (data_0209f220[0] != 1) ? 0x93 : (unsigned int)(short)(player->param1 + 0x96);
                    if (player->ShowMessage(*(fBase_c *)this, b, (Vector3 *)(&v), 0, 0)) {
                        func_02012694(0x12a, &this->mCamSpacePosX);
                        this->mIntroTalked = 1;
                        KingBobOmb_SetState(&data_ov078_0212700c);
                    }
                }
            }
        } else {
            dist = Vec3_Dist(&this->mPos, &ppos);
            if (dist < 0x258000) {
                if (this->mArenaPosY - 0xa000 < ppos.y) {
                    KingBobOmb_SetState(&data_ov078_0212701c);
                }
            }
        }
    }

    return 1;
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02125bc8Ev
int daBombking_c::func_ov078_02125bc8() {
    this->mHorzSpeed = 0;
    this->mAnimSpeed = 1;
    this->mVertAccel = -0x2000;
    this->mMusicLayer3Timer = 0;
    this->mBlendAnmModel.SetAnim(*(BCA_File *)gBombkingWait2AnimationFile.filePtr, 8, 0, 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02125c24Ei
void daBombking_c::func_ov078_02125c24(int strength) {
    func_0200d8c8(data_0209f318, &this->mPosX, strength);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19KingBobOmb_SetStateEPv
int daBombking_c::KingBobOmb_SetState(void *pv) {
    mState = (PMF *)pv;
    PMF *q = mState;
    if (*q == 0) return 1;
    return (this->**q)();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02125c98Ev
void daBombking_c::func_ov078_02125c98() {
  int h = mPosY;
  if (this->mWithMeshClsn.IsOnGround() == 0) {
    dBgCh_Gnd rg;
    rg.SetObjAndPos(this->mPos, 0);
    if (rg.DetectClsn() != 0)
      h = rg.clsnY;
  }
  int b = (this->mFlags & 0x4000) != 0;
  if (b) {
    Player *p = this->mHeldActor;
    if (p != 0)
      h = p->mPosY;
  }
  int ip = this->mPosY - h;
  if (ip <= 0x1000)
    ip = 0x1000;
  int scale = 0x15e000 - FX_MUL(ip, 0x180);
  if (scale < 0xa000)
    scale = 0xa000;
  *(struct M12 *)this->mShadowMtx = *(struct M12 *)&IDENTITY_MATRIX4X3;
  this->mShadowMtx[9] = this->mPosX >> 3;
  this->mShadowMtx[10] = this->mPosY >> 3;
  this->mShadowMtx[11] = this->mPosZ >> 3;
  DropShadowRadHeight(this->mShadowModel, *(Matrix4x3 *)this->mShadowMtx, scale, ip + 0x28000, 0xf);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02125de0Ev
void daBombking_c::func_ov078_02125de0()
{
    struct Vector3 lv;
    int i;

    Matrix4x3_FromRotationY(&this->mBlendAnmModel.mat4x3, this->mAngleY);
    this->mBlendAnmModel.mat4x3.m[9] = this->mPosX >> 3;
    this->mBlendAnmModel.mat4x3.m[10] = this->mPosY >> 3;
    this->mBlendAnmModel.mat4x3.m[11] = this->mPosZ >> 3;
    MulMat4x3Mat4x3(&this->mBlendAnmModel.data.transforms[10], &this->mBlendAnmModel.mat4x3,
                    this->mHoldMtx);

    /* Held-matrix pointer: dActor_c's mCarryMatrix. */
    this->mCarryMatrix = (Matrix4x3 *)this->mHoldMtx;
    this->mThrowPosX = 0;
    this->mThrowPosY = 0;
    this->mThrowPosZ = 0;
    this->mThrowPosX = this->mHoldMtx[9];
    this->mThrowPosY = this->mHoldMtx[10];
    this->mThrowPosZ = this->mHoldMtx[11];
    Vec3_Lsl(&lv, &this->mThrowPosX, 3);
    this->mThrowPosX = lv.x;
    this->mThrowPosY = lv.y;
    this->mThrowPosZ = lv.z;

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

        data_020a0e68 = *(struct M12 *)this->mHoldMtx;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0xb000, -0x7000, -0x6000);
        Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, 0x4000, -0x1000, 0x2000);
        *(struct M12 *)this->mHoldMtx = data_020a0e68;
        ((dActor_c *)actor)->mCarryMatrix = (struct Matrix4x3 *)this->mHoldMtx;
    }
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c19func_ov078_02125f8cEv


void daBombking_c::func_ov078_02125f8c(){
    Player *player = mHeldActor;
    int idx = 0;
    if (player == 0) return;
    if (player->param1 == 2) idx = 1;
    Matrix4x3 *res = this->UpdateCarry(*player, data_ov078_0212711c[idx]);
    *(struct M12 *)&this->mBlendAnmModel.mat4x3 = *(struct M12 *)res;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c16CleanupResourcesEv
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
// @symbol _ZN12daBombking_c16OnPendingDestroyEv
void daBombking_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c6RenderEv
int daBombking_c::Render()
{
    void *held = mHeldActor;
    if (held != 0) {
        int flags = mFlags;
        int flag = (flags & 0x4000) ? 1 : 0;
        if (flag != 0) {
            if (((dActor_c *)held)->mCarryMatrix != 0) {
                func_ov078_02125f8c();
            }
        }
    }
    mBlendAnmModel.dExtSimpleModel_c::Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c8BehaviorEv


extern "C" {
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
}

int daBombking_c::Behavior()
{
    if (DistToCPlayer() < 0x1770000) {
        ((dCamera_c *)data_0209f318)->mFramedActor = this;
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
        func_ov078_02125de0();
    skip_de0:
        func_ov078_02125c98();
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
        if (mWithMeshClsn.IsOnWall() != 0
            || mWithMeshClsn.IsOnGround() == 0
            || (mArenaPosY - 0x28000) > mPosY) {
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

    func_ov078_02125de0();
    func_ov078_02125c98();
    return 1;
}

#pragma push
#pragma opt_strength_reduction off
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c13InitResourcesEv
/* SharedFilePtr's complete declaration is included above. The file handles
   below still use their existing ROM-backed data declarations. */
int daBombking_c::InitResources()
{
    BMD_File *f;
    Vector3 v0;
    Vector3 v1;
    int i;
    f = (BMD_File *)dExtSimpleModel_c::LoadFile(gBombkingModelFile);
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
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mArenaPosX = 0xb1d000;
    mArenaPosY = 0x1060000;
    mArenaPosZ = 0xfee15000;
    mWithMeshClsn.Init(this, 0x190000, 0x190000, 0, 0);
    func_02035550(&mWithMeshClsn);
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
    mPhase = ((unsigned int)RandomIntInternal(&data_0209e650) >> 0x1e) & 1;
    mPhase = mPhase + 1;
    mInitAngleY = mAngleY;
    KingBobOmb_SetState(&data_ov078_0212710c);
    return 1;
}

#pragma pop

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c16OnAimedAtWithEggEv
/* Egg-aim query. Returns the fixed distance the slot stores. */
s32 daBombking_c::OnAimedAtWithEgg() {
    return 1024000;
}

/* -------------------------------------------------------------------------- */
// @symbol daBombking_c_classInit
/* Registry factory for the BOMBKING profile. */
extern "C" daBombking_c *daBombking_c_classInit(void)
{
    return new daBombking_c();
}

// @symbol g_profile_BOMBKING
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
