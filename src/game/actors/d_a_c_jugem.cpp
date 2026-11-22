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
 *   - The SharedFilePtr triple, the eleven state records and the two
 *     carry-offset vectors are file-scope objects at the end of this file
 *     (folded sinit 0x0212fa40). The records stay StateFn[2]; the sinit
 *     pairs they copy from are unlicensed .data descriptors.
 *   - Position locals are Vector3. Several are written and never read
 *     (StateIntroMain's pos, StateApproachInit's look/pos); Vector3's
 *     declared destructor is what keeps those stores and frame slots.
 *     StateHiddenMain's copy of the player position is the exception: the
 *     ROM block-copies it (ldm/stm), which only a POD VecFx32 produces.
 *   - unk_2e0 is written once (StateApproachInit) and never read.
 *   - mAuxCounter is per-state scratch: the approach turn rate in
 *     StateApproachMain, the bob phase in StateBobMain, a play-once flag
 *     in StateHoverMain. No single name covers all three.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daC_Jugem_c_AssetId {
    ASSET_C_JUGEM_BMD = 0x02d7,
    ASSET_C_JUGEM_WAIT_BCA = 0x02d8,
    ASSET_C_JUGEM_WAIT_C_BTP = 0x02d9
};

#include "daC_Jugem_c.h"
#include "common.h"
#include "math/CalcLib.h"
#include "dActor_c.h"
#include "daPly_c.h"
#include "dCamera_c.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "SaveData.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "math/Approach.h"
#include "math/MtxStack.h"

/* File-scope objects at the end of this file construct the three resource
 * handles (model file 0x2d7, animation file 0x2d8, texture-sequence file
 * 0x2d9), fill the eleven state pairs and build the two carry-offset
 * vectors. mwcc emits __sinit_d_a_c_jugem.cpp from those definitions. The
 * wrapper names are local; the handle constructors and destructors are the
 * ROM resource-family functions, aliased in the manifest. JugemVec3's plain
 * overload builds the first vector with ordinary stores; the (int, int,
 * long) overload builds the second through a volatile lvalue, exactly as
 * retail. Both share the one declared-not-defined destructor, aliased to
 * the ROM's canonical ~Vector3. Plain `Vector3(x, y, z)` globals were
 * measured (#821): the first reproduces, but the second hoists its zero x
 * ahead of the address load and __sinit stops matching, so JugemVec3 stays
 * until the second vector's real shape is known. Also measured and ruled out
 * for the second vector: copy-initialisation from a Vector3 temporary,
 * const/static, every literal spelling of the arguments, by-reference, short
 * and long long ctor parameters, a member Set() from the ctor body, and
 * scheduling/peephole/optimisation-level pragmas at end of file. */
struct JugemModelFile : SharedFilePtr {
    JugemModelFile(u32 fileID);
    ~JugemModelFile();
};

struct JugemAnimationFilePtr : SharedFilePtr {
    JugemAnimationFilePtr(u32 fileID);
    ~JugemAnimationFilePtr();
};

struct JugemTexSequenceFilePtr : SharedFilePtr {
    JugemTexSequenceFilePtr(u32 fileID);
    ~JugemTexSequenceFilePtr();
};

struct JugemVec3 {
    int x, y, z;
    JugemVec3(int a, int b, int c)
    {
        x = a;
        y = b;
        z = c;
    }
    JugemVec3(int a, int b, long c)
    {
        /* Kept (#821 r79): a member initialiser list, the stores in reverse order and a plain
           int pointer are 6, 8 and 6 words off in the __sinit. */
        volatile int *p = &x;
        p[0] = a;
        p[1] = b;
        p[2] = c;
    }
    ~JugemVec3();
};

extern "C" {

/* Helpers not declared by the included headers. */
int AngleDiff(int a, int b);

/* other overlays / arm9 */
void func_ov002_020c3e8c(void *player);
void func_ov002_020c3f18(daPly_c *player);
void func_ov002_020c3f2c(void *p);

/* globals */
extern dCamera_c *gCamera;                  /* the camera object */
extern u8             data_0209d66c;
extern u8             data_0209d6bc;
extern u8 gShowTouchScreenArrows;
extern s8 gCurrentSublevel;

extern JugemModelFile  data_ov085_0213074c;  /* model file */
extern JugemAnimationFilePtr  data_ov085_02130744;  /* animation file */
extern JugemTexSequenceFilePtr  data_ov085_0213073c;  /* texture sequence file */
extern JugemVec3        data_ov085_02130840;  /* home position */
extern JugemVec3        data_ov085_0213084c;  /* approach target */

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

/* The ROM PMF constants the eleven state pairs copy from (unlicensed .data). */
extern daC_Jugem_c::StateFn data_ov085_0213025c;
extern daC_Jugem_c::StateFn data_ov085_021302d4;
extern daC_Jugem_c::StateFn data_ov085_021302bc;
extern daC_Jugem_c::StateFn data_ov085_0213026c;
extern daC_Jugem_c::StateFn data_ov085_02130254;
extern daC_Jugem_c::StateFn data_ov085_021302dc;
extern daC_Jugem_c::StateFn data_ov085_021302a4;
extern daC_Jugem_c::StateFn data_ov085_02130294;
extern daC_Jugem_c::StateFn data_ov085_02130264;
extern daC_Jugem_c::StateFn data_ov085_021302f4;
extern daC_Jugem_c::StateFn data_ov085_021302cc;
extern daC_Jugem_c::StateFn data_ov085_021302fc;
extern daC_Jugem_c::StateFn data_ov085_021302ec;
extern daC_Jugem_c::StateFn data_ov085_0213028c;
extern daC_Jugem_c::StateFn data_ov085_021302e4;
extern daC_Jugem_c::StateFn data_ov085_021302b4;
extern daC_Jugem_c::StateFn data_ov085_021302c4;
extern daC_Jugem_c::StateFn data_ov085_0213027c;
extern daC_Jugem_c::StateFn data_ov085_021302ac;
extern daC_Jugem_c::StateFn data_ov085_02130284;
extern daC_Jugem_c::StateFn data_ov085_0213029c;
extern daC_Jugem_c::StateFn data_ov085_02130274;
}
#include "Message.h"

#pragma defer_codegen off

/* Empty body: one vptr store and the member destructors in reverse
   declaration order are the whole work, and D0's deallocation is the
   inline operator delete reached through dEnemyBase_c. */
daC_Jugem_c::~daC_Jugem_c()
{
}

/* The intro camera sweep: pull the look-at and eye down toward the player
   while the fanfare plays; on the last frame hand the camera back and go
   wait in StateHidden. */
int daC_Jugem_c::StateIntroMain() {
    dCamera_c *cam = gCamera;
    Vector3 look, pos;
    cam->SetFlag_3();
    look.x = -0x4b0000;
    look.y = 0x19f000;
    look.z = 0x1a90000;
    pos.x = -0x4b0000;
    pos.y = 0x250000;
    pos.z = 0x1d4c000;
    ApproachLinear(mCamLook, look, 0x70000);
    cam->SetLookAt(mCamLook);
    cam->SetPos(mCamPos);
    ::Vec3_Dist(&mCamLook, &look);
    mTimer++;
    if (mTimer > 100) {
        if (Sound::PlaySub(0x4b, 0x7f, 0, 0x7222, false) != 0) {
            cam->mFlags &= ~8;
            mHorzSpeed = 0;
            mTimer = 0;
            mAuxCounter = 0;
            mSpeed.x = 0;
            mSpeed.y = 0;
            mSpeed.z = 0;
            mAngle.x = 0;
            daPly_c *player = ClosestPlayer();
            if (player != 0) {
                (player)->func_ov002_020c3e8c();
                gSaveBlock.file.flags2 |= FILE_FLAGS2_INTRO_DONE;
            }
            SetState(data_ov085_021307e0);
        }
    }
    return 1;
}

int daC_Jugem_c::StateIntroInit()
{
  mHidden = 1;
  mTimer = 0;
  return 1;
}

/* Fly toward the fixed vantage point, camera tracking, until close enough
   to start the intro sweep.
   Kept (#821 r77): the approach target data_ov085_0213084c is the coined
   JugemVec3 (see the note above struct JugemVec3: a plain Vector3 global
   moves its zero x store and __sinit stops matching), so the four uses below
   reach the game's Vector3 helpers through casts. Re-measured on this base:
   the plain `Vector3 data_ov085_0213084c(0, 0x4d8000, -0x104000)` still
   hoists `mov r2, #0` above the address load (__sinit 0x370 -> 0x374). */
int daC_Jugem_c::StateApproachMain()
{
    Vector3 v[3];
    dCamera_c *cam;
    int spd;
    int len;

    cam = gCamera;
    cam->SetFlag_3();
    mSfxHandle = Sound::PlayLong(mSfxHandle, 3, 0x182, mCamSpacePos, 0);
    ApproachLinear(mAngle.y, ::Vec3_HorzAngle(&mPos, (const Vector3 *)&data_ov085_0213084c), 0x200);
    ApproachLinear(mAngle.x, Vec3_VertAngle(&mPos, (const Vector3 *)&data_ov085_0213084c), 0x200);
    ApproachLinear(mHorzSpeed, 0x28000, 0x2000);
    ApproachLinear(mPrevAngle.y, ::Vec3_HorzAngle(&mPos, (const Vector3 *)&data_ov085_0213084c),
                   (s16)mAuxCounter);
    mAngle.y = mPrevAngle.y;
    mAuxCounter += 5;
    if (mAuxCounter > 0x800)
        mAuxCounter = 0x800;
    mTimer += 1;
    if (mTimer < 25)
        return 1;
    v[1].x = -0x50c000;
    v[1].y = 0x115000;
    v[1].z = 0x1d15000;
    spd = mHorzSpeed >> 1;
    ApproachLinear(mCamLook, mPos, spd);
    ApproachLinear(mCamPos, v[1], spd);
    cam->SetLookAt(mCamLook);
    cam->SetPos(mCamPos);
    Vec3_Sub(&v[2], &mPos, (const Vector3 *)&data_ov085_0213084c);
    len = VEC_Mag(&v[2]);
    if (len == 0 || len < 0x7d0000)
        SetState(data_ov085_021307a0);
    return 1;
}

int daC_Jugem_c::StateApproachInit()
{
    Vector3 look, pos;
    dCamera_c *cam;
    mHorzSpeed = 0;
    mTimer = 0;
    mAuxCounter = 0;
    unk_2e0 = 0;
    cam = gCamera;
    cam->SetFlag_3();
    mCamLook.x = -0x399000;
    mCamLook.y = 0x6ea000;
    mCamLook.z = 0x1212000;
    mCamPos.x = -0x49b000;
    mCamPos.y = 0x1d5000;
    mCamPos.z = 0x17fc000;
    cam->SetLookAt(mCamLook);
    cam->SetPos(mCamPos);
    mAuxCounter = 0xa0;
    mPos.x = data_ov085_02130840.x;
    mPos.y = data_ov085_02130840.y;
    mPos.z = data_ov085_02130840.z;
    return 1;
}

/* Bob on a sine wave and pick up speed; at the end of the timer switch to
   the long approach. */
int daC_Jugem_c::StateBobMain()
{
    daPly_c *pl = ClosestPlayer();
    if (pl == 0) return 1;

    mTimer += 1;
    mAuxCounter += 0x500;

    ApproachLinear(mPos.y, FX_MUL(FX_SinIdx((u16)(s16)mAuxCounter >> 4), 0x1a000) + mTarget.y,
                   0x10000000);

    if (mTimer == 50) {
        pl->func_ov002_020c3ea0();
    }
    mAngle.y = 0x6000;
    mPrevAngle.y = mAngle.y;
    {
        int a1 = 0x4000;
        if (mTimer >= 75)
            a1 = 0x8000;
        ApproachLinear(mHorzSpeed, a1, 0x1000);
    }

    mSfxHandle = Sound::PlayLong(mSfxHandle, 3, 0x182, mCamSpacePos, 0);

    if (mTimer > 120) {
        mAngle.y = 0x4000;
        mPrevAngle.y = mAngle.y;
        SetState(data_ov085_02130820);
    }
    return 1;
}

int daC_Jugem_c::StateBobInit()
{
    Vector3 look, pos;
    dCamera_c *cam;
    mTimer = 0;
    mAuxCounter = 0;
    mPos.x = -0x5a0000;
    mPos.y = 0x2c0000;
    mPos.z = 0x1c6f000;
    mTarget.x = mPos.x;
    mTarget.y = mPos.y;
    mTarget.z = mPos.z;
    cam = gCamera;
    cam->SetFlag_3();
    look.x = -0x304000;
    look.y = 0x3c1000;
    look.z = 0x1c77000;
    pos.x = -0x540000;
    pos.y = 0xe1000;
    pos.z = 0x19e4000;
    cam->SetLookAt(look);
    cam->SetPos(pos);
    mHorzSpeed = 0;
    return 1;
}

/* Walk the player into the talking spot on a fixed beat: message steps at
   0x14 and 0x28..0xee, player angles snapped at 0xf0. */
int daC_Jugem_c::StateGuideMain()
{
    daPly_c *p = ClosestPlayer();
    if (p == 0)
        return 1;
    mTimer += 1;
    switch (mTimer) {
    case 1:
        Message::func_0201f32c(2);
        break;
    case 20:
        (p)->func_ov002_020c3f2c();
        break;
    case 40:
        data_0209d66c = 1;
        break;
    case 140:
        Message::func_0201f32c(3);
        break;
    case 238:
        data_0209d66c = 1;
        break;
    case 240: {
        s16 a = mSavedAngleY;
        s16 b;
        p->mAngle.x = 0;
        p->mAngle.y = a;
        p->mAngle.z = 0;
        b = mSavedAngleY;
        p->mPrevAngle.x = 0;
        p->mPrevAngle.y = b;
        p->mPrevAngle.z = 0;
        SetState(data_ov085_021307f0);
    } break;
    }
    return 1;
}

int daC_Jugem_c::StateGuideInit()
{
    mTalkPlayer = 0;
    mTimer = 0;
    return 1;
}

/* Turn the player around to face Lakitu while Lakitu pitches up toward
   him, then move on to StateGuide. */
int daC_Jugem_c::StateTurnMain()
{
    daPly_c *p = ClosestPlayer();
    if (!p) return 1;
    {
        short v = mAngle.y;
        short w = v + 0x8000;
        p->mAngle.x = 0;
        p->mAngle.y = w;
        p->mAngle.z = 0;
        mTimer += 1;
    }
    if (mTimer == 90)
        func_ov002_020c3f18(p);
    {
        int s = mTimer;
        if (s > 0x57 && s < 0x5b)
            ApproachLinear(mAngle.x, 0x2000, 0x400);
        else
            ApproachLinear(mAngle.x, 0x1000, 0x400);
    }
    if (mTimer > 120)
        SetState(data_ov085_021307c0);
    return 1;
}

int daC_Jugem_c::StateTurnInit()
{
    Vector3 look, pos;
    dCamera_c *cam;
    mTimer = 0;
    mTalkPlayer = 0;
    mAngle.z = 0;
    cam = gCamera;
    cam->SetFlag_3();
    look.x = 0xffadd000;
    look.y = 0x17e000;
    look.z = 0x1a29000;
    pos.x = 0xffa54000;
    pos.y = 0x1f4000;
    pos.z = 0x1ccf000;
    cam->SetLookAt(look);
    cam->SetPos(pos);
    return 1;
}

/* Face the player and pitch up, stepping through the greeting beats; the
   last beat hands off to StateTurn. */
int daC_Jugem_c::StateArriveMain()
{
    daPly_c *p = ClosestPlayer();
    if (p == 0)
        return 1;
    {
        short t = mAngle.y + 0x8000;
        p->mAngle.x = 0;
        p->mAngle.y = t;
        p->mAngle.z = 0;
    }
    mAngle.y = HorzAngleToCPlayer();
    mHidden = 0;
    mAngle.x = 0x1000;
    mAngle.z = 0x800;
    mTimer += 1;
    Sound::PlaySub(0x4b, 0x14, 0x7f, 0x15666, false);
    switch (mTimer) {
    case 1:
        p->mMuteSleepVoice = 1;
        break;
    case 120:
        Message::func_0201f32c(1);
        break;
    case 130:
        p->mMuteSleepVoice = 0;
        break;
    case 180:
        data_0209d66c = 1;
        break;
    case 210:
        SetState(data_ov085_021307b0);
        break;
    }
    return 1;
}

int daC_Jugem_c::StateArriveInit()
{
    Vector3 look, pos;
    dCamera_c *cam;
    daPly_c *player;
    mTarget.x = -0x5a0000;
    mTarget.y = 0x1c0000;
    mTarget.z = 0x1a66000;
    player = ClosestPlayer();
    if (player) {
        mSavedAngleY = player->mAngle.y;
        player->func_ov002_020d228c();
    }
    cam = gCamera;
    cam->SetFlag_3();
    look.x = 0xff883000;
    look.y = 0x2ef000;
    look.z = 0x1a36000;
    pos.x = 0xffb18000;
    pos.y = 0x18c000;
    pos.z = 0x1a89000;
    cam->SetLookAt(look);
    cam->SetPos(pos);
    mStateTimer = 121;
    mPos.x = mTarget.x;
    mPos.y = mTarget.y;
    mPos.z = mTarget.z;
    mTalkPlayer = 0;
    return 1;
}

/* Fly to the target point; when the countdown ends and the talk is over,
   mark for destruction. */
int daC_Jugem_c::StateLeaveMain() {
    ApproachLinear(mPrevAngle.y, ::Vec3_HorzAngle(&mPos, &mTarget), 0x800);
    ApproachLinear(mAngle.y, mPrevAngle.y, 0x800);
    mSfxHandle = Sound::PlayLong(mSfxHandle, 3, 0x182, mCamSpacePos, 0);
    if (AngleDiff(mPrevAngle.y, ::Vec3_HorzAngle(&mPos, &mTarget)) < 0x2000) {
        Vec3_ApproachHorz(&mPos, &mTarget, 0x1e000);
        ApproachLinear(mPos.y, mTarget.y, 0x1e000);
    }
    if (mStateTimer == 0) {
        if (Sound::PlaySub(0x4a, 0x7f, 0, 0x7222, 0) != 0) {
            if (mTalkPlayer->HasFinishedTalking() == 1) {
                MarkForDestruction();
                gShowTouchScreenArrows = 0;
            }
        }
    }
    return 1;
}

int daC_Jugem_c::StateLeaveInit()
{
    short r3 = mAngle.y;
    mPrevAngle.y = r3;
    mStateTimer = 70;
    return 1;
}

/* The message 0x182 exchange, step by step; when the talk is done go
   StateLeave. */
int daC_Jugem_c::StateTalkMain()
{
    ApproachLinear(mAngle.y, HorzAngleToCPlayer(), 0x800);
    switch (mTalkStep) {
    case 0:
        mTalkPlayer->mStateFlags |= 0x400;
        if (mTalkPlayer->ShowMessage(*this, 0x182, &mPos, 1, 0) == 1) {
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
        gShowTouchScreenArrows = 1;
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

int daC_Jugem_c::StateTalkInit()
{
    SaveBlock *save = &gSaveBlock;
    save->file.flags2 |= 0x20000;
    mTalkStep = 0;
    return 1;
}

/* Home in on a point above and ahead of the player; once close enough,
   start the talk. */
int daC_Jugem_c::StateFlyToPlayerMain()
{
    Vector3 in;
    Vector3 out;
    daPly_c *p;

    p = ClosestPlayer();
    if (p == 0) {
        return 1;
    }

    p->mStateTarget = mPos;

    if (mStateTimer == 0) {
        Sound::PlaySub(0x4a, 0x14, 0x7f, 0x15666, 0);
    }

    mSfxHandle = Sound::PlayLong(mSfxHandle, 3, 0x182, mCamSpacePos, 0);

    in.x = 0;
    in.y = 0;
    in.z = 0;
    out.x = 0;
    out.y = 0;
    out.z = 0;
    in.z = 0xc8000;
    Matrix4x3_FromRotationY(&gScratchMtx, 0);
    MTX_MultVec43(&in, &gScratchMtx, &out);
    out.x += p->mPos.x;
    out.y += p->mPos.y + 0x70000;
    out.z += p->mPos.z;

    ApproachLinear(mPos, out, 0x14000);
    ApproachLinear(mPrevAngle.y, ::Vec3_HorzAngle(&mPos, &out), 0x800);
    mAngle.y = mPrevAngle.y;

    if (::Vec3_Dist(&mPos, &out) < 0x14000) {
        mTalkPlayer = p;
        SetState(data_ov085_02130810);
    }

    return 1;
}

int daC_Jugem_c::StateFlyToPlayerInit()
{
    mPos.z = 2457600;
    mTimer = 0;
    mStateTimer = 40;
    return 1;
}

/* Hidden; watch for the player to step inside the trigger volume, then
   take his control and fly to him. */
int daC_Jugem_c::StateHiddenMain()
{
    daPly_c *p = ClosestPlayer();
    if (p != 0) {
        /* Measured (#821 r68): copied as a Vector3 this is memberwise, not the ROM's ldm/stm. */
        VecFx32 v = *(VecFx32 *)&p->mPos;
        if ((gSaveBlock.file.flags2 & 0x10000) != 0 && v.z > -0x28000 &&
            p->SetNoControlState(0x12, -1, 0) != 0) {
            p->mStateTarget = mPos;
            *(VecFx32 *)&mPos = v;
            mPos.x -= 0x3e8000;
            mPos.y = p->mGroundY + 0x3e8000;
            mTarget.x = mPos.x;
            mTarget.y = mPos.y;
            mTarget.z = mPos.z;
            mHidden = 0;
            SetState(data_ov085_02130800);
        }
    }
    return 1;
}

int daC_Jugem_c::StateHiddenInit()
{
    mHidden = 1; return 1;
}

/* The ambient state: hover in place beside the camera, one shadow tracking
   the player, one sound trigger behind it. */
int daC_Jugem_c::StateHoverMain()
{
    Vector3 in, out, plpos;
    dCamera_c *cam;
    Vector3 *src;
    daPly_c *p;

    cam = gCamera;
    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;
    src = &cam->pos;
    plpos.x = src->x;
    plpos.y = src->y;
    plpos.z = src->z;
    if (gCurrentSublevel == 0x2f) {
        mPos.x = 0;
    } else {
        mPos.x = 0x1086000;
        if (mAuxCounter == 0) {
            p = ClosestPlayer();
            if (p != 0 && p->mPos.x > 0x1086000) {
                SpawnSoundObj(0);
                mAuxCounter = 1;
            }
        }
    }
    mPos.y = plpos.y;
    mPos.z = plpos.z;
    in.z = ::Vec3_HorzDist(&plpos, &mPos);
    Matrix4x3_FromRotationY(&gScratchMtx, ::Vec3_HorzAngle(&plpos, &mPos));
    MTX_MultVec43(&in, &gScratchMtx, &out);
    mPos.x += out.x;
    if (gCurrentSublevel == 0x2f) {
        if (mPos.x > 0x79e000) {
            mPos.x = 0x79e000;
        }
    }
    mPos.z += out.z;
    mPrevAngle.y = 0x8000 - cam->mAngleY;
    mPrevAngle.x = -cam->mAngleX;
    return 1;
}

int daC_Jugem_c::StateHoverInit()
{
    return 1;
}

/* Install a state record and run its enter slot, if it has one. */
int daC_Jugem_c::SetState(StateFn *record)
{
    mState = record;
    StateFn *q = mState;
    if (*q == 0) return 1;
    return (this->**q)();
}

/* The single blob shadow, used by the cutscene variant. */
void daC_Jugem_c::UpdateShadow()
{
    Vector3 v;
    Vec3_Asr(&v, &mPos, 3);
    Matrix4x3_FromTranslation(&gScratchMtx, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationZXYExt(&gScratchMtx, mAngle.x, mAngle.y, mAngle.z);
    mAnmModel1.mat4x3 = gScratchMtx;
    Matrix4x3_FromTranslation(&gScratchMtx, mPos.x >> 3, (mPos.y - 0x38000) >> 3, mPos.z >> 3);
    mShadowMat1 = gScratchMtx;
    DropShadowRadHeight(mShadowModel1, mShadowMat1, 0x46000, 0x258000, 0xf);
}

/* The ambient variant's two shadows: one at Lakitu's own position, one
   projected ahead of the player. */
void daC_Jugem_c::UpdateShadowPlayer()
{
    Vector3 p;
    Vector3 z1;
    Vector3 off;
    Vector3 pp;
    Vector3 t;
    daPly_c *pl;
    int p1, p2;
    signed char lvl;
    Vector3 *ps;

    Vec3_Asr(&t, &mPos, 3);
    Matrix4x3_FromTranslation(&gScratchMtx, t.x, t.y, t.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&gScratchMtx, mAngle.x, mAngle.y, mAngle.z);
    mAnmModel1.mat4x3 = gScratchMtx;

    Matrix4x3_FromTranslation(&gScratchMtx, mPos.x >> 3, (mPos.y - 0x38000) >> 3, mPos.z >> 3);
    mShadowMat1 = gScratchMtx;

    DropShadowRadHeight(mShadowModel1, mShadowMat1, 0x46000, 0x258000, 0xf);

    pl = ClosestPlayer();
    if (pl == 0) return;

    lvl = gCurrentSublevel;
    p.x = 0;
    p.y = 0;
    p.z = 0;
    z1.x = 0;
    z1.y = 0;
    z1.z = 0;
    off.x = 0;
    off.y = 0;
    off.z = 0;
    ps = &pl->mPos;
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

    z1.z = ::Vec3_HorzDist(&pp, &p);
    {
        short ang = ::Vec3_HorzAngle(&pp, &p);
        Matrix4x3_FromRotationY(&gScratchMtx, ang);
    }
    MTX_MultVec43(&z1, &gScratchMtx, &off);

    p.x = p.x + off.x;
    p.y = pp.y;
    p.z = p.z + off.z;
    Matrix4x3_FromTranslation(&gScratchMtx, p.x >> 3, p.y >> 3, p.z >> 3);

    pl->func_ov002_020e4374(&p1, &p2);

    mShadowMat2 = gScratchMtx;
    DropShadowRadHeight(mShadowModel2, mShadowMat2, p2, p1, 0xf);
}

int daC_Jugem_c::CleanupResources()
{
    data_ov085_0213074c.Release();
    data_ov085_02130744.Release();
    data_ov085_0213073c.Release();
    return 1;
}

/* Empty on purpose: the override suppresses whatever the base does on
   pending destroy. */
void daC_Jugem_c::OnPendingDestroy()
{
}

int daC_Jugem_c::Render()
{
    if (mHidden == 1)
        return 1;
    mTextureSequence.Update(mAnmModel1.data);
    mAnmModel1.Render(0);
    return 1;
}

int daC_Jugem_c::Behavior()
{
    DecIfAbove0_Short(&mStateTimer);
    if (mState[1])
        (this->*mState[1])();
    UpdatePos(0);
    mAnmModel1.Advance();
    mTextureSequence.Advance();
    if (mState == data_ov085_021307d0) {
        mAngle.x = mPrevAngle.x;
        mAngle.y = mPrevAngle.y;
        mAngle.z = mPrevAngle.z;
    }
    if (mState == data_ov085_021307e0)
        return 1;
    if (mVariant == 0)
        UpdateShadowPlayer();
    else
        UpdateShadow();
    return 1;
}

int daC_Jugem_c::InitResources()
{
    BMD_File *bmd = dExtSimpleModel_c::LoadFile(data_ov085_0213074c);
    mAnmModel1.SetFile(bmd, 1, -1);
    dExtFrameCtrl_c::LoadFile(data_ov085_02130744);
    dExtAnmTexPat_c::LoadFile(data_ov085_0213073c);
    mShadowModel1.InitCylinder();
    mShadowModel2.InitCylinder();
    dExtAnmTexPat_c::Prepare(*(BMD_File *)data_ov085_0213074c.filePtr,
                             *(BTP_File *)data_ov085_0213073c.filePtr);
    mAnmModel1.SetAnim((BCA_File *)data_ov085_02130744.filePtr, 0, 0x1000, 0);
    mTextureSequence.SetFile(*(BTP_File *)data_ov085_0213073c.filePtr, 0, 0x1000, 0);
    mAnmModel1.speed = 0x1000;
    mVariant = param1 & 0xff;
    if (mVariant == 0xff)
        mVariant = 0;
    switch (mVariant) {
    case 0:
        SetState(data_ov085_021307d0);
        break;
    case 1: {
        int v = gSaveBlock.file.flags2;
        if (v & 0x20000)
            return 0;
        if (v & 0x10000)
            gSaveBlock.file.flags2 = v & ~0x10000;
        mHidden = 1;
        if (!(gSaveBlock.file.flags2 & FILE_FLAGS2_INTRO_DONE))
            SetState(data_ov085_02130790);
        else
            SetState(data_ov085_021307e0);
    } break;
    }
    return 1;
}

/* The registry factory behind the C_JUGEM profile: allocates this class's
   0x2e8 and installs its vtable. Reconstructed source-style name
   (historical alias LakituBro_Spawn); the exact original spelling is not
   preserved. */
daC_Jugem_c *daC_Jugem_c_classInit(void) {
    return new daC_Jugem_c();
}

/* Static-init globals (was the handwritten __sinit_ov085_0212fa40 shard).
 * Definition order is the retail initializer's construction order: the
 * model handle (file 0x2d7), the animation handle (file 0x2d8), the
 * texture-sequence handle (file 0x2d9), the eleven state pairs copied from
 * the ROM PMF constants, then the two carry-offset vectors (first plain,
 * the second volatile). */
JugemModelFile data_ov085_0213074c(ASSET_C_JUGEM_BMD);
JugemAnimationFilePtr data_ov085_02130744(ASSET_C_JUGEM_WAIT_BCA);
JugemTexSequenceFilePtr data_ov085_0213073c(ASSET_C_JUGEM_WAIT_C_BTP);
daC_Jugem_c::StateFn data_ov085_021307d0[2] = {data_ov085_0213025c, data_ov085_021302d4};
daC_Jugem_c::StateFn data_ov085_021307e0[2] = {data_ov085_021302bc, data_ov085_0213026c};
daC_Jugem_c::StateFn data_ov085_02130800[2] = {data_ov085_02130254, data_ov085_021302dc};
daC_Jugem_c::StateFn data_ov085_02130810[2] = {data_ov085_021302a4, data_ov085_02130294};
daC_Jugem_c::StateFn data_ov085_02130830[2] = {data_ov085_02130264, data_ov085_021302f4};
daC_Jugem_c::StateFn data_ov085_02130790[2] = {data_ov085_021302cc, data_ov085_021302fc};
daC_Jugem_c::StateFn data_ov085_021307b0[2] = {data_ov085_021302ec, data_ov085_0213028c};
daC_Jugem_c::StateFn data_ov085_021307c0[2] = {data_ov085_021302e4, data_ov085_021302b4};
daC_Jugem_c::StateFn data_ov085_021307f0[2] = {data_ov085_021302c4, data_ov085_0213027c};
daC_Jugem_c::StateFn data_ov085_02130820[2] = {data_ov085_021302ac, data_ov085_02130284};
daC_Jugem_c::StateFn data_ov085_021307a0[2] = {data_ov085_0213029c, data_ov085_02130274};
JugemVec3 data_ov085_02130840(-0x640000, 0x378000, 0x1670000);
JugemVec3 data_ov085_0213084c(0, 0x4d8000, (long)-0x104000);
