//cpp
/* daObjPowerUpItem_c -- the power flower: a pickup that gives the player who
 * touches it a character-specific power-up (see CheckPickup).
 *
 * ROM evidence: _ZTS18daObjPowerUpItem_c is the cartridge type name; the tree's
 * earlier coined spelling was PowerFlower (same vtable, ov002 0x02109800).
 * 19 functions, .text 0x020b9148..0x020b9e64.
 *
 * The out-of-line destructor is the key function, so this TU emits _ZTV/_ZTI/
 * _ZTS. Under `#pragma defer_codegen off` it comes out D1 (0x020b9148), D0
 * (0x020b9198), then a D2 the cartridge has no home for; the same pragma lays
 * .text down in source order, so this file is ROM-ascending. The last
 * function is the registry factory daObjPowerUpItem_c_classInit
 * (0x020b9e0c), `new daObjPowerUpItem_c()`.
 *
 * common.h comes before the class header so the flat Matrix4x3 stands. The
 * two matrix copies were matched against that spelling; Matrix4x3.t is not
 * used. The class header still precedes decl_common.h.
 *
 * The flower runs a three-state machine (mState 0..2). The per-state
 * enter/update pairs live in the pointer-to-member table
 * data_ov002_021097bc, defined at the bottom of the file; its PMF fields
 * are zero in the overlay image and filled at run time by this TU's
 * __sinit from six descriptor records.
 *
 * leftovers:
 *  - dBgCh_Actr_UpdateContinuous_Veneer is a real ROM symbol callers name
 *    directly, not a wrapper to collapse into UpdateContinuous.
 *  - StateOpening still writes mWobbleAngle through a `short*` pun:
 *    `mWobbleAngle += 0x2000` loads it zero-extended (u16) where the ROM
 *    sign-extends (s16) -- one word different.
 *  - Several bodies keep `(long long)` intermediate spellings and the
 *    `(int)((x & flag) != 0)` bool-widening idiom from byte matching.
 */

#pragma defer_codegen off

#include "common.h"
#include "daObjPowerUpItem_c.h"
#include "SpawnInfo.h"
#include "SharedFilePtr.h"
#include "SaveData.h"
#include "fBase_c.h"
#include "dCc_c.h"
#include "decl_common.h"
#include "dBgCh_Gnd.h"
#include "daPly_c.h"
#include "dCamera_c.h"
#include "nitro/fx/fx.h"
#include "math/Approach.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "nitro/fx/fx_cp.h"
#include "nitro/fx/fx_trig.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"

/* One row of the state table: enter action (pmf[0]), per-frame update
 * (pmf[1]) and a name literal. */
typedef void (daObjPowerUpItem_c::*FlowerPMF)();
struct FlowerEntry { FlowerPMF pmf[2]; const char *name; };
extern FlowerEntry data_ov002_021097bc[3];

/* Values of mState, by the transitions between the helpers below. */
enum {
    FLOWER_STATE_LAUNCHED = 0, /* falls (StateLaunched) until it lands; Render draws mCloseModel */
    FLOWER_STATE_OPENING = 1,  /* pop animation (StateOpening); mOpenModel */
    FLOWER_STATE_RESTING = 2   /* waits for a player (StateResting); mOpenModel */
};

/* The life timer never runs for a flower with this param1 (see
 * StateResting). InitResources starts such a flower resting only when
 * the closest player is Luigi and SaveData::HasPlayerLostCap is false;
 * otherwise it returns 0. */
#define FLOWER_PARAM_PERSISTENT 0xffff

/* Actor IDs from symbols/actor_debug_names.tsv. */
enum {
    ACTOR_PLAYER = 0xbf,
    ACTOR_MONKEY_THIEF = 0x10b,
    ACTOR_MONKEY_STAR = 0x10c
};

/* A daPly_c's param1, as the Init* calls it selects in CheckPickup
 * imply (InitVanishLuigi, InitMetalWario, InitFireYoshi). */
enum {
    CHARACTER_MARIO = 0,
    CHARACTER_LUIGI = 1,
    CHARACTER_WARIO = 2,
    CHARACTER_YOSHI = 3
};

extern "C" {
extern dCamera_c *gCamera;
extern void *gPFlowerCloseModelFile[];
extern void *gPFlowerOpenModelFile[];
/* Parameter names keep this TU's @NNN. */
typedef void BgchInitAt_(void *self, void *act, fx32 a, fx32 b, void *d, void *e);
}
extern void func_02012694(unsigned int id, const Vector3 *v);

daObjPowerUpItem_c::~daObjPowerUpItem_c()
{
}

/* Update for a resting flower. Unless its param1 is FLOWER_PARAM_PERSISTENT it
 * counts mLifeTimer down and, once that reaches zero, marks itself for
 * destruction (not while the 0x20000 / 0x40000 yoshi-mouth bits are set).
 * Every frame it keeps the particle effect 0x104 following 0x82000 above the
 * flower. */
void daObjPowerUpItem_c::StateResting()
{
    int flags;
    VecFx32 pos;

    do {
        if (param1 == FLOWER_PARAM_PERSISTENT) break;
        if (DecIfAbove0_Byte(&mLifeTimer) != 0) break;
        flags = mFlags;
        if ((int)((flags & 0x40000) != 0) != 0) break;
        if ((int)((flags & 0x20000) != 0) != 0) break;
        MarkForDestruction();
    } while (0);

    {
        int z = mPosZ;
        int x = mPosX;
        int y = mPosY + 0x82000;
        ((int*)&pos)[0] = x;
        ((int*)&pos)[1] = y;
        ((int*)&pos)[2] = z;
        mEffectHandle =
            (u32)Particle::System::New(mEffectHandle, kPtclPowerFlowerResting, ((int *)&pos)[0],
                                       ((int *)&pos)[1], ((int *)&pos)[2], 0, 0);
    }
}

/* Forgets the particle effect handle. */
void daObjPowerUpItem_c::BeginResting()
{
    mEffectHandle = 0;
}

/* Pop animation: while mWobbleTimer counts down, mScaleX and mScaleY follow the
 * table pair selected by mWobbleAngle (scaled by the fraction of the timer
 * left) and the angle advances 0x2000 a frame. When the timer is spent,
 * mScaleY eases back to 0xfa0 and, once there, the flower enters
 * FLOWER_STATE_RESTING. */
void daObjPowerUpItem_c::StateOpening()
{
    if (DecIfAbove0_Byte(&mWobbleTimer)) {
        int a = (int)mWobbleTimer << 12;
        int fdivResult = FX_Div(a, 0x1c000);
        unsigned short hw = mWobbleAngle;
        signed short odd = FX_CosIdx(hw >> 4);
        int t1 = FX_MUL(odd, fdivResult);
        int u1 = FX_MUL(t1, 0x332);
        int v1 = u1 + 0xffa;
        int w1 = FX_MUL(v1, 0xfa0);
        mScaleY = w1;
        signed short even = FX_SinIdx(hw >> 4);
        int t2 = FX_MUL(even, fdivResult);
        int u2 = FX_MUL(t2, 0x332);
        int v2 = u2 + 0xffa;
        int w2 = FX_MUL(v2, 0xfa0);
        mScaleX = w2;
        // materialized base for halfword at offset >= 0x100
        short* p = (short*)(((int)this + 0x3c8));
        *p = *p + 0x2000;
    } else {
        int ret = ApproachLinear(mScaleY, 0xfa0, 0x199);
        if (ret) {
            SetState(FLOWER_STATE_RESTING);
        }
    }
}

/* Enter action that starts the pop: puffs particle 0x102 above the flower,
 * plays sound 0x7d, drops the effect handle, takes mAngleY from the camera's
 * mAngleY and starts mWobbleTimer at 0x1b. */
void daObjPowerUpItem_c::BeginOpening()
{
    Vector3 pos;
    int x = mPosX;
    int y = mPosY + 0x82000;
    int z = mPosZ;
    ((int *)&pos)[0] = x;
    ((int *)&pos)[1] = y;
    ((int *)&pos)[2] = z;
    Particle::System::NewSimple(kPtclPowerFlowerOpen, ((int *)&pos)[0], ((int *)&pos)[1],
                                ((int *)&pos)[2]);
    func_02012694(0x7d, &mCamSpacePos);
    mEffectHandle = 0;
    mAngleY = gCamera->mAngleY;
    mWobbleTimer = 0x1b;
}

/* Falling update: emits particle 0x103, turns the flower about Y by 0x250 plus
 * an amount that grows with its vertical speed (towards the speed's sign), and
 * moves it with the mesh collision. Touching water either dissolves it in a
 * puff (when the ground probe under it is missing or more than 0x64000 away) or
 * stops the water detection. Landing calls func_0200fccc(1) and enters
 * FLOWER_STATE_OPENING. */
void daObjPowerUpItem_c::StateLaunched()
{
    Vector3 pos;
    int a;
    int mag;
    int y;
    s16 delta;
    s16 X;
    int gy;
    int diff;

    mEffectHandle = (u32)Particle::System::New(mEffectHandle, kPtclPowerFlowerLaunched, mPosX,
                                               mPosY, mPosZ, 0, 0);

    a = mVertSpeed;
    mag = (a < 0) ? -a : a;
    y = FX_MUL(mag, 0x120000);
    X = (s16)(y / 4096);
    delta = 0x250;
    if (X > 0)
        delta += X;
    if (a > 0)
        mAngleY += delta;
    else
        mAngleY -= delta;

    mWithMeshClsn.SetRadius(0x3c000);
    mWithMeshClsn.SetHeight(0x3c000);
    UpdatePos((dCc_c*)&mdCcAc_c);
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);

    if (mWithMeshClsn.TouchesWater()) {
        pos.x = mPosX;
        pos.y = mPosY;
        pos.z = mPosZ;
        dBgCh_Gnd rg;
        rg.SetObjAndPos(pos, 0);
        if (rg.DetectClsn()) {
            gy = rg.clsnY;
            pos.y = gy;
            diff = mPosY - gy;
            if (diff < 0)
                diff = -diff;
            if (diff > 0x64000) {
                SmallPoofDust();
                MarkForDestruction();
                return;
            }
            mWithMeshClsn.StopDetectingWater();
        } else {
            SmallPoofDust();
            MarkForDestruction();
            return;
        }
        return;
    }

    if (!mWithMeshClsn.IsOnGround())
        return;
    func_0200fccc(1);
    SetState(FLOWER_STATE_OPENING);
}

/* Enter action for the launch: plays sound 0x7c, drops the effect handle and
 * sets gravity (-0x668), terminal velocity (-0xf000) and vertical speed
 * (0xd000). */
void daObjPowerUpItem_c::BeginLaunched()
{
    func_02012694(0x7c, &mCamSpacePos);
    mEffectHandle = 0;
    mVertAccel = 0xfffff998;
    mTerminalVelocity = -0xf000;
    mVertSpeed = 0xd000;
}

/* Switches to state `i` and runs that state's enter action. */
void daObjPowerUpItem_c::SetState(int i) {
  mState = i;
  int j = mState;
  (this->*data_ov002_021097bc[j].pmf[0])();
}

/* Runs the current state's per-frame update. */
void daObjPowerUpItem_c::UpdateState() {
  int j = mState;
  (this->*data_ov002_021097bc[j].pmf[1])();
}

/* Pickup. Looks up the actor whose id sits in the flower's collider
 * (mdCcAc_c.otherOwner) and, if it is a daPly_c that is not collecting a cap,
 * not holding a MONKEY_THIEF and (as Yoshi) not holding a MONKEY_STAR in its
 * mouth, gives it the power-up for its character and destroys the flower:
 * Mario gets wing feathers when this flower's param1 is 1 and a balloon
 * otherwise, Luigi vanishes, Wario turns to metal, Yoshi breathes fire. With
 * the 0x20000 yoshi-mouth bit set on the flower it only resets mLifeTimer to
 * 0x64. */
void daObjPowerUpItem_c::CheckPickup() {
    daPly_c *player;
    u32 id = mdCcAc_c.otherOwner;
    if (id == 0) return;

    player = (daPly_c *)dActor_c::FindWithID(id);
    if (player == 0) return;

    {
        int b = (int)(player->actorID == ACTOR_PLAYER);
        if (b == 0) return;
    }

    if (player->IsCollectingCap() != 0) return;

    {
        dActor_c *held = player->mHeldObj;
        int t = (int)(held != 0);
        if (t != 0) {
            int b = (int)(held->actorID == ACTOR_MONKEY_THIEF);
            if (b != 0) return;
        }
    }

    {
        u32 flags = mFlags;
        int t = (int)((flags & 0x20000) != 0);
        if (t != 0) {
            mLifeTimer = 0x64;
            return;
        }
    }

    switch (player->param1) {
    case CHARACTER_MARIO:
        if (param1 == 1) {
            player->InitWingFeathers(1);
        } else {
            player->InitBalloonMario();
        }
        MarkForDestruction();
        return;
    case CHARACTER_WARIO:
        player->InitMetalWario();
        MarkForDestruction();
        return;
    case CHARACTER_LUIGI:
        player->InitVanishLuigi();
        MarkForDestruction();
        return;
    case CHARACTER_YOSHI:
        if (player->mObjInMouth != 0) {
            dActor_c *inMouth = player->mObjInMouth;
            if (inMouth->actorID == ACTOR_MONKEY_STAR) return;
        }
        player->InitFireYoshi();
        MarkForDestruction();
        return;
    }
}

/* Shadow: copies mOpenModel's matrix into mShadowMat, puts its Y at the ground
 * height (mGroundY >> 3) and drops the shadow. In state 0 the shadow radius
 * shrinks with the flower's height above the ground (0x64000 down to a floor of
 * 0x3c000); otherwise it is 0x78000. Does nothing while the 0x40000 yoshi-mouth
 * bit is set. */
void daObjPowerUpItem_c::DropShadow() {
    int r3;
    int b = (int)((mFlags & 0x40000) != 0);
    if (b != 0)
        return;
    *(struct Matrix4x3*)&mShadowMat = *(struct Matrix4x3*)&mOpenModel.mat4x3;
    mShadowMat.m[10] = mGroundY >> 3;
    r3 = 0x78000;
    if (mState == FLOWER_STATE_LAUNCHED) {
        int d = mPosY - mGroundY;
        if (d <= 0x1000) d = 0x1000;
        r3 = 0x64000 - FX_MUL(d, 0x180);
        if (r3 < 0x3c000) r3 = 0x3c000;
    }
    DropShadowRadHeight(mShadowModel, mShadowMat, r3, 0x3c000, 0xf);
}

/* dExtSimpleModel_c matrices: rotates mOpenModel's matrix by mAngleY, sets its translation
 * to the position >> 3 and copies it to mCloseModel. */
void daObjPowerUpItem_c::UpdateMatrices(){
  Matrix4x3_FromRotationY(&mOpenModel.mat4x3, mAngleY);
  mOpenModel.mat4x3.m[9] = mPosX >> 3;
  mOpenModel.mat4x3.m[10] = mPosY >> 3;
  mOpenModel.mat4x3.m[11] = mPosZ >> 3;
  *(struct Matrix4x3*)&mCloseModel.mat4x3 = *(struct Matrix4x3*)&mOpenModel.mat4x3;
}

s32 daObjPowerUpItem_c::CleanupResources()
{
    ((SharedFilePtr *)gPFlowerCloseModelFile)->Release();
    ((SharedFilePtr *)gPFlowerOpenModelFile)->Release();
    return 1;
}

int daObjPowerUpItem_c::Render()
{
  int f = (int)((mFlags & 0x40000) != 0);
  if (f != 0) return 1;
  /* blinks while mLifeTimer is below 0x2d */
  unsigned char st = mLifeTimer;
  if (st < 0x2d && (st & 1)) return 1;
  switch (mState) {
  case FLOWER_STATE_LAUNCHED:
      mCloseModel.Render(&mScale);
      break;
  case FLOWER_STATE_OPENING:
      mOpenModel.Render(&mScale);
      break;
  case FLOWER_STATE_RESTING:
      mOpenModel.Render(&mScale);
      break;
  }
  return 1;
}

int daObjPowerUpItem_c::Behavior()
{
    int b = (int)((mFlags & 0x40000) != 0);
    if (b != 0) return 1;
    mScaleX = 0xfa0;
    mScaleY = 0xfa0;
    mScaleZ = 0xfa0;
    UpdateState();
    CheckPickup();
    UpdateMatrices();
    DropShadow();
    ((dCc_c *)&mdCcAc_c)->Clear();
    ((dCc_c *)&mdCcAc_c)->Update();
    if (SaveData::HasPlayerLostCap()) {
        SmallPoofDust();
        MarkForDestruction();
    }
    return 1;
}

int daObjPowerUpItem_c::InitResources()
{
    struct Vector3 pos;

    dExtSimpleModel_c::LoadFile(*(SharedFilePtr *)gPFlowerCloseModelFile);
    dExtSimpleModel_c::LoadFile(*(SharedFilePtr *)gPFlowerOpenModelFile);
    if (mOpenModel.SetFile((BMD_File*)gPFlowerOpenModelFile[1], 1, -1) == 0)
        return 0;
    if (mCloseModel.SetFile((BMD_File*)gPFlowerCloseModelFile[1], 1, -1) == 0)
        return 0;
    if (mShadowModel.InitCylinder() == 0)
        return 0;

    mVertAccel = -0x668;
    mTerminalVelocity = -0xf000;
    UpdateMatrices();

    mScaleX = 0xfa0;
    mScaleY = 0xfa0;
    mScaleZ = 0xfa0;

    mdCcAc_c.Init(this, 0x32000, 0x64000, 0x800002, 0x8000);
    mWithMeshClsn.Init(this, 0x3c000, 0x3c000, 0, 0);
    mWithMeshClsn.StartDetectingWater();

    /* ground probe: a ray starting 0x14000 above the flower */
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x14000;
    dBgCh_Gnd ground;
    ground.SetObjAndPos(pos, 0);
    mGroundY = pos.y;
    if (ground.DetectClsn())
        mGroundY = ground.clsnY;
    mLifeTimer = 0xb4;

    if (param1 == FLOWER_PARAM_PERSISTENT) {
        if (ClosestPlayer()->param1 == CHARACTER_LUIGI && SaveData::HasPlayerLostCap() == 0) {
            SetState(FLOWER_STATE_RESTING);
        } else {
            return 0;
        }
    } else {
        SetState(FLOWER_STATE_LAUNCHED);
    }
    mAngleY -= 0x4000;
    return 1;
}

s32 daObjPowerUpItem_c::OnYoshiTryEat()
{
    return 5;
}

/* Reconstructed source-style name: SM64DS proves daObjPowerUpItem_c through
 * RTTI, allocation size, vtable identity, and the POWER_UP_ITEM registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical alias: PowerFlower_Spawn. */
daObjPowerUpItem_c *daObjPowerUpItem_c_classInit() {
    return new daObjPowerUpItem_c();
}

extern "C" SpawnInfo<daObjPowerUpItem_c> g_profile_POWER_UP_ITEM = {
    daObjPowerUpItem_c_classInit,
    0x0132, /* POWER_UP_ITEM registry id 306 */
    0x0125,
    2,
    0x00060000,
    0x00200000,
    0x01000000,
    0x01000000,
};

/* -------------------------------------------------------------------------- */
/* The state names sit in 8/12-byte slots ahead of the pointer-to-member
 * descriptors. */
static char s_closeName[8] = "CLOSE";
static char s_openName[8] = "OPEN";
static char s_openWaitName[12] = "OPEN_WAIT";

/* The state table SetState/UpdateState dispatch through, by mState: the closed
 * flower falling (LAUNCHED), the pop animation (OPENING) and the open flower
 * waiting for a player (RESTING). The definition lives at the bottom so the
 * descriptor objects mwccarm materializes for the PMF constants are numbered
 * clear of the anonymous objects earlier TUs already pinned (@NNN is a shared
 * namespace). */
FlowerEntry data_ov002_021097bc[3] = {
    {&daObjPowerUpItem_c::BeginLaunched, &daObjPowerUpItem_c::StateLaunched, s_closeName},
    {&daObjPowerUpItem_c::BeginOpening,  &daObjPowerUpItem_c::StateOpening,  s_openName},
    {&daObjPowerUpItem_c::BeginResting,  &daObjPowerUpItem_c::StateResting,  s_openWaitName},
};
