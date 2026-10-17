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
 *                                  below mSpawnPosY and on the ground, then
 *                                  face mSpawnPos and go to ..3400.
 *   ..3400     ..081c    ..0610    waypoint hop whose landing sets mJumpCount
 *                                  to 1 and returns to ..3320.
 *   ..3320     ..05f0    ..03ac    waypoint hops, counting landings.
 *   ..3340     ..0390    ..00e0    leap toward mSpawnPos; while falling it
 *                                  calls Player::Unk_020c6a10 on the player
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
 *                                  it is below mSpawnPosY and on the ground,
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
 *   file-global for func_ov073_0211f61c. common.h stays first; BlendModelAnim's
 *   Matrix4x3 makes the twelve-word copies change size.
 * - struct C is a complete PMF shadow on purpose (mwccarm picks the
 *   pointer-to-member shape from completeness). ChiefChilly_ChangeState
 *   takes C*. The sixteen state labels are descriptions; the sinit owns
 *   the records. The header's 0x504 size wrap stays.
 * - BlendModelAnim::SetAnim, Particle::System::New / NewSimple,
 *   RunningSlidingDustAt, Player::Hurt, Sound::ChangeMusicVolume,
 *   DropShadowRadHeight and cstd::atan2 stay mangled (Fix12<int> by value).
 *   NewSimple and the layer-3 sound calls retain their local declarations.
 *   FromUniqueID is followed by a store at sys+0x44, which is still pad_042
 *   on Particle::System.
 * - Vec3_* / Matrix4x3_* / ApproachLinear have no shared header this TU
 *   can include alone. data_02082214 is the sine table func_ov073_0211f494
 *   indexes. data_0209f318, data_020a0e68 and data_0209e650 are arm9
 *   globals. data_ov002_0210da30 is ov002's; Cleanup only Releases it.
 *   g_profile_KING_DONKETU is outside this TU.
 * - Behavior's opt_propagation off is a push/pop around that function.
 * - void* cam in func_ov073_0211fa74 / 0212128c / 02121388: a dCamera_c*
 *   swaps two registers. Those functions also ldrh mStateTimer and
 *   mAngleX/Y/Z through unsigned short; a plain s16 read is ldrsh.
 * - s16* behind mPrevAngleY in func_ov073_0211f61c and 02120d80, the
 *   const s32* walk of mPosX, unk_4d4[i] in the dust loops, and
 *   *(void**)&mState in func_ov073_0211f2c0 stay. The direct member forms
 *   change addressing. data_020a0e68's translation words stay a char*
 *   walk; .m[9..11] changes bytes. dCamera_c +0x114 is inside pad_114.
 * - unk_4a8..4b0, unk_4c5, unk_4ca, unk_4d0 and unk_4d4 keep offset names.
 *   The hit masks in func_ov073_0211f61c stay literals (0x10000 is not in
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
#pragma opt_loop_invariants off

#include "common.h"
#include "daKing_Donketu_c.h"
#include "decl_common.h"
#include "decl_Message.h"
#include "types.h"
#include "SharedFilePtr.h"
#include "Particle__System.h"
#include "SaveData.h"
#include "Message.h"
#include "Player.h"
#include "dCamera_c.h"
#include "dBgCh_Lin.h"
#include "nitro/fx/fx.h"

bool ApproachLinear(short &value, short target, short step);

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
    SND_CHASE_BRAKE = 0x169,    /* chase update: leg 0 hands over to the braking leg (leg 1) */
    SND_REVERSE_AFTER_HURT = 0x16a, /* the boss hurt the player and turned around */
    SND_HIT_REGISTERED = 0x16b,     /* a hit registered; also the boss hurting the player in ..33d0 */
    SND_BIG_LANDING = 0x16c,        /* landing after a leap (states 33f0 / 33c0) */
    SND_HOP_LANDING = 0x16d,        /* landing after a waypoint hop (also the ..3340 leap) */
    SND_HOP_LAUNCH = 0x16f,         /* func_ov073_021200e0, frame the hop starts */
    SND_TALK_DONE = 0x12a,          /* ShowMessage finished in a talk state */
    SND_POOF = 0xbb                 /* func_ov073_0211fa74: the boss vanishes */
};

struct C;
typedef int (C::*PMF)();
struct C { char pad[0x37c]; PMF *pp; };

struct V3 { int x; int y; int z; };
typedef struct { int x, y, z; } Vec3;

enum Bool { FALSE, TRUE };

typedef struct Mtx43 { Vec3 r0, r1, r2, t; } Mtx43;

struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void M(void*); };

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
    u16 fileID;
    u8 numRefs;
    u8 pad_003;
    BMD_File *file;
    KingDonketuModelFileHandle(u32 fileID);
    ~KingDonketuModelFileHandle();
};
struct KingDonketuAnimResFileHandle : SharedFilePtr {
    u16 fileID;
    u8 numRefs;
    u8 pad_003;
    struct BCA_File *file;
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

int ChiefChilly_ChangeState(C *c, PMF *p);

extern struct BCA_File *data_ov002_0210da30[2];
extern short Vec3_HorzAngle(const Vector3* a, const Vector3* b);
extern void Matrix4x3_FromRotationY(void* m, short ang);
extern void MulVec3Mat4x3(const void* in, void* m, void* out);
extern unsigned int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned int, unsigned int, Fix12i, Fix12i, Fix12i, const void*, void*);
extern void func_0200d8c8(void *cam, void *v, int strength);
extern void MulMat4x3Mat4x3(void *dst, void *a, void *b);
extern void Vec3_Lsl(void *d, void *s, int sh);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void *data_0209f318;
extern struct Matrix4x3 data_020a0e68;
extern void Vec3_Sub(Vec3 *out, Vec3 *a, Vec3 *b);
extern int _ZN4cstd5atan2E5Fix12IiES1_(Fix12i y, Fix12i x);
extern Fix12i Vec3_HorzLen(Vec3 *v);
extern short data_02082214[];
extern u16 data_0209e650;
extern u16 DecIfAbove0_Short(void* p);
extern void func_02012694(int a, void* b);
extern int RandomIntInternal(u16* seed);
extern int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, const Vector3* v, u32 a, Fix12i f, u32 b, u32 c, u32 d);
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void* thiz, struct BCA_File* f, int i, int j, Fix12i fx, u16 k);
extern void _Z14ApproachLinearRiii(int* p, int t, int s);
extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int a, int b);
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int c, struct Vector3* v, unsigned int d);
extern s16 Vec3_VertAngle(const void* a, const void* b);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, int angX);
extern void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(int a, int b, int c);
extern void Vec3_Asr(Vec3* d, Vec3* s, int sh);
extern void Matrix4x3_FromTranslation(struct Matrix4x3 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern void _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(void* thiz, void* sm, void* m, int rad, int h, unsigned int u);
extern void UnloadKeyModels(int i);
}

// @symbol _ZN16daKing_Donketu_cD1Ev
// @symbol _ZN16daKing_Donketu_cD0Ev
daKing_Donketu_c::~daKing_Donketu_c()
{
}

/* The 0x77 / 0x78 effects the cut scenes and the defeat keep alive each frame. The point is
   mPos with y raised by 30.0 * the scale (mScaleX is Fix12, so the 0x1e000
   constant shrinks with the boss) and x/z moved 200.0 (0xc8000) along
   mPrevAngleY. Particle systems 0x77 and 0x78 are renewed there (ids kept in
   mParticleId0/1), and word +0x44 of each is set to 20.0 * the scale. The
   Vec3_HorzAngle result is not used. */
// @symbol _ZN16daKing_Donketu_c19func_ov073_0211f144Ev
void daKing_Donketu_c::func_ov073_0211f144() {
    daKing_Donketu_c *c = this;

    Vector3 pos, in, out;
    int y;
    ((int*)&pos)[0] = c->mPosX;
    y = c->mPosY;
    ((int*)&pos)[1] = y;
    ((int*)&pos)[2] = c->mPosZ;
    ((int *)&pos)[1] = y + FX_MUL(c->mScaleX, 0x1e000);

    Vec3_HorzAngle(&c->mPos, (Vector3 *)&c->mSpawnPosX);
    in.z = 0; in.z = 0xc8000; in.x = 0; in.y = 0;
    out.x = 0; out.y = 0; out.z = 0;
    Matrix4x3_FromRotationY(&data_020a0e68, c->mPrevAngleY);
    MulVec3Mat4x3(&in, &data_020a0e68, &out);
    pos.x = pos.x + out.x;
    pos.z = pos.z + out.z;
    c->mParticleId0 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(c->mParticleId0, 0x77, pos.x, pos.y, pos.z, 0, 0);
    c->mParticleId1 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(c->mParticleId1, 0x78, pos.x, pos.y, pos.z, 0, 0);
    if (c->mParticleId0 != 0) {
        void* sys = Particle::System::FromUniqueID(c->mParticleId0);
        if (sys != 0) *(int*)((char*)sys + 0x44) = c->mScaleX * 0x14;
    }
    if (c->mParticleId1 == 0) return;
    {
        void* sys = Particle::System::FromUniqueID(c->mParticleId1);
        if (sys != 0) *(int*)((char*)sys + 0x44) = c->mScaleX * 0x14;
    }
}

/* Called where the boss lands or steps (a reading from the callers). Shakes the camera by `strength` (func_0200d8c8; callers pass
   0x3e8000 to 0x1388000), then takes the translation of bone mStepBoneIdx
   times the model matrix, shifted left by 3 (the matrix words are held >> 3),
   and flips mStepBoneIdx between 2 and 3 for the next call. In the chase
   state (..3360) that position gets HugeLandingDustAt(pos, 1). In any other
   state the bone position is computed but not used: particles 0x88 and 0x89
   go off at mPos instead, moved 230.0 back (z = -0xe6000, rotated by mAngleY)
   when the state is ..33c0 or ..33f0. */
// @symbol _ZN16daKing_Donketu_c19func_ov073_0211f2c0Ei
void daKing_Donketu_c::func_ov073_0211f2c0(int strength)
{
  daKing_Donketu_c *c = this;
  struct V3 src;
  volatile struct V3 pv;
  struct V3 *new_var;
  struct V3 in;
  struct V3 res;
  struct V3 out;
  struct V3 dv;
  int ty;
  int tx;
  int tz;
  int x;
  int y;
  int z;
  func_0200d8c8(data_0209f318, &c->mPosX, strength);
  data_020a0e68 = c->mBlendModelAnim.mat4x3;
  MulMat4x3Mat4x3(((char *) c->mBlendModelAnim.data.transforms) + (c->mStepBoneIdx * 0x30), &data_020a0e68, &data_020a0e68);
  c->mStepBoneIdx ^= 1;
  ty = *((int *) (((char *) (&data_020a0e68)) + 0x28));
  tx = *((int *) (((char *) (&data_020a0e68)) + 0x24));
  src.y = ty;
  tz = *((int *) (((char *) (&data_020a0e68)) + 0x2c));
  src.z = tz;
  src.x = tx;
  Vec3_Lsl(&out, &src, 3);
  x = out.x;
  y = out.y;
  z = out.z;
  src.x = x;
  src.y = y;
  src.z = z;
  if (*(void **)&c->mState == (&data_ov073_02123360))
  {
    dv.x = x;
    dv.y = y;
    dv.z = z;
    c->HugeLandingDustAt(*(Vector3 *)&dv, 1);
    return;
  }
  pv.x = c->mPosX;
  pv.y = c->mPosY;
  pv.z = c->mPosZ;
  {
    void *anim = c->mState;
    if (anim == (&data_ov073_021233c0))
    {
      goto do_mtx;
    }
    if (anim != (&data_ov073_021233f0))
    {
      goto spawn;
    }
  }
  do_mtx:
  in.z = 0;

  in.z = -0xe6000;
  in.x = 0;
  in.y = 0;
  res.x = 0;
  res.y = 0;
  res.z = 0;
  new_var = &res;
  Matrix4x3_FromRotationY(&data_020a0e68, c->mAngleY);
  MulVec3Mat4x3(&in, &data_020a0e68, &res);

  {
    int px = pv.x;
    int rx = (*new_var).x;
    px = px + rx;
    rx = px;
    int pz = pv.z;
    int rz = res.z;
    {
      int t = pz + rz;
      rz = t;
    }
    pv.x = rx;
    pv.z = rz;
  }

  spawn:
  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x88, pv.x, pv.y, pv.z);

  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x89, pv.x, pv.y, pv.z);
}

/* Particles 0x84 and 0x85 on the surface of a's collision sphere,
   in the direction of b. The sphere's centre is a's mPos raised by its
   mdCcAcPos_c.height; the aim point is b's mPos raised 70.0 (0x46000). The
   two atan2 results give a heading and an elevation; shifted right by 4 they
   index data_02082214, which holds a sine at [2i] and a cosine at [2i+1]
   (Fix12), and the radius times those gives the offset from the centre. Within
   this file both arguments are always the boss. */
// @symbol _ZN16daKing_Donketu_c19func_ov073_0211f494EPv
void daKing_Donketu_c::func_ov073_0211f494(void *pb)
{
    daKing_Donketu_c *a = this;
    dActor_c *b = (dActor_c *)pb;
    Vec3 t, p, q, d;
    int ay, ax, vx, tmp, sin_ax, cos_ax, sin_ay, cos_ay, rx, ry, rz;
    int y0;
    int *bp = &b->mPosX;

    p.x = a->mPosX;
    y0 = a->mPosY;
    p.y = y0;
    {
        Vec3 *pq = &q;
        int zval = a->mPosZ;
        Vec3 *pp = &p;
        pp->z = zval;
        pq->x = bp[0];
        pq->y = bp[1];
        pq->z = bp[2];
        {
            int qy = pq->y + 0x46000;
            int h = a->mdCcAcPos_c.height;
            vx = a->mdCcAcPos_c.radius;
            pp->y = y0 + h;
            pq->y = qy;
            Vec3_Sub(&d, pq, pp);
        }
    }
    t.x = d.x; t.y = d.y; t.z = d.z;
    ay = _ZN4cstd5atan2E5Fix12IiES1_(t.x, t.z);
    ax = _ZN4cstd5atan2E5Fix12IiES1_(t.y, Vec3_HorzLen(&t));
    ax = (int)((unsigned short)ax >> 4);
    ay = (int)((unsigned short)ay >> 4);
    cos_ax = data_02082214[ax * 2 + 1];
    sin_ax = data_02082214[ax * 2];
    tmp = FX_MUL(vx, cos_ax);
    ry = FX_MUL(vx, sin_ax);
    sin_ay = data_02082214[ay * 2];
    cos_ay = data_02082214[ay * 2 + 1];
    rx = FX_MUL(tmp, sin_ay);
    rz = FX_MUL(tmp, cos_ay);
    p.x += rx; p.y += ry; p.z += rz;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x84, p.x, p.y, p.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x85, p.x, p.y, p.z);
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
     - With no hit, Player::Hurt is tried on the other actor. If it hurt: a
       particle 0x8a at the other actor's position, and in ..33d0
       SND_HIT_REGISTERED plays and the function returns 1. Elsewhere the boss turns mPrevAngleY by half a turn
       (0x8000), plays SND_REVERSE_AFTER_HURT, goes to ..3360 with mPhase 2,
       unk_4d0 0x2000 and mStateTimer 0x1e, and the function still returns 0. */
// @symbol _ZN16daKing_Donketu_c19func_ov073_0211f61cEv
s32 daKing_Donketu_c::func_ov073_0211f61c()
{
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
        c->func_ov073_0211f494(c);
        c->mHorzSpeed = 0x20000;
        hit = 1;
    }

    {
        int isBf = (int)(target->actorID == ACTOR_PLAYER);
        if (isBf == 1) {
            if (((Player *)target)->mIsMega != 0) {
                s32 flags = c->mdCcAcPos_c.hitFlags & 0x107e0;
                if (flags) {
                    c->mHorzSpeed = 0x41000;
                    hit = 1;
                }
            }
            if (hit == 0) {
                s32 flags = c->mdCcAcPos_c.hitFlags & 0x50380;
                if (flags) {
                    c->func_ov073_0211f494(c);
                    c->mHorzSpeed = 0x2d000;
                    hit = 1;
                }
            }
            if (hit == 0) {
                if ((c->mdCcAcPos_c.hitFlags & 0x70) || (((Player *)target)->mIsMetal != 0)
                    || (c->JumpedOnByPlayer(c->mdCcAcPos_c, *(Player *)target) != 0)) {
                    c->func_ov073_0211f494(c);
                    c->mHorzSpeed = 0x20000;
                    hit = 1;
                }
            }
        }
    }

    if (hit == 0) {
        if (c->mdCcAcPos_c.hitFlags & 0x400) {
            c->func_ov073_0211f494(c);
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
        rnd = RandomIntInternal(&data_0209e650);
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
                        rnd = RandomIntInternal(&data_0209e650);
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
        if (_ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(target, &v2, 0, 0x14000, 1, 0, 1) == 0)
            goto done0;
    }
    {
        s32* pv = &target->mPosX;
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x8a, pv[0], pv[1], pv[2]);
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
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&c->mBlendModelAnim, gKingIceDonketuHitAnimationFile.file, 4, 0x40000000, 0x1000, 0);
    c->mBlendModelAnim.speed = 0x1000;
    c->mPhase = 2;
    c->unk_4d0 = 0x2000;
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
// @symbol _ZN16daKing_Donketu_c19func_ov073_0211fa74Ev
int daKing_Donketu_c::func_ov073_0211fa74() {
    daKing_Donketu_c *c = this;
    void* cam;
    dActor_c* spawned;
    dActor_c* found;
    volatile struct Vector3_16 rot;
    struct Vector3 pos;
    cam = data_0209f318;
    ((dCamera_c *)cam)->SetFlag_3();
    _Z14ApproachLinearRiii((int*)&c->mScaleX, 0, 0x80);
    c->mScaleZ = c->mScaleX;
    c->mScaleY = c->mScaleZ;
    c->func_ov073_0211f144();
    if (c->mScaleX >= 0x100) goto end;
    pos.x = c->mSpawnPosX;
    pos.y = c->mSpawnPosY;
    pos.z = c->mSpawnPosZ;
    Message::EndTalk();
    _ZN5Sound22StopLoadedMusic_Layer3Ev();
    func_02011cfc();
    _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0x15666);
    pos.y = pos.y + 0x190000;
    func_0200fa8c(c, 1);

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
        ((dCamera_c *)cam)->mFlags &= ~8;
        c->MarkForDestruction();
    }
end:
    return 1;
}

/* ..3370 enter: nothing to do. */
// @symbol _ZN16daKing_Donketu_c19func_ov073_0211fbecEv
int daKing_Donketu_c::func_ov073_0211fbec()
{
    return 1;
}

/* ..3410 update, cut scene part 3: keeps the looped sound, the camera and the
   effects going until mTalkPlayer's talk state reads -1, then goes to ..3370. */
// @symbol _ZN16daKing_Donketu_c19func_ov073_0211fbf4Ev
int daKing_Donketu_c::func_ov073_0211fbf4() {
    daKing_Donketu_c *c = this;
  Player* pl = c->mTalkPlayer;
  c->mSoundHandle =
      _ZN5Sound8PlayLongEjjjRK7Vector3s(c->mSoundHandle, 3, 0x170, &c->mCamSpacePos, 0);
  ((dCamera_c *)data_0209f318)->SetFlag_3();
  c->func_ov073_0211f144();
  if(pl->GetTalkState() == -1){
    ChiefChilly_ChangeState((C *)(c), (PMF *)&data_ov073_02123370);
  }
  return 1;
}

/* ..3410 enter: nothing to do. */
// @symbol _ZN16daKing_Donketu_c19func_ov073_0211fc70Ev
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
// @symbol _ZN16daKing_Donketu_c19func_ov073_0211fc78Ev
int daKing_Donketu_c::func_ov073_0211fc78() {
    daKing_Donketu_c *c = this;
    struct Vector3 msgpos[2];
    struct Vector3 la, ps, in, out;
    Player* player;
    dCamera_c* cam;

    player = c->ClosestPlayer();
    c->mSoundHandle =
        _ZN5Sound8PlayLongEjjjRK7Vector3s(c->mSoundHandle, 3, 0x170, &c->mCamSpacePos, 0);
    if (player == 0) return 1;

    c->func_ov073_0211f144();

    {
        const s32* pv = &player->mPosX;
        msgpos[0].x = pv[0];
        msgpos[0].y = pv[1];
        msgpos[0].z = pv[2];
    }
    cam = (dCamera_c *)data_0209f318;
    msgpos[1].x = c->mPosX;
    msgpos[1].y = c->mPosY;
    msgpos[1].z = c->mPosZ;

    ((dCamera_c *)cam)->SetFlag_3();

    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;

    la.x = c->mSpawnPosX;
    la.y = c->mSpawnPosY;
    la.z = c->mSpawnPosZ;
    ps.x = c->mPosX;
    ps.y = c->mPosY;
    ps.z = c->mPosZ;

    la.y -= 0x300000;
    in.z = -0x55e000;

    Matrix4x3_FromRotationY(&data_020a0e68, c->mAngleY);
    MulVec3Mat4x3(&in, &data_020a0e68, &out);

    ps.x = ps.x + out.x;
    ps.y = ps.y + 0x200000;
    ps.z = ps.z + out.z;

    cam->SetLookAt(la);
    cam->SetPos(ps);

    if (player != 0) {
        int msg;
        c->mTalkPlayer = player;
        msg = (short)(c->mTalkPlayer->param1 + 0xe7);
        _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x14, 0x15666);
        _ZN7Message11PrepareTalkEv();
        if (c->mTalkPlayer->StartTalk(*c, 1)) {
            ((dCamera_c *)cam)->SetFlag_3();
            if (c->mTalkPlayer->ShowMessage(*c, msg, &msgpos[1], 0, 2)) {
                func_02012694(SND_TALK_DONE, (void*)&c->mCamSpacePosX);
                ChiefChilly_ChangeState((C *)(c), (PMF *)(&data_ov073_02123410));
            }
        }
    }
    return 1;
}

/* ..33e0 enter: nothing to do. */
// @symbol _ZN16daKing_Donketu_c19func_ov073_0211fe84Ev
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
// @symbol _ZN16daKing_Donketu_c19func_ov073_0211fe8cEv
int daKing_Donketu_c::func_ov073_0211fe8c() {
    daKing_Donketu_c *c = this;
    struct Vector3 look, pos, in, out;
    Player* player;
    dCamera_c* cam;

    c->mSoundHandle =
        _ZN5Sound8PlayLongEjjjRK7Vector3s(c->mSoundHandle, 3, 0x170, &c->mCamSpacePos, 0);

    player = c->ClosestPlayer();
    if (player != 0 && player->mIsAirborne == 0) {
        int angle;
        player->StartTalk(*c, 1);
        angle = c->HorzAngleToCPlayer();
        player->mAngleX = 0;
        player->mAngleY = angle + 0x8000;
        player->mAngleZ = 0;
    }

    c->func_ov073_0211f144();

    cam = (dCamera_c *)data_0209f318;
    ((dCamera_c *)cam)->SetFlag_3();

    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;

    look.x = c->mPosX;
    look.y = c->mPosY;
    look.z = c->mPosZ;
    pos.x = c->mSpawnPosX;
    pos.y = c->mSpawnPosY;
    pos.z = c->mSpawnPosZ;

    look.y += 0x100000;
    in.z = -0x59c000;

    Matrix4x3_FromRotationY(&data_020a0e68, c->mAngleY);
    MulVec3Mat4x3(&in, &data_020a0e68, &out);

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

/* ..33b0 enter: start the animation in gKingIceDonketuDeadAnimationFile.file;
   mStateTimer = 0x32 (50 frames). */
// @symbol _ZN16daKing_Donketu_c19func_ov073_0212000cEv
short daKing_Donketu_c::func_ov073_0212000c() {
    daKing_Donketu_c *c = this;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&c->mBlendModelAnim, gKingIceDonketuDeadAnimationFile.file, 4, 0, 0x1000, 0);
    c->mStateTimer=0x32;
    return 1;
}


/* ..3380 update: when the animation finishes, unk_4c5 = 0xff and back to the
   chase (..3360). */
// @symbol _ZN16daKing_Donketu_c19func_ov073_0212005cEv
int daKing_Donketu_c::func_ov073_0212005c() {
    daKing_Donketu_c *c = this;
  if(c->mBlendModelAnim.Finished()){
    c->unk_4c5=0xff;
    ChiefChilly_ChangeState((C *)(c), (PMF *)&data_ov073_02123360);
  }
  return 1;
}


/* ..3380 enter: clear unk_4ca and start the animation in
   gKingIceDonketuJumpEdAnimationFile.file. */
// @symbol _ZN16daKing_Donketu_c19func_ov073_02120098Ev
struct BCA_File;
int daKing_Donketu_c::func_ov073_02120098() {
    daKing_Donketu_c *c = this;
  c->unk_4ca = 0;
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&c->mBlendModelAnim, gKingIceDonketuJumpEdAnimationFile.file, 4, 0x40000000, 0x1000, 0);
  return 1;
}

/* ..3340 update, a leap toward mSpawnPos. mStateTimer counts down from 10
   (set by the enter handler). At 1 the leap's animation starts: mVertSpeed
   0x46000 (70.0), mVertAccel -0x3000 (-3.0), and SND_HOP_LAUNCH if unk_4ca is
   set. At 0, while mStateScratch is 0, mAngleX is straightened (0x2000 a
   frame), mPrevAngleY turns toward the angle to mSpawnPos by 0x800, and the
   x/z velocity words unk_0a4 / unk_0ac are set to 50.0 (0x32000) when
   mHitsRemaining is 2, else 30.0 (0x1e000), along that angle. While falling,
   if the actor touching the boss is the player and the boss is the higher,
   Player::Unk_020c6a10(1) is called on it. On the ground the motion is
   zeroed, the camera is shaken (0x1388000) and SND_HOP_LANDING plays (once,
   while mStateScratch is 0). With mHitsRemaining 1 and unk_4ca clear the boss
   then waits 0x82 frames (counted in mStateScratch) before ..3380; otherwise
   it goes to ..3380 at once. */
// @symbol _ZN16daKing_Donketu_c19func_ov073_021200e0Ev
int daKing_Donketu_c::func_ov073_021200e0()
{
    daKing_Donketu_c *thiz = this;
    u16 state = thiz->mStateTimer;
    if (state != 0) {
        if (state == 1) {
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&thiz->mBlendModelAnim, gKingIceDonketuJumpStAnimationFile.file, 4, 0x40000000, 0x1000, 0);
            thiz->mBlendModelAnim.speed = 0x1000;
            thiz->mVertSpeed = 0x46000;
            thiz->unk_4c5 = 0xff;
            thiz->mVertAccel = -0x3000;
            if (thiz->unk_4ca != 0) {
                func_02012694(SND_HOP_LAUNCH, &thiz->mCamSpacePosX);
            }
        }
        return 1;
    }
    /* state == 0 */
    if (thiz->mStateScratch == 0) {
        Vec3 offset;
        Vec3 rotated;
        s16 horz;
        ApproachLinear(thiz->mAngleX, 0, 0x2000);
        horz = Vec3_HorzAngle(&thiz->mPos, (const Vector3 *)&thiz->mSpawnPosX);
        Vec3_VertAngle((const void*)&thiz->mPosX, (const void*)&thiz->mSpawnPosX);
        ApproachLinear(thiz->mPrevAngleY, horz, 0x800);
        offset.x = 0;
        offset.y = 0;
        offset.z = 0;
        rotated.x = 0;
        rotated.y = 0;
        rotated.z = 0;
        if (thiz->mHitsRemaining == 2) {
            offset.z = 0x32000;
        } else {
            offset.z = 0x1e000;
        }
        Matrix4x3_FromRotationY(&data_020a0e68, horz);
        MulVec3Mat4x3(&offset, &data_020a0e68, &rotated);
        thiz->unk_0a4 = rotated.x;
        thiz->unk_0ac = rotated.z;
        if (thiz->mVertSpeed < 0) {
            int id = thiz->mdCcAcPos_c.otherOwner;
            if (id != 0) {
                dActor_c* actor = dActor_c::FindWithID((u32)id);
                if (actor != 0) {
                    enum Bool eq = (enum Bool)(actor->actorID == ACTOR_PLAYER);
                    if (eq != FALSE) {
                        Vec3 pos = *(Vec3*)&actor->mPosX;
                        if (thiz->mPosY > pos.y) {
                            ((Player *)actor)->Unk_020c6a10(1);
                        }
                    }
                }
            }
        }
    }
    if (thiz->mWithMeshClsn.IsOnGround() != 0) {
        if (thiz->mStateScratch == 0) {
            thiz->mHorzSpeed = 0;
            thiz->unk_0a4 = 0;
            thiz->mVertSpeed = 0;
            thiz->unk_0ac = 0;
            thiz->func_ov073_0211f2c0(0x1388000);
            func_02012694(SND_HOP_LANDING, &thiz->mCamSpacePosX);
        }
        if (thiz->mHitsRemaining == 1 && thiz->unk_4ca == 0) {
            thiz->unk_4c5 = 0;
            if (thiz->mStateScratch == 0) {
                _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&thiz->mBlendModelAnim, gKingIceDonketuJumpEdAnimationFile.file, 4, 0x40000000, 0x1000, 0);
            }
            thiz->mStateScratch += 1;
            if (thiz->mStateScratch < 0x82) {
                return 1;
            }
        }
        ChiefChilly_ChangeState((C *)(thiz), (PMF *)&data_ov073_02123380);
    }
    thiz->mAngleY = thiz->mPrevAngleY;
    return 1;
}

/* ..3340 enter: mStateScratch = 0; mStateTimer = 10. */
// @symbol _ZN16daKing_Donketu_c19func_ov073_02120390Ev
int daKing_Donketu_c::func_ov073_02120390()
{
    daKing_Donketu_c *p = this;
    p->mStateScratch = 0;
    p->mStateTimer = 10;
    return 1;
}

/* ..3320 update, a waypoint hop. mStateTimer starts at 2. At 1 the hop
   launches: mVertSpeed 0x5a000 (90.0), mVertAccel -0x5000 (-5.0), mWaypointCursor
   advances modulo 8 and the animation in gKingIceDonketuDamageAnimationFile.file runs at speed
   0x2000. At 0 the boss steers toward the current waypoint (set B while
   mHitsRemaining is 2, else set A): mPrevAngleY turns toward it by 0x800 and
   the x/z velocity words are set to 80.0 (0x50000) along the heading and
   pitch. On landing it copies the cursor to unk_4c5, stops, shakes the camera
   (0x7d0000), plays SND_HOP_LANDING and counts the landing in mJumpCount: past
   7 the state becomes ..3340, otherwise it re-enters ..3320. */
// @symbol _ZN16daKing_Donketu_c19func_ov073_021203acEv
int daKing_Donketu_c::func_ov073_021203ac()
{
    daKing_Donketu_c *c = this;
    int *src;
    int v[3];

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

        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&c->mBlendModelAnim, gKingIceDonketuDamageAnimationFile.file, 4, 0, 0x1000, 0);
        c->mBlendModelAnim.speed = 0x2000;
    }
    return 1;

mainblock:
    if (c->mHitsRemaining == 2) {
        src = (int *)&c->mWaypointsB[c->mWaypointCursor];
        v[0] = src[0];
        v[1] = src[1];
        v[2] = src[2];
    } else {
        src = (int *)&c->mWaypointsA[c->mWaypointCursor];
        v[0] = src[0];
        v[1] = src[1];
        v[2] = src[2];
    }
    {
        short hz = Vec3_HorzAngle(&c->mPos, (const Vector3 *)v);
        short vt = Vec3_VertAngle(&c->mPosX, v);
        int in[3];
        int out[3];
        ApproachLinear(c->mPrevAngleY, hz, 0x800);
        in[0] = 0; in[1] = 0; in[2] = 0;
        out[0] = 0; out[1] = 0; out[2] = 0;
        in[2] = 0x50000;
        Matrix4x3_FromRotationY(&data_020a0e68, hz);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, vt);
        MulVec3Mat4x3(in, &data_020a0e68, out);
        c->unk_0a4 = out[0];
        c->unk_0ac = out[2];
    }
    if (c->mWithMeshClsn.IsOnGround() != 0) {
        c->unk_4c5 = c->mWaypointCursor;
        c->mHorzSpeed = 0;
        c->unk_0a4 = 0;
        c->mVertSpeed = 0;
        c->unk_0ac = 0;
        c->func_ov073_0211f2c0(0x7d0000);
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
// @symbol _ZN16daKing_Donketu_c19func_ov073_021205f0Ev
int daKing_Donketu_c::func_ov073_021205f0()
{
    daKing_Donketu_c *p = this;
    p->mBlendModelAnim.speed = 0;
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
// @symbol _ZN16daKing_Donketu_c19func_ov073_02120610Ev
int daKing_Donketu_c::func_ov073_02120610()
{
        daKing_Donketu_c *c = this;
        int *src;
        int v[3];

        if ((unsigned short)c->mStateTimer == 0)
            goto mainblock;

        if ((unsigned short)c->mStateTimer == 1) {
            c->mVertSpeed = 0x5a000;
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&c->mBlendModelAnim, gKingIceDonketuDamageAnimationFile.file, 4, 0, 0x1000, 0);
            c->mBlendModelAnim.speed = 0x2000;
        }
        return 1;

    mainblock:
        ApproachLinear(c->mAngleX, 0, 0x2000);
        if (c->mHitsRemaining == 2) {
            src = (int *)&c->mWaypointsB[c->mWaypointCursor];
            v[0] = src[0];
            v[1] = src[1];
            v[2] = src[2];
        } else {
            src = (int *)&c->mWaypointsA[c->mWaypointCursor];
            v[0] = src[0];
            v[1] = src[1];
            v[2] = src[2];
        }
        {
            short hz = Vec3_HorzAngle(&c->mPos, (const Vector3 *)v);
            short vt = Vec3_VertAngle(&c->mPosX, v);
            int in[3];
            int out[3];
            in[2] = 0;
            in[0] = 0; in[1] = 0; out[0] = 0; out[1] = 0; out[2] = 0;
            in[2] = 0x28000;
            Matrix4x3_FromRotationY(&data_020a0e68, hz);
            Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, vt);
            MulVec3Mat4x3(in, &data_020a0e68, out);
            c->unk_0a4 = out[0];
            if (c->mStateScratch != 0 || c->mVertSpeed < 0) {
                c->mVertSpeed = out[1];
                c->mStateScratch = 1;
            }
            c->unk_0ac = out[2];
        }
        if (c->mWithMeshClsn.IsOnGround() != 0) {
            c->unk_4c5 = c->mWaypointCursor;
            c->mHorzSpeed = 0;
            c->unk_0a4 = 0;
            c->mVertSpeed = 0;
            c->unk_0ac = 0;
            c->func_ov073_0211f2c0(0x7d0000);
            func_02012694(SND_HOP_LANDING, &c->mCamSpacePosX);
            c->mJumpCount = 1;
            ChiefChilly_ChangeState((C *)(c), (PMF *)&data_ov073_02123320);
        }
        c->mAngleY = c->mPrevAngleY;
        return 1;
    }

/* ..3400 enter: mStateTimer = 10, mStateScratch = 0, mJumpCount = 0,
   unk_4c5 = 0xff. */
// @symbol _ZN16daKing_Donketu_c19func_ov073_0212081cEv
int daKing_Donketu_c::func_ov073_0212081c()
{
    daKing_Donketu_c *self = this;
    self->mStateTimer = 0xa;
    self->mStateScratch = 0;
    self->mJumpCount = 0;
    self->unk_4c5 = 0xff;
    return 1;
}

/* ..33f0 update: mAngleX approaches -0x4000 (a quarter turn) by 0x400 a frame
   with the looped sound playing. Once the boss is below mSpawnPosY and on the
   ground it shakes the camera (0x7d0000), plays SND_BIG_LANDING, zeroes
   mHorzSpeed and goes to ..33b0. */
// @symbol _ZN16daKing_Donketu_c19func_ov073_02120844Ev
int daKing_Donketu_c::func_ov073_02120844()
{
    daKing_Donketu_c *t = this;
    ApproachLinear(t->mAngleX, -0x4000, 0x400);
    t->mSoundHandle =
        _ZN5Sound8PlayLongEjjjRK7Vector3s(t->mSoundHandle, 3, 0x170, &t->mCamSpacePos, 0);
    if (t->mSpawnPosY > t->mPosY && t->mWithMeshClsn.IsOnGround()) {
        t->func_ov073_0211f2c0(0x7d0000);
        func_02012694(SND_BIG_LANDING, &t->mCamSpacePosX);
        t->mHorzSpeed = 0;
        ChiefChilly_ChangeState((C *)(t), (PMF *)&data_ov073_021233b0);
    }
    return 1;
}

/* ..33f0 enter: mHorzSpeed 0x14000 (20.0), mVertSpeed 0x1e000 (30.0),
   mVertAccel -0x3000 (-3.0); clears the sound handle. */
// @symbol _ZN16daKing_Donketu_c19func_ov073_021208e4Ev
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
   becomes 1, unk_4ca becomes 1 and the state is ..3340. Otherwise a looped
   sound plays and the boss moves toward the point 120.0 (0x78000) from the
   saved miss position (unk_4a8..4b0) along mAngleY + 0x8000, at 20.0 (0x14000)
   a frame; mAngleY becomes the angle to mSpawnPos, mPrevAngleY that + 0x8000.
   mStateScratch advances 0x500 a frame and mAngleX approaches unk_4d0 times
   the sine of it (data_02082214, Fix12) by 0x400. Then,
   when func_ov073_0211f61c returns nonzero (the boss took a hit or, in this
   state, hurt the player) the state becomes ..33f0. */
// @symbol _ZN16daKing_Donketu_c19func_ov073_02120910Ev
int daKing_Donketu_c::func_ov073_02120910()
{
    daKing_Donketu_c *c = this;
    Vector3 in;
    Vector3 out;

    c->mVertAccel = 0;
    c->mHorzSpeed = 0;
    c->unk_0a4 = 0;
    c->mVertSpeed = 0;
    c->unk_0ac = 0;

    if ((unsigned short)c->mStateTimer == 0) {
        *&c->mHitsRemaining = 1;
        ChiefChilly_ChangeState((C *)(c), (PMF *)&data_ov073_02123340);
        *&c->unk_4ca = 1;
        c->mPrevAngleY = c->mAngleY;
        return 1;
    }

    c->mSoundHandle =
        _ZN5Sound8PlayLongEjjjRK7Vector3s(c->mSoundHandle, 3, 0x16e, &c->mCamSpacePos, 0);

    in.z = 0;
    in.z = 0x78000;
    in.x = 0;
    in.y = 0;
    out.x = 0;
    out.y = 0;
    out.z = 0;
    Matrix4x3_FromRotationY(&data_020a0e68, c->mAngleY + 0x8000);
    MulVec3Mat4x3(&in, &data_020a0e68, &out);
    in.x = c->unk_4a8 + out.x;
    in.y = c->unk_4ac;
    in.z = c->unk_4b0 + out.z;
    Vec3_ApproachHorz(&c->mPosX, &in, 0x14000);
    c->mAngleY = Vec3_HorzAngle(&c->mPos, (const Vector3 *)&c->mSpawnPosX);
    c->mPrevAngleY = c->mAngleY + 0x8000;
    c->mStateScratch += 0x500;
    ApproachLinear(c->mAngleX,
        FX_MUL(c->unk_4d0, data_02082214[((unsigned short)(short)c->mStateScratch >> 4) * 2]),
        0x400);
    if (c->func_ov073_0211f61c()) {
        ChiefChilly_ChangeState((C *)(c), (PMF *)&data_ov073_021233f0);
    }
    return 1;
}

/* ..33d0 enter: animation gKingIceDonketuWalkAnimationFile.file at speed 0x4000; mAngleX,
   mStateScratch and all the motion words cleared; unk_4d0 = -0x1000;
   mGroundMissPos copied into unk_4a8..4b0; sound handle cleared;
   mStateTimer = 0x5a (90 frames). */
// @symbol _ZN16daKing_Donketu_c19func_ov073_02120ad8Ev
int daKing_Donketu_c::func_ov073_02120ad8()
{
    daKing_Donketu_c *t = this;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&t->mBlendModelAnim, gKingIceDonketuWalkAnimationFile.file, 4, 0, 0x1000, 0);
    t->mBlendModelAnim.speed = 0x4000;
    t->mAngleX = 0;
    t->mStateScratch = 0;
    t->mVertAccel = 0;
    t->mHorzSpeed = 0;
    t->unk_0a4 = 0;
    t->mVertSpeed = 0;
    t->unk_0ac = 0;
    t->unk_4d0 = -0x1000;
    t->unk_4a8 = t->mGroundMissPosX;
    t->unk_4ac = t->mGroundMissPosY;
    t->unk_4b0 = t->mGroundMissPosZ;
    t->mSoundHandle = 0;
    t->mStateTimer = 0x5a;
    return 1;
}

/* ..33c0 update: mAngleX approaches -0x4000 by 0x400 a frame. Once the boss is
   below mSpawnPosY and on the ground it shakes the camera (0xfa0000), stops,
   turns to face mSpawnPos (both angles), plays SND_BIG_LANDING and goes to
   ..3400. */
// @symbol _ZN16daKing_Donketu_c19func_ov073_02120b78Ev
int daKing_Donketu_c::func_ov073_02120b78() {
    daKing_Donketu_c *c = this;
    ApproachLinear(c->mAngleX, -0x4000, 0x400);
    if(c->mSpawnPosY > c->mPosY){
        if(c->mWithMeshClsn.IsOnGround()){
            c->func_ov073_0211f2c0(0xfa0000);
            c->mHorzSpeed = 0;
            c->mPrevAngleY = Vec3_HorzAngle(&c->mPos, (const Vector3 *)&c->mSpawnPosX);
            c->mAngleY = c->mPrevAngleY;
            func_02012694(SND_BIG_LANDING, &c->mCamSpacePosX);
            ChiefChilly_ChangeState((C *)(c), (PMF *)&data_ov073_02123400);
        }
    }
    return 1;
}

/* ..33c0 enter: animation gKingIceDonketuWalkAnimationFile.file at speed 0x4000;
   mHorzSpeed 0x14000 (20.0), mVertSpeed 0x1e000 (30.0), mVertAccel -0x3000
   (-3.0); mAngleX and mStateScratch cleared. */
// @symbol _ZN16daKing_Donketu_c19func_ov073_02120c08Ev
int daKing_Donketu_c::func_ov073_02120c08()
{
    daKing_Donketu_c *t = this;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&t->mBlendModelAnim, gKingIceDonketuWalkAnimationFile.file, 4, 0, 0x1000, 0);
    t->mBlendModelAnim.speed = 0x4000;
    t->mHorzSpeed = 0x14000;
    t->mVertSpeed = 0x1e000;
    t->mAngleX = 0;
    t->mVertAccel = -0x3000;
    t->mStateScratch = 0;
    return 1;
}

/* ..33a0 update, the hit reaction. While |mHorzSpeed| > 0xa the dust follows
   unk_4d4[0..1]. If mNoGroundAhead is 1 mHitsRemaining drops by one and the
   state becomes ..33c0 while it is nonzero, ..33d0 when it reaches 0.
   Otherwise mHorzSpeed is braked to 0 by 0x1000 a frame, and once the
   animation has finished with |mHorzSpeed| < 0xa the boss stops, takes mAngleY
   back into mPrevAngleY and returns to the chase (..3360). */
// @symbol _ZN16daKing_Donketu_c19func_ov073_02120c7cEv
int daKing_Donketu_c::func_ov073_02120c7c()
{
    daKing_Donketu_c *c = this;
    int a = c->mHorzSpeed; if (a < 0) a = -a;
    if (a > 0xa) {
        int i = 0;
        do {
            _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(c->unk_4d4[i].x, c->unk_4d4[i].y, c->unk_4d4[i].z);
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
    _Z14ApproachLinearRiii((int *)&c->mHorzSpeed, 0, 0x1000);
    if (c->mBlendModelAnim.Finished()) {
        int b = c->mHorzSpeed; if (b < 0) b = -b;
        if (b < 0xa) {
            c->mHorzSpeed = 0;
            c->mPrevAngleY = c->mAngleY;
            ChiefChilly_ChangeState((C *)(c), (PMF *)&data_ov073_02123360);
        }
    }
    return 1;
}


/* ..33a0 enter: animation gKingIceDonketuDamageAnimationFile.file at speed 0x1000; mPrevAngleY
   = the angle to the player + 0x8000. */
// @symbol _ZN16daKing_Donketu_c19func_ov073_02120d80Ev
int daKing_Donketu_c::func_ov073_02120d80()
{
    daKing_Donketu_c *c = this;
    int fix;
    unsigned short t;
    short ang;

    fix = 0x1000;
    t = 0;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&c->mBlendModelAnim, gKingIceDonketuDamageAnimationFile.file, 4, 0x40000000, fix, t);
    c->mBlendModelAnim.speed = fix;
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
// @symbol _ZN16daKing_Donketu_c19func_ov073_02120decEv
int daKing_Donketu_c::func_ov073_02120dec() {
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

/* ..3390 enter: animation gKingIceDonketuWalkAnimationFile.file at speed 0x2000;
   mTargetAngle = the angle to mSpawnPos; mStateScratch = 0;
   mPrevAngleY = mAngleY. */
// @symbol _ZN16daKing_Donketu_c19func_ov073_02120e60Ev
int daKing_Donketu_c::func_ov073_02120e60() {
    daKing_Donketu_c *c = this;
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&c->mBlendModelAnim, gKingIceDonketuWalkAnimationFile.file, 4, 0, 0x1000, 0);
  c->mBlendModelAnim.speed = 0x2000;
  c->mTargetAngle = Vec3_HorzAngle(&c->mPos, (const Vector3 *)&c->mSpawnPosX);
  c->mStateScratch = 0;
  c->mPrevAngleY = c->mAngleY;
  return 1;
}

/* ..3360 update, the chase, in legs selected by mPhase. mNoGroundAhead being
   1 sends the boss to ..3390 first.
     leg 0  with mStateTimer at 0: mTargetAngle = the angle to the player. If
            mPrevAngleY is within 0x2000 of it the timer is set to 0x1e and
            the leg continues; otherwise leg 1 with unk_4d0 = 0x1000 and
            SND_CHASE_BRAKE. While the timer runs: mHorzSpeed approaches
            0x1e000 (30.0) by 0x3000, mPrevAngleY turns toward mTargetAngle by
            0x1d0, and the first time the player is within 0x1f4000 (500.0)
            there is a one-in-eight chance of leg 3 (speed zeroed,
            mStateTimer 0xf).
     leg 3  when mStateTimer is 0: unk_4d0 = 0x1000, leg 1.
     legs 1, 2  mHorzSpeed is braked to 0 by unk_4d0 with the dust following
            while it is above 0xa; leg 1 also turns mAngleY toward the player
            by 0x500. When |mHorzSpeed| < 0xa and mStateTimer is 0, leg 2
            first retargets (to mSpawnPos if the player's hurt state reads 4 or
            5 or it is mIsNoControl, else to the player), then the boss stops,
            restarts the animation and returns to leg 0 with mStateTimer 0x1e.
   In leg 0 the model hitting animation frame 0 or 0xe shakes the camera
   (0x3e8000) and plays SND_CHASE_STEP. */
// @symbol _ZN16daKing_Donketu_c19func_ov073_02120ed0Ev
int daKing_Donketu_c::func_ov073_02120ed0() {
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
                c->unk_4d0 = 0x1000;
                (c->mPhase)++;
                func_02012694(SND_CHASE_BRAKE, &c->mCamSpacePosX);
                break;
            }
        } else {
            _Z14ApproachLinearRiii((int *)&c->mHorzSpeed, 0x1e000, 0x3000);
            if (c->mStateScratch == 0) {
                if (c->DistToCPlayer() < 0x1f4000) {
                    c->mStateScratch = 1;
                    if ((((unsigned int)RandomIntInternal(&data_0209e650) >> 0x18) & 7) == 0) {
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
            c->unk_4d0 = 0x1000;
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
                _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(c->unk_4d4[i].x, c->unk_4d4[i].y,
                                                                   c->unk_4d4[i].z);
                i++;
            } while (i < 2);
        }
        _Z14ApproachLinearRiii((int *)&c->mHorzSpeed, 0, c->unk_4d0);
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
            Player *p = c->ClosestPlayer();
            if (p != 0) {
                int t;
                if (p->GetHurtState() == 4)
                    goto hz;
                if (p->GetHurtState() == 5)
                    goto hz;
                t = p->mIsNoControl ? 1 : 0;
                if (t == 1) {
                hz:
                    c->mTargetAngle = Vec3_HorzAngle(&c->mPos, (Vector3 *)&c->mSpawnPosX);
                } else {
                    c->mTargetAngle = c->HorzAngleToCPlayer();
                }
            }
        }
        c->mHorzSpeed = 0;
        c->mPrevAngleY = c->mAngleY;
        c->mStateTimer = 0x1e;
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
            &c->mBlendModelAnim, gKingIceDonketuWalkAnimationFile.file, 4, 0, 0x1000, 0);
        c->mBlendModelAnim.speed = 0x2000;
        c->mStateScratch = 0;
        c->mPhase = 0;
        break;
    }
    }

    if (c->mPhase == 0) {
        if (c->mBlendModelAnim.WillHitFrame(0) != 0 || c->mBlendModelAnim.WillHitFrame(0xe) != 0) {
            c->func_ov073_0211f2c0(0x3e8000);
            func_02012694(SND_CHASE_STEP, &c->mCamSpacePosX);
        }
    }
    return 1;
}

/* ..3360 enter: mStateTimer = 0, mPhase = 0, unk_4d0 = 0x2000; animation
   gKingIceDonketuWalkAnimationFile.file at speed 0x2000. */
// @symbol _ZN16daKing_Donketu_c19func_ov073_0212122cEv
int daKing_Donketu_c::func_ov073_0212122c()
{
    daKing_Donketu_c *t = this;
    t->mStateTimer = 0;
    t->mPhase = 0;
    t->unk_4d0 = 0x2000;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&t->mBlendModelAnim, gKingIceDonketuWalkAnimationFile.file, 4, 0, 0x1000, 0);
    t->mBlendModelAnim.speed = 0x2000;
    return 1;
}

/* ..3350 update. The camera looks at mPos raised 112.0 (0x70000) and moved
   672.0 (0x2a0000) in -z, from mPos moved 768.0 (0x300000) in -x, raised 32.0
   (0x20000), and moved in z by 0xffa34000, which as a signed word is -0x5cc000
   (-1484.0). When mTalkPlayer's talk state reads -1 camera flag 0x8 is
   cleared, layer-3 music 0x2d is loaded and set, func_02011d08 is called and
   the state becomes ..3360. */
// @symbol _ZN16daKing_Donketu_c19func_ov073_0212128cEv
int daKing_Donketu_c::func_ov073_0212128c()
{
    daKing_Donketu_c *c = this;
    struct Vector3 la;
    struct Vector3 ps;
    void* cam;
    Player* player;

    cam = data_0209f318;
    player = c->mTalkPlayer;
    ((dCamera_c *)cam)->SetFlag_3();

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

    ((dCamera_c *)cam)->SetLookAt(la);
    ((dCamera_c *)cam)->SetPos(ps);

    if (player->GetTalkState() == -1) {
        ((dCamera_c *)cam)->mFlags &= ~8;
        _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2d);
        func_02011d08();
        ChiefChilly_ChangeState((C *)(c), (PMF *)&data_ov073_02123360);
    }
    return 1;
}

/* ..3350 enter: mPhase = 0. */
// @symbol _ZN16daKing_Donketu_c19func_ov073_02121378Ev
int daKing_Donketu_c::func_ov073_02121378()
{
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
// @symbol _ZN16daKing_Donketu_c19func_ov073_02121388Ev
int daKing_Donketu_c::func_ov073_02121388() {
    daKing_Donketu_c *c = this;
    struct Vector3 vplayer;
    struct Vector3 vmsg;
    struct Vector3 la;
    struct Vector3 ps;
    Player* player;
    void* cam;
    int msg;
    Player* p;

    player = c->ClosestPlayer();
    if (player == 0) return 1;

    {
        const s32* pv = &player->mPosX;
        vplayer.x = pv[0];
        vplayer.y = pv[1];
        vplayer.z = pv[2];
    }
    cam = data_0209f318;
    vmsg.x = c->mPosX;
    vmsg.y = c->mPosY;
    vmsg.z = c->mPosZ;

    ((dCamera_c *)cam)->SetFlag_3();

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

    ((dCamera_c *)cam)->SetLookAt(la);
    ((dCamera_c *)cam)->SetPos(ps);

    vmsg.y = vmsg.y + 0x64000;
    ApproachLinear(c->mPrevAngleY, Vec3_HorzAngle(&c->mPos, &vplayer), 0x800);

    c->mTalkPlayer = player;
    p = c->mTalkPlayer;
    msg = (short)(p->param1 + 0xe3);
    if (p->StartTalk(*c, 1)) {
        if (c->mPhase == 0) {
            _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2c);
            c->mPhase = 0;
        }

        if (c->mTalkPlayer->ShowMessage(*c, msg, &vmsg, 0, 2)) {
            func_02012694(SND_TALK_DONE, &c->mCamSpacePosX);
            ChiefChilly_ChangeState((C *)(c), (PMF *)&data_ov073_02123350);
        }
    }
    return 1;
}

/* ..3330 enter: mPhase = 0; animation gKingIceDonketuWaitAnimationFile.file. */
// @symbol _ZN16daKing_Donketu_c19func_ov073_02121538Ev
int daKing_Donketu_c::func_ov073_02121538() {
    daKing_Donketu_c *c = this;
  c->mPhase=0;
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&c->mBlendModelAnim, gKingIceDonketuWaitAnimationFile.file, 4, 0, 0x1000, 0);
  return 1;
}

/* Stores the state record in mState (the shadow struct's pp, at +0x37c) and
   calls the enter member held in the record's first pointer-to-member; returns
   1 without calling if that pointer is null. */
// @symbol ChiefChilly_ChangeState
extern "C" int ChiefChilly_ChangeState(C *c, PMF *p) { c->pp = p; PMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(); }

/* Called at the end of every Behavior. Builds the model matrix (mPos >> 3,
   rotated by mAngleX/Y/Z) into mBlendModelAnim.mat4x3, then sets unk_4d4[0..1]
   to the world positions of bones 2 and 3 (bone matrix times the model
   matrix, translation words shifted left by 3). In ..33b0, ..33e0, ..3410,
   ..33f0 and ..3370 it stops there; in every other state it also draws the
   drop shadow from mShadowMtx (mPos >> 3 with y lowered 10.0 (0xa000)),
   radius 300.0 (0x12c000), height 1000.0 (0x3e8000). */
// @symbol _ZN16daKing_Donketu_c19func_ov073_021215ccEv
void daKing_Donketu_c::func_ov073_021215cc()
{
    daKing_Donketu_c *c = this;
    int sh;
    Vec3 v;
    Vec3 out;
    void* m;
    Mtx43* saved;
    int i;
    Vec3* p;

    Vec3_Asr(&v, (Vec3*)&c->mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, c->mAngleX, c->mAngleY, c->mAngleZ);

    p = (Vec3*)&c->unk_4d4;
    c->mBlendModelAnim.mat4x3 = data_020a0e68;
    saved = (Mtx43*)&c->mBlendModelAnim.mat4x3;
    sh = 3;
    i = 0;
    for (; i < 2; i++) {
        data_020a0e68 = *(struct Matrix4x3 *)saved;
        MulMat4x3Mat4x3(*(Mtx43**)&c->mBlendModelAnim.data.transforms + (i + 2), &data_020a0e68, &data_020a0e68);
        p->x = data_020a0e68.m[9];
        p->y = data_020a0e68.m[10];
        p->z = data_020a0e68.m[11];
        Vec3_Lsl(&out, p, sh);
        c->unk_4d4[i].x = out.x;
        c->unk_4d4[i].y = out.y;
        c->unk_4d4[i].z = out.z;
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
    _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
        c, &c->mShadowModel, &c->mShadowMtx, 0x12c000, 0x3e8000, 0xf);
}

// @symbol _ZN16daKing_Donketu_c16CleanupResourcesEv
int daKing_Donketu_c::CleanupResources(){
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

// @symbol _ZN16daKing_Donketu_c16OnPendingDestroyEv
void daKing_Donketu_c::OnPendingDestroy()
{
}

// @symbol _ZN16daKing_Donketu_c6RenderEv
int daKing_Donketu_c::Render()
{
  ((Base *)&mBlendModelAnim)->M((char*)&mScaleX);
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
 * The first thing it does is store `this` into the camera object at +0x114
 * (dCamera_c.h has that word inside pad_114). What the camera does with it is not
 * recovered here.
 *
 * `opt_propagation off` is what stops the many position temporaries being
 * folded. It is bracketed to this one member: under `defer_codegen off` above,
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

// @symbol _ZN16daKing_Donketu_c8BehaviorEv
int daKing_Donketu_c::Behavior()
{
    char *self = (char *)this;
    C *c = (C *)this;
    int angx;
    Vector3 v0;
    RayParams rp;
    Vector3 v3C;
    Vector3 v48;
    Vector3 v54;

    *(C **)((char *)data_0209f318 + 0x114) = c;
    DecIfAbove0_Short(&mStateTimer);

    if (*(void **)((char *)c->pp + 8) != 0) {
        PMF *p = c->pp + 1;
        (c->**p)();
    }

    if ((char *)c->pp != (char *)&data_ov073_02123400
        && (char *)c->pp != (char *)&data_ov073_02123320
        && (char *)c->pp != (char *)&data_ov073_02123340) {
        UpdatePos(&mdCcAcPos_c);
    } else {
        int sum = mVertSpeed + mVertAccel;
        int m = mTerminalVelocity;
        int ac = unk_0ac;
        if (sum >= m) m = sum;
        mVertSpeed = m;
        unk_0ac = ac;
        UpdatePosWithOnlySpeed(&mdCcAcPos_c);
    }

    if (((char *)c->pp == (char *)&data_ov073_02123330 || (char *)c->pp == (char *)&data_ov073_02123350)
        && mBlendModelAnim.WillHitFrame(7) != 0) {
        data_020a0e68 = mBlendModelAnim.mat4x3;
        MulMat4x3Mat4x3((char *)mBlendModelAnim.data.transforms + 0x60, &data_020a0e68, &data_020a0e68);
        v0.x = data_020a0e68.m[9];
        v0.y = data_020a0e68.m[10];
        v0.z = data_020a0e68.m[11];
        Vec3_Lsl(&v3C, &v0, 3);
        v0 = v3C;
        func_02012694(SND_ANIM_FRAME_STOMP, &mCamSpacePosX);
        v48 = v0;
        HugeLandingDustAt(v48, 1);
    }

    if ((char *)c->pp != (char *)&data_ov073_021233c0
        && (char *)c->pp != (char *)&data_ov073_021233d0
        && (char *)c->pp != (char *)&data_ov073_021233f0
        && (char *)c->pp != (char *)&data_ov073_02123400
        && (char *)c->pp != (char *)&data_ov073_02123320
        && (char *)c->pp != (char *)&data_ov073_02123340
        && (char *)c->pp != (char *)&data_ov073_02123380) {
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
        MulVec3Mat4x3(&rp.in, &data_020a0e68, &rp.out);
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
            if ((char *)c->pp != (char *)&data_ov073_021233a0) {
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

    if ((char *)c->pp == (char *)&data_ov073_02123360
        || (char *)c->pp == (char *)&data_ov073_02123390) {
        ((daKing_Donketu_c *)self)->func_ov073_0211f61c();
    }
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.Update();
    ((daKing_Donketu_c *)self)->func_ov073_021215cc();
    mBlendModelAnim.Advance();
    return 1;
}

extern "C" {
extern void LoadKeyModels(int idx);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void* self, dActor_c* a, Vector3* v, Fix12i r, Fix12i h, unsigned int e, unsigned int g);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, dActor_c* a, Fix12i r, Fix12i h, Vector3_16* p, Vector3_16* q);
}

#pragma pop

// @symbol _ZN16daKing_Donketu_c13InitResourcesEv
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
    Model::LoadFile(*(SharedFilePtr *)data_ov002_0210da30);
    f = (BMD_File *)Model::LoadFile(gKingIceDonketuModelModelFile);
    mBlendModelAnim.SetFile(f, 1, -1);
    mShadowModel.InitCylinder();
    mVertAccel = -0x3000;
    mTerminalVelocity = -0x3c000;
    v.x = data_ov073_02123040.x;
    v.y = data_ov073_02123040.y;
    v.z = data_ov073_02123040.z;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, &v, 0xa4000, 0x1e4000, 0x200000, 0x567f0);
    mBlendModelAnim.speed = 0x2000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x96000, 0x94000, 0, 0);
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    for (i = 0; i < 8; i++) {
        mWaypointsA[i].x = mPosX;
        mWaypointsA[i].y = mPosY;
        mWaypointsA[i].z = mPosZ;
        mWaypointsB[i].x = mPosX;
        mWaypointsB[i].y = mPosY;
        mWaypointsB[i].z = mPosZ;
    }
    unk_4c5 = 0xff;
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

// @symbol _ZN16daKing_Donketu_c16OnAimedAtWithEggEv
s32 daKing_Donketu_c::OnAimedAtWithEgg()
{
    return 0x64000;
}

/* Construct the KING_DONKETU actor and its members in declaration order. */
// @symbol daKing_Donketu_c_classInit
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

daKing_Donketu_c::State data_ov073_02123330 = { &daKing_Donketu_c::func_ov073_02121538,
                                                &daKing_Donketu_c::func_ov073_02121388 };
daKing_Donketu_c::State data_ov073_02123350 = { &daKing_Donketu_c::func_ov073_02121378,
                                                &daKing_Donketu_c::func_ov073_0212128c };
daKing_Donketu_c::State data_ov073_02123360 = { &daKing_Donketu_c::func_ov073_0212122c,
                                                &daKing_Donketu_c::func_ov073_02120ed0 };
daKing_Donketu_c::State data_ov073_02123390 = { &daKing_Donketu_c::func_ov073_02120e60,
                                                &daKing_Donketu_c::func_ov073_02120dec };
daKing_Donketu_c::State data_ov073_021233a0 = { &daKing_Donketu_c::func_ov073_02120d80,
                                                &daKing_Donketu_c::func_ov073_02120c7c };
daKing_Donketu_c::State data_ov073_021233c0 = { &daKing_Donketu_c::func_ov073_02120c08,
                                                &daKing_Donketu_c::func_ov073_02120b78 };
daKing_Donketu_c::State data_ov073_021233d0 = { &daKing_Donketu_c::func_ov073_02120ad8,
                                                &daKing_Donketu_c::func_ov073_02120910 };
daKing_Donketu_c::State data_ov073_021233f0 = { &daKing_Donketu_c::func_ov073_021208e4,
                                                &daKing_Donketu_c::func_ov073_02120844 };
daKing_Donketu_c::State data_ov073_02123400 = { &daKing_Donketu_c::func_ov073_0212081c,
                                                &daKing_Donketu_c::func_ov073_02120610 };
daKing_Donketu_c::State data_ov073_02123320 = { &daKing_Donketu_c::func_ov073_021205f0,
                                                &daKing_Donketu_c::func_ov073_021203ac };
daKing_Donketu_c::State data_ov073_02123340 = { &daKing_Donketu_c::func_ov073_02120390,
                                                &daKing_Donketu_c::func_ov073_021200e0 };
daKing_Donketu_c::State data_ov073_02123380 = { &daKing_Donketu_c::func_ov073_02120098,
                                                &daKing_Donketu_c::func_ov073_0212005c };
daKing_Donketu_c::State data_ov073_021233b0 = { (int (daKing_Donketu_c::*)())&daKing_Donketu_c::func_ov073_0212000c,
                                                &daKing_Donketu_c::func_ov073_0211fe8c };
daKing_Donketu_c::State data_ov073_021233e0 = { &daKing_Donketu_c::func_ov073_0211fe84,
                                                &daKing_Donketu_c::func_ov073_0211fc78 };
daKing_Donketu_c::State data_ov073_02123410 = { &daKing_Donketu_c::func_ov073_0211fc70,
                                                &daKing_Donketu_c::func_ov073_0211fbf4 };
daKing_Donketu_c::State data_ov073_02123370 = { &daKing_Donketu_c::func_ov073_0211fbec,
                                                &daKing_Donketu_c::func_ov073_0211fa74 };
