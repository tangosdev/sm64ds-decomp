//cpp
/* daC_Jugem_c -- Lakitu in the level (C_JUGEM). He turns to face Mario,
 * takes his control, and talks through message 0x182 until Mario has
 * finished listening. ov085, 33 functions (.text 0x0212d528..0x0212edac).
 *
 * Class name is ROM RTTI: "11daC_Jugem_c" at 0x02130310,
 * __si_class_type_info 0x02130304, vtable 0x02130344. The destructor is the
 * key function; out-of-line under `#pragma defer_codegen off` it emits D1
 * then D0, so source order is ROM-ascending. Do not reorder.
 *
 * Behavior runs a twelve-record state machine: each record is a pair of
 * pointers-to-member (enter, then run), installed by SetState and stored
 * in mState. The variant-1 path opens with StateArrive -> StateTurn ->
 * StateGuide -> StateBob -> StateApproach; both paths then share
 * StateIntro (the camera sweep), StateHidden (wait for the player),
 * StateFlyToPlayer, StateTalk and StateLeave. StateHover is the ambient
 * variant-0 state.
 *
comment leftovers:
 *   - Calls whose real signatures take Fix12<int> BY VALUE stay mangled:
 *     Sound::PlaySub, ModelAnim::SetAnim, TextureSequence::SetFile and
 *     dActor_c::DropShadowRadHeight. mwccarm passes those differently at
 *     the call site than the loose scalar spelling; this file's externs
 *     are the spelling that matches.
 *   - The SharedFilePtr triple, the twelve state records, the two Vector3
 *     globals and the arm9 level globals keep linker names; their
 *     definitions live in the module sinit and data TUs. The records are
 *     declared here as StateFn[2]; the sinit file writes them as pairs of
 *     8-byte words, which is the same object.
 *   - Matrix copies go through local 12-word structs (M48 in UpdateShadow,
 *     M48e858 in UpdateShadowPlayer): types.h's Vector3 and math/Matrix.h's
 *     Matrix4x3 are non-POD here, so a typed copy scalarizes or destroys.
 *   - CamPose, V3 and the local Vec3/Vector3 spellings are the same kind
 *     of non-POD dodge for vector locals.
 *   - unk_2e0 is written once (StateApproachInit) and never read.
 *   - mAuxCounter is per-state scratch: the approach turn rate in
 *     StateApproachMain, the bob phase in StateBobMain, a play-once flag
 *     in StateHoverMain. No single name covers all three.
 */

#include "daC_Jugem_c.h"
#include "common.h"
#include "dActor_c.h"
#include "Player.h"
#include "dCamera_c.h"
#include "SharedFilePtr.h"
#include "Sound.h"

bool ApproachLinear(short &value, short target, short step);

extern "C" {

/* math / vector helpers -- no shared header declares these */
void  _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(void *cur, const void *tgt, int step);
void  _Z14ApproachLinearRiii(int *cur, int tgt, int step);
int   Vec3_Dist(const void *a, const void *b);
int   Vec3_HorzDist(const void *a, const void *b);
s16   Vec3_HorzAngle(const void *a, const void *b);
s16   Vec3_VertAngle(const void *a, const void *b);
void  Vec3_Sub(void *out, const void *a, const void *b);
void  Vec3_Asr(void *dst, const void *src, int sh);
int   Vec3_ApproachHorz(void *cur, const void *tgt, int maxStep);
int   LenVec3(const void *v);
int   AngleDiff(int a, int b);
u16   DecIfAbove0_Short(u16 *p);
void  Matrix4x3_FromRotationY(void *m, int angY);
void  Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void  Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
void  Matrix4x3_ApplyInPlaceToRotationZXYExt(void *m, int x, int y, int z);
void  MulVec3Mat4x3(const void *v, const void *m, void *out);

/* local extern: each of these real members takes a Fix12<int> BY VALUE,
   which mwccarm passes differently at the call site than the loose scalar
   spelling -- the mangled spelling is the one that matches. Sound.h covers
   PlayLong, so only PlaySub stays. */
int   _ZN5Sound7PlaySubEjjj5Fix12IiEb(u32 a, u32 b, u32 c, Fix12i d, bool loop);
void  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thiz, void *f, int a, Fix12i b, u32 c);
void  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *thiz, BTP_File &f, int a, Fix12i b, u32 c);
void  _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
         void *thiz, void *sm, void *mtx, int rad, int height, u32 flags);

/* other overlays / arm9 */
void  func_ov002_020c3e8c(void *player);
void  func_ov002_020c3f18(void *p);
void  func_ov002_020c3f2c(void *p);
void  func_ov002_020d228c(void *p);
void  func_ov002_020e4374(void *p, int *a, int *b);
void  func_0201f32c(int a);
void  func_02012790(int a);

/* globals */
extern void          *data_0209f318;        /* the camera object */
extern int            data_0209caa0[];      /* level progress flags */
extern u8             data_0209d66c;
extern u8             data_0209d6bc;
extern u8             data_0209f284;
extern s8             data_0209f2f8;
extern short          data_02082214[];      /* the sine lookup table */
extern Matrix4x3      data_020a0e68;        /* scratch matrix */

extern SharedFilePtr  data_ov085_0213074c;  /* model file */
extern SharedFilePtr  data_ov085_02130744;  /* animation file */
extern SharedFilePtr  data_ov085_0213073c;  /* texture sequence file */
extern Vector3        data_ov085_02130840;  /* home position */
extern Vector3        data_ov085_0213084c;  /* approach target */

/* The twelve state records, {enter, run} pairs written by the module sinit. */
extern daC_Jugem_c::StateFn data_ov085_02130790[2];
extern daC_Jugem_c::StateFn data_ov085_021307a0[2];
extern daC_Jugem_c::StateFn data_ov085_021307b0[2];
extern daC_Jugem_c::StateFn data_ov085_021307c0[2];
extern daC_Jugem_c::StateFn data_ov085_021307d0[2];
extern daC_Jugem_c::StateFn data_ov085_021307e0[2];
extern daC_Jugem_c::StateFn data_ov085_021307f0[2];
extern daC_Jugem_c::StateFn data_ov085_02130800[2];
extern daC_Jugem_c::StateFn data_ov085_02130810[2];
extern daC_Jugem_c::StateFn data_ov085_02130820[2];
extern daC_Jugem_c::StateFn data_ov085_02130830[2];

}

/* The intro camera pose StateIntroMain sweeps from. */
struct CamPose { s32 lx, ly, lz, px, py, pz; };

/* Flat three-word vector view for block copies where the real Vector3
   (non-POD here, it has a declared destructor) would change codegen. */
struct V3 { s32 x, y, z; };
struct M48 { s32 w[12]; };

#pragma defer_codegen off

// @symbol _ZN11daC_Jugem_cD1Ev
// @symbol _ZN11daC_Jugem_cD0Ev
/* Empty body: one vptr store and the member destructors in reverse
   declaration order are the whole work, and D0's deallocation is the
   inline operator delete reached through dEnemyBase_c. */
daC_Jugem_c::~daC_Jugem_c()
{
}

// @symbol _ZN11daC_Jugem_c14StateIntroMainEv
/* The intro camera sweep: pull the look-at and eye down toward the player
   while the fanfare plays; on the last frame hand the camera back and go
   wait in StateHidden. */
int daC_Jugem_c::StateIntroMain()
{
  dCamera_c *cam = (dCamera_c *)data_0209f318;
  CamPose r;
  cam->SetFlag_3();
  r.lx = -0x4b0000;
  r.ly = 0x19f000;
  r.lz = 0x1a90000;
  r.px = -0x4b0000;
  r.py = 0x250000;
  r.pz = 0x1d4c000;
  _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(&mCamLookX, &r, 0x70000);
  cam->SetLookAt(*(Vector3 *)&mCamLookX);
  cam->SetPos(*(Vector3 *)&mCamPosX);
  Vec3_Dist(&mCamLookX, &r);
  mTimer++;
  if (mTimer > 0x64) {
    if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x4b, 0x7f, 0, 0x7222, false) != 0) {
      *(int *)&cam->mFlags &= ~8;
      mHorzSpeed = 0;
      mTimer = 0;
      mAuxCounter = 0;
      unk_0a4 = 0;
      mVertSpeed = 0;
      unk_0ac = 0;
      mAngleX = 0;
      Player *player = ClosestPlayer();
      if (player != 0) {
        func_ov002_020c3e8c(player);
        data_0209caa0[2] |= 0x80;
      }
      SetState(data_ov085_021307e0);
    }
  }
  return 1;
}

// @symbol _ZN11daC_Jugem_c14StateIntroInitEv
int daC_Jugem_c::StateIntroInit()
{
  mHidden = 1;
  mTimer = 0;
  return 1;
}

// @symbol _ZN11daC_Jugem_c17StateApproachMainEv
/* Fly toward the fixed vantage point, camera tracking, until close enough
   to start the intro sweep. */
int daC_Jugem_c::StateApproachMain()
{
    Vector3 v[3];
    dCamera_c *cam;
    int spd;
    int len;

    cam = (dCamera_c *)data_0209f318;
    cam->SetFlag_3();
    mSfxHandle = Sound::PlayLong(mSfxHandle, 3, 0x182, *(Vector3 *)&mCamSpacePosX, 0);
    ApproachLinear(mAngleY, Vec3_HorzAngle(&mPosX, &data_ov085_0213084c), 0x200);
    ApproachLinear(mAngleX, Vec3_VertAngle(&mPosX, &data_ov085_0213084c), 0x200);
    _Z14ApproachLinearRiii(&mHorzSpeed, 0x28000, 0x2000);
    ApproachLinear(mPrevAngleY, Vec3_HorzAngle(&mPosX, &data_ov085_0213084c), (s16)mAuxCounter);
    mAngleY = mPrevAngleY;
    mAuxCounter += 5;
    if (mAuxCounter > 0x800)
        mAuxCounter = 0x800;
    mTimer += 1;
    if (mTimer < 0x19)
        return 1;
    v[1].x = -0x50c000;
    v[1].y = 0x115000;
    v[1].z = 0x1d15000;
    spd = mHorzSpeed >> 1;
    _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(&mCamLookX, &mPosX, spd);
    _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(&mCamPosX, &v[1], spd);
    cam->SetLookAt(*(Vector3 *)&mCamLookX);
    cam->SetPos(*(Vector3 *)&mCamPosX);
    Vec3_Sub(&v[2], &mPosX, &data_ov085_0213084c);
    len = LenVec3(&v[2]);
    if (len == 0 || len < 0x7d0000)
        SetState(data_ov085_021307a0);
    return 1;
}

// @symbol _ZN11daC_Jugem_c17StateApproachInitEv
int daC_Jugem_c::StateApproachInit()
{
  volatile Vector3 look, pos; (void)&look; (void)&pos;
  dCamera_c *cam;
  mHorzSpeed = 0;
  mTimer = 0;
  mAuxCounter = 0;
  unk_2e0 = 0;
  cam = (dCamera_c *)data_0209f318;
  cam->SetFlag_3();
  mCamLookX = 0xffc67000;
  mCamLookY = 0x6ea000;
  mCamLookZ = 0x1212000;
  mCamPosX = 0xffb65000;
  mCamPosY = 0x1d5000;
  mCamPosZ = 0x17fc000;
  cam->SetLookAt(*(Vector3 *)&mCamLookX);
  cam->SetPos(*(Vector3 *)&mCamPosX);
  mAuxCounter = 0xa0;
  mPosX = data_ov085_02130840.x;
  mPosY = data_ov085_02130840.y;
  mPosZ = data_ov085_02130840.z;
  return 1;
}

// @symbol _ZN11daC_Jugem_c12StateBobMainEv
/* Bob on a sine wave and pick up speed; at the end of the timer switch to
   the long approach. */
int daC_Jugem_c::StateBobMain()
{
    Player *pl = ClosestPlayer();
    if (pl == 0) return 1;

    mTimer += 1;
    mAuxCounter += 0x500;

    {
        int v = (short)mAuxCounter;
        int idx = (int)((unsigned int)(v << 16) >> 16) >> 4;
        short e = data_02082214[idx * 2];
        _Z14ApproachLinearRiii(&mPosY,
            (int)(((long long)e * 0x1a000 + 0x800) >> 12) + mTargetY,
            0x10000000);
    }

    if (mTimer == 0x32) {
        pl->func_ov002_020c3ea0();
    }
    mAngleY = 0x6000;
    mPrevAngleY = mAngleY;
    {
        int a1 = 0x4000;
        if (mTimer >= 0x4b) a1 = 0x8000;
        _Z14ApproachLinearRiii(&mHorzSpeed, a1, 0x1000);
    }

    mSfxHandle = Sound::PlayLong(
        mSfxHandle, 3, 0x182, *(Vector3 *)&mCamSpacePosX, 0);

    if (mTimer > 0x78) {
        mAngleY = 0x4000;
        mPrevAngleY = mAngleY;
        SetState(data_ov085_02130820);
    }
    return 1;
}

// @symbol _ZN11daC_Jugem_c12StateBobInitEv
int daC_Jugem_c::StateBobInit()
{
  Vector3 look, pos;
  dCamera_c *cam;
  mTimer = 0;
  mAuxCounter = 0;
  mPosX = -0x5a0000;
  mPosY = 0x2c0000;
  mPosZ = 0x1c6f000;
  mTargetX = mPosX;
  mTargetY = mPosY;
  mTargetZ = mPosZ;
  cam = (dCamera_c *)data_0209f318;
  cam->SetFlag_3();
  look.x = -0x304000;
  look.y = 0x3c1000;
  look.z = 0x1c77000;
  pos.x = -0x540000;
  pos.y = 0xe1000;
  pos.z = 0x19e4000;
  cam->SetLookAt(*(Vector3 *)&look);
  cam->SetPos(*(Vector3 *)&pos);
  mHorzSpeed = 0;
  return 1;
}

// @symbol _ZN11daC_Jugem_c14StateGuideMainEv
/* Walk the player into the talking spot on a fixed beat: message steps at
   0x14 and 0x28..0xee, player angles snapped at 0xf0. */
int daC_Jugem_c::StateGuideMain()
{
    Player *p = ClosestPlayer();
    if (p == 0)
        return 1;
    mTimer += 1;
    switch (mTimer) {
    case 1:
        func_0201f32c(2);
        break;
    case 0x14:
        func_ov002_020c3f2c(p);
        break;
    case 0x28:
        data_0209d66c = 1;
        break;
    case 0x8c:
        func_0201f32c(3);
        break;
    case 0xee:
        data_0209d66c = 1;
        break;
    case 0xf0:
        {
            s16 a = mSavedAngleY;
            s16 b;
            p->mAngleX = 0;
            p->mAngleY = a;
            p->mAngleZ = 0;
            b = mSavedAngleY;
            p->mPrevAngleX = 0;
            p->mPrevAngleY = b;
            p->mPrevAngleZ = 0;
            SetState(data_ov085_021307f0);
        }
        break;
    }
    return 1;
}

// @symbol _ZN11daC_Jugem_c14StateGuideInitEv
int daC_Jugem_c::StateGuideInit()
{
    mTalkPlayer = 0;
    mTimer = 0;
    return 1;
}

// @symbol _ZN11daC_Jugem_c13StateTurnMainEv
/* Turn the player around to face Lakitu while Lakitu pitches up toward
   him, then move on to StateGuide. */
int daC_Jugem_c::StateTurnMain()
{
    Player *p = ClosestPlayer();
    if (!p) return 1;
    {
        short v = mAngleY;
        short w = v + 0x8000;
        p->mAngleX = 0;
        p->mAngleY = w;
        p->mAngleZ = 0;
        mTimer += 1;
    }
    if (mTimer == 0x5a) func_ov002_020c3f18(p);
    {
        int s = mTimer;
        if (s > 0x57 && s < 0x5b)
            ApproachLinear(mAngleX, 0x2000, 0x400);
        else
            ApproachLinear(mAngleX, 0x1000, 0x400);
    }
    if (mTimer > 0x78)
        SetState(data_ov085_021307c0);
    return 1;
}

// @symbol _ZN11daC_Jugem_c13StateTurnInitEv
int daC_Jugem_c::StateTurnInit()
{
  Vector3 look, pos;
  dCamera_c *cam;
  mTimer = 0;
  mTalkPlayer = 0;
  mAngleZ = 0;
  cam = (dCamera_c *)data_0209f318;
  cam->SetFlag_3();
  look.x = 0xffadd000;
  look.y = 0x17e000;
  look.z = 0x1a29000;
  pos.x = 0xffa54000;
  pos.y = 0x1f4000;
  pos.z = 0x1ccf000;
  cam->SetLookAt(*(Vector3 *)&look);
  cam->SetPos(*(Vector3 *)&pos);
  return 1;
}

// @symbol _ZN11daC_Jugem_c15StateArriveMainEv
/* Face the player and pitch up, stepping through the greeting beats; the
   last beat hands off to StateTurn. */
int daC_Jugem_c::StateArriveMain()
{
    Player *p = ClosestPlayer();
    if (p == 0)
        return 1;
    {
        short t = mAngleY + 0x8000;
        p->mAngleX = 0;
        p->mAngleY = t;
        p->mAngleZ = 0;
    }
    mAngleY = HorzAngleToCPlayer();
    mHidden = 0;
    mAngleX = 0x1000;
    mAngleZ = 0x800;
    mTimer += 1;
    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x4b, 0x14, 0x7f, 0x15666, false);
    switch (mTimer) {
    case 1:
        p->unk_723 = 1;
        break;
    case 0x78:
        func_0201f32c(1);
        break;
    case 0x82:
        p->unk_723 = 0;
        break;
    case 0xb4:
        data_0209d66c = 1;
        break;
    case 0xd2:
        SetState(data_ov085_021307b0);
        break;
    }
    return 1;
}

// @symbol _ZN11daC_Jugem_c15StateArriveInitEv
int daC_Jugem_c::StateArriveInit()
{
    Vector3 look, pos;
    dCamera_c *cam;
    Player* player;
    mTargetX = -0x5a0000;
    mTargetY = 0x1c0000;
    mTargetZ = 0x1a66000;
    player = ClosestPlayer();
    if (player) {
        mSavedAngleY = player->mAngleY;
        func_ov002_020d228c(player);
    }
    cam = (dCamera_c *)data_0209f318;
    cam->SetFlag_3();
    look.x = 0xff883000;
    look.y = 0x2ef000;
    look.z = 0x1a36000;
    pos.x = 0xffb18000;
    pos.y = 0x18c000;
    pos.z = 0x1a89000;
    cam->SetLookAt(*(Vector3 *)&look);
    cam->SetPos(*(Vector3 *)&pos);
    mStateTimer = 0x79;
    mPosX = mTargetX;
    mPosY = mTargetY;
    mPosZ = mTargetZ;
    mTalkPlayer = 0;
    return 1;
}

// @symbol _ZN11daC_Jugem_c14StateLeaveMainEv
/* Fly to the target point; when the countdown ends and the talk is over,
   mark for destruction. */
int daC_Jugem_c::StateLeaveMain()
{
    ApproachLinear(mPrevAngleY, Vec3_HorzAngle(&mPosX, &mTargetX), 0x800);
    ApproachLinear(mAngleY, mPrevAngleY, 0x800);
    mSfxHandle = Sound::PlayLong(
        mSfxHandle, 3, 0x182, *(Vector3 *)&mCamSpacePosX, 0);
    if (AngleDiff(mPrevAngleY, Vec3_HorzAngle(&mPosX, &mTargetX)) < 0x2000) {
        Vec3_ApproachHorz(&mPosX, &mTargetX, 0x1e000);
        _Z14ApproachLinearRiii(&mPosY, mTargetY, 0x1e000);
    }
    if (*(unsigned short *)&mStateTimer == 0) {
        if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x4a, 0x7f, 0, 0x7222, 0) != 0) {
            if (mTalkPlayer->HasFinishedTalking() == 1) {
                MarkForDestruction();
                data_0209f284 = 0;
            }
        }
    }
    return 1;
}

// @symbol _ZN11daC_Jugem_c14StateLeaveInitEv
int daC_Jugem_c::StateLeaveInit()
{
    short r3 = mAngleY;
    mPrevAngleY = r3;
    mStateTimer = 0x46;
    return 1;
}

// @symbol _ZN11daC_Jugem_c13StateTalkMainEv
/* The message 0x182 exchange, step by step; when the talk is done go
   StateLeave. */
int daC_Jugem_c::StateTalkMain()
{
    ApproachLinear(mAngleY, HorzAngleToCPlayer(), 0x800);
    switch (mTalkStep) {
    case 0:
        {
            mTalkPlayer->mStateFlags |= 0x400;
        }
        if (mTalkPlayer->ShowMessage(*this, 0x182, (Vector3 *)&mPosX, 1, 0) == 1) {
            mTalkStep += 1;
        }
        break;
    case 1:
        if (data_0209d6bc == 7) { mTalkStep += 1; }
        break;
    case 2:
        if (data_0209d6bc == 7)
            break;
        func_02012790(0x24);
        data_0209f284 = 1;
        { mTalkStep += 1; }
        break;
    case 3:
        if (data_0209d6bc == 9) { mTalkStep += 1; }
        break;
    case 4:
        if (mTalkPlayer->GetTalkState() == 2)
            SetState(data_ov085_02130830);
        break;
    }
    return 1;
}

// @symbol _ZN11daC_Jugem_c13StateTalkInitEv
int daC_Jugem_c::StateTalkInit()
{
    int *p = data_0209caa0;
    p[2] |= 0x20000;
    mTalkStep = 0;
    return 1;
}

// @symbol _ZN11daC_Jugem_c20StateFlyToPlayerMainEv
/* Home in on a point above and ahead of the player; once close enough,
   start the talk. */
int daC_Jugem_c::StateFlyToPlayerMain()
{
    Vector3 in;
    Vector3 out;
    Player *p;

    p = ClosestPlayer();
    if (p == 0) {
        return 1;
    }

    p->unk_744 = mPosX;
    p->unk_748 = mPosY;
    p->unk_74c = mPosZ;

    if (*(unsigned short *)&mStateTimer == 0) {
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x4a, 0x14, 0x7f, 0x15666, 0);
    }

    mSfxHandle = Sound::PlayLong(mSfxHandle, 3, 0x182, *(Vector3 *)&mCamSpacePosX, 0);

    in.x = 0;
    in.y = 0;
    in.z = 0;
    out.x = 0;
    out.y = 0;
    out.z = 0;
    in.z = 0xc8000;
    Matrix4x3_FromRotationY(&data_020a0e68, 0);
    MulVec3Mat4x3(&in, &data_020a0e68, &out);
    out.x += p->mPosX;
    out.y += p->mPosY + 0x70000;
    out.z += p->mPosZ;

    _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(&mPosX, &out, 0x14000);
    ApproachLinear(mPrevAngleY, Vec3_HorzAngle(&mPosX, &out), 0x800);
    mAngleY = mPrevAngleY;

    if (Vec3_Dist(&mPosX, &out) < 0x14000) {
        mTalkPlayer = p;
        SetState(data_ov085_02130810);
    }

    return 1;
}

// @symbol _ZN11daC_Jugem_c20StateFlyToPlayerInitEv
int daC_Jugem_c::StateFlyToPlayerInit()
{
    mPosZ = 2457600;
    mTimer = 0;
    mStateTimer = 40;
    return 1;
}

// @symbol _ZN11daC_Jugem_c15StateHiddenMainEv
/* Hidden; watch for the player to step inside the trigger volume, then
   take his control and fly to him. */
#pragma opt_common_subs off
int daC_Jugem_c::StateHiddenMain()
{
    Player *p = ClosestPlayer();
    if (p != 0) {
        V3 v = *(V3 *)&p->mPosX;
        if ((data_0209caa0[2] & 0x10000) != 0 &&
            v.z > -0x28000 &&
            p->SetNoControlState(0x12, -1, 0) != 0) {
            p->unk_744 = mPosX;
            p->unk_748 = mPosY;
            p->unk_74c = mPosZ;
            mPosX = v.x;
            mPosY = v.y;
            mPosZ = v.z;
            mPosX -= 0x3e8000;
            mPosY = p->mGroundY + 0x3e8000;
            mTargetX = mPosX;
            mTargetY = mPosY;
            mTargetZ = mPosZ;
            mHidden = 0;
            SetState(data_ov085_02130800);
        }
    }
    return 1;
}
#pragma opt_common_subs on

// @symbol _ZN11daC_Jugem_c15StateHiddenInitEv
int daC_Jugem_c::StateHiddenInit()
{
    mHidden = 1; return 1;
}

// @symbol _ZN11daC_Jugem_c14StateHoverMainEv
/* The ambient state: hover in place beside the camera, one shadow tracking
   the player, one sound trigger behind it. */
int daC_Jugem_c::StateHoverMain()
{
    Vector3 in, out, plpos;
    dCamera_c *cam;
    Vector3* src;
    Player *p;

    cam = (dCamera_c *)data_0209f318;
    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;
    src = &cam->pos;
    plpos.x = src->x;
    plpos.y = src->y;
    plpos.z = src->z;
    if (data_0209f2f8 == 0x2f) {
        mPosX = 0;
    } else {
        mPosX = 0x1086000;
        if (mAuxCounter == 0) {
            p = ClosestPlayer();
            if (p != 0 && p->mPosX > 0x1086000) {
                SpawnSoundObj(0);
                mAuxCounter = 1;
            }
        }
    }
    mPosY = plpos.y;
    mPosZ = plpos.z;
    in.z = Vec3_HorzDist(&plpos, &mPosX);
    Matrix4x3_FromRotationY(&data_020a0e68, Vec3_HorzAngle(&plpos, &mPosX));
    MulVec3Mat4x3(&in, &data_020a0e68, &out);
    mPosX += out.x;
    if (data_0209f2f8 == 0x2f) {
        if (mPosX > 0x79e000) {
            mPosX = 0x79e000;
        }
    }
    mPosZ += out.z;
    mPrevAngleY = 0x8000 - cam->mAngleY;
    /* The halfword after dCamera_c::mAngleY at 0x17e (dCamera_c.h has no member for
       it). Possibly the pitch, but only this one negated use suggests so. */
    mPrevAngleX = -*(short *)((char *)&cam->mAngleY + 2);
    return 1;
}

// @symbol _ZN11daC_Jugem_c14StateHoverInitEv
int daC_Jugem_c::StateHoverInit()
{
    return 1;
}

// @symbol _ZN11daC_Jugem_c8SetStateEPMS_FivE
/* Install a state record and run its enter slot, if it has one. */
int daC_Jugem_c::SetState(StateFn *record)
{
    mState = record;
    StateFn *q = mState;
    if (*q == 0) return 1;
    return (this->**q)();
}

// @symbol _ZN11daC_Jugem_c12UpdateShadowEv
/* The single blob shadow, used by the cutscene variant. */
void daC_Jugem_c::UpdateShadow()
{
    V3 v;
    Vec3_Asr(&v, &mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68, mAngleX, mAngleY, mAngleZ);
    *(M48 *)&mModelAnim1.mat4x3 = *(M48 *)&data_020a0e68;
    Matrix4x3_FromTranslation(&data_020a0e68, mPosX >> 3, (mPosY - 0x38000) >> 3, mPosZ >> 3);
    *(M48 *)&mShadowMat1 = *(M48 *)&data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
        this, &mShadowModel1, &mShadowMat1, 0x46000, 0x258000, 0xf);
}

// @symbol _ZN11daC_Jugem_c18UpdateShadowPlayerEv
/* The ambient variant's two shadows: one at Lakitu's own position, one
   projected ahead of the player. The matrix copies stay 12-word block
   moves through M48; the shared Matrix4x3 spelling scalarizes them. */
void daC_Jugem_c::UpdateShadowPlayer()
{
    struct Vector3 p;
    struct Vector3 z1;
    struct Vector3 off;
    struct Vector3 pp;
    struct Vector3 t;
    Player *pl;
    int p1, p2;
    signed char lvl;
    struct Vector3 *ps;

    Vec3_Asr(&t, &mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, t.x, t.y, t.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, mAngleX, mAngleY, mAngleZ);
    *(M48 *)&mModelAnim1.mat4x3 = *(M48 *)&data_020a0e68;

    Matrix4x3_FromTranslation(&data_020a0e68,
        mPosX >> 3,
        (mPosY - 0x38000) >> 3,
        mPosZ >> 3);
    *(M48 *)&mShadowMat1 = *(M48 *)&data_020a0e68;

    _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
        this, &mShadowModel1, &mShadowMat1, 0x46000, 0x258000, 0xf);

    pl = ClosestPlayer();
    if (pl == 0) return;

    lvl = data_0209f2f8;
    p.x = 0;
    p.y = 0;
    p.z = 0;
    z1.x = 0;
    z1.y = 0;
    z1.z = 0;
    off.x = 0;
    off.y = 0;
    off.z = 0;
    ps = (struct Vector3 *)&pl->mPosX;
    pp.x = ps->x;
    pp.y = ps->y;
    pp.z = ps->z;
    if (lvl == 0x2f) {
        p.x = 0;
    } else {
        p.x = 0x1086000;
    }
    p.y = pp.y;
    p.z = pp.z;

    z1.z = Vec3_HorzDist(&pp, &p);
    {
        short ang = Vec3_HorzAngle(&pp, &p);
        Matrix4x3_FromRotationY(&data_020a0e68, ang);
    }
    MulVec3Mat4x3(&z1, &data_020a0e68, &off);

    p.x = p.x + off.x;
    p.y = pp.y;
    p.z = p.z + off.z;
    Matrix4x3_FromTranslation(&data_020a0e68, p.x >> 3, p.y >> 3, p.z >> 3);

    func_ov002_020e4374(pl, &p1, &p2);

    *(M48 *)&mShadowMat2 = *(M48 *)&data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
        this, &mShadowModel2, &mShadowMat2, p2, p1, 0xf);
}

// @symbol _ZN11daC_Jugem_c16CleanupResourcesEv
int daC_Jugem_c::CleanupResources()
{
    data_ov085_0213074c.Release();
    data_ov085_02130744.Release();
    data_ov085_0213073c.Release();
    return 1;
}

// @symbol _ZN11daC_Jugem_c16OnPendingDestroyEv
/* Empty on purpose: the override suppresses whatever the base does on
   pending destroy. */
void daC_Jugem_c::OnPendingDestroy()
{
}

// @symbol _ZN11daC_Jugem_c6RenderEv
int daC_Jugem_c::Render()
{
  if (mHidden == 1) return 1;
  mTextureSequence.Update(mModelAnim1.data);
  mModelAnim1.Render(0);
  return 1;
}

// @symbol _ZN11daC_Jugem_c8BehaviorEv
int daC_Jugem_c::Behavior()
{
  DecIfAbove0_Short((unsigned short *)&mStateTimer);
  if (mState[1]) (this->*mState[1])();
  UpdatePos(0);
  mModelAnim1.Advance();
  mTextureSequence.Advance();
  if (mState == data_ov085_021307d0) {
    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
  }
  if (mState == data_ov085_021307e0) return 1;
  if (mVariant == 0) UpdateShadowPlayer();
  else UpdateShadow();
  return 1;
}

// @symbol _ZN11daC_Jugem_c13InitResourcesEv
int daC_Jugem_c::InitResources()
{
  BMD_File* bmd = (BMD_File*)Model::LoadFile(data_ov085_0213074c);
  mModelAnim1.SetFile(bmd, 1, -1);
  dExtFrameCtrl_c::LoadFile(data_ov085_02130744);
  TextureSequence::LoadFile(data_ov085_0213073c);
  mShadowModel1.InitCylinder();
  mShadowModel2.InitCylinder();
  TextureSequence::Prepare(
      **(BMD_File**)((char*)&data_ov085_0213074c + 4), **(BTP_File**)((char*)&data_ov085_0213073c + 4));
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
      &mModelAnim1, *(BCA_File**)((char*)&data_ov085_02130744 + 4), 0, 0x1000, 0);
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
      &mTextureSequence, **(BTP_File**)((char*)&data_ov085_0213073c + 4), 0, 0x1000, 0);
  mModelAnim1.speed = 0x1000;
  mVariant = param1 & 0xff;
  if (mVariant == 0xff)
    mVariant = 0;
  switch (mVariant) {
  case 0:
    SetState(data_ov085_021307d0);
    break;
  case 1:
    {
      int v = data_0209caa0[2];
      if (v & 0x20000)
        return 0;
      if (v & 0x10000)
        data_0209caa0[2] = v & ~0x10000;
      mHidden = 1;
      if (!(data_0209caa0[2] & 0x80))
        SetState(data_ov085_02130790);
      else
        SetState(data_ov085_021307e0);
    }
    break;
  }
  return 1;
}

// @symbol daC_Jugem_c_classInit
/* The registry factory behind the C_JUGEM profile: allocates this class's
   0x2e8 and installs its vtable. Reconstructed source-style name
   (historical alias LakituBro_Spawn); the exact original spelling is not
   preserved. */
extern "C" daC_Jugem_c *daC_Jugem_c_classInit(void)
{
    return new daC_Jugem_c();
}
