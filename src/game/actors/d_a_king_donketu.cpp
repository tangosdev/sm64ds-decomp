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
 * (Fix12: 0x1000 = 1.0; angles: 0x10000 = a full turn.)
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
 *   ..33d0     ..0ad8    ..0910    wobble (mAngleX) while moving toward a
 *                                  point 120.0 past the position where the
 *                                  ground ray last missed; when
 *                                  mStateTimer (0x5a frames) runs out it sets
 *                                  mHitsRemaining to 1 and goes to ..3340; a
 *                                  nonzero 0211f61c (a hit, or the
 *                                  boss hurting the player) goes to ..33f0.
 *   ..33f0     ..08e4    ..0844    leap, tilting mAngleX toward -0x4000, until
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
 *   file-global for HandleHit. common.h stays first; dExtBlendAnmModel_c's
 *   Matrix4x3 makes the twelve-word copies change size.
 * - struct C is a complete PMF shadow on purpose (mwccarm picks the
 *   pointer-to-member shape from completeness). ChiefChilly_ChangeState
 *   takes C*. The sixteen state labels are descriptions; the sinit owns
 *   the records. The header's 0x504 size wrap stays.
 * - dExtBlendAnmModel_c::SetAnim, Particle::System::New / NewSimple,
 *   RunningSlidingDustAt, daPly_c::Hurt, Sound::ChangeMusicVolume and
 *   DropShadowRadHeight stay mangled (Fix12<int> by value).
 *   NewSimple and the layer-3 sound calls retain their local declarations.
 * - Vec3_* / Matrix4x3_* have no shared header this TU
 *   can include alone. gCamera, data_020a0e68 and data_0209e650 are arm9
 *   globals. data_ov002_0210da30 is ov002's; Cleanup only Releases it.
 *   g_profile_KING_DONKETU is outside this TU.
 * - Behavior's opt_propagation off is a push/pop around that function.
 * - void* cam in UpdateDefeat / 0212128c / 02121388: a dCamera_c*
 *   swaps two registers. Those functions also ldrh mStateTimer and
 *   mAngleX/Y/Z through unsigned short; a plain s16 read is ldrsh.
 * - s16* behind mPrevAngleY in HandleHit and 02120d80, the
 *   const s32* walk of mPosX, and mFootPos[i] in the dust loops stay. The
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
   plays a sound at the boss's camera-space position (mCamSpacePosX). Named for the site that plays them;
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

#include "private/mtx43.h"

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

namespace Sound {
/* local extern: including Sound.h renumbers @NNN. */
int PlayLong(u32, u32, u32, const Vector3 &, s16);
}

extern "C" {

int ChiefChilly_ChangeState(C *c, PMF *p);

extern struct BCA_File *data_ov002_0210da30[2];
extern short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
extern void Matrix4x3_FromRotationY(void* m, short ang);
extern void MTX_MultVec43(Vector3 *in, Matrix4x3 *m, Vector3 *out);
extern void MTX_Concat43(void *dst, void *a, void *b);
extern void Vec3_Lsl(void *d, void *s, int sh);
extern void d_a_king_donketuNewSimpleHold(unsigned int id, int x, int y, int z);
extern dCamera_c *gCamera;
extern struct Matrix4x3 data_020a0e68;
/* Vec3_Sub is not declared by math/CalcLib.h. */
extern void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);
extern int atan2_hold_d_a_king_donketu_1(Fix12i y, Fix12i x);
extern Fix12i Vec3_HorzLen(const Vector3 *v);

extern u16 DecIfAbove0_Short(void* p);
extern void func_02012694(unsigned int a, void *b);
/* local extern: CalcLib.h conflicts with DecIfAbove0_Short(void *). */
extern int RandomIntInternal(int *seed);
}
extern "C" {
extern void d_a_king_donketuPlayLongHold(unsigned int a, unsigned int b, unsigned int c,
                                         struct Vector3 *v, unsigned int d);
extern s16 Vec3_VertAngle(const Vector3 *a, const Vector3 *b);
extern void Matrix4x3_ApplyInPlaceToRotationX(struct Matrix4x3 *m, short angX);
extern void d_a_king_donketuRunningSlidingDustAtHold(int a, int b, int c);
/* local extern: the Vector3 spelling math/CalcLib.h declares. This TU's
   locals are the POD VecFx32, so the call casts. */
extern void Vec3_Asr(Vector3 *d, Vector3 *s, int sh);
extern void Matrix4x3_FromTranslation(struct Matrix4x3 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern void UnloadKeyModels(int i);
}

daKing_Donketu_c::~daKing_Donketu_c()
{
}

/* The 0x77 / 0x78 effects the cut scenes and the defeat keep alive each frame. The point is
   mPos with y raised by 30.0 * the scale (mScaleX is Fix12, so the 0x1e000
   constant shrinks with the boss) and x/z moved 200.0 (0xc8000) along
   mPrevAngleY. Particle systems 0x77 and 0x78 are renewed there (ids kept in
   mParticleId0/1), and the emitRadius of each is set to 20.0 * the scale. The
   Vec3_HorzAngle result is not used. */
void daKing_Donketu_c::RenewLoopedParticles() {
    daKing_Donketu_c *c = this;

    Vector3 pos, in, out;
    int y;
    ((int*)&pos)[0] = c->mPosX;
    y = c->mPosY;
    ((int*)&pos)[1] = y;
    ((int*)&pos)[2] = c->mPosZ;
    ((int *)&pos)[1] = y + FX_MUL(c->mScaleX, 0x1e000);

    Vec3_HorzAngle(&c->mPos, &c->mSpawnPos);
    in.z = 0; in.z = 0xc8000; in.x = 0; in.y = 0;
    out.x = 0; out.y = 0; out.z = 0;
    Matrix4x3_FromRotationY(&data_020a0e68, c->mPrevAngleY);
    MTX_MultVec43((Vector3 *)&in, &data_020a0e68, (Vector3 *)&out);
    pos.x = pos.x + out.x;
    pos.z = pos.z + out.z;
    c->mParticleId0 = Particle::System::New(c->mParticleId0, 0x77, pos.x, pos.y, pos.z, 0, 0);
    c->mParticleId1 = Particle::System::New(c->mParticleId1, 0x78, pos.x, pos.y, pos.z, 0, 0);
    if (c->mParticleId0 != 0) {
        Particle::System *sys = Particle::System::FromUniqueID(c->mParticleId0);
        if (sys != 0)
            sys->emitRadius = c->mScaleX * 0x14;
    }
    if (c->mParticleId1 == 0) return;
    {
        Particle::System *sys = Particle::System::FromUniqueID(c->mParticleId1);
        if (sys != 0)
            sys->emitRadius = c->mScaleX * 0x14;
    }
}

/* Called where the boss lands or steps (a reading from the callers). Shakes the camera by `strength` (dCamera_c::func_0200d8c8; callers pass
   0x3e8000 to 0x1388000), then takes the translation of bone mStepBoneIdx
   times the model matrix, shifted left by 3 (the matrix words are held >> 3),
   and flips mStepBoneIdx between 2 and 3 for the next call. In the chase
   state (..3360) that position gets HugeLandingDustAt(pos, 1). In any other
   state the bone position is computed but not used: particles 0x88 and 0x89
   go off at mPos instead, moved 230.0 back (z = -0xe6000, rotated by mAngleY)
   when the state is ..33c0 or ..33f0. */
void daKing_Donketu_c::SpawnLandingEffects(int strength) {
    Vector3 bonePos;
    Vector3 pos;
    Vector3 offset;
    Vector3 rotated;
    Vector3 shifted;
    Vector3 dustPos;
    gCamera->func_0200d8c8(&mPos, strength);
    data_020a0e68 = mBlendAnmModel.mat4x3;
    MTX_Concat43(&mBlendAnmModel.data.transforms[mStepBoneIdx], &data_020a0e68, &data_020a0e68);
    mStepBoneIdx ^= 1;
    bonePos.x = data_020a0e68.m[9];
    bonePos.y = data_020a0e68.m[10];
    bonePos.z = data_020a0e68.m[11];
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
        Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
        MTX_MultVec43(&offset, &data_020a0e68, &rotated);
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
void daKing_Donketu_c::SpawnHitParticles(void *pb) {
    dActor_c *b = (dActor_c *)pb;
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
       (0x64000) above the boss, each with mPrevAngleY a multiple of 0x1000
       and mHorzSpeed 0xa000 (10.0). While SaveData character 2 is unlocked each
       coin counts into mCoinsSpawned and the one that takes it past 0x1e also
       spawns a SCALEUP_KINOKO and resets the count. Then the boss goes to
       ..33a0 unless it is already in ..33a0 or ..33d0.
     - With no hit, daPly_c::Hurt is tried on the other actor. If it hurt: a
       particle 0x8a at the other actor's position, and in ..33d0
       SND_HIT_REGISTERED plays and the function returns 1. Elsewhere the boss turns mPrevAngleY by half a turn
       (0x8000), plays SND_REVERSE_AFTER_HURT, goes to ..3360 with mPhase 2,
       mStateParam 0x2000 and mStateTimer 0x1e, and the function still returns 0. */
s32 daKing_Donketu_c::HandleHit() {
    daKing_Donketu_c *c = this;
    dActor_c* target;
    s32 hit;
    u32 id;

    if (DecIfAbove0_Short(&c->mHitCooldown) != 0)
        return 0;
    id = c->mdCcAcPos_c.otherOwner;
    if (id == 0)
        return 0;
    target = dActor_c::FindWithID(id);
    if (!target)
        return 0;

    hit = 0;
    if (c->mdCcAcPos_c.hitFlags & 0x6000) {
        c->SpawnHitParticles(c);
        c->mHorzSpeed = 0x20000;
        hit = 1;
    }

    {
        int isBf = (int)(target->actorID == ACTOR_PLAYER);
        if (isBf == 1) {
            if (((daPly_c *)target)->mIsMega != 0) {
                s32 flags = c->mdCcAcPos_c.hitFlags & 0x107e0;
                if (flags) {
                    c->mHorzSpeed = 0x41000;
                    hit = 1;
                }
            }
            if (hit == 0) {
                s32 flags = c->mdCcAcPos_c.hitFlags & 0x50380;
                if (flags) {
                    c->SpawnHitParticles(c);
                    c->mHorzSpeed = 0x2d000;
                    hit = 1;
                }
            }
            if (hit == 0) {
                if ((c->mdCcAcPos_c.hitFlags & 0x70) || (((daPly_c *)target)->mIsMetal != 0) ||
                    (c->JumpedOnByPlayer(c->mdCcAcPos_c, *(daPly_c *)target) != 0)) {
                    c->SpawnHitParticles(c);
                    c->mHorzSpeed = 0x20000;
                    hit = 1;
                }
            }
        }
    }

    if (hit == 0) {
        if (c->mdCcAcPos_c.hitFlags & 0x400) {
            c->SpawnHitParticles(c);
            c->mHorzSpeed = 0x3d000;
            hit = 1;
        }
    }

    if (hit != 0) {
        s32 count;
        s32 i;
        s32 shortY;
        Vector3 v;
        s32 rnd;

        c->mHitCooldown = 0x10;
        func_02012694(SND_HIT_REGISTERED, &c->mCamSpacePosX);
        rnd = RandomIntInternal(&data_0209e650.value);
        if (((rnd >> 8) & 7) == 0) {
            v.x = c->mPosX;
            v.y = c->mPosY;
            v.z = c->mPosZ;
            count = (rnd >> 0xc) & 3;
            v.y = v.y + 0x64000;
            if (count == 0)
                count = 1;
            i = 0;
            if (count > 0) {
                do {
                    dActor_c* actor = dActor_c::Spawn(ACTOR_COIN, 0, v, 0, c->mAreaId, -1);
                    if (actor != 0) {
                        rnd = RandomIntInternal(&data_0209e650.value);
                        shortY = ((s32)((((u32)rnd >> 8) & 0xf) << 0x1c)) >> 0x10;
                        actor->mPrevAngleX = 0;
                        actor->mPrevAngleY = (s16)shortY;
                        actor->mPrevAngleZ = 0;
                        actor->mHorzSpeed = 0xa000;
                        if (SaveData::IsCharacterUnlocked(2) != 0) {
                            c->mCoinsSpawned += 1;
                            if (c->mCoinsSpawned > 0x1e) {
                                dActor_c* actor2 = dActor_c::Spawn(ACTOR_SCALEUP_KINOKO, 0, v, 0, c->mAreaId, -1);
                                if (actor2 != 0) {
                                    actor2->mPrevAngleX = 0;
                                    actor2->mPrevAngleY = (s16)shortY;
                                    actor2->mPrevAngleZ = 0;
                                    actor2->mHorzSpeed = 0xa000;
                                }
                                c->mCoinsSpawned = 0;
                            }
                        }
                    }
                    i = i + 1;
                } while (i < count);
            }
        }
        {
            void* anim = c->mState;
            if (anim != &data_ov073_021233d0 && anim != &data_ov073_021233a0) {
                ChiefChilly_ChangeState((C *)(c), (PMF *)&data_ov073_021233a0);
            }
        }
        return 1;
    }

    {
        Vector3 v2;
        v2.x = c->mPosX;
        v2.y = c->mPosY;
        v2.z = c->mPosZ;
        if (((daPly_c *)target)->Hurt(v2, 0, 0x14000, 1, 0, 1) == 0)
            goto done0;
    }
    {
        s32* pv = &target->mPosX;
        Particle::System::NewSimple(kPtclChiefChillyHitPlayer, pv[0], pv[1], pv[2]);
    }
    if (c->mState == &data_ov073_021233d0) {
        func_02012694(SND_HIT_REGISTERED, &c->mCamSpacePosX);
        return 1;
    }
    {
        s16* py = &c->mPrevAngleY;
        *py = (s16)(*py + 0x8000);
    }
    func_02012694(SND_REVERSE_AFTER_HURT, &c->mCamSpacePosX);
    ChiefChilly_ChangeState((C *)(c), (PMF *)&data_ov073_02123360);
    c->mBlendAnmModel.SetAnim(*(BCA_File *)((BCA_File *)gKingIceDonketuHitAnimationFile.filePtr), 4,
                              0x40000000, 0x1000, 0);
    c->mBlendAnmModel.speed = 0x1000;
    c->mPhase = 2;
    c->mStateParam = 0x2000;
    c->mStateTimer = 0x1e;
done0:
    return 0;
}

/* ..3370 update, the defeat. mScaleX is approached toward 0 by 0x80 a frame
   and copied to Y and Z, with those effects kept alive. Once mScaleX is below 0x100
   (1/16) the boss ends the talk, stops the layer-3 music, restores the music
   volume, spawns OBJ_KEY 400.0 (0x190000) above mSpawnPos, plays SND_POOF,
   and if an OBJ_BLUE_FIRE actor exists puts a puff of dust at it and marks it
   for destruction. If the key spawned, camera flag 0x8 is cleared and the
   boss marks itself for destruction. */
int daKing_Donketu_c::UpdateDefeat() {
    daKing_Donketu_c *c = this;
    dCamera_c *cam;
    dActor_c* spawned;
    dActor_c* found;
    volatile struct Vector3_16 rot;
    struct Vector3 pos;
    cam = gCamera;
    cam->SetFlag_3();
    ApproachLinear(*(int *)&c->mScaleX, 0, 0x80);
    c->mScaleZ = c->mScaleX;
    c->mScaleY = c->mScaleZ;
    c->RenewLoopedParticles();
    if (c->mScaleX >= 0x100) goto end;
    pos.x = c->mSpawnPos.x;
    pos.y = c->mSpawnPos.y;
    pos.z = c->mSpawnPos.z;
    Message::EndTalk();
    Sound::StopLoadedMusic_Layer3();
    func_02011cfc();
    Sound::ChangeMusicVolume(0x7f, 0x15666);
    pos.y = pos.y + 0x190000;
    c->func_0200fa8c(1);

    {
        unsigned short a = *(unsigned short *)&c->mAngleX;
        unsigned short b = *(unsigned short *)&c->mAngleY;
        rot.x = b ? a : a;
        rot.y = b;
        rot.z = *(unsigned short *)&c->mAngleZ;
        rot.y = (unsigned short)c->HorzAngleToCPlayer();
    }

    spawned = dActor_c::Spawn(ACTOR_OBJ_KEY, 4, pos, 0, c->mAreaId, -1);
    found = dActor_c::FindWithActorID(ACTOR_OBJ_BLUE_FIRE, 0);
    func_02012694(SND_POOF, &c->mCamSpacePosX);
    if (found != 0) {
        struct Vector3 fp;
        const s32* pv = &found->mPosX;
        fp.x = pv[0];
        fp.y = pv[1];
        fp.z = pv[2];
        c->PoofDustAt(fp);
        found->MarkForDestruction();
    }
    if (spawned != 0) {
        cam->mFlags &= ~8;
        c->MarkForDestruction();
    }
end:
    return 1;
}

/* ..3370 enter: nothing to do. */
int daKing_Donketu_c::func_ov073_0211fbec()
{
    return 1;
}

/* ..3410 update, cut scene part 3: keeps the looped sound, the camera and the
   effects going until mTalkPlayer's talk state reads -1, then goes to ..3370. */
int daKing_Donketu_c::UpdateDefeatCutscene3() {
    daKing_Donketu_c *c = this;
    daPly_c *pl = c->mTalkPlayer;
    c->mSoundHandle = Sound::PlayLong(c->mSoundHandle, 3, 0x170, *(Vector3 *)&c->mCamSpacePos, 0);
    gCamera->SetFlag_3();
    c->RenewLoopedParticles();
    if (pl->GetTalkState() == -1) {
        ChiefChilly_ChangeState((C *)(c), (PMF *)&data_ov073_02123370);
    }
    return 1;
}

/* ..3410 enter: nothing to do. */
int daKing_Donketu_c::func_ov073_0211fc70()
{
    return 1;
}

/* ..33e0 update, cut scene part 2. Returns at once if there is no player. The
   camera looks at mSpawnPos lowered 768.0 (0x300000) and sits at mPos raised
   512.0 (0x200000), moved 1374.0 (0x55e000) back along z rotated by mAngleY.
   The player is stored in mTalkPlayer; the music volume is set down and once
   StartTalk accepts, ShowMessage shows message (player's param1 + 0xe7)
   at mPos. When it finishes SND_TALK_DONE plays and the state becomes ..3410. */
int daKing_Donketu_c::UpdateDefeatCutscene2() {
    daKing_Donketu_c *c = this;
    struct Vector3 msgpos[2];
    struct Vector3 la, ps, in, out;
    daPly_c *player;
    dCamera_c* cam;

    player = c->ClosestPlayer();
    c->mSoundHandle = Sound::PlayLong(c->mSoundHandle, 3, 0x170, *(Vector3 *)&c->mCamSpacePos, 0);
    if (player == 0) return 1;

    c->RenewLoopedParticles();

    {
        const s32* pv = &player->mPosX;
        msgpos[0].x = pv[0];
        msgpos[0].y = pv[1];
        msgpos[0].z = pv[2];
    }
    cam = gCamera;
    msgpos[1].x = c->mPosX;
    msgpos[1].y = c->mPosY;
    msgpos[1].z = c->mPosZ;

    cam->SetFlag_3();

    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;

    la.x = c->mSpawnPos.x;
    la.y = c->mSpawnPos.y;
    la.z = c->mSpawnPos.z;
    ps.x = c->mPosX;
    ps.y = c->mPosY;
    ps.z = c->mPosZ;

    la.y -= 0x300000;
    in.z = -0x55e000;

    Matrix4x3_FromRotationY(&data_020a0e68, c->mAngleY);
    MTX_MultVec43((Vector3 *)&in, &data_020a0e68, (Vector3 *)&out);

    ps.x = ps.x + out.x;
    ps.y = ps.y + 0x200000;
    ps.z = ps.z + out.z;

    cam->SetLookAt(la);
    cam->SetPos(ps);

    if (player != 0) {
        int msg;
        c->mTalkPlayer = player;
        msg = (short)(c->mTalkPlayer->param1 + 0xe7);
        Sound::ChangeMusicVolume(0x14, 0x15666);
        Message::PrepareTalk();
        if (c->mTalkPlayer->StartTalk(*c, 1)) {
            cam->SetFlag_3();
            if (c->mTalkPlayer->ShowMessage(*c, msg, &msgpos[1], 0, 2)) {
                func_02012694(SND_TALK_DONE, (void*)&c->mCamSpacePosX);
                ChiefChilly_ChangeState((C *)(c), (PMF *)(&data_ov073_02123410));
            }
        }
    }
    return 1;
}

/* ..33e0 enter: nothing to do. */
int daKing_Donketu_c::func_ov073_0211fe84()
{
    return 1;
}

/* ..33b0 update, cut scene part 1. The nearest player, when it is not
   airborne, is put into a talk and its angles set to face the boss (the
   boss-to-player angle + 0x8000). The 0x77 / 0x78 effects are renewed and the camera looks at
   a point 256.0 (0x100000) above the boss from 256.0 below mSpawnPos, moved
   1436.0 (0x59c000) back along z rotated by mAngleY. When mStateTimer reaches
   0 the state becomes ..33e0. */
int daKing_Donketu_c::UpdateDefeatCutscene1() {
    daKing_Donketu_c *c = this;
    struct Vector3 look, pos, in, out;
    daPly_c *player;
    dCamera_c* cam;

    c->mSoundHandle = Sound::PlayLong(c->mSoundHandle, 3, 0x170, *(Vector3 *)&c->mCamSpacePos, 0);

    player = c->ClosestPlayer();
    if (player != 0 && player->mIsAirborne == 0) {
        int angle;
        player->StartTalk(*c, 1);
        angle = c->HorzAngleToCPlayer();
        player->mAngleX = 0;
        player->mAngleY = angle + 0x8000;
        player->mAngleZ = 0;
    }

    c->RenewLoopedParticles();

    cam = gCamera;
    cam->SetFlag_3();

    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;

    look.x = c->mPosX;
    look.y = c->mPosY;
    look.z = c->mPosZ;
    pos.x = c->mSpawnPos.x;
    pos.y = c->mSpawnPos.y;
    pos.z = c->mSpawnPos.z;

    look.y += 0x100000;
    in.z = -0x59c000;

    Matrix4x3_FromRotationY(&data_020a0e68, c->mAngleY);
    MTX_MultVec43((Vector3 *)&in, &data_020a0e68, (Vector3 *)&out);

    pos.y -= 0x100000;
    pos.x = pos.x + out.x;
    pos.z = pos.z + out.z;

    cam->SetLookAt(look);
    cam->SetPos(pos);

    if ((unsigned short)c->mStateTimer == 0) {
        ChiefChilly_ChangeState((C *)(c), (PMF *)(&data_ov073_021233e0));
    }
    return 1;
}

/* ..33b0 enter: start the animation in gKingIceDonketuDeadAnimationFile.filePtr;
   mStateTimer = 0x32 (50 frames). */
short daKing_Donketu_c::EnterDefeatCutscene1() {
    daKing_Donketu_c *c = this;
    c->mBlendAnmModel.SetAnim(*(BCA_File *)((BCA_File *)gKingIceDonketuDeadAnimationFile.filePtr),
                              4, 0, 0x1000, 0);
    c->mStateTimer = 0x32;
    return 1;
}

/* ..3380 update: when the animation finishes, mLandedWaypoint = 0xff and back to the
   chase (..3360). */
int daKing_Donketu_c::UpdateJumpEnd() {
    daKing_Donketu_c *c = this;
    if (c->mBlendAnmModel.Finished()) {
        c->mLandedWaypoint = 0xff;
        ChiefChilly_ChangeState((C *)(c), (PMF *)&data_ov073_02123360);
    }
    return 1;
}

/* ..3380 enter: clear mEdgeRecovered and start the animation in
   (BCA_File *)gKingIceDonketuJumpEdAnimationFile.filePtr. */
struct BCA_File;
int daKing_Donketu_c::EnterJumpEnd() {
    mEdgeRecovered = 0;
    mBlendAnmModel.SetAnim(*(BCA_File *)((BCA_File *)gKingIceDonketuJumpEdAnimationFile.filePtr), 4,
                           0x40000000, 0x1000, 0);
    return 1;
}

/* ..3340 update, a leap toward mSpawnPos. mStateTimer counts down from 10
   (set by the enter handler). At 1 the leap's animation starts: mVertSpeed
   0x46000 (70.0), mVertAccel -0x3000 (-3.0), and SND_HOP_LAUNCH if mEdgeRecovered is
   set. At 0, while mStateScratch is 0, mAngleX is straightened (0x2000 a
   frame), mPrevAngleY turns toward the angle to mSpawnPos by 0x800, and the
   x/z velocity words mSpeedX / mSpeedZ are set to 50.0 (0x32000) when
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
            mBlendAnmModel.SetAnim(
                *(BCA_File *)((BCA_File *)gKingIceDonketuJumpStAnimationFile.filePtr), 4,
                0x40000000, 0x1000, 0);
            mBlendAnmModel.speed = 0x1000;
            mVertSpeed = 0x46000;
            mLandedWaypoint = 0xff;
            mVertAccel = -0x3000;
            if (mEdgeRecovered != 0) {
                func_02012694(SND_HOP_LAUNCH, &mCamSpacePosX);
            }
        }
        return 1;
    }
    /* state == 0 */
    if (mStateScratch == 0) {
        VecFx32 offset;
        VecFx32 rotated;
        s16 horz;
        ApproachLinear(mAngleX, 0, 0x2000);
        horz = Vec3_HorzAngle(&mPos, &mSpawnPos);
        Vec3_VertAngle(&mPos, &mSpawnPos);
        ApproachLinear(mPrevAngleY, horz, 0x800);
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
        Matrix4x3_FromRotationY(&data_020a0e68, horz);
        MTX_MultVec43((Vector3 *)&offset, &data_020a0e68, (Vector3 *)&rotated);
        mSpeedX = rotated.x;
        mSpeedZ = rotated.z;
        if (mVertSpeed < 0) {
            int id = mdCcAcPos_c.otherOwner;
            if (id != 0) {
                dActor_c* actor = dActor_c::FindWithID((u32)id);
                if (actor != 0) {
                    enum Bool eq = (enum Bool)(actor->actorID == ACTOR_PLAYER);
                    if (eq != FALSE) {
                        VecFx32 pos = *(VecFx32 *)&actor->mPosX;
                        if (mPosY > pos.y) {
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
            mSpeedX = 0;
            mVertSpeed = 0;
            mSpeedZ = 0;
            SpawnLandingEffects(0x1388000);
            func_02012694(SND_HOP_LANDING, &mCamSpacePosX);
        }
        if (mHitsRemaining == 1 && mEdgeRecovered == 0) {
            mLandedWaypoint = 0;
            if (mStateScratch == 0) {
                mBlendAnmModel.SetAnim(
                    *(BCA_File *)((BCA_File *)gKingIceDonketuJumpEdAnimationFile.filePtr), 4,
                    0x40000000, 0x1000, 0);
            }
            mStateScratch += 1;
            if (mStateScratch < 0x82) {
                return 1;
            }
        }
        ChiefChilly_ChangeState((C *)this, (PMF *)&data_ov073_02123380);
    }
    mAngleY = mPrevAngleY;
    return 1;
}

/* ..3340 enter: mStateScratch = 0; mStateTimer = 10. */
int daKing_Donketu_c::EnterLeapToSpawn() {
    daKing_Donketu_c *p = this;
    p->mStateScratch = 0;
    p->mStateTimer = 10;
    return 1;
}

/* ..3320 update, a waypoint hop. mStateTimer starts at 2. At 1 the hop
   launches: mVertSpeed 0x5a000 (90.0), mVertAccel -0x5000 (-5.0), mWaypointCursor
   advances modulo 8 and the animation in gKingIceDonketuDamageAnimationFile.filePtr runs at speed
   0x2000. At 0 the boss steers toward the current waypoint (set B while
   mHitsRemaining is 2, else set A): mPrevAngleY turns toward it by 0x800 and
   the x/z velocity words are set to 80.0 (0x50000) along the heading and
   pitch. On landing it copies the cursor to mLandedWaypoint, stops, shakes the camera
   (0x7d0000), plays SND_HOP_LANDING and counts the landing in mJumpCount: past
   7 the state becomes ..3340, otherwise it re-enters ..3320. */
int daKing_Donketu_c::UpdateWaypointHop() {
    daKing_Donketu_c *c = this;
    int *src;
    Vector3 v;

    if ((unsigned short)c->mStateTimer == 0)
        goto mainblock;

    if ((unsigned short)c->mStateTimer == 1) {

        int neg = 0x5000;
        c->mVertSpeed = 0x5a000;
        c->mVertAccel = -neg;
        {
            unsigned char *p = (unsigned char *)&c->mWaypointCursor;
            *p = *p + 1;
            *p = *p & 7;
        }

        c->mBlendAnmModel.SetAnim(
            *(BCA_File *)((BCA_File *)gKingIceDonketuDamageAnimationFile.filePtr), 4, 0, 0x1000, 0);
        c->mBlendAnmModel.speed = 0x2000;
    }
    return 1;

mainblock:
    if (c->mHitsRemaining == 2) {
        src = (int *)&c->mWaypointsB[c->mWaypointCursor];
        v.x = src[0];
        v.y = src[1];
        v.z = src[2];
    } else {
        src = (int *)&c->mWaypointsA[c->mWaypointCursor];
        v.x = src[0];
        v.y = src[1];
        v.z = src[2];
    }
    {
        short hz = Vec3_HorzAngle(&c->mPos, &v);
        short vt = Vec3_VertAngle(&c->mPos, &v);
        int in[3];
        int out[3];
        ApproachLinear(c->mPrevAngleY, hz, 0x800);
        in[0] = 0; in[1] = 0; in[2] = 0;
        out[0] = 0; out[1] = 0; out[2] = 0;
        in[2] = 0x50000;
        Matrix4x3_FromRotationY(&data_020a0e68, hz);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, vt);
        MTX_MultVec43((Vector3 *)in, &data_020a0e68, (Vector3 *)out);
        c->mSpeedX = out[0];
        c->mSpeedZ = out[2];
    }
    if (c->mWithMeshClsn.IsOnGround() != 0) {
        c->mLandedWaypoint = c->mWaypointCursor;
        c->mHorzSpeed = 0;
        c->mSpeedX = 0;
        c->mVertSpeed = 0;
        c->mSpeedZ = 0;
        c->SpawnLandingEffects(0x7d0000);
        func_02012694(SND_HOP_LANDING, &c->mCamSpacePosX);
        {
            int *cnt = (int *)&c->mJumpCount;
            *cnt = *cnt + 1;
        }
        if (c->mJumpCount > 7) {
            ChiefChilly_ChangeState((C *)(c), (PMF *)&data_ov073_02123340);
        } else {
            ChiefChilly_ChangeState((C *)(c), (PMF *)&data_ov073_02123320);
        }
    }
    c->mAngleY = c->mPrevAngleY;
    return 1;
}

/* ..3320 enter: animation speed 0; mStateScratch = 0; mStateTimer = 2. */
int daKing_Donketu_c::EnterWaypointHop() {
    daKing_Donketu_c *p = this;
    p->mBlendAnmModel.speed = 0;
    p->mStateScratch = 0;
    p->mStateTimer = 2;
    return 1;
}

/* ..3400 update: a waypoint hop like ..3320, but it also straightens mAngleX
   (0x2000 a frame), never turns mPrevAngleY toward the waypoint, and at
   timer 1 it only sets
   mVertSpeed 0x5a000 (90.0) and the animation (no gravity change, no cursor
   advance), and at timer 0 the aim speed is 40.0 (0x28000). Once mStateScratch
   is set or the boss is falling, mVertSpeed also follows the aim. The landing
   sets mJumpCount to 1 and goes to ..3320. */
int daKing_Donketu_c::func_ov073_02120610() {
    daKing_Donketu_c *c = this;
    int *src;
    Vector3 v;

    if ((unsigned short)c->mStateTimer == 0)
        goto mainblock;

    if ((unsigned short)c->mStateTimer == 1) {
        c->mVertSpeed = 0x5a000;
        c->mBlendAnmModel.SetAnim(
            *(BCA_File *)((BCA_File *)gKingIceDonketuDamageAnimationFile.filePtr), 4, 0, 0x1000, 0);
        c->mBlendAnmModel.speed = 0x2000;
    }
    return 1;

mainblock:
    ApproachLinear(c->mAngleX, 0, 0x2000);
    if (c->mHitsRemaining == 2) {
        src = (int *)&c->mWaypointsB[c->mWaypointCursor];
        v.x = src[0];
        v.y = src[1];
        v.z = src[2];
    } else {
        src = (int *)&c->mWaypointsA[c->mWaypointCursor];
        v.x = src[0];
        v.y = src[1];
        v.z = src[2];
    }
    {
        short hz = Vec3_HorzAngle(&c->mPos, &v);
        short vt = Vec3_VertAngle(&c->mPos, &v);
        int in[3];
        int out[3];
        in[2] = 0;
        in[0] = 0;
        in[1] = 0;
        out[0] = 0;
        out[1] = 0;
        out[2] = 0;
        in[2] = 0x28000;
        Matrix4x3_FromRotationY(&data_020a0e68, hz);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, vt);
        MTX_MultVec43((Vector3 *)in, &data_020a0e68, (Vector3 *)out);
        c->mSpeedX = out[0];
        if (c->mStateScratch != 0 || c->mVertSpeed < 0) {
            c->mVertSpeed = out[1];
            c->mStateScratch = 1;
        }
        c->mSpeedZ = out[2];
    }
    if (c->mWithMeshClsn.IsOnGround() != 0) {
        c->mLandedWaypoint = c->mWaypointCursor;
        c->mHorzSpeed = 0;
        c->mSpeedX = 0;
        c->mVertSpeed = 0;
        c->mSpeedZ = 0;
        c->SpawnLandingEffects(0x7d0000);
        func_02012694(SND_HOP_LANDING, &c->mCamSpacePosX);
        c->mJumpCount = 1;
        ChiefChilly_ChangeState((C *)(c), (PMF *)&data_ov073_02123320);
    }
    c->mAngleY = c->mPrevAngleY;
    return 1;
}

/* ..3400 enter: mStateTimer = 10, mStateScratch = 0, mJumpCount = 0,
   mLandedWaypoint = 0xff. */
int daKing_Donketu_c::func_ov073_0212081c() {
    daKing_Donketu_c *self = this;
    self->mStateTimer = 0xa;
    self->mStateScratch = 0;
    self->mJumpCount = 0;
    self->mLandedWaypoint = 0xff;
    return 1;
}

/* ..33f0 update: mAngleX approaches -0x4000 (a quarter turn) by 0x400 a frame
   with the looped sound playing. Once the boss is below mSpawnPos.y and on the
   ground it shakes the camera (0x7d0000), plays SND_BIG_LANDING, zeroes
   mHorzSpeed and goes to ..33b0. */
int daKing_Donketu_c::func_ov073_02120844()
{
    daKing_Donketu_c *t = this;
    ApproachLinear(t->mAngleX, -0x4000, 0x400);
    t->mSoundHandle = Sound::PlayLong(t->mSoundHandle, 3, 0x170, *(Vector3 *)&t->mCamSpacePos, 0);
    if (t->mSpawnPos.y > t->mPosY && t->mWithMeshClsn.IsOnGround()) {
        t->SpawnLandingEffects(0x7d0000);
        func_02012694(SND_BIG_LANDING, &t->mCamSpacePosX);
        t->mHorzSpeed = 0;
        ChiefChilly_ChangeState((C *)(t), (PMF *)&data_ov073_021233b0);
    }
    return 1;
}

/* ..33f0 enter: mHorzSpeed 0x14000 (20.0), mVertSpeed 0x1e000 (30.0),
   mVertAccel -0x3000 (-3.0); clears the sound handle. */
int daKing_Donketu_c::func_ov073_021208e4() {
    daKing_Donketu_c *self = this;
    self->mHorzSpeed = 0x14000;
    self->mVertSpeed = 0x1e000;
    self->mVertAccel = -0x3000;
    self->mSoundHandle = 0;
    return 1;
}

/* ..33d0 update. Gravity, mHorzSpeed and the velocity words are zeroed. When
   mStateTimer runs out (the enter handler sets it to 0x5a) mHitsRemaining
   becomes 1, mEdgeRecovered becomes 1 and the state is ..3340. Otherwise a looped
   sound plays and the boss moves toward the point 120.0 (0x78000) from the
   saved miss position (mEdgePos) along mAngleY + 0x8000, at 20.0 (0x14000)
   a frame; mAngleY becomes the angle to mSpawnPos, mPrevAngleY that + 0x8000.
   mStateScratch advances 0x500 a frame and mAngleX approaches mStateParam times
   the sine of it (Fix12) by 0x400. Then,
   when HandleHit returns nonzero (the boss took a hit or, in this
   state, hurt the player) the state becomes ..33f0. */
int daKing_Donketu_c::UpdateEdgeWobble() {
    daKing_Donketu_c *c = this;
    Vector3 in;
    Vector3 out;

    c->mVertAccel = 0;
    c->mHorzSpeed = 0;
    c->mSpeedX = 0;
    c->mVertSpeed = 0;
    c->mSpeedZ = 0;

    if ((unsigned short)c->mStateTimer == 0) {
        *&c->mHitsRemaining = 1;
        ChiefChilly_ChangeState((C *)(c), (PMF *)&data_ov073_02123340);
        *&c->mEdgeRecovered = 1;
        c->mPrevAngleY = c->mAngleY;
        return 1;
    }

    c->mSoundHandle = Sound::PlayLong(c->mSoundHandle, 3, 0x16e, *(Vector3 *)&c->mCamSpacePos, 0);

    in.z = 0;
    in.z = 0x78000;
    in.x = 0;
    in.y = 0;
    out.x = 0;
    out.y = 0;
    out.z = 0;
    Matrix4x3_FromRotationY(&data_020a0e68, c->mAngleY + 0x8000);
    MTX_MultVec43((Vector3 *)&in, &data_020a0e68, (Vector3 *)&out);
    in.x = c->mEdgePosX + out.x;
    in.y = c->mEdgePosY;
    in.z = c->mEdgePosZ + out.z;
    Vec3_ApproachHorz(&c->mPosX, &in, 0x14000);
    c->mAngleY = Vec3_HorzAngle(&c->mPos, &c->mSpawnPos);
    c->mPrevAngleY = c->mAngleY + 0x8000;
    c->mStateScratch += 0x500;
    ApproachLinear(c->mAngleX, FX_MUL(c->mStateParam, FX_SinIdx((u16)(s16)c->mStateScratch >> 4)),
                   0x400);
    if (c->HandleHit()) {
        ChiefChilly_ChangeState((C *)(c), (PMF *)&data_ov073_021233f0);
    }
    return 1;
}

/* ..33d0 enter: animation gKingIceDonketuWalkAnimationFile.filePtr at speed 0x4000; mAngleX,
   mStateScratch and all the motion words cleared; mStateParam = -0x1000;
   mGroundMissPos copied into mEdgePos; sound handle cleared;
   mStateTimer = 0x5a (90 frames). */
int daKing_Donketu_c::EnterEdgeWobble() {
    daKing_Donketu_c *t = this;
    t->mBlendAnmModel.SetAnim(*(BCA_File *)((BCA_File *)gKingIceDonketuWalkAnimationFile.filePtr),
                              4, 0, 0x1000, 0);
    t->mBlendAnmModel.speed = 0x4000;
    t->mAngleX = 0;
    t->mStateScratch = 0;
    t->mVertAccel = 0;
    t->mHorzSpeed = 0;
    t->mSpeedX = 0;
    t->mVertSpeed = 0;
    t->mSpeedZ = 0;
    t->mStateParam = -0x1000;
    t->mEdgePosX = t->mGroundMissPosX;
    t->mEdgePosY = t->mGroundMissPosY;
    t->mEdgePosZ = t->mGroundMissPosZ;
    t->mSoundHandle = 0;
    t->mStateTimer = 0x5a;
    return 1;
}

/* ..33c0 update: mAngleX approaches -0x4000 by 0x400 a frame. Once the boss is
   below mSpawnPos.y and on the ground it shakes the camera (0xfa0000), stops,
   turns to face mSpawnPos (both angles), plays SND_BIG_LANDING and goes to
   ..3400. */
int daKing_Donketu_c::func_ov073_02120b78() {
    daKing_Donketu_c *c = this;
    ApproachLinear(c->mAngleX, -0x4000, 0x400);
    if (c->mSpawnPos.y > c->mPosY) {
        if (c->mWithMeshClsn.IsOnGround()) {
            c->SpawnLandingEffects(0xfa0000);
            c->mHorzSpeed = 0;
            c->mPrevAngleY = Vec3_HorzAngle(&c->mPos, &c->mSpawnPos);
            c->mAngleY = c->mPrevAngleY;
            func_02012694(SND_BIG_LANDING, &c->mCamSpacePosX);
            ChiefChilly_ChangeState((C *)(c), (PMF *)&data_ov073_02123400);
        }
    }
    return 1;
}

/* ..33c0 enter: animation gKingIceDonketuWalkAnimationFile.filePtr at speed 0x4000;
   mHorzSpeed 0x14000 (20.0), mVertSpeed 0x1e000 (30.0), mVertAccel -0x3000
   (-3.0); mAngleX and mStateScratch cleared. */
int daKing_Donketu_c::func_ov073_02120c08()
{
    daKing_Donketu_c *t = this;
    t->mBlendAnmModel.SetAnim(*(BCA_File *)((BCA_File *)gKingIceDonketuWalkAnimationFile.filePtr),
                              4, 0, 0x1000, 0);
    t->mBlendAnmModel.speed = 0x4000;
    t->mHorzSpeed = 0x14000;
    t->mVertSpeed = 0x1e000;
    t->mAngleX = 0;
    t->mVertAccel = -0x3000;
    t->mStateScratch = 0;
    return 1;
}

/* ..33a0 update, the hit reaction. While |mHorzSpeed| > 0xa the dust follows
   mFootPos[0..1]. If mNoGroundAhead is 1 mHitsRemaining drops by one and the
   state becomes ..33c0 while it is nonzero, ..33d0 when it reaches 0.
   Otherwise mHorzSpeed is braked to 0 by 0x1000 a frame, and once the
   animation has finished with |mHorzSpeed| < 0xa the boss stops, takes mAngleY
   back into mPrevAngleY and returns to the chase (..3360). */
int daKing_Donketu_c::UpdateHitReaction() {
    daKing_Donketu_c *c = this;
    int a = c->mHorzSpeed; if (a < 0) a = -a;
    if (a > 0xa) {
        int i = 0;
        do {
            Particle::RunningSlidingDustAt(c->mFootPos[i].x, c->mFootPos[i].y, c->mFootPos[i].z);
            i++;
        } while (i < 2);
    }
    if (c->mNoGroundAhead == 1) {
        c->mHitsRemaining -= 1;
        if (c->mHitsRemaining != 0)
            ChiefChilly_ChangeState((C *)(c), (PMF *)&data_ov073_021233c0);
        else
            ChiefChilly_ChangeState((C *)(c), (PMF *)&data_ov073_021233d0);
        return 1;
    }
    ApproachLinear(*(int *)&c->mHorzSpeed, 0, 0x1000);
    if (c->mBlendAnmModel.Finished()) {
        int b = c->mHorzSpeed; if (b < 0) b = -b;
        if (b < 0xa) {
            c->mHorzSpeed = 0;
            c->mPrevAngleY = c->mAngleY;
            ChiefChilly_ChangeState((C *)(c), (PMF *)&data_ov073_02123360);
        }
    }
    return 1;
}

/* ..33a0 enter: animation gKingIceDonketuDamageAnimationFile.filePtr at speed 0x1000; mPrevAngleY
   = the angle to the player + 0x8000. */
int daKing_Donketu_c::EnterHitReaction() {
    daKing_Donketu_c *c = this;
    int fix;
    unsigned short t;
    short ang;

    fix = 0x1000;
    t = 0;
    c->mBlendAnmModel.SetAnim(*(BCA_File *)((BCA_File *)gKingIceDonketuDamageAnimationFile.filePtr),
                              4, 0x40000000, fix, t);
    c->mBlendAnmModel.speed = fix;
    ang = c->HorzAngleToCPlayer();
    short *py = &c->mPrevAngleY;
    c->mPrevAngleY = ang;
    *py =
        (short)((int)*py + 0x8000);
    return 1;
}

/* ..3390 update: mPrevAngleY turns toward mTargetAngle by 0x500 a frame. When
   mTargetAngle is within 0x100 of mAngleY the state becomes ..3360 with
   mStateTimer 0x1e (30 frames). The Prev angles are copied into the angles. */
int daKing_Donketu_c::UpdateTurnToSpawn() {
    daKing_Donketu_c *c = this;
    ApproachLinear(c->mPrevAngleY, c->mTargetAngle, 0x500);
    if (AngleDiff(c->mTargetAngle, c->mAngleY) < 0x100) {
        ChiefChilly_ChangeState((C *)(c), (PMF *)&data_ov073_02123360);
        c->mStateTimer = 0x1e;
    }
    c->mAngleX = c->mPrevAngleX;
    c->mAngleY = c->mPrevAngleY;
    c->mAngleZ = c->mPrevAngleZ;
    return 1;
}

/* ..3390 enter: animation gKingIceDonketuWalkAnimationFile.filePtr at speed 0x2000;
   mTargetAngle = the angle to mSpawnPos; mStateScratch = 0;
   mPrevAngleY = mAngleY. */
int daKing_Donketu_c::EnterTurnToSpawn() {
    daKing_Donketu_c *c = this;
    c->mBlendAnmModel.SetAnim(*(BCA_File *)((BCA_File *)gKingIceDonketuWalkAnimationFile.filePtr),
                              4, 0, 0x1000, 0);
    c->mBlendAnmModel.speed = 0x2000;
    c->mTargetAngle = Vec3_HorzAngle(&c->mPos, &c->mSpawnPos);
    c->mStateScratch = 0;
    c->mPrevAngleY = c->mAngleY;
    return 1;
}

/* ..3360 update, the chase, in legs selected by mPhase. mNoGroundAhead being
   1 sends the boss to ..3390 first.
     leg 0  with mStateTimer at 0: mTargetAngle = the angle to the player. If
            mPrevAngleY is within 0x2000 of it the timer is set to 0x1e and
            the leg continues; otherwise leg 1 with mStateParam = 0x1000 and
            SND_CHASE_BRAKE. While the timer runs: mHorzSpeed approaches
            0x1e000 (30.0) by 0x3000, mPrevAngleY turns toward mTargetAngle by
            0x1d0, and the first time the player is within 0x1f4000 (500.0)
            there is a one-in-eight chance of leg 3 (speed zeroed,
            mStateTimer 0xf).
     leg 3  when mStateTimer is 0: mStateParam = 0x1000, leg 1.
     legs 1, 2  mHorzSpeed is braked to 0 by mStateParam with the dust following
            while it is above 0xa; leg 1 also turns mAngleY toward the player
            by 0x500. When |mHorzSpeed| < 0xa and mStateTimer is 0, leg 2
            first retargets (to mSpawnPos if the player's hurt state reads 4 or
            5 or it is mIsNoControl, else to the player), then the boss stops,
            restarts the animation and returns to leg 0 with mStateTimer 0x1e.
   In leg 0 the model hitting animation frame 0 or 0xe shakes the camera
   (0x3e8000) and plays SND_CHASE_STEP. */
int daKing_Donketu_c::UpdateChase() {
    daKing_Donketu_c *c = this;

    if (c->mNoGroundAhead == 1) {
        ChiefChilly_ChangeState((C *)(c), (PMF *)&data_ov073_02123390);
        return 1;
    }

    switch (c->mPhase) {
    case 0:
        if ((unsigned short)c->mStateTimer == 0) {
            c->mTargetAngle = c->HorzAngleToCPlayer();
            c->mStateScratch = 0;
            if (AngleDiff(c->mPrevAngleY, c->mTargetAngle) <= 0x2000) {
                c->mStateTimer = 0x1e;
                break;
            } else {
                c->mStateTimer = 0;
                c->mStateParam = 0x1000;
                (c->mPhase)++;
                func_02012694(SND_CHASE_BRAKE, &c->mCamSpacePosX);
                break;
            }
        } else {
            ApproachLinear(*(int *)&c->mHorzSpeed, 0x1e000, 0x3000);
            if (c->mStateScratch == 0) {
                if (c->DistToCPlayer() < 0x1f4000) {
                    c->mStateScratch = 1;
                    if ((((unsigned int)RandomIntInternal(&data_0209e650.value) >> 0x18) & 7) ==
                        0) {
                        c->mPhase = 3;
                        c->mHorzSpeed = 0;
                        c->mStateTimer = 0xf;
                    }
                }
            }
            ApproachLinear(c->mPrevAngleY, c->mTargetAngle, 0x1d0);
            c->mAngleX = c->mPrevAngleX;
            c->mAngleY = c->mPrevAngleY;
            c->mAngleZ = c->mPrevAngleZ;
            break;
        }

    case 3:
        if ((unsigned short)c->mStateTimer == 0) {
            c->mStateParam = 0x1000;
            c->mPhase = 1;
        }
        break;

    case 1:
    case 2: {
        int d;
        int i;
        c->mStateScratch = 0;
        d = c->mHorzSpeed;
        if (d < 0)
            d = -d;
        if (d > 0xa) {
            i = 0;
            do {
                Particle::RunningSlidingDustAt(c->mFootPos[i].x, c->mFootPos[i].y,
                                               c->mFootPos[i].z);
                i++;
            } while (i < 2);
        }
        ApproachLinear(*(int *)&c->mHorzSpeed, 0, c->mStateParam);
        if (c->mPhase == 1) {
            c->mTargetAngle = c->HorzAngleToCPlayer();
            ApproachLinear(c->mAngleY, c->mTargetAngle, 0x500);
        }
        d = c->mHorzSpeed;
        if (d < 0)
            d = -d;
        if (d >= 0xa)
            break;
        if ((unsigned short)c->mStateTimer != 0)
            break;
        if (c->mPhase == 2) {
            daPly_c *p = c->ClosestPlayer();
            if (p != 0) {
                int t;
                if (p->GetHurtState() == 4)
                    goto hz;
                if (p->GetHurtState() == 5)
                    goto hz;
                t = p->mIsNoControl ? 1 : 0;
                if (t == 1) {
                hz:
                    c->mTargetAngle = Vec3_HorzAngle(&c->mPos, &c->mSpawnPos);
                } else {
                    c->mTargetAngle = c->HorzAngleToCPlayer();
                }
            }
        }
        c->mHorzSpeed = 0;
        c->mPrevAngleY = c->mAngleY;
        c->mStateTimer = 0x1e;
        c->mBlendAnmModel.SetAnim(
            *(BCA_File *)((BCA_File *)gKingIceDonketuWalkAnimationFile.filePtr), 4, 0, 0x1000, 0);
        c->mBlendAnmModel.speed = 0x2000;
        c->mStateScratch = 0;
        c->mPhase = 0;
        break;
    }
    }

    if (c->mPhase == 0) {
        if (c->mBlendAnmModel.WillHitFrame(0) != 0 || c->mBlendAnmModel.WillHitFrame(0xe) != 0) {
            c->SpawnLandingEffects(0x3e8000);
            func_02012694(SND_CHASE_STEP, &c->mCamSpacePosX);
        }
    }
    return 1;
}

/* ..3360 enter: mStateTimer = 0, mPhase = 0, mStateParam = 0x2000; animation
   (BCA_File *)gKingIceDonketuWalkAnimationFile.filePtr at speed 0x2000. */
int daKing_Donketu_c::EnterChase() {
    daKing_Donketu_c *t = this;
    t->mStateTimer = 0;
    t->mPhase = 0;
    t->mStateParam = 0x2000;
    t->mBlendAnmModel.SetAnim(*(BCA_File *)((BCA_File *)gKingIceDonketuWalkAnimationFile.filePtr),
                              4, 0, 0x1000, 0);
    t->mBlendAnmModel.speed = 0x2000;
    return 1;
}

/* ..3350 update. The camera looks at mPos raised 112.0 (0x70000) and moved
   672.0 (0x2a0000) in -z, from mPos moved 768.0 (0x300000) in -x, raised 32.0
   (0x20000), and moved in z by 0xffa34000, which as a signed word is -0x5cc000
   (-1484.0). When mTalkPlayer's talk state reads -1 camera flag 0x8 is
   cleared, layer-3 music 0x2d is loaded and set, func_02011d08 is called and
   the state becomes ..3360. */
int daKing_Donketu_c::UpdateIntroTalkEnd() {
    daKing_Donketu_c *c = this;
    struct Vector3 la;
    struct Vector3 ps;
    dCamera_c *cam;
    daPly_c *player;

    cam = gCamera;
    player = c->mTalkPlayer;
    cam->SetFlag_3();

    la.x = c->mPosX;
    la.y = c->mPosY;
    la.z = c->mPosZ;
    ps.x = c->mPosX;
    ps.y = c->mPosY;
    ps.z = c->mPosZ;
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
        ChiefChilly_ChangeState((C *)(c), (PMF *)&data_ov073_02123360);
    }
    return 1;
}

/* ..3350 enter: mPhase = 0. */
int daKing_Donketu_c::EnterIntroTalkEnd() {
    daKing_Donketu_c *p = this;
    p->mPhase = 0; return 1;
}

/* ..3330 update, the opening talk. Returns at once without a player. The
   camera is placed as in ..3350 and mPrevAngleY turns toward the player by
   0x800. The player is stored in mTalkPlayer and, once StartTalk accepts,
   layer-3 music 0x2c is loaded (nothing in this file changes mPhase while this
   state runs, so the test always passes) and ShowMessage runs for message (player's param1 + 0xe3) at mPos
   raised 100.0 (0x64000). When it finishes SND_TALK_DONE plays and the state
   becomes ..3350. */
int daKing_Donketu_c::UpdateIntroTalk() {
    daKing_Donketu_c *c = this;
    struct Vector3 vplayer;
    struct Vector3 vmsg;
    struct Vector3 la;
    struct Vector3 ps;
    daPly_c *player;
    dCamera_c *cam;
    int msg;
    daPly_c *p;

    player = c->ClosestPlayer();
    if (player == 0) return 1;

    {
        const s32* pv = &player->mPosX;
        vplayer.x = pv[0];
        vplayer.y = pv[1];
        vplayer.z = pv[2];
    }
    cam = gCamera;
    vmsg.x = c->mPosX;
    vmsg.y = c->mPosY;
    vmsg.z = c->mPosZ;

    cam->SetFlag_3();

    la.x = c->mPosX;
    la.y = c->mPosY;
    la.z = c->mPosZ;
    ps.x = c->mPosX;
    ps.y = c->mPosY;
    ps.z = c->mPosZ;
    la.y = la.y + 0x70000;
    la.z = la.z - 0x2a0000;
    ps.x = ps.x - 0x300000;
    ps.y = ps.y + 0x20000;
    ps.z = ps.z + 0xffa34000;

    cam->SetLookAt(la);
    cam->SetPos(ps);

    vmsg.y = vmsg.y + 0x64000;
    ApproachLinear(c->mPrevAngleY, Vec3_HorzAngle(&c->mPos, &vplayer), 0x800);

    c->mTalkPlayer = player;
    p = c->mTalkPlayer;
    msg = (short)(p->param1 + 0xe3);
    if (p->StartTalk(*c, 1)) {
        if (c->mPhase == 0) {
            Sound::LoadAndSetMusic_Layer3(0x2c);
            c->mPhase = 0;
        }

        if (c->mTalkPlayer->ShowMessage(*c, msg, &vmsg, 0, 2)) {
            func_02012694(SND_TALK_DONE, &c->mCamSpacePosX);
            ChiefChilly_ChangeState((C *)(c), (PMF *)&data_ov073_02123350);
        }
    }
    return 1;
}

/* ..3330 enter: mPhase = 0; animation gKingIceDonketuWaitAnimationFile.filePtr. */
int daKing_Donketu_c::EnterIntroTalk() {
    daKing_Donketu_c *c = this;
    c->mPhase = 0;
    c->mBlendAnmModel.SetAnim(*(BCA_File *)((BCA_File *)gKingIceDonketuWaitAnimationFile.filePtr),
                              4, 0, 0x1000, 0);
    return 1;
}

/* Stores the state record in mState (the shadow struct's pp, at +0x37c) and
   calls the enter member held in the record's first pointer-to-member; returns
   1 without calling if that pointer is null. */
extern "C" int ChiefChilly_ChangeState(C *c, PMF *p) { c->pp = p; PMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(); }

/* Called at the end of every Behavior. Builds the model matrix (mPos >> 3,
   rotated by mAngleX/Y/Z) into mBlendAnmModel.mat4x3, then sets mFootPos[0..1]
   to the world positions of bones 2 and 3 (bone matrix times the model
   matrix, translation words shifted left by 3). In ..33b0, ..33e0, ..3410,
   ..33f0 and ..3370 it stops there; in every other state it also draws the
   drop shadow from mShadowMtx (mPos >> 3 with y lowered 10.0 (0xa000)),
   radius 300.0 (0x12c000), height 1000.0 (0x3e8000). */
void daKing_Donketu_c::UpdateModelAndShadow() {
    daKing_Donketu_c *c = this;
    int sh;
    Vector3 v;
    VecFx32 out;
    void* m;
    Mtx43* saved;
    int i;
    VecFx32 *p;

    Vec3_Asr(&v, &c->mPos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, c->mAngleX, c->mAngleY, c->mAngleZ);

    p = (VecFx32 *)&c->mFootPos;
    c->mBlendAnmModel.mat4x3 = data_020a0e68;
    saved = (Mtx43 *)&c->mBlendAnmModel.mat4x3;
    sh = 3;
    i = 0;
    for (; i < 2; i++) {
        data_020a0e68 = *(struct Matrix4x3 *)saved;
        MTX_Concat43(&c->mBlendAnmModel.data.transforms[i + 2], &data_020a0e68, &data_020a0e68);
        p->x = data_020a0e68.m[9];
        p->y = data_020a0e68.m[10];
        p->z = data_020a0e68.m[11];
        Vec3_Lsl(&out, p, sh);
        c->mFootPos[i].x = out.x;
        c->mFootPos[i].y = out.y;
        c->mFootPos[i].z = out.z;
        p++;
    }

    m = c->mState;
    if (m == (void*)&data_ov073_021233b0) return;
    if (m == (void*)&data_ov073_021233e0) return;
    if (m == (void*)&data_ov073_02123410) return;
    if (m == (void*)&data_ov073_021233f0) return;
    if (m == (void*)&data_ov073_02123370) return;

    Matrix4x3_FromTranslation(&data_020a0e68, c->mPosX >> 3, (c->mPosY - 0xa000) >> 3, c->mPosZ >> 3);
    c->mShadowMtx = data_020a0e68;
    c->DropShadowRadHeight(c->mShadowModel, c->mShadowMtx, 0x12c000, 0x3e8000, 0xf);
}

int daKing_Donketu_c::CleanupResources() {
    UnloadKeyModels(4);
    ((SharedFilePtr *)&gKingIceDonketuDeadAnimationFile)->Release();
    ((SharedFilePtr *)&gKingIceDonketuHitAnimationFile)->Release();
    ((SharedFilePtr *)&gKingIceDonketuWalkAnimationFile)->Release();
    ((SharedFilePtr *)&gKingIceDonketuDamageAnimationFile)->Release();
    ((SharedFilePtr *)&gKingIceDonketuJumpEdAnimationFile)->Release();
    ((SharedFilePtr *)&gKingIceDonketuJumpStAnimationFile)->Release();
    ((SharedFilePtr *)&gKingIceDonketuWaitAnimationFile)->Release();
    ((SharedFilePtr *)data_ov002_0210da30)->Release();
    ((SharedFilePtr *)&gKingIceDonketuModelModelFile)->Release();
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
 *                      apply gravity by hand first: mVertSpeed becomes
 *                      max(mTerminalVelocity, mVertSpeed + mVertAccel)
 *   two states         at animation frame 7, build a world matrix from the
 *                      model's own bone and spawn the landing dust there --
 *                      the impact is tied to the ANIMATION, not to contact
 *   seven states       skip the ground ray entirely
 *   one more           is exempt from the rewind below
 *   two states         get an extra per-frame call
 *
 * THE GROUND RAY IS A FALL-THROUGH GUARD. It starts 120.0 (0x78000) above the
 * boss and runs along mPrevAngleY, tilted by 0x2000 (45 degrees) about X, for
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
 * folded (without it Behavior is 0x428, not 0x42c: angx's 0x2000 and the
 * data_020a0e68 address are folded instead of held in r4). It is bracketed to this one member: under `defer_codegen off` above,
 * the bracket binds positionally, so the members before and after it keep
 * the file's default propagation. The closing `#pragma pop` must NOT follow
 * Behavior's closing brace directly: the parser reads one token past the `}`
 * before generating the body, so a pop there is already in force when
 * Behavior is compiled and Behavior no longer matches. The extern block below
 * Behavior (the declarations InitResources needs) is what keeps it clear.
 */
struct RayParams { Vector3 start, end, in, out; };

#pragma push
#pragma opt_propagation off

int daKing_Donketu_c::Behavior()
{
    C *c = (C *)this;
    short angx;
    Vector3 v0;
    RayParams rp;
    Vector3 v3C;
    Vector3 v48;
    Vector3 v54;

    gCamera->mFramedActor = this;
    DecIfAbove0_Short(&mStateTimer);

    if (c->pp[1] != 0)
        (c->*c->pp[1])();

    if (mState != &data_ov073_02123400 && mState != &data_ov073_02123320 &&
        mState != &data_ov073_02123340) {
        UpdatePos(&mdCcAcPos_c);
    } else {
        int sum = mVertSpeed + mVertAccel;
        int m = mTerminalVelocity;
        int ac = mSpeedZ;
        if (sum >= m) m = sum;
        mVertSpeed = m;
        mSpeedZ = ac;
        UpdatePosWithOnlySpeed(&mdCcAcPos_c);
    }

    if ((mState == &data_ov073_02123330 || mState == &data_ov073_02123350) &&
        mBlendAnmModel.WillHitFrame(7) != 0) {
        data_020a0e68 = mBlendAnmModel.mat4x3;
        MTX_Concat43(&mBlendAnmModel.data.transforms[2], &data_020a0e68, &data_020a0e68);
        v0.x = data_020a0e68.m[9];
        v0.y = data_020a0e68.m[10];
        v0.z = data_020a0e68.m[11];
        Vec3_Lsl(&v3C, &v0, 3);
        v0 = v3C;
        func_02012694(SND_ANIM_FRAME_STOMP, &mCamSpacePosX);
        v48 = v0;
        HugeLandingDustAt(v48, 1);
    }

    if (mState != &data_ov073_021233c0 && mState != &data_ov073_021233d0 &&
        mState != &data_ov073_021233f0 && mState != &data_ov073_02123400 &&
        mState != &data_ov073_02123320 && mState != &data_ov073_02123340 &&
        mState != &data_ov073_02123380) {
        dBgCh_Lin line;
        rp.start.x = 0; rp.start.y = 0; rp.start.z = 0;
        rp.end.x = 0; rp.end.y = 0; rp.end.z = 0;
        rp.in.x = 0; rp.in.y = 0; rp.in.z = 0;
        rp.out.x = 0; rp.out.y = 0; rp.out.z = 0;
        {
            int y;
            rp.start.x = mPosX;
            angx = 0x2000;
            y = mPosY;
            rp.start.y = y;
            rp.start.z = mPosZ;
            rp.start.y = y + 0x78000;
            if (mHitsRemaining > 1)
                rp.in.z = 0x258000;
            else
                rp.in.z = 0x12c000;
            Matrix4x3_FromRotationY(&data_020a0e68, mPrevAngleY);
            Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, angx);
        }
        MTX_MultVec43((Vector3 *)&rp.in, &data_020a0e68, (Vector3 *)&rp.out);
        {
            int sx = rp.start.x;
            int ox = rp.out.x;
            int sy = rp.start.y;
            int sz = rp.start.z;
            int oy, oz;
            rp.end.x = sx;
            rp.end.x = sx + ox;
            oy = rp.out.y;
            oz = rp.out.z;
            rp.end.y = sy;
            rp.end.y = sy + oy;
            rp.end.z = sz;
            rp.end.z = sz + oz;
        }
        line.SetObjAndLine(rp.start, rp.end, this);
        if (!line.DetectClsn()) {
            if (mHorzSpeed > 0xa000) {
                mNoGroundAhead = 1;
            }
            mGroundMissPosX = mPosX;
            mGroundMissPosY = mPosY;
            mGroundMissPosZ = mPosZ;
            if (mState != &data_ov073_021233a0) {
                mPosX = mPrevPosX;
                mPosY = mPrevPosY;
                mPosZ = mPrevPosZ;
                mHorzSpeed = 0;
            }
        } else {
            mNoGroundAhead = 0;
        }
    }

    UpdateWMClsn(mWithMeshClsn, 0);

    v54 = data_ov073_02123040;
    mdCcAcPos_c.SetPosRelativeToActor(v54);

    if (mState == &data_ov073_02123360 || mState == &data_ov073_02123390) {
        HandleHit();
    }
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.Update();
    UpdateModelAndShadow();
    mBlendAnmModel.Advance();
    return 1;
}

extern "C" {
extern void LoadKeyModels(int idx);
/* Parameter names keep this TU's @NNN. */
typedef void BgchInitAt_(void *self, dActor_c *a, Fix12i r, Fix12i h, Vector3_16 *p, Vector3_16 *q);
}

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
    dExtSimpleModel_c::LoadFile(*(SharedFilePtr *)data_ov002_0210da30);
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
    mSpawnPos.x = mPosX;
    mSpawnPos.y = mPosY;
    mSpawnPos.z = mPosZ;
    for (i = 0; i < 8; i++) {
        mWaypointsA[i].x = mPosX;
        mWaypointsA[i].y = mPosY;
        mWaypointsA[i].z = mPosZ;
        mWaypointsB[i].x = mPosX;
        mWaypointsB[i].y = mPosY;
        mWaypointsB[i].z = mPosZ;
    }
    mLandedWaypoint = 0xff;
    mPrevAngleY = HorzAngleToCPlayer();
    mAngleY = mPrevAngleY;
    mTargetAngle = mAngleY;
    mStepBoneIdx = 2;
    mHitsRemaining = 3;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    ChiefChilly_ChangeState((C *)this, (PMF *)&data_ov073_02123330);
    return 1;
}

s32 daKing_Donketu_c::OnAimedAtWithEgg()
{
    return 0x64000;
}

/* Construct the KING_DONKETU actor and its members in declaration order. */
extern "C" daKing_Donketu_c *daKing_Donketu_c_classInit()
{
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
daKing_Donketu_c::State data_ov073_021233c0 = { &daKing_Donketu_c::func_ov073_02120c08,
                                                &daKing_Donketu_c::func_ov073_02120b78 };
daKing_Donketu_c::State data_ov073_021233d0 = {&daKing_Donketu_c::EnterEdgeWobble,
                                               &daKing_Donketu_c::UpdateEdgeWobble};
daKing_Donketu_c::State data_ov073_021233f0 = { &daKing_Donketu_c::func_ov073_021208e4,
                                                &daKing_Donketu_c::func_ov073_02120844 };
daKing_Donketu_c::State data_ov073_02123400 = { &daKing_Donketu_c::func_ov073_0212081c,
                                                &daKing_Donketu_c::func_ov073_02120610 };
daKing_Donketu_c::State data_ov073_02123320 = {&daKing_Donketu_c::EnterWaypointHop,
                                               &daKing_Donketu_c::UpdateWaypointHop};
daKing_Donketu_c::State data_ov073_02123340 = {&daKing_Donketu_c::EnterLeapToSpawn,
                                               &daKing_Donketu_c::UpdateLeapToSpawn};
daKing_Donketu_c::State data_ov073_02123380 = {&daKing_Donketu_c::EnterJumpEnd,
                                               &daKing_Donketu_c::UpdateJumpEnd};
daKing_Donketu_c::State data_ov073_021233b0 = {(int(daKing_Donketu_c::*)()) &
                                                   daKing_Donketu_c::EnterDefeatCutscene1,
                                               &daKing_Donketu_c::UpdateDefeatCutscene1};
daKing_Donketu_c::State data_ov073_021233e0 = {&daKing_Donketu_c::func_ov073_0211fe84,
                                               &daKing_Donketu_c::UpdateDefeatCutscene2};
daKing_Donketu_c::State data_ov073_02123410 = {&daKing_Donketu_c::func_ov073_0211fc70,
                                               &daKing_Donketu_c::UpdateDefeatCutscene3};
daKing_Donketu_c::State data_ov073_02123370 = {&daKing_Donketu_c::func_ov073_0211fbec,
                                               &daKing_Donketu_c::UpdateDefeat};
