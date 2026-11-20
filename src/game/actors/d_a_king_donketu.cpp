//cpp
/**
 * daKing_Donketu_c -- Chief Chilly (KING_DONKETU 218), ov073.
 *
 * Snowman's Land's ice-bully boss, derived from dEnemyBase_c (ROM RTTI; not
 * from daOts_c). Sixteen file-scope state records drive it; mState holds the
 * address of the current one, ChiefChilly_ChangeState stores it and runs its
 * enter member, and Behavior runs its update member every frame.
 *
 * THE STATE RECORDS. Each record is two pointers-to-member, {enter, update},
 * filled in by ov073's sinit (__sinit_ov073_02122874). The pairs below were
 * read off that sinit; the descriptions are what the handlers do, and the
 * labels are mine -- the cartridge's names for these states are not recovered.
 * (fx32: 0x1000 = 1.0; angles: 0x10000 = a full turn.)
 *
 *   record     enter     update    what the pair does
 *   ---------  --------  --------  ---------------------------------------
 *   ..3330     ..1538    ..1388    opening talk: the player is addressed,
 *                                  message (param1 + 0xe3) is shown. The
 *                                  state InitResources starts in.
 *   ..3350     ..1378    ..128c    opening talk, camera held until the
 *                                  player's talk state reads -1.
 *   ..3360     ..122c    ..0ed0    the main chase: charge at the player,
 *                                  brake, turn; hits are checked here (and in
 *                                  ..3390).
 *   ..3390     ..0e60    ..0dec    turn back toward mSpawnPos (entered from
 *                                  the chase when the ground ray misses).
 *   ..33a0     ..0d80    ..0c7c    hit reaction: slide to a stop; when the
 *                                  ground ray misses, mHitsRemaining drops.
 *   ..33c0     ..0c08    ..0b78    leap (20.0 out, 30.0 up) until the boss is
 *                                  below mSpawnPos.y and on the ground, then
 *                                  face mSpawnPos and go to ..3400.
 *   ..3400     ..081c    ..0610    waypoint hop whose landing sets mJumpCount
 *                                  to 1 and returns to ..3320.
 *   ..3320     ..05f0    ..03ac    waypoint hops, counting landings.
 *   ..3340     ..0390    ..00e0    leap toward mSpawnPos; while falling it
 *                                  calls daPly_c::Unk_020c6a10 on the player
 *                                  touching it if the boss is the higher.
 *   ..3380     ..0098    ..005c    pose until the animation finishes, then
 *                                  back to the chase.
 *   ..33d0     ..0ad8    ..0910    wobble (mAngle.x) while moving toward a
 *                                  point 120.0 past the position where the
 *                                  ground ray last missed; when
 *                                  mStateTimer (0x5a frames) runs out it sets
 *                                  mHitsRemaining to 1 and goes to ..3340; a
 *                                  nonzero 0211f61c (a hit, or the
 *                                  boss hurting the player) goes to ..33f0.
 *   ..33f0     ..08e4    ..0844    leap, tilting mAngle.x toward -0x4000, until
 *                                  it is below mSpawnPos.y and on the ground,
 *                                  then ..33b0.
 *   ..33b0     ..000c    ..fe8c    cut scene, part 1: StartTalk the player,
 *                                  camera on the boss, until mStateTimer (0x32
 *                                  frames) runs out.
 *   ..33e0     ..fe84    ..fc78    cut scene, part 2: message (param1 + 0xe7).
 *   ..3410     ..fc70    ..fbf4    cut scene, part 3: camera held until the
 *                                  talk state reads -1.
 *   ..3370     ..fbec    ..fa74    defeat: shrink, spawn the key, vanish.
 *
 * (Addresses are data_ov073_0212xxxx and func_ov073_0211/0212xxxx, with the
 * last four hex digits shown; ..fe84 is 0211fe84 and so on.) The three
 * enter handlers fbec / fc70 / fe84 are empty.
 *
 * mHitsRemaining starts at 3 and is decremented by the hit-reaction state
 * (..33a0). The states that end in the defeat chain together as
 * ..33d0 -> ..33f0 -> ..33b0 -> ..33e0 -> ..3410 -> ..3370, each hop being a
 * transition its update handler makes. There are two waypoint sets and a
 * ground-ray guard in Behavior.
 */

/* leftovers:
 * - defer_codegen off stays file-global: deferred codegen emits D2, D0, D1
 *   and the cartridge has D1 then D0. opt_loop_invariants off stays
 *   file-global for HandleHit.
 * - struct C is a complete PMF shadow on purpose (mwccarm picks the
 *   pointer-to-member shape from completeness). ChiefChilly_ChangeState
 *   takes C*. The sixteen state labels are descriptions; the sinit owns
 *   the records. The header's 0x504 size wrap stays.
 * - Vec3_* / Matrix4x3_* have no shared header this TU
 *   can include alone. gCamera, gScratchMtx and data_0209e650 are arm9
 *   globals. gScaleUpKinokoModelFile is ov002's; Cleanup only Releases it.
 *   g_profile_KING_DONKETU is outside this TU.
 * - Behavior's opt_propagation off is a push/pop around that function.
 * - void* cam in UpdateDefeat / 0212128c / 02121388: a dCamera_c*
 *   swaps two registers. Those functions also ldrh mStateTimer and
 *   mAngle.x/.y/.z through unsigned short; a plain s16 read is ldrsh.
 * - s16* behind mPrevAngle.y in HandleHit and 02120d80, the
 *   const s32* walk of mPos.x, and mFootPos[i] in the dust loops stay. The
 *   direct member forms change addressing.
 * - SpawnLandingEffects's six Vector3 locals are declared up front in
 *   frame order (bone, spawn, offset, rotated, shifted, dust). Declared
 *   inside the state branch, offset/rotated land above shifted/dust in the
 *   frame. Vector3's user-declared dtor is what keeps the dead stores
 *   (bonePos = shifted, the zeroed offset.z before the -230.0 store).
 * - mEdgePos, mLandedWaypoint, mEdgeRecovered, mStateParam and mFootPos are
 *   named from their readers in this TU and d_a_obj_ewb_ice.cpp.
 *   The hit masks in HandleHit stay literals (0x10000 is not in
 *   dCc_c.h's bit table). Sound ids are named for the site that plays them.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daKing_Donketu_c_AssetId {
    ASSET_KING_ICE_DONKETU_DAMAGE_BCA = 0x0354,
    ASSET_KING_ICE_DONKETU_DEAD_BCA = 0x0355,
    ASSET_KING_ICE_DONKETU_HIT_BCA = 0x0356,
    ASSET_KING_ICE_DONKETU_JUMP_ED_BCA = 0x0357,
    ASSET_KING_ICE_DONKETU_JUMP_ST_BCA = 0x0358,
    ASSET_KING_ICE_DONKETU_MODEL_BMD = 0x0359,
    ASSET_KING_ICE_DONKETU_WAIT_BCA = 0x035a,
    ASSET_KING_ICE_DONKETU_WALK_BCA = 0x035b
};

#pragma defer_codegen off

#include "common.h"
#include "daKing_Donketu_c.h"
#include "decl_common.h"
#include "types.h"
#include "SharedFilePtr.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "SaveData.h"
#include "Message.h"
#include "daPly_c.h"
#include "dCamera_c.h"
#include "dBgCh_Lin.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "Sound.h"
#include "cstd.h"
#include "math/Approach.h"
#include "LevelRng.h"

/* Actor ids (symbols/actor_debug_names.tsv gives them in decimal) that this
   boss looks up or spawns. */
enum KingDonketuActorID {
    ACTOR_PLAYER = 191,           /* 0xbf */
    ACTOR_SCALEUP_KINOKO = 277,   /* 0x115 */
    ACTOR_OBJ_KEY = 282,          /* 0x11a */
    ACTOR_COIN = 288,             /* 0x120 */
    ACTOR_OBJ_BLUE_FIRE = 317     /* 0x13d */
};

/* Sound ids passed to func_02012694, which, going by its call sites here,
   plays a sound at the boss's camera-space position (mCamSpacePos). Named for the site that plays them;
   no sound table was consulted, so the meanings are the call sites', not the
   cartridge's. */
enum KingDonketuSound {
    SND_ANIM_FRAME_STOMP = 0x167,   /* Behavior: the model hits frame 7 in an opening-talk state */
    SND_CHASE_STEP = 0x168,         /* chase update: model hits frame 0 or 0xe in leg 0 */
    SND_CHASE_BRAKE = 0x169,        /* chase update: leg 0 hands over to the braking leg (leg 1) */
    SND_REVERSE_AFTER_HURT = 0x16a, /* the boss hurt the player and turned around */
    SND_HIT_REGISTERED = 0x16b, /* a hit registered; also the boss hurting the player in ..33d0 */
    SND_BIG_LANDING = 0x16c,    /* landing after a leap (states 33f0 / 33c0) */
    SND_HOP_LANDING = 0x16d,    /* landing after a waypoint hop (also the ..3340 leap) */
    SND_HOP_LAUNCH = 0x16f,     /* UpdateLeapToSpawn, frame the hop starts */
    SND_TALK_DONE = 0x12a,      /* ShowMessage finished in a talk state */
    SND_POOF = 0xbb             /* UpdateDefeat: the boss vanishes */
};

struct C;
typedef int (C::*PMF)();
struct C { char pad[0x37c]; PMF *pp; };

enum Bool { FALSE, TRUE };


/* The eight file handles __sinit_d_a_king_donketu.cpp constructs below.
 * Construct writes the u16 file ID at +0, the u8 reference count at +2,
 * and the loaded-file pointer at +4. Byte +3 is alignment padding.
 * The model payload is BMD_File; animation payloads are BCA_File.
 * CleanupResources releases the actor's references; the registered
 * handle destructors only publish the file ID through func_02017e34. The
 * model handle's ctor/dtor are the cartridge's func_02017acc / func_02017ab4
 * pair and the seven animation handles construct through
 * SharedFilePtr::Construct and register
 * SharedFilePtr_Destruct_Anim; the manifest aliases these. */
struct KingDonketuModelFileHandle : SharedFilePtr {
    KingDonketuModelFileHandle(u32 fileID);
    ~KingDonketuModelFileHandle();
};
struct KingDonketuAnimResFileHandle : SharedFilePtr {
    KingDonketuAnimResFileHandle(u32 fileID);
    ~KingDonketuAnimResFileHandle();
};

extern KingDonketuAnimResFileHandle gKingIceDonketuDeadAnimationFile; /* anim, file 0x355 */
extern KingDonketuAnimResFileHandle gKingIceDonketuWalkAnimationFile; /* anim, file 0x35b */
extern KingDonketuAnimResFileHandle gKingIceDonketuJumpEdAnimationFile; /* anim, file 0x357 */
extern KingDonketuModelFileHandle gKingIceDonketuModelModelFile;   /* model, file 0x359 */
extern KingDonketuAnimResFileHandle gKingIceDonketuHitAnimationFile; /* anim, file 0x356 */
extern KingDonketuAnimResFileHandle gKingIceDonketuDamageAnimationFile; /* anim, file 0x354 */
extern KingDonketuAnimResFileHandle gKingIceDonketuJumpStAnimationFile; /* anim, file 0x358 */
extern KingDonketuAnimResFileHandle gKingIceDonketuWaitAnimationFile; /* anim, file 0x35a */

/* The sixteen state records __sinit_d_a_king_donketu.cpp fills from the PMF
 * literals at 0x02122f40, dispatched through the struct C shadow. */
extern daKing_Donketu_c::State data_ov073_02123320;
extern daKing_Donketu_c::State data_ov073_02123330;
extern daKing_Donketu_c::State data_ov073_02123340;
extern daKing_Donketu_c::State data_ov073_02123350;
extern daKing_Donketu_c::State data_ov073_02123360;
extern daKing_Donketu_c::State data_ov073_02123370;
extern daKing_Donketu_c::State data_ov073_02123380;
extern daKing_Donketu_c::State data_ov073_02123390;
extern daKing_Donketu_c::State data_ov073_021233a0;
extern daKing_Donketu_c::State data_ov073_021233b0;
extern daKing_Donketu_c::State data_ov073_021233c0;
extern daKing_Donketu_c::State data_ov073_021233d0;
extern daKing_Donketu_c::State data_ov073_021233e0;
extern daKing_Donketu_c::State data_ov073_021233f0;
extern daKing_Donketu_c::State data_ov073_02123400;
extern daKing_Donketu_c::State data_ov073_02123410;

extern "C" {

struct BCA_File;
extern SharedFilePtr gScaleUpKinokoModelFile;
extern short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
extern void Matrix4x3_FromRotationY(Matrix4x3 *m, int angle);
extern void MTX_MultVec43(const Vector3 *in, const Matrix4x3 *m, Vector3 *out);
extern void MTX_Concat43(void *dst, void *a, void *b);
extern void Vec3_Lsl(void *d, void *s, int sh);
extern dCamera_c *gCamera;
extern union Matrix4x3 gScratchMtx;
/* Vec3_Sub is not declared by math/CalcLib.h. */
extern void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);
/* local extern: CalcLib.h clashes with this TU's C DecIfAbove0_Short and Vec3_Lsl (void *) */
extern fx32 Vec3_HorzLen(const Vector3 *v);
/* local extern: the same CalcLib.h clash; the spelling CalcLib.h declares. */
extern int Vec3_ApproachHorz(Vector3 *, Vector3 *, int);

extern u16 DecIfAbove0_Short(void* p);
/* local extern: CalcLib.h conflicts with DecIfAbove0_Short(void *). */
extern int RandomIntInternal(int *seed);
}
int ChiefChilly_ChangeState(C *c, PMF *p);
extern void func_02012694(unsigned int a, const Vector3 *b);
extern "C" {
extern void Matrix4x3_ApplyInPlaceToRotationX(union Matrix4x3 *m, short angX);
/* local extern: the Vector3 spelling math/CalcLib.h declares. This TU's
   locals are the POD VecFx32, so the call casts. */
extern void Vec3_Asr(Vector3 *d, Vector3 *s, int sh);
extern void Matrix4x3_FromTranslation(union Matrix4x3 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
}
extern s16 Vec3_VertAngle(const Vector3 *a, const Vector3 *b);
extern void UnloadKeyModels(int i);

daKing_Donketu_c::~daKing_Donketu_c()
{
}

/* The 0x77 / 0x78 effects the cut scenes and the defeat keep alive each frame. The point is
   mPos with y raised by 30.0 * the scale (mScale.x is fx32, so the 0x1e000
   constant shrinks with the boss) and x/z moved 200.0 (0xc8000) along
   mPrevAngle.y. Particle systems 0x77 and 0x78 are renewed there (ids kept in
   mParticleId0/1), and the emitRadius of each is set to 20.0 * the scale. The
   Vec3_HorzAngle result is not used. */
void daKing_Donketu_c::RenewLoopedParticles() {

    Vector3 pos, in, out;
    int y;
    pos.x = mPos.x;
    y = mPos.y;
    pos.y = y;
    pos.z = mPos.z;
    pos.y = y + FX_MUL(mScale.x, 0x1e000);

    Vec3_HorzAngle(&mPos, &mSpawnPos);
    in.z = 0; in.z = 0xc8000; in.x = 0; in.y = 0;
    out.x = 0; out.y = 0; out.z = 0;
    Matrix4x3_FromRotationY(&gScratchMtx, mPrevAngle.y);
    MTX_MultVec43(&in, &gScratchMtx, &out);
    pos.x = pos.x + out.x;
    pos.z = pos.z + out.z;
    mParticleId0 = Particle::System::New(mParticleId0, 0x77, pos.x, pos.y, pos.z, 0, 0);
    mParticleId1 = Particle::System::New(mParticleId1, 0x78, pos.x, pos.y, pos.z, 0, 0);
    if (mParticleId0 != 0) {
        Particle::System *sys = Particle::System::FromUniqueID(mParticleId0);
        if (sys != 0)
            sys->emitRadius = mScale.x * 0x14;
    }
    if (mParticleId1 == 0)
        return;
    {
        Particle::System *sys = Particle::System::FromUniqueID(mParticleId1);
        if (sys != 0)
            sys->emitRadius = mScale.x * 0x14;
    }
}

/* Called where the boss lands or steps (a reading from the callers). Shakes the camera by `strength` (dCamera_c::func_0200d8c8; callers pass
   0x3e8000 to 0x1388000), then takes the translation of bone mStepBoneIdx
   times the model matrix, shifted left by 3 (the matrix words are held >> 3),
   and flips mStepBoneIdx between 2 and 3 for the next call. In the chase
   state (..3360) that position gets HugeLandingDustAt(pos, 1). In any other
   state the bone position is computed but not used: particles 0x88 and 0x89
   go off at mPos instead, moved 230.0 back (z = -0xe6000, rotated by mAngle.y)
   when the state is ..33c0 or ..33f0. */
void daKing_Donketu_c::SpawnLandingEffects(int strength) {
    Vector3 bonePos;
    Vector3 pos;
    Vector3 offset;
    Vector3 rotated;
    Vector3 shifted;
    Vector3 dustPos;
    gCamera->func_0200d8c8(&mPos, strength);
    gScratchMtx = mBlendAnmModel.mat4x3;
    MTX_Concat43(&mBlendAnmModel.data.transforms[mStepBoneIdx], &gScratchMtx, &gScratchMtx);
    mStepBoneIdx ^= 1;
    bonePos.x = gScratchMtx.t.x;
    bonePos.y = gScratchMtx.t.y;
    bonePos.z = gScratchMtx.t.z;
    Vec3_Lsl(&shifted, &bonePos, 3);
    bonePos = shifted;
    if (mState == &data_ov073_02123360) {
        dustPos = bonePos;
        HugeLandingDustAt(dustPos, 1);
        return;
    }
    pos = mPos;
    if (mState == &data_ov073_021233c0 || mState == &data_ov073_021233f0) {
        offset.x = 0;
        offset.y = 0;
        offset.z = 0;
        offset.z = -0xe6000;
        rotated.x = 0;
        rotated.y = 0;
        rotated.z = 0;
        Matrix4x3_FromRotationY(&gScratchMtx, mAngle.y);
        MTX_MultVec43(&offset, &gScratchMtx, &rotated);
        pos.x += rotated.x;
        pos.z += rotated.z;
    }
    Particle::System::NewSimple(kPtclChiefChillyStepA, pos.x, pos.y, pos.z);
    Particle::System::NewSimple(kPtclChiefChillyStepB, pos.x, pos.y, pos.z);
}

/* Particles 0x84 and 0x85 on the surface of this actor's collision sphere,
   in the direction of b. The sphere's centre is mPos raised by
   mdCcAcPos_c.height; the aim point is b's mPos raised 70.0 (0x46000). The
   two atan2 results give a yaw and a pitch, and the radius times their sines
   and cosines gives the offset from the centre. Within this file b is always
   the boss itself. The ROM copies the difference vector once more before
   reading it (`dir = diff`) and holds &b->mPos across the copy of mPos
   (`target`); both are needed for the bytes. */
void daKing_Donketu_c::SpawnHitParticles(dActor_c *b) {
    Vector3 dir, pos, aim, diff;
    s16 yaw, pitch;
    fx32 radius, horz;

    const Vector3 &target = b->mPos;
    pos = mPos;
    aim = target;
    radius = mdCcAcPos_c.radius;
    pos.y += mdCcAcPos_c.height;
    aim.y += 0x46000;
    Vec3_Sub(&diff, &aim, &pos);
    dir = diff;
    yaw = cstd::atan2(dir.x, dir.z);
    pitch = cstd::atan2(dir.y, Vec3_HorzLen(&dir));
    horz = FX_MUL(radius, FX_CosIdx((u16)pitch >> 4));
    pos.x += FX_MUL(horz, FX_SinIdx((u16)yaw >> 4));
    pos.y += FX_MUL(radius, FX_SinIdx((u16)pitch >> 4));
    pos.z += FX_MUL(horz, FX_CosIdx((u16)yaw >> 4));
    Particle::System::NewSimple(kPtclChiefChillyHitA, pos.x, pos.y, pos.z);
    Particle::System::NewSimple(kPtclChiefChillyHitB, pos.x, pos.y, pos.z);
}

/* Collision step, called from Behavior in the chase states (..3360, ..3390)
   and from the ..33d0 update. Returns 1 when the boss took a hit, or, in
   ..33d0, when it hurt the player; otherwise 0.
     - Does nothing while mHitCooldown (set to 0x10 on a hit) counts down, or
       when mdCcAcPos_c.otherOwner is 0 or names no actor.
     - What hit it comes from mdCcAcPos_c.hitFlags, using dCc_c.h's bit table
       (a best-effort reading; only 0x2000 egg and 0x4000 explosion are
       proven):
         0x6000 (egg, explosion)     particles, mHorzSpeed = 0x20000 (32.0)
       and when the other actor is the player (actorID 191):
         mega player, and hitFlags & 0x107e0 (0x20..0x400 and 0x10000;
           0x10000 is not in the table)
                                     mHorzSpeed = 0x41000 (65.0), no particles
         else hitFlags & 0x50380 (0x80, 0x100, 0x200, 0x10000, 0x40000 fire)
                                     particles, 0x2d000 (45.0)
         else hitFlags & 0x70 (0x10, 0x20, 0x40), a metal player, or
           JumpedOnByPlayer         particles, 0x20000 (32.0)
       and, if none of those fired, hitFlags & 0x400 (dive): particles, 0x3d000
       (61.0).
     - A hit sounds SND_HIT_REGISTERED. One time in eight it also spawns 1 to
       3 coins (the count comes from the random word; 0 is read as 1) 100.0
       (0x64000) above the boss, each with mPrevAngle.y a multiple of 0x1000
       and mHorzSpeed 0xa000 (10.0). While SaveData character 2 is unlocked each
       coin counts into mCoinsSpawned and the one that takes it past 0x1e also
       spawns a SCALEUP_KINOKO and resets the count. Then the boss goes to
       ..33a0 unless it is already in ..33a0 or ..33d0.
     - With no hit, daPly_c::Hurt is tried on the other actor. If it hurt: a
       particle 0x8a at the other actor's position, and in ..33d0
       SND_HIT_REGISTERED plays and the function returns 1. Elsewhere the boss turns mPrevAngle.y by half a turn
       (0x8000), plays SND_REVERSE_AFTER_HURT, goes to ..3360 with mPhase 2,
       mStateParam 0x2000 and mStateTimer 0x1e, and the function still returns 0. */
s32 daKing_Donketu_c::HandleHit() {
    dActor_c* target;
    s32 hit;
    u32 id;

    if (DecIfAbove0_Short(&mHitCooldown) != 0)
        return 0;
    id = mdCcAcPos_c.otherOwner;
    if (id == 0)
        return 0;
    target = dActor_c::FindWithID(id);
    if (!target)
        return 0;

    hit = 0;
    if (mdCcAcPos_c.hitFlags & 0x6000) {
        SpawnHitParticles(this);
        mHorzSpeed = 0x20000;
        hit = 1;
    }

    {
        int isBf = (target->actorID == ACTOR_PLAYER);
        if (isBf == 1) {
            daPly_c *player = (daPly_c *)target;
            if (player->mIsMega != 0) {
                s32 flags = mdCcAcPos_c.hitFlags & 0x107e0;
                if (flags) {
                    mHorzSpeed = 0x41000;
                    hit = 1;
                }
            }
            if (hit == 0) {
                s32 flags = mdCcAcPos_c.hitFlags & 0x50380;
                if (flags) {
                    SpawnHitParticles(this);
                    mHorzSpeed = 0x2d000;
                    hit = 1;
                }
            }
            if (hit == 0) {
                if ((mdCcAcPos_c.hitFlags & 0x70) || (player->mIsMetal != 0) ||
                    (JumpedOnByPlayer(mdCcAcPos_c, *player) != 0)) {
                    SpawnHitParticles(this);
                    mHorzSpeed = 0x20000;
                    hit = 1;
                }
            }
        }
    }

    if (hit == 0) {
        if (mdCcAcPos_c.hitFlags & 0x400) {
            SpawnHitParticles(this);
            mHorzSpeed = 0x3d000;
            hit = 1;
        }
    }

    if (hit != 0) {
        s32 count;
        s32 i;
        s32 shortY;
        Vector3 v;
        s32 rnd;

        mHitCooldown = 16;
        func_02012694(SND_HIT_REGISTERED, &mCamSpacePos);
        rnd = RandomIntInternal(&data_0209e650.value);
        if (((rnd >> 8) & 7) == 0) {
            v.x = mPos.x;
            v.y = mPos.y;
            v.z = mPos.z;
            count = (rnd >> 0xc) & 3;
            v.y = v.y + 0x64000;
            if (count == 0)
                count = 1;
            i = 0;
            if (count > 0) {
                do {
                    dActor_c *actor = dActor_c::Spawn(ACTOR_COIN, 0, v, 0, mAreaId, -1);
                    if (actor != 0) {
                        rnd = RandomIntInternal(&data_0209e650.value);
                        shortY = ((s32)((((u32)rnd >> 8) & 0xf) << 0x1c)) >> 0x10;
                        actor->mPrevAngle.x = 0;
                        actor->mPrevAngle.y = (s16)shortY;
                        actor->mPrevAngle.z = 0;
                        actor->mHorzSpeed = 0xa000;
                        if (SaveData::IsCharacterUnlocked(2) != 0) {
                            mCoinsSpawned += 1;
                            if (mCoinsSpawned > 30) {
                                dActor_c *actor2 =
                                    dActor_c::Spawn(ACTOR_SCALEUP_KINOKO, 0, v, 0, mAreaId, -1);
                                if (actor2 != 0) {
                                    actor2->mPrevAngle.x = 0;
                                    actor2->mPrevAngle.y = (s16)shortY;
                                    actor2->mPrevAngle.z = 0;
                                    actor2->mHorzSpeed = 0xa000;
                                }
                                mCoinsSpawned = 0;
                            }
                        }
                    }
                    i = i + 1;
                } while (i < count);
            }
        }
        {
            void *anim = mState;
            if (anim != &data_ov073_021233d0 && anim != &data_ov073_021233a0) {
                ChiefChilly_ChangeState((C *)(this), (PMF *)&data_ov073_021233a0);
            }
        }
        return 1;
    }

    Vector3 v2;
    v2.x = mPos.x;
    v2.y = mPos.y;
    v2.z = mPos.z;
    if (((daPly_c *)target)->Hurt(v2, 0, 0x14000, 1, 0, 1) != 0) {
        s32 *pv = &target->mPos.x;
        Particle::System::NewSimple(kPtclChiefChillyHitPlayer, pv[0], pv[1], pv[2]);
        if (mState == &data_ov073_021233d0) {
            func_02012694(SND_HIT_REGISTERED, &mCamSpacePos);
            return 1;
        }
        mPrevAngle.y += 0x8000;
        func_02012694(SND_REVERSE_AFTER_HURT, &mCamSpacePos);
        ChiefChilly_ChangeState((C *)(this), (PMF *)&data_ov073_02123360);
        mBlendAnmModel.SetAnim(*(BCA_File *)gKingIceDonketuHitAnimationFile.filePtr, 4, 0x40000000,
                               0x1000, 0);
        mBlendAnmModel.speed = 0x1000;
        mPhase = 2;
        mStateParam = 0x2000;
        mStateTimer = 30;
    }
    return 0;
}

/* ..3370 update, the defeat. mScale.x is approached toward 0 by 0x80 a frame
   and copied to Y and Z, with those effects kept alive. Once mScale.x is below 0x100
   (1/16) the boss ends the talk, stops the layer-3 music, restores the music
   volume, spawns OBJ_KEY 400.0 (0x190000) above mSpawnPos, plays SND_POOF,
   and if an OBJ_BLUE_FIRE actor exists puts a puff of dust at it and marks it
   for destruction. If the key spawned, camera flag 0x8 is cleared and the
   boss marks itself for destruction. */
int daKing_Donketu_c::UpdateDefeat() {
    dCamera_c *cam;
    dActor_c* spawned;
    dActor_c* found;
    /* Kept (#821 r60): Vector3s keeps the stores but loads ldrsh, not
       ldrh (5 words); a plain Vector3_16 drops them (0x15c). */
    /* Kept (#821 r79): Vector3_16 rot copy-initialised or assigned from mAngle, directly or
       through a pointer, is 0x15c; the stores are dropped. */
    volatile struct Vector3_16 rot;
    struct Vector3 pos;
    cam = gCamera;
    cam->SetFlag_3();
    ApproachLinear(mScale.x, 0, 0x80);
    mScale.z = mScale.x;
    mScale.y = mScale.z;
    RenewLoopedParticles();
    if (mScale.x < 0x100) {
        pos.x = mSpawnPos.x;
        pos.y = mSpawnPos.y;
        pos.z = mSpawnPos.z;
        Message::EndTalk();
        Sound::StopLoadedMusic_Layer3();
        func_02011cfc();
        Sound::ChangeMusicVolume(0x7f, 0x15666);
        pos.y = pos.y + 0x190000;
        func_0200fa8c(1);

        /* Kept (#821 r20, measured against ov073 0x0211fa74, 0x178). rot is
           built and never read; the ROM still stores all four halfwords.
             non-volatile rot with plain field stores: 0x15c (the stores are
               dropped).
             volatile rot, `rot.x = mAngle.x; rot.y = mAngle.y; ...`: 5 of 94
               words differ.
             volatile rot with the reads below but `rot.x = a`: 4 differ.
           Vector3_16 has no destructor, so the non-POD lever that keeps a
           dead Vector3 copy does not apply. */
        {
            unsigned short a = *(unsigned short *)&mAngle.x;
            unsigned short b = *(unsigned short *)&mAngle.y;
            rot.x = b ? a : a;
            rot.y = b;
            rot.z = *(unsigned short *)&mAngle.z;
            rot.y = HorzAngleToCPlayer();
        }

        spawned = dActor_c::Spawn(ACTOR_OBJ_KEY, 4, pos, 0, mAreaId, -1);
        found = dActor_c::FindWithActorID(ACTOR_OBJ_BLUE_FIRE, 0);
        func_02012694(SND_POOF, &mCamSpacePos);
        if (found != 0) {
            struct Vector3 fp;
            const s32 *pv = &found->mPos.x;
            fp.x = pv[0];
            fp.y = pv[1];
            fp.z = pv[2];
            PoofDustAt(fp);
            found->MarkForDestruction();
        }
        if (spawned != 0) {
            cam->mFlags &= ~8;
            MarkForDestruction();
        }
    }
    return 1;
}

/* ..3370 enter: nothing to do. */
int daKing_Donketu_c::EnterDefeat() {
    return 1;
}

/* ..3410 update, cut scene part 3: keeps the looped sound, the camera and the
   effects going until mTalkPlayer's talk state reads -1, then goes to ..3370. */
int daKing_Donketu_c::UpdateDefeatCutscene3() {
    daPly_c *pl = mTalkPlayer;
    mSoundHandle = Sound::PlayLong(mSoundHandle, 3, 0x170, mCamSpacePos, 0);
    gCamera->SetFlag_3();
    RenewLoopedParticles();
    if (pl->GetTalkState() == -1) {
        ChiefChilly_ChangeState((C *)(this), (PMF *)&data_ov073_02123370);
    }
    return 1;
}

/* ..3410 enter: nothing to do. */
int daKing_Donketu_c::EnterDefeatCutscene3() {
    return 1;
}

/* ..33e0 update, cut scene part 2. Returns at once if there is no player. The
   camera looks at mSpawnPos lowered 768.0 (0x300000) and sits at mPos raised
   512.0 (0x200000), moved 1374.0 (0x55e000) back along z rotated by mAngle.y.
   The player is stored in mTalkPlayer; the music volume is set down and once
   StartTalk accepts, ShowMessage shows message (player's param1 + 0xe7)
   at mPos. When it finishes SND_TALK_DONE plays and the state becomes ..3410. */
int daKing_Donketu_c::UpdateDefeatCutscene2() {
    struct Vector3 msgpos[2];
    struct Vector3 la, ps, in, out;
    daPly_c *player;
    dCamera_c* cam;

    player = ClosestPlayer();
    mSoundHandle = Sound::PlayLong(mSoundHandle, 3, 0x170, mCamSpacePos, 0);
    if (player == 0) return 1;

    RenewLoopedParticles();

    {
        const s32 *pv = &player->mPos.x;
        msgpos[0].x = pv[0];
        msgpos[0].y = pv[1];
        msgpos[0].z = pv[2];
    }
    cam = gCamera;
    msgpos[1].x = mPos.x;
    msgpos[1].y = mPos.y;
    msgpos[1].z = mPos.z;

    cam->SetFlag_3();

    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;

    la.x = mSpawnPos.x;
    la.y = mSpawnPos.y;
    la.z = mSpawnPos.z;
    ps.x = mPos.x;
    ps.y = mPos.y;
    ps.z = mPos.z;

    la.y -= 0x300000;
    in.z = -0x55e000;

    Matrix4x3_FromRotationY(&gScratchMtx, mAngle.y);
    MTX_MultVec43(&in, &gScratchMtx, &out);

    ps.x = ps.x + out.x;
    ps.y = ps.y + 0x200000;
    ps.z = ps.z + out.z;

    cam->SetLookAt(la);
    cam->SetPos(ps);

    if (player != 0) {
        int msg;
        mTalkPlayer = player;
        msg = (short)(mTalkPlayer->param1 + 0xe7);
        Sound::ChangeMusicVolume(0x14, 0x15666);
        Message::PrepareTalk();
        if (mTalkPlayer->StartTalk(*this, 1)) {
            cam->SetFlag_3();
            if (mTalkPlayer->ShowMessage(*this, msg, &msgpos[1], 0, 2)) {
                func_02012694(SND_TALK_DONE, &mCamSpacePos);
                ChiefChilly_ChangeState((C *)(this), (PMF *)(&data_ov073_02123410));
            }
        }
    }
    return 1;
}

/* ..33e0 enter: nothing to do. */
int daKing_Donketu_c::EnterDefeatCutscene2() {
    return 1;
}

/* ..33b0 update, cut scene part 1. The nearest player, when it is not
   airborne, is put into a talk and its angles set to face the boss (the
   boss-to-player angle + 0x8000). The 0x77 / 0x78 effects are renewed and the camera looks at
   a point 256.0 (0x100000) above the boss from 256.0 below mSpawnPos, moved
   1436.0 (0x59c000) back along z rotated by mAngle.y. When mStateTimer reaches
   0 the state becomes ..33e0. */
int daKing_Donketu_c::UpdateDefeatCutscene1() {
    struct Vector3 look, pos, in, out;
    daPly_c *player;
    dCamera_c* cam;

    mSoundHandle = Sound::PlayLong(mSoundHandle, 3, 0x170, mCamSpacePos, 0);

    player = ClosestPlayer();
    if (player != 0 && player->mIsAirborne == 0) {
        int angle;
        player->StartTalk(*this, 1);
        angle = HorzAngleToCPlayer();
        player->mAngle.x = 0;
        player->mAngle.y = angle + 0x8000;
        player->mAngle.z = 0;
    }

    RenewLoopedParticles();

    cam = gCamera;
    cam->SetFlag_3();

    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;

    look.x = mPos.x;
    look.y = mPos.y;
    look.z = mPos.z;
    pos.x = mSpawnPos.x;
    pos.y = mSpawnPos.y;
    pos.z = mSpawnPos.z;

    look.y += 0x100000;
    in.z = -0x59c000;

    Matrix4x3_FromRotationY(&gScratchMtx, mAngle.y);
    MTX_MultVec43(&in, &gScratchMtx, &out);

    pos.y -= 0x100000;
    pos.x = pos.x + out.x;
    pos.z = pos.z + out.z;

    cam->SetLookAt(look);
    cam->SetPos(pos);

    if (mStateTimer == 0) {
        ChiefChilly_ChangeState((C *)(this), (PMF *)&data_ov073_021233e0);
    }
    return 1;
}

/* ..33b0 enter: start the animation in gKingIceDonketuDeadAnimationFile.filePtr;
   mStateTimer = 0x32 (50 frames). */
short daKing_Donketu_c::EnterDefeatCutscene1() {
    mBlendAnmModel.SetAnim(*(BCA_File *)gKingIceDonketuDeadAnimationFile.filePtr, 4, 0, 0x1000, 0);
    mStateTimer = 50;
    return 1;
}

/* ..3380 update: when the animation finishes, mLandedWaypoint = 0xff and back to the
   chase (..3360). */
int daKing_Donketu_c::UpdateJumpEnd() {
    if (mBlendAnmModel.Finished()) {
        mLandedWaypoint = 0xff;
        ChiefChilly_ChangeState((C *)(this), (PMF *)&data_ov073_02123360);
    }
    return 1;
}

/* ..3380 enter: clear mEdgeRecovered and start the animation in
   (BCA_File *)gKingIceDonketuJumpEdAnimationFile.filePtr. */
struct BCA_File;
int daKing_Donketu_c::EnterJumpEnd() {
    mEdgeRecovered = 0;
    mBlendAnmModel.SetAnim(*(BCA_File *)gKingIceDonketuJumpEdAnimationFile.filePtr, 4, 0x40000000,
                           0x1000, 0);
    return 1;
}

/* ..3340 update, a leap toward mSpawnPos. mStateTimer counts down from 10
   (set by the enter handler). At 1 the leap's animation starts: mSpeed.y
   0x46000 (70.0), mVertAccel -0x3000 (-3.0), and SND_HOP_LAUNCH if mEdgeRecovered is
   set. At 0, while mStateScratch is 0, mAngle.x is straightened (0x2000 a
   frame), mPrevAngle.y turns toward the angle to mSpawnPos by 0x800, and the
   x/z velocity words mSpeed.x / mSpeed.z are set to 50.0 (0x32000) when
   mHitsRemaining is 2, else 30.0 (0x1e000), along that angle. While falling,
   if the actor touching the boss is the player and the boss is the higher,
   daPly_c::Unk_020c6a10(1) is called on it. On the ground the motion is
   zeroed, the camera is shaken (0x1388000) and SND_HOP_LANDING plays (once,
   while mStateScratch is 0). With mHitsRemaining 1 and mEdgeRecovered clear the boss
   then waits 0x82 frames (counted in mStateScratch) before ..3380; otherwise
   it goes to ..3380 at once. */
int daKing_Donketu_c::UpdateLeapToSpawn() {
    u16 state = mStateTimer;
    if (state != 0) {
        if (state == 1) {
            mBlendAnmModel.SetAnim(*(BCA_File *)gKingIceDonketuJumpStAnimationFile.filePtr, 4,
                                   0x40000000, 0x1000, 0);
            mBlendAnmModel.speed = 0x1000;
            mSpeed.y = 0x46000;
            mLandedWaypoint = 0xff;
            mVertAccel = -0x3000;
            if (mEdgeRecovered != 0) {
                func_02012694(SND_HOP_LAUNCH, &mCamSpacePos);
            }
        }
        return 1;
    }
    /* state == 0 */
    if (mStateScratch == 0) {
        Vector3 offset;
        Vector3 rotated;
        s16 horz;
        ApproachLinear(mAngle.x, 0, 0x2000);
        horz = Vec3_HorzAngle(&mPos, &mSpawnPos);
        Vec3_VertAngle(&mPos, &mSpawnPos);
        ApproachLinear(mPrevAngle.y, horz, 0x800);
        offset.x = 0;
        offset.y = 0;
        offset.z = 0;
        rotated.x = 0;
        rotated.y = 0;
        rotated.z = 0;
        if (mHitsRemaining == 2) {
            offset.z = 0x32000;
        } else {
            offset.z = 0x1e000;
        }
        Matrix4x3_FromRotationY(&gScratchMtx, horz);
        MTX_MultVec43(&offset, &gScratchMtx, &rotated);
        mSpeed.x = rotated.x;
        mSpeed.z = rotated.z;
        if (mSpeed.y < 0) {
            int id = mdCcAcPos_c.otherOwner;
            if (id != 0) {
                dActor_c *actor = dActor_c::FindWithID(id);
                if (actor != 0) {
                    enum Bool eq = (enum Bool)(actor->actorID == ACTOR_PLAYER);
                    if (eq != FALSE) {
                        /* Kept (#821 r76): `Vector3 pos = actor->mPos;` is three ldr/str
                           (UpdateLeapToSpawn 0x2b0, 10+ words off); the ROM's ldm/stm needs a POD
                           source. */
                        VecFx32 pos = *(VecFx32 *)&actor->mPos;
                        if (mPos.y > pos.y) {
                            ((daPly_c *)actor)->Unk_020c6a10(1);
                        }
                    }
                }
            }
        }
    }
    if (mWithMeshClsn.IsOnGround() != 0) {
        if (mStateScratch == 0) {
            mHorzSpeed = 0;
            mSpeed.x = 0;
            mSpeed.y = 0;
            mSpeed.z = 0;
            SpawnLandingEffects(0x1388000);
            func_02012694(SND_HOP_LANDING, &mCamSpacePos);
        }
        if (mHitsRemaining == 1 && mEdgeRecovered == 0) {
            mLandedWaypoint = 0;
            if (mStateScratch == 0) {
                mBlendAnmModel.SetAnim(*(BCA_File *)gKingIceDonketuJumpEdAnimationFile.filePtr, 4,
                                       0x40000000, 0x1000, 0);
            }
            mStateScratch += 1;
            if (mStateScratch < 0x82) {
                return 1;
            }
        }
        ChiefChilly_ChangeState((C *)this, (PMF *)&data_ov073_02123380);
    }
    mAngle.y = mPrevAngle.y;
    return 1;
}

/* ..3340 enter: mStateScratch = 0; mStateTimer = 10. */
int daKing_Donketu_c::EnterLeapToSpawn() {
    mStateScratch = 0;
    mStateTimer = 10;
    return 1;
}

/* ..3320 update, a waypoint hop. mStateTimer starts at 2. At 1 the hop
   launches: mSpeed.y 0x5a000 (90.0), mVertAccel -0x5000 (-5.0), mWaypointCursor
   advances modulo 8 and the animation in gKingIceDonketuDamageAnimationFile.filePtr runs at speed
   0x2000. At 0 the boss steers toward the current waypoint (set B while
   mHitsRemaining is 2, else set A): mPrevAngle.y turns toward it by 0x800 and
   the x/z velocity words are set to 80.0 (0x50000) along the heading and
   pitch. On landing it copies the cursor to mLandedWaypoint, stops, shakes the camera
   (0x7d0000), plays SND_HOP_LANDING and counts the landing in mJumpCount: past
   7 the state becomes ..3340, otherwise it re-enters ..3320. */
int daKing_Donketu_c::UpdateWaypointHop() {
    Vector3 v;

    if (mStateTimer != 0) {
        if (mStateTimer == 1) {
            mSpeed.y = 0x5a000;
            mVertAccel = -0x5000;
            mWaypointCursor++;
            mWaypointCursor &= 7;

            mBlendAnmModel.SetAnim(*(BCA_File *)gKingIceDonketuDamageAnimationFile.filePtr, 4, 0,
                                   0x1000, 0);
            mBlendAnmModel.speed = 0x2000;
        }
        return 1;
    }

    if (mHitsRemaining == 2) {
        v = mWaypointsB[mWaypointCursor];
    } else {
        v = mWaypointsA[mWaypointCursor];
    }
    {
        short hz = Vec3_HorzAngle(&mPos, &v);
        short vt = Vec3_VertAngle(&mPos, &v);
        Vector3 in;
        Vector3 out;
        ApproachLinear(mPrevAngle.y, hz, 0x800);
        in.x = 0;
        in.y = 0;
        in.z = 0;
        out.x = 0;
        out.y = 0;
        out.z = 0;
        in.z = 0x50000;
        Matrix4x3_FromRotationY(&gScratchMtx, hz);
        Matrix4x3_ApplyInPlaceToRotationX(&gScratchMtx, vt);
        MTX_MultVec43(&in, &gScratchMtx, &out);
        mSpeed.x = out.x;
        mSpeed.z = out.z;
    }
    if (mWithMeshClsn.IsOnGround() != 0) {
        mLandedWaypoint = mWaypointCursor;
        mHorzSpeed = 0;
        mSpeed.x = 0;
        mSpeed.y = 0;
        mSpeed.z = 0;
        SpawnLandingEffects(0x7d0000);
        func_02012694(SND_HOP_LANDING, &mCamSpacePos);
        mJumpCount++;
        if (mJumpCount > 7) {
            ChiefChilly_ChangeState((C *)this, (PMF *)&data_ov073_02123340);
        } else {
            ChiefChilly_ChangeState((C *)this, (PMF *)&data_ov073_02123320);
        }
    }
    mAngle.y = mPrevAngle.y;
    return 1;
}

/* ..3320 enter: animation speed 0; mStateScratch = 0; mStateTimer = 2. */
int daKing_Donketu_c::EnterWaypointHop() {
    mBlendAnmModel.speed = 0;
    mStateScratch = 0;
    mStateTimer = 2;
    return 1;
}

/* ..3400 update: a waypoint hop like ..3320, but it also straightens mAngle.x
   (0x2000 a frame), never turns mPrevAngle.y toward the waypoint, and at
   timer 1 it only sets
   mSpeed.y 0x5a000 (90.0) and the animation (no gravity change, no cursor
   advance), and at timer 0 the aim speed is 40.0 (0x28000). Once mStateScratch
   is set or the boss is falling, mSpeed.y also follows the aim. The landing
   sets mJumpCount to 1 and goes to ..3320. */
int daKing_Donketu_c::UpdateFirstWaypointHop() {
    Vector3 v;

    if (mStateTimer != 0) {
        if (mStateTimer == 1) {
            mSpeed.y = 0x5a000;
            mBlendAnmModel.SetAnim(*(BCA_File *)gKingIceDonketuDamageAnimationFile.filePtr, 4, 0,
                                   0x1000, 0);
            mBlendAnmModel.speed = 0x2000;
        }
        return 1;
    }

    ApproachLinear(mAngle.x, 0, 0x2000);
    if (mHitsRemaining == 2) {
        v = mWaypointsB[mWaypointCursor];
    } else {
        v = mWaypointsA[mWaypointCursor];
    }
    {
        short hz = Vec3_HorzAngle(&mPos, &v);
        short vt = Vec3_VertAngle(&mPos, &v);
        Vector3 in;
        Vector3 out;
        in.z = 0;
        in.x = 0;
        in.y = 0;
        out.x = 0;
        out.y = 0;
        out.z = 0;
        in.z = 0x28000;
        Matrix4x3_FromRotationY(&gScratchMtx, hz);
        Matrix4x3_ApplyInPlaceToRotationX(&gScratchMtx, vt);
        MTX_MultVec43(&in, &gScratchMtx, &out);
        mSpeed.x = out.x;
        if (mStateScratch != 0 || mSpeed.y < 0) {
            mSpeed.y = out.y;
            mStateScratch = 1;
        }
        mSpeed.z = out.z;
    }
    if (mWithMeshClsn.IsOnGround() != 0) {
        mLandedWaypoint = mWaypointCursor;
        mHorzSpeed = 0;
        mSpeed.x = 0;
        mSpeed.y = 0;
        mSpeed.z = 0;
        SpawnLandingEffects(0x7d0000);
        func_02012694(SND_HOP_LANDING, &mCamSpacePos);
        mJumpCount = 1;
        ChiefChilly_ChangeState((C *)(this), (PMF *)&data_ov073_02123320);
    }
    mAngle.y = mPrevAngle.y;
    return 1;
}

/* ..3400 enter: mStateTimer = 10, mStateScratch = 0, mJumpCount = 0,
   mLandedWaypoint = 0xff. */
int daKing_Donketu_c::EnterFirstWaypointHop() {
    mStateTimer = 10;
    mStateScratch = 0;
    mJumpCount = 0;
    mLandedWaypoint = 0xff;
    return 1;
}

/* ..33f0 update: mAngle.x approaches -0x4000 (a quarter turn) by 0x400 a frame
   with the looped sound playing. Once the boss is below mSpawnPos.y and on the
   ground it shakes the camera (0x7d0000), plays SND_BIG_LANDING, zeroes
   mHorzSpeed and goes to ..33b0. */
int daKing_Donketu_c::UpdateDefeatFall() {
    ApproachLinear(mAngle.x, -0x4000, 0x400);
    mSoundHandle = Sound::PlayLong(mSoundHandle, 3, 0x170, mCamSpacePos, 0);
    if (mSpawnPos.y > mPos.y && mWithMeshClsn.IsOnGround()) {
        SpawnLandingEffects(0x7d0000);
        func_02012694(SND_BIG_LANDING, &mCamSpacePos);
        mHorzSpeed = 0;
        ChiefChilly_ChangeState((C *)(this), (PMF *)&data_ov073_021233b0);
    }
    return 1;
}

/* ..33f0 enter: mHorzSpeed 0x14000 (20.0), mSpeed.y 0x1e000 (30.0),
   mVertAccel -0x3000 (-3.0); clears the sound handle. */
int daKing_Donketu_c::EnterDefeatFall() {
    mHorzSpeed = 0x14000;
    mSpeed.y = 0x1e000;
    mVertAccel = -0x3000;
    mSoundHandle = 0;
    return 1;
}

/* ..33d0 update. Gravity, mHorzSpeed and the velocity words are zeroed. When
   mStateTimer runs out (the enter handler sets it to 0x5a) mHitsRemaining
   becomes 1, mEdgeRecovered becomes 1 and the state is ..3340. Otherwise a looped
   sound plays and the boss moves toward the point 120.0 (0x78000) from the
   saved miss position (mEdgePos) along mAngle.y + 0x8000, at 20.0 (0x14000)
   a frame; mAngle.y becomes the angle to mSpawnPos, mPrevAngle.y that + 0x8000.
   mStateScratch advances 0x500 a frame and mAngle.x approaches mStateParam times
   the sine of it (fx16) by 0x400. Then,
   when HandleHit returns nonzero (the boss took a hit or, in this
   state, hurt the player) the state becomes ..33f0. */
int daKing_Donketu_c::UpdateEdgeWobble() {
    Vector3 in;
    Vector3 out;

    mVertAccel = 0;
    mHorzSpeed = 0;
    mSpeed.x = 0;
    mSpeed.y = 0;
    mSpeed.z = 0;

    if (mStateTimer == 0) {
        mHitsRemaining = 1;
        ChiefChilly_ChangeState((C *)(this), (PMF *)&data_ov073_02123340);
        mEdgeRecovered = 1;
        mPrevAngle.y = mAngle.y;
        return 1;
    }

    mSoundHandle = Sound::PlayLong(mSoundHandle, 3, 0x16e, mCamSpacePos, 0);

    in.z = 0;
    in.z = 0x78000;
    in.x = 0;
    in.y = 0;
    out.x = 0;
    out.y = 0;
    out.z = 0;
    Matrix4x3_FromRotationY(&gScratchMtx, (s16)(mAngle.y + 0x8000));
    MTX_MultVec43(&in, &gScratchMtx, &out);
    in.x = mEdgePos.x + out.x;
    in.y = mEdgePos.y;
    in.z = mEdgePos.z + out.z;
    Vec3_ApproachHorz(&mPos, &in, 0x14000);
    mAngle.y = Vec3_HorzAngle(&mPos, &mSpawnPos);
    mPrevAngle.y = mAngle.y + 0x8000;
    mStateScratch += 0x500;
    ApproachLinear(mAngle.x, FX_MUL(mStateParam, FX_SinIdx((u16)(s16)mStateScratch >> 4)), 0x400);
    if (HandleHit()) {
        ChiefChilly_ChangeState((C *)(this), (PMF *)&data_ov073_021233f0);
    }
    return 1;
}

/* ..33d0 enter: animation gKingIceDonketuWalkAnimationFile.filePtr at speed 0x4000; mAngle.x,
   mStateScratch and all the motion words cleared; mStateParam = -0x1000;
   mGroundMissPos copied into mEdgePos; sound handle cleared;
   mStateTimer = 0x5a (90 frames). */
int daKing_Donketu_c::EnterEdgeWobble() {
    mBlendAnmModel.SetAnim(*(BCA_File *)gKingIceDonketuWalkAnimationFile.filePtr, 4, 0, 0x1000, 0);
    mBlendAnmModel.speed = 0x4000;
    mAngle.x = 0;
    mStateScratch = 0;
    mVertAccel = 0;
    mHorzSpeed = 0;
    mSpeed.x = 0;
    mSpeed.y = 0;
    mSpeed.z = 0;
    mStateParam = -0x1000;
    mEdgePos.x = mGroundMissPos.x;
    mEdgePos.y = mGroundMissPos.y;
    mEdgePos.z = mGroundMissPos.z;
    mSoundHandle = 0;
    mStateTimer = 90;
    return 1;
}

/* ..33c0 update: mAngle.x approaches -0x4000 by 0x400 a frame. Once the boss is
   below mSpawnPos.y and on the ground it shakes the camera (0xfa0000), stops,
   turns to face mSpawnPos (both angles), plays SND_BIG_LANDING and goes to
   ..3400. */
int daKing_Donketu_c::UpdateKnockedOffEdge() {
    ApproachLinear(mAngle.x, -0x4000, 0x400);
    if (mSpawnPos.y > mPos.y) {
        if (mWithMeshClsn.IsOnGround()) {
            SpawnLandingEffects(0xfa0000);
            mHorzSpeed = 0;
            mPrevAngle.y = Vec3_HorzAngle(&mPos, &mSpawnPos);
            mAngle.y = mPrevAngle.y;
            func_02012694(SND_BIG_LANDING, &mCamSpacePos);
            ChiefChilly_ChangeState((C *)(this), (PMF *)&data_ov073_02123400);
        }
    }
    return 1;
}

/* ..33c0 enter: animation gKingIceDonketuWalkAnimationFile.filePtr at speed 0x4000;
   mHorzSpeed 0x14000 (20.0), mSpeed.y 0x1e000 (30.0), mVertAccel -0x3000
   (-3.0); mAngle.x and mStateScratch cleared. */
int daKing_Donketu_c::EnterKnockedOffEdge() {
    mBlendAnmModel.SetAnim(*(BCA_File *)gKingIceDonketuWalkAnimationFile.filePtr, 4, 0, 0x1000, 0);
    mBlendAnmModel.speed = 0x4000;
    mHorzSpeed = 0x14000;
    mSpeed.y = 0x1e000;
    mAngle.x = 0;
    mVertAccel = -0x3000;
    mStateScratch = 0;
    return 1;
}

/* ..33a0 update, the hit reaction. While |mHorzSpeed| > 0xa the dust follows
   mFootPos[0..1]. If mNoGroundAhead is 1 mHitsRemaining drops by one and the
   state becomes ..33c0 while it is nonzero, ..33d0 when it reaches 0.
   Otherwise mHorzSpeed is braked to 0 by 0x1000 a frame, and once the
   animation has finished with |mHorzSpeed| < 0xa the boss stops, takes mAngle.y
   back into mPrevAngle.y and returns to the chase (..3360). */
int daKing_Donketu_c::UpdateHitReaction() {
    int a = mHorzSpeed;
    if (a < 0)
        a = -a;
    if (a > 0xa) {
        int i = 0;
        do {
            Particle::RunningSlidingDustAt(mFootPos[i].x, mFootPos[i].y, mFootPos[i].z);
            i++;
        } while (i < 2);
    }
    if (mNoGroundAhead == 1) {
        mHitsRemaining -= 1;
        if (mHitsRemaining != 0)
            ChiefChilly_ChangeState((C *)(this), (PMF *)&data_ov073_021233c0);
        else
            ChiefChilly_ChangeState((C *)(this), (PMF *)&data_ov073_021233d0);
        return 1;
    }
    ApproachLinear(mHorzSpeed, 0, 0x1000);
    if (mBlendAnmModel.Finished()) {
        int b = mHorzSpeed;
        if (b < 0)
            b = -b;
        if (b < 0xa) {
            mHorzSpeed = 0;
            mPrevAngle.y = mAngle.y;
            ChiefChilly_ChangeState((C *)(this), (PMF *)&data_ov073_02123360);
        }
    }
    return 1;
}

/* ..33a0 enter: animation gKingIceDonketuDamageAnimationFile.filePtr at speed 0x1000; mPrevAngle.y
   = the angle to the player + 0x8000. */
int daKing_Donketu_c::EnterHitReaction() {
    int fix;
    unsigned short t;
    short ang;

    fix = 0x1000;
    t = 0;
    mBlendAnmModel.SetAnim(*(BCA_File *)gKingIceDonketuDamageAnimationFile.filePtr, 4, 0x40000000,
                           fix, t);
    mBlendAnmModel.speed = fix;
    ang = HorzAngleToCPlayer();
    mPrevAngle.y = ang;
    mPrevAngle.y += 0x8000;
    return 1;
}

/* ..3390 update: mPrevAngle.y turns toward mTargetAngle by 0x500 a frame. When
   mTargetAngle is within 0x100 of mAngle.y the state becomes ..3360 with
   mStateTimer 0x1e (30 frames). The Prev angles are copied into the angles. */
int daKing_Donketu_c::UpdateTurnToSpawn() {
    ApproachLinear(mPrevAngle.y, mTargetAngle, 0x500);
    if (AngleDiff(mTargetAngle, mAngle.y) < 0x100) {
        ChiefChilly_ChangeState((C *)(this), (PMF *)&data_ov073_02123360);
        mStateTimer = 30;
    }
    mAngle.x = mPrevAngle.x;
    mAngle.y = mPrevAngle.y;
    mAngle.z = mPrevAngle.z;
    return 1;
}

/* ..3390 enter: animation gKingIceDonketuWalkAnimationFile.filePtr at speed 0x2000;
   mTargetAngle = the angle to mSpawnPos; mStateScratch = 0;
   mPrevAngle.y = mAngle.y. */
int daKing_Donketu_c::EnterTurnToSpawn() {
    mBlendAnmModel.SetAnim(*(BCA_File *)gKingIceDonketuWalkAnimationFile.filePtr, 4, 0, 0x1000, 0);
    mBlendAnmModel.speed = 0x2000;
    mTargetAngle = Vec3_HorzAngle(&mPos, &mSpawnPos);
    mStateScratch = 0;
    mPrevAngle.y = mAngle.y;
    return 1;
}

static inline int IsNoControl(daPly_c *player) {
    return player->mIsNoControl != 0;
}

/* ..3360 update, the chase, in legs selected by mPhase. mNoGroundAhead being
   1 sends the boss to ..3390 first.
     leg 0  with mStateTimer at 0: mTargetAngle = the angle to the player. If
            mPrevAngle.y is within 0x2000 of it the timer is set to 0x1e and
            the leg continues; otherwise leg 1 with mStateParam = 0x1000 and
            SND_CHASE_BRAKE. While the timer runs: mHorzSpeed approaches
            0x1e000 (30.0) by 0x3000, mPrevAngle.y turns toward mTargetAngle by
            0x1d0, and the first time the player is within 0x1f4000 (500.0)
            there is a one-in-eight chance of leg 3 (speed zeroed,
            mStateTimer 0xf).
     leg 3  when mStateTimer is 0: mStateParam = 0x1000, leg 1.
     legs 1, 2  mHorzSpeed is braked to 0 by mStateParam with the dust following
            while it is above 0xa; leg 1 also turns mAngle.y toward the player
            by 0x500. When |mHorzSpeed| < 0xa and mStateTimer is 0, leg 2
            first retargets (to mSpawnPos if the player's hurt state reads 4 or
            5 or it is mIsNoControl, else to the player), then the boss stops,
            restarts the animation and returns to leg 0 with mStateTimer 0x1e.
   In leg 0 the model hitting animation frame 0 or 0xe shakes the camera
   (0x3e8000) and plays SND_CHASE_STEP. */
int daKing_Donketu_c::UpdateChase() {

    if (mNoGroundAhead == 1) {
        ChiefChilly_ChangeState((C *)(this), (PMF *)&data_ov073_02123390);
        return 1;
    }

    switch (mPhase) {
    case 0:
        if (mStateTimer == 0) {
            mTargetAngle = HorzAngleToCPlayer();
            mStateScratch = 0;
            if (AngleDiff(mPrevAngle.y, mTargetAngle) <= 0x2000) {
                mStateTimer = 30;
                break;
            } else {
                mStateTimer = 0;
                mStateParam = 0x1000;
                (mPhase)++;
                func_02012694(SND_CHASE_BRAKE, &mCamSpacePos);
                break;
            }
        } else {
            ApproachLinear(mHorzSpeed, 0x1e000, 0x3000);
            if (mStateScratch == 0) {
                if (DistToCPlayer() < 0x1f4000) {
                    mStateScratch = 1;
                    if ((((unsigned int)RandomIntInternal(&data_0209e650.value) >> 0x18) & 7) ==
                        0) {
                        mPhase = 3;
                        mHorzSpeed = 0;
                        mStateTimer = 15;
                    }
                }
            }
            ApproachLinear(mPrevAngle.y, mTargetAngle, 0x1d0);
            mAngle.x = mPrevAngle.x;
            mAngle.y = mPrevAngle.y;
            mAngle.z = mPrevAngle.z;
            break;
        }

    case 3:
        if (mStateTimer == 0) {
            mStateParam = 0x1000;
            mPhase = 1;
        }
        break;

    case 1:
    case 2: {
        int d;
        int i;
        mStateScratch = 0;
        d = mHorzSpeed;
        if (d < 0)
            d = -d;
        if (d > 0xa) {
            i = 0;
            do {
                Particle::RunningSlidingDustAt(mFootPos[i].x, mFootPos[i].y, mFootPos[i].z);
                i++;
            } while (i < 2);
        }
        ApproachLinear(mHorzSpeed, 0, mStateParam);
        if (mPhase == 1) {
            mTargetAngle = HorzAngleToCPlayer();
            ApproachLinear(mAngle.y, mTargetAngle, 0x500);
        }
        d = mHorzSpeed;
        if (d < 0)
            d = -d;
        if (d >= 0xa)
            break;
        if (mStateTimer != 0)
            break;
        if (mPhase == 2) {
            daPly_c *p = ClosestPlayer();
            if (p != 0) {
                if (p->GetHurtState() == 4 || p->GetHurtState() == 5 || IsNoControl(p) == 1)
                    mTargetAngle = Vec3_HorzAngle(&mPos, &mSpawnPos);
                else
                    mTargetAngle = HorzAngleToCPlayer();
            }
        }
        mHorzSpeed = 0;
        mPrevAngle.y = mAngle.y;
        mStateTimer = 30;
        mBlendAnmModel.SetAnim(*(BCA_File *)gKingIceDonketuWalkAnimationFile.filePtr, 4, 0, 0x1000,
                               0);
        mBlendAnmModel.speed = 0x2000;
        mStateScratch = 0;
        mPhase = 0;
        break;
    }
    }

    if (mPhase == 0) {
        if (mBlendAnmModel.WillHitFrame(0) != 0 || mBlendAnmModel.WillHitFrame(0xe) != 0) {
            SpawnLandingEffects(0x3e8000);
            func_02012694(SND_CHASE_STEP, &mCamSpacePos);
        }
    }
    return 1;
}

/* ..3360 enter: mStateTimer = 0, mPhase = 0, mStateParam = 0x2000; animation
   (BCA_File *)gKingIceDonketuWalkAnimationFile.filePtr at speed 0x2000. */
int daKing_Donketu_c::EnterChase() {
    mStateTimer = 0;
    mPhase = 0;
    mStateParam = 0x2000;
    mBlendAnmModel.SetAnim(*(BCA_File *)gKingIceDonketuWalkAnimationFile.filePtr, 4, 0, 0x1000, 0);
    mBlendAnmModel.speed = 0x2000;
    return 1;
}

/* ..3350 update. The camera looks at mPos raised 112.0 (0x70000) and moved
   672.0 (0x2a0000) in -z, from mPos moved 768.0 (0x300000) in -x, raised 32.0
   (0x20000), and moved in z by 0xffa34000, which as a signed word is -0x5cc000
   (-1484.0). When mTalkPlayer's talk state reads -1 camera flag 0x8 is
   cleared, layer-3 music 0x2d is loaded and set, func_02011d08 is called and
   the state becomes ..3360. */
int daKing_Donketu_c::UpdateIntroTalkEnd() {
    struct Vector3 la;
    struct Vector3 ps;
    dCamera_c *cam;
    daPly_c *player;

    cam = gCamera;
    player = mTalkPlayer;
    cam->SetFlag_3();

    la.x = mPos.x;
    la.y = mPos.y;
    la.z = mPos.z;
    ps.x = mPos.x;
    ps.y = mPos.y;
    ps.z = mPos.z;
    la.y = la.y + 0x70000;
    la.z = la.z - 0x2a0000;
    ps.x = ps.x - 0x300000;
    ps.y = ps.y + 0x20000;
    ps.z = ps.z + 0xffa34000;

    cam->SetLookAt(la);
    cam->SetPos(ps);

    if (player->GetTalkState() == -1) {
        cam->mFlags &= ~8;
        Sound::LoadAndSetMusic_Layer3(0x2d);
        func_02011d08();
        ChiefChilly_ChangeState((C *)(this), (PMF *)&data_ov073_02123360);
    }
    return 1;
}

/* ..3350 enter: mPhase = 0. */
int daKing_Donketu_c::EnterIntroTalkEnd() {
    mPhase = 0;
    return 1;
}

/* ..3330 update, the opening talk. Returns at once without a player. The
   camera is placed as in ..3350 and mPrevAngle.y turns toward the player by
   0x800. The player is stored in mTalkPlayer and, once StartTalk accepts,
   layer-3 music 0x2c is loaded (nothing in this file changes mPhase while this
   state runs, so the test always passes) and ShowMessage runs for message (player's param1 + 0xe3) at mPos
   raised 100.0 (0x64000). When it finishes SND_TALK_DONE plays and the state
   becomes ..3350. */
int daKing_Donketu_c::UpdateIntroTalk() {
    struct Vector3 vplayer;
    struct Vector3 vmsg;
    struct Vector3 la;
    struct Vector3 ps;
    daPly_c *player;
    dCamera_c *cam;
    int msg;
    daPly_c *p;

    player = ClosestPlayer();
    if (player == 0) return 1;

    {
        const s32 *pv = &player->mPos.x;
        vplayer.x = pv[0];
        vplayer.y = pv[1];
        vplayer.z = pv[2];
    }
    cam = gCamera;
    vmsg.x = mPos.x;
    vmsg.y = mPos.y;
    vmsg.z = mPos.z;

    cam->SetFlag_3();

    la.x = mPos.x;
    la.y = mPos.y;
    la.z = mPos.z;
    ps.x = mPos.x;
    ps.y = mPos.y;
    ps.z = mPos.z;
    la.y = la.y + 0x70000;
    la.z = la.z - 0x2a0000;
    ps.x = ps.x - 0x300000;
    ps.y = ps.y + 0x20000;
    ps.z = ps.z + 0xffa34000;

    cam->SetLookAt(la);
    cam->SetPos(ps);

    vmsg.y = vmsg.y + 0x64000;
    ApproachLinear(mPrevAngle.y, Vec3_HorzAngle(&mPos, &vplayer), 0x800);

    mTalkPlayer = player;
    p = mTalkPlayer;
    msg = (short)(p->param1 + 0xe3);
    if (p->StartTalk(*this, 1)) {
        if (mPhase == 0) {
            Sound::LoadAndSetMusic_Layer3(0x2c);
            mPhase = 0;
        }

        if (mTalkPlayer->ShowMessage(*this, msg, &vmsg, 0, 2)) {
            func_02012694(SND_TALK_DONE, &mCamSpacePos);
            ChiefChilly_ChangeState((C *)(this), (PMF *)&data_ov073_02123350);
        }
    }
    return 1;
}

/* ..3330 enter: mPhase = 0; animation gKingIceDonketuWaitAnimationFile.filePtr. */
int daKing_Donketu_c::EnterIntroTalk() {
    mPhase = 0;
    mBlendAnmModel.SetAnim(*(BCA_File *)gKingIceDonketuWaitAnimationFile.filePtr, 4, 0, 0x1000, 0);
    return 1;
}

/* Stores the state record in mState (the shadow struct's pp, at +0x37c) and
   calls the enter member held in the record's first pointer-to-member; returns
   1 without calling if that pointer is null. */
int ChiefChilly_ChangeState(C *c, PMF *p) {
    c->pp = p;
    PMF *q = c->pp;
    if (*q == 0)
        return 1;
    return (c->**q)();
}

/* Called at the end of every Behavior. Builds the model matrix (mPos >> 3,
   rotated by mAngle.x/.y/.z) into mBlendAnmModel.mat4x3, then sets mFootPos[0..1]
   to the world positions of bones 2 and 3 (bone matrix times the model
   matrix, translation words shifted left by 3). In ..33b0, ..33e0, ..3410,
   ..33f0 and ..3370 it stops there; in every other state it also draws the
   drop shadow from mShadowMtx (mPos >> 3 with y lowered 10.0 (0xa000)),
   radius 300.0 (0x12c000), height 1000.0 (0x3e8000). */
void daKing_Donketu_c::UpdateModelAndShadow() {
    int sh;
    Vector3 v;
    VecFx32 out;
    void* m;
    Matrix4x3 *saved;
    int i;
    Vector3 *p;

    Vec3_Asr(&v, &mPos, 3);
    Matrix4x3_FromTranslation(&gScratchMtx, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&gScratchMtx, mAngle.x, mAngle.y, mAngle.z);

    p = mFootPos;
    mBlendAnmModel.mat4x3 = gScratchMtx;
    saved = &mBlendAnmModel.mat4x3;
    sh = 3;
    i = 0;
    for (; i < 2; i++) {
        gScratchMtx = *saved;
        MTX_Concat43(&mBlendAnmModel.data.transforms[i + 2], &gScratchMtx, &gScratchMtx);
        p->x = gScratchMtx.t.x;
        p->y = gScratchMtx.t.y;
        p->z = gScratchMtx.t.z;
        Vec3_Lsl(&out, p, sh);
        mFootPos[i].x = out.x;
        mFootPos[i].y = out.y;
        mFootPos[i].z = out.z;
        p++;
    }

    m = mState;
    if (m == &data_ov073_021233b0)
        return;
    if (m == &data_ov073_021233e0)
        return;
    if (m == &data_ov073_02123410)
        return;
    if (m == &data_ov073_021233f0)
        return;
    if (m == &data_ov073_02123370)
        return;

    Matrix4x3_FromTranslation(&gScratchMtx, mPos.x >> 3, (mPos.y - 0xa000) >> 3, mPos.z >> 3);
    mShadowMtx = gScratchMtx;
    DropShadowRadHeight(mShadowModel, mShadowMtx, 0x12c000, 0x3e8000, 0xf);
}

int daKing_Donketu_c::CleanupResources() {
    UnloadKeyModels(4);
    gKingIceDonketuDeadAnimationFile.Release();
    gKingIceDonketuHitAnimationFile.Release();
    gKingIceDonketuWalkAnimationFile.Release();
    gKingIceDonketuDamageAnimationFile.Release();
    gKingIceDonketuJumpEdAnimationFile.Release();
    gKingIceDonketuJumpStAnimationFile.Release();
    gKingIceDonketuWaitAnimationFile.Release();
    gScaleUpKinokoModelFile.Release();
    gKingIceDonketuModelModelFile.Release();
    return 1;
}

void daKing_Donketu_c::OnPendingDestroy()
{
}

int daKing_Donketu_c::Render()
{
    mBlendAnmModel.Render(&mScale);
    return 1;
}

/*
 * One frame of the boss. Almost everything here is gated on WHICH state is
 * current, compared by ADDRESS against the file-scope state records, and it
 * gates five separate things rather than one:
 *
 *   three states       use UpdatePosWithOnlySpeed instead of UpdatePos, and
 *                      apply gravity by hand first: mSpeed.y becomes
 *                      max(mTerminalVelocity, mSpeed.y + mVertAccel)
 *   two states         at animation frame 7, build a world matrix from the
 *                      model's own bone and spawn the landing dust there --
 *                      the impact is tied to the ANIMATION, not to contact
 *   seven states       skip the ground ray entirely
 *   one more           is exempt from the rewind below
 *   two states         get an extra per-frame call
 *
 * THE GROUND RAY IS A FALL-THROUGH GUARD. It starts 120.0 (0x78000) above the
 * boss and runs along mPrevAngle.y, tilted by 0x2000 (45 degrees) about X, for
 * 300.0 (0x12c000) -- 600.0 while mHitsRemaining > 1. When it finds NOTHING it
 * saves the current position into mGroundMissPos and, outside the ..33a0
 * state, rewinds mPos to the previous position and zeroes mHorzSpeed -- so the
 * boss from walking off in the states that run the ray. Finding ground clears mNoGroundAhead;
 * a miss while moving faster than 0xa000 (10.0) sets it.
 *
 * The first thing it does is store `this` into the camera's mFramedActor, so
 * the camera frames the boss with the player.
 *
 * `opt_propagation off` is what stops the many position temporaries being
 * folded (without it Behavior is 0x428, not 0x42c: pitch's 0x2000 and the
 * gScratchMtx address are folded instead of held in r4). It is bracketed to this one member: under `defer_codegen off` above,
 * the bracket binds positionally, so the members before and after it keep
 * the file's default propagation. The closing `#pragma pop` must NOT follow
 * Behavior's closing brace directly: the parser reads one token past the `}`
 * before generating the body, so a pop there is already in force when
 * Behavior is compiled and Behavior no longer matches. The extern block below
 * Behavior (the declarations InitResources needs) is what keeps it clear.
 *
 * Kept (#821, measured 2026-11-16 against ov073 0x021218a0, 0x42c):
 *   - bracket removed: 0x428; fdiff aligns 182 of 267 words, 85 replaced and
 *     1 deleted. `this` moves from r5 to r4 for the whole body, because r4 no
 *     longer holds the gScratchMtx address across the ldm/stm copy and
 *     MTX_Concat43, or 0x2000 across Matrix4x3_FromRotationY.
 *   - the same pragma at file level (no bracket): Behavior matches, but 10
 *     other members stop matching (SpawnHitParticles, UpdateDefeat,
 *     UpdateWaypointHop, UpdateEdgeWobble, UpdateHitReaction,
 *     EnterHitReaction, UpdateChase, UpdateIntroTalkEnd,
 *     UpdateModelAndShadow and InitResources).
 *   - bracket removed, matrix block spelled as `Matrix4x3 *m =
 *     &gScratchMtx`, as `Matrix4x3 &m = gScratchMtx`, or with the bone
 *     position as one Vector3 copy from m[9]: each is 0x428, the same 85
 *     replaced words.
 *   SpawnLandingEffects (0x0211f2c0) is the same copy-and-concat block, and
 *   it holds the address in lr under the default propagation.
 * Re-measured after the body was rewritten without the RayParams bundle and
 * the `C *` alias: with the bracket removed it is still 0x428, and the same
 * 182 equal / 85 replaced / 1 deleted words, so the launders were not what
 * needed the pragma. Ignoring register names (fdiff --align-shape), only two
 * sites differ: after MTX_Concat43 the bone reads reload &gScratchMtx from
 * the literal pool instead of using the copy held in r4, and 0x2000 goes to
 * Matrix4x3_ApplyInPlaceToRotationX as an immediate instead of being held in
 * r4 from before the dir.z if/else. Both are copy/constant propagation
 * undone. Also measured with the bracket removed, each unchanged: pitch as
 * `int` or `register short`, pitch (and yaw) as Vector3_16 fields, and a
 * `Matrix4x3 *m` local declared at the top or in the stomp block.
 *
 * The eight Vector3 locals are declared up front in frame order (bonePos at
 * sp+0, then start, end, dir, ray, shifted, dustPos, offset); declared inside
 * the ray block, the four ray vectors land above the others. The ray block
 * zeroes all four before filling them in, and the ROM keeps those stores.
 */
#pragma push
#pragma opt_propagation off

int daKing_Donketu_c::Behavior()
{
    Vector3 bonePos;
    Vector3 start;
    Vector3 end;
    Vector3 dir;
    Vector3 ray;
    Vector3 shifted;
    Vector3 dustPos;
    Vector3 offset;
    short pitch;

    gCamera->mFramedActor = this;
    DecIfAbove0_Short(&mStateTimer);

    State *state = (State *)mState;
    if (state->update != 0)
        (this->*state->update)();

    if (mState != &data_ov073_02123400 && mState != &data_ov073_02123320 &&
        mState != &data_ov073_02123340) {
        UpdatePos(&mdCcAcPos_c);
    } else {
        /* Gravity, clamped at terminal velocity. mSpeed.z is reloaded and
           stored back unchanged, as in every sibling actor's copy of this. */
        int vertSpeed = mSpeed.y + mVertAccel;
        int clamped = mTerminalVelocity;
        int keep = mSpeed.z;
        if (vertSpeed >= clamped)
            clamped = vertSpeed;
        mSpeed.y = clamped;
        mSpeed.z = keep;
        UpdatePosWithOnlySpeed(&mdCcAcPos_c);
    }

    if ((mState == &data_ov073_02123330 || mState == &data_ov073_02123350) &&
        mBlendAnmModel.WillHitFrame(7) != 0) {
        gScratchMtx = mBlendAnmModel.mat4x3;
        MTX_Concat43(&mBlendAnmModel.data.transforms[2], &gScratchMtx, &gScratchMtx);
        bonePos.x = gScratchMtx.t.x;
        bonePos.y = gScratchMtx.t.y;
        bonePos.z = gScratchMtx.t.z;
        Vec3_Lsl(&shifted, &bonePos, 3);
        bonePos = shifted;
        func_02012694(SND_ANIM_FRAME_STOMP, &mCamSpacePos);
        dustPos = bonePos;
        HugeLandingDustAt(dustPos, 1);
    }

    if (mState != &data_ov073_021233c0 && mState != &data_ov073_021233d0 &&
        mState != &data_ov073_021233f0 && mState != &data_ov073_02123400 &&
        mState != &data_ov073_02123320 && mState != &data_ov073_02123340 &&
        mState != &data_ov073_02123380) {
        dBgCh_Lin line;
        start.x = 0;
        start.y = 0;
        start.z = 0;
        end.x = 0;
        end.y = 0;
        end.z = 0;
        dir.x = 0;
        dir.y = 0;
        dir.z = 0;
        ray.x = 0;
        ray.y = 0;
        ray.z = 0;
        start = mPos;
        pitch = 0x2000;
        start.y += 0x78000;
        if (mHitsRemaining > 1)
            dir.z = 0x258000;
        else
            dir.z = 0x12c000;
        Matrix4x3_FromRotationY(&gScratchMtx, mPrevAngle.y);
        Matrix4x3_ApplyInPlaceToRotationX(&gScratchMtx, pitch);
        MTX_MultVec43(&dir, &gScratchMtx, &ray);
        end = start;
        end.x += ray.x;
        end.y += ray.y;
        end.z += ray.z;
        line.SetObjAndLine(start, end, this);
        if (!line.DetectClsn()) {
            if (mHorzSpeed > 0xa000)
                mNoGroundAhead = 1;
            mGroundMissPos.x = mPos.x;
            mGroundMissPos.y = mPos.y;
            mGroundMissPos.z = mPos.z;
            if (mState != &data_ov073_021233a0) {
                mPos.x = mPrevPos.x;
                mPos.y = mPrevPos.y;
                mPos.z = mPrevPos.z;
                mHorzSpeed = 0;
            }
        } else {
            mNoGroundAhead = 0;
        }
    }

    UpdateWMClsn(mWithMeshClsn, 0);

    offset = data_ov073_02123040;
    mdCcAcPos_c.SetPosRelativeToActor(offset);

    if (mState == &data_ov073_02123360 || mState == &data_ov073_02123390)
        HandleHit();
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.Update();
    UpdateModelAndShadow();
    mBlendAnmModel.Advance();
    return 1;
}

extern void LoadKeyModels(int idx);

#pragma pop

int daKing_Donketu_c::InitResources()
{
    struct BMD_File* f;
    Vector3 v;
    int i;
    LoadKeyModels(4);
    dExtFrameCtrl_c::LoadFile(gKingIceDonketuDeadAnimationFile);
    dExtFrameCtrl_c::LoadFile(gKingIceDonketuHitAnimationFile);
    dExtFrameCtrl_c::LoadFile(gKingIceDonketuWalkAnimationFile);
    dExtFrameCtrl_c::LoadFile(gKingIceDonketuDamageAnimationFile);
    dExtFrameCtrl_c::LoadFile(gKingIceDonketuJumpEdAnimationFile);
    dExtFrameCtrl_c::LoadFile(gKingIceDonketuJumpStAnimationFile);
    dExtFrameCtrl_c::LoadFile(gKingIceDonketuWaitAnimationFile);
    dExtSimpleModel_c::LoadFile(gScaleUpKinokoModelFile);
    f = dExtSimpleModel_c::LoadFile(gKingIceDonketuModelModelFile);
    mBlendAnmModel.SetFile(f, 1, -1);
    mShadowModel.InitCylinder();
    mVertAccel = -0x3000;
    mTerminalVelocity = -0x3c000;
    v.x = data_ov073_02123040.x;
    v.y = data_ov073_02123040.y;
    v.z = data_ov073_02123040.z;
    mdCcAcPos_c.Init(this, v, 0xa4000, 0x1e4000, 0x200000, 0x567f0);
    mBlendAnmModel.speed = 0x2000;
    mWithMeshClsn.Init(this, 0x96000, 0x94000, 0, 0);
    mSpawnPos.x = mPos.x;
    mSpawnPos.y = mPos.y;
    mSpawnPos.z = mPos.z;
    for (i = 0; i < 8; i++) {
        mWaypointsA[i].x = mPos.x;
        mWaypointsA[i].y = mPos.y;
        mWaypointsA[i].z = mPos.z;
        mWaypointsB[i].x = mPos.x;
        mWaypointsB[i].y = mPos.y;
        mWaypointsB[i].z = mPos.z;
    }
    mLandedWaypoint = 0xff;
    mPrevAngle.y = HorzAngleToCPlayer();
    mAngle.y = mPrevAngle.y;
    mTargetAngle = mAngle.y;
    mStepBoneIdx = 2;
    mHitsRemaining = 3;
    mScale.x = 0x1000;
    mScale.y = 0x1000;
    mScale.z = 0x1000;
    ChiefChilly_ChangeState((C *)this, (PMF *)&data_ov073_02123330);
    return 1;
}

s32 daKing_Donketu_c::OnAimedAtWithEgg()
{
    return 0x64000;
}

/* Construct the KING_DONKETU actor and its members in declaration order. */
daKing_Donketu_c *daKing_Donketu_c_classInit() {
    return new daKing_Donketu_c;
}

/* Construction order is source order: the eight file handles first, then the
 * sixteen State records in the order the initializer copies them. The
 * registration nodes and the PMF literals are compiler temporaries. */
KingDonketuModelFileHandle gKingIceDonketuModelModelFile(ASSET_KING_ICE_DONKETU_MODEL_BMD);
KingDonketuAnimResFileHandle gKingIceDonketuDeadAnimationFile(ASSET_KING_ICE_DONKETU_DEAD_BCA);
KingDonketuAnimResFileHandle gKingIceDonketuHitAnimationFile(ASSET_KING_ICE_DONKETU_HIT_BCA);
KingDonketuAnimResFileHandle gKingIceDonketuWalkAnimationFile(ASSET_KING_ICE_DONKETU_WALK_BCA);
KingDonketuAnimResFileHandle gKingIceDonketuDamageAnimationFile(ASSET_KING_ICE_DONKETU_DAMAGE_BCA);
KingDonketuAnimResFileHandle gKingIceDonketuJumpEdAnimationFile(ASSET_KING_ICE_DONKETU_JUMP_ED_BCA);
KingDonketuAnimResFileHandle gKingIceDonketuJumpStAnimationFile(ASSET_KING_ICE_DONKETU_JUMP_ST_BCA);
KingDonketuAnimResFileHandle gKingIceDonketuWaitAnimationFile(ASSET_KING_ICE_DONKETU_WAIT_BCA);

daKing_Donketu_c::State data_ov073_02123330 = {&daKing_Donketu_c::EnterIntroTalk,
                                               &daKing_Donketu_c::UpdateIntroTalk};
daKing_Donketu_c::State data_ov073_02123350 = {&daKing_Donketu_c::EnterIntroTalkEnd,
                                               &daKing_Donketu_c::UpdateIntroTalkEnd};
daKing_Donketu_c::State data_ov073_02123360 = {&daKing_Donketu_c::EnterChase,
                                               &daKing_Donketu_c::UpdateChase};
daKing_Donketu_c::State data_ov073_02123390 = {&daKing_Donketu_c::EnterTurnToSpawn,
                                               &daKing_Donketu_c::UpdateTurnToSpawn};
daKing_Donketu_c::State data_ov073_021233a0 = {&daKing_Donketu_c::EnterHitReaction,
                                               &daKing_Donketu_c::UpdateHitReaction};
daKing_Donketu_c::State data_ov073_021233c0 = {&daKing_Donketu_c::EnterKnockedOffEdge,
                                               &daKing_Donketu_c::UpdateKnockedOffEdge};
daKing_Donketu_c::State data_ov073_021233d0 = {&daKing_Donketu_c::EnterEdgeWobble,
                                               &daKing_Donketu_c::UpdateEdgeWobble};
daKing_Donketu_c::State data_ov073_021233f0 = {&daKing_Donketu_c::EnterDefeatFall,
                                               &daKing_Donketu_c::UpdateDefeatFall};
daKing_Donketu_c::State data_ov073_02123400 = {&daKing_Donketu_c::EnterFirstWaypointHop,
                                               &daKing_Donketu_c::UpdateFirstWaypointHop};
daKing_Donketu_c::State data_ov073_02123320 = {&daKing_Donketu_c::EnterWaypointHop,
                                               &daKing_Donketu_c::UpdateWaypointHop};
daKing_Donketu_c::State data_ov073_02123340 = {&daKing_Donketu_c::EnterLeapToSpawn,
                                               &daKing_Donketu_c::UpdateLeapToSpawn};
daKing_Donketu_c::State data_ov073_02123380 = {&daKing_Donketu_c::EnterJumpEnd,
                                               &daKing_Donketu_c::UpdateJumpEnd};
daKing_Donketu_c::State data_ov073_021233b0 = {(int(daKing_Donketu_c::*)()) &
                                                   daKing_Donketu_c::EnterDefeatCutscene1,
                                               &daKing_Donketu_c::UpdateDefeatCutscene1};
daKing_Donketu_c::State data_ov073_021233e0 = {&daKing_Donketu_c::EnterDefeatCutscene2,
                                               &daKing_Donketu_c::UpdateDefeatCutscene2};
daKing_Donketu_c::State data_ov073_02123410 = {&daKing_Donketu_c::EnterDefeatCutscene3,
                                               &daKing_Donketu_c::UpdateDefeatCutscene3};
daKing_Donketu_c::State data_ov073_02123370 = {&daKing_Donketu_c::EnterDefeat,
                                               &daKing_Donketu_c::UpdateDefeat};
