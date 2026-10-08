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
 * data_ov002_021097bc, which is zero in the overlay image and filled at run
 * time by __sinit_ov002_021014e4 from six Pair records; relocs.txt maps
 * each record to its function, so every Begin/State slot is proven.
 *
 * comment leftovers:
 *  - DropShadow and InitResources keep their mangled `_ZN` spellings for
 *    DropShadowRadHeight, dCcAc_c::Init and dBgCh_Actr::Init: those take
 *    Fix12<int> by value, and Fix12<int> has no int conversion (the Fix12
 *    wall the rest of the tree banks on too).
 *  - dBgCh_Actr_UpdateContinuous_Veneer is a real ROM symbol callers name
 *    directly, not a wrapper to collapse into UpdateContinuous.
 *  - StateOpening still writes mWobbleAngle through a `short*` pun:
 *    `mWobbleAngle += 0x2000` loads it zero-extended (u16) where the ROM
 *    sign-extends (s16) -- one word different.
 *  - BeginOpening reads the camera facing angle as
 *    `*(short *)((char *)data_0209f318 + 0x17c)`; the Camera header owns the
 *    0x17c offset but data_0209f318 is typed `void *` here.
 *  - Several bodies keep `(long long)` intermediate spellings and the
 *    `(int)((x & flag) != 0)` bool-widening idiom from byte matching.
 */

#pragma defer_codegen off

#include "common.h"
#include "daObjPowerUpItem_c.h"
#include "SharedFilePtr.h"
#include "SaveData.h"
#include "fBase_c.h"
#include "dCc_c.h"
#include "decl_SaveData.h"
#include "decl_common.h"
#include "dBgCh_Gnd.h"
#include "Player.h"

struct Vector3_16f;
struct Callback;
typedef struct { int x, y, z; } V3;

/* One row of the state table: enter (pmf[0]) and per-frame update (pmf[1]). */
typedef void (daObjPowerUpItem_c::*FlowerPMF)();
struct FlowerEntry { FlowerPMF pmf[2]; int extra; };
extern FlowerEntry data_ov002_021097bc[];

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

/* A Player's param1, as the Init* calls it selects in CheckPickup
 * imply (InitVanishLuigi, InitMetalWario, InitFireYoshi). */
enum {
    CHARACTER_MARIO = 0,
    CHARACTER_LUIGI = 1,
    CHARACTER_WARIO = 2,
    CHARACTER_YOSHI = 3
};

int ApproachLinear(int&, int, int);
namespace cstd { int fdiv(int, int); }

extern "C" {
extern u8 DecIfAbove0_Byte(u8* p);
extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 id, u32 a, int x, int y, int z, const struct Vector3_16f* rot, struct Callback* cb);
extern signed short data_02082214[];
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int, int, int, int);
extern void func_02012694(unsigned int id, const Vector3 *v);
extern void *data_0209f318;
extern void func_0203568c(int *p, int v);
extern void func_02035684(int *p, int v);
extern void dBgCh_Actr_UpdateContinuous_Veneer(void* p);
extern int func_0200fccc(char* s, int r1);
extern int _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
    char* self, dExtShadowModel_c* sm, struct Matrix4x3* m, int fix, int t, u32 f);
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void *gPFlowerCloseModelFile[];
extern void *gPFlowerOpenModelFile[];
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    void *self, void *act, Fix12i a, Fix12i b, unsigned int c2, unsigned int d);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void *self, void *act, Fix12i a, Fix12i b, void *d, void *e);
}

// @symbol _ZN18daObjPowerUpItem_cD1Ev
// @symbol _ZN18daObjPowerUpItem_cD0Ev
daObjPowerUpItem_c::~daObjPowerUpItem_c()
{
}

/* Update for a resting flower. Unless its param1 is FLOWER_PARAM_PERSISTENT it
 * counts mLifeTimer down and, once that reaches zero, marks itself for
 * destruction (not while the 0x20000 / 0x40000 yoshi-mouth bits are set).
 * Every frame it keeps the particle effect 0x104 following 0x82000 above the
 * flower. */
// @symbol _ZN18daObjPowerUpItem_c12StateRestingEv
void daObjPowerUpItem_c::StateResting()
{
    int flags;
    V3 pos;

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
        mEffectHandle = (u32)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            mEffectHandle, 0x104,
            ((int*)&pos)[0], ((int*)&pos)[1], ((int*)&pos)[2],
            0, 0);
    }
}

/* Forgets the particle effect handle. */
// @symbol _ZN18daObjPowerUpItem_c12BeginRestingEv
void daObjPowerUpItem_c::BeginResting()
{
    mEffectHandle = 0;
}

/* Pop animation: while mWobbleTimer counts down, mScaleX and mScaleY follow the
 * table pair selected by mWobbleAngle (scaled by the fraction of the timer
 * left) and the angle advances 0x2000 a frame. When the timer is spent,
 * mScaleY eases back to 0xfa0 and, once there, the flower enters
 * FLOWER_STATE_RESTING. */
// @symbol _ZN18daObjPowerUpItem_c12StateOpeningEv
void daObjPowerUpItem_c::StateOpening()
{
    if (DecIfAbove0_Byte(&mWobbleTimer)) {
        int a = (int)mWobbleTimer << 12;
        int fdivResult = cstd::fdiv(a, 0x1c000);
        unsigned short hw = mWobbleAngle;
        int idx = hw >> 4;
        signed short odd = data_02082214[idx * 2 + 1];
        int t1 = (int)(((long long)odd * fdivResult + 0x800) >> 12);
        int u1 = (int)(((long long)t1 * 0x332 + 0x800) >> 12);
        int v1 = u1 + 0xffa;
        int w1 = (int)(((long long)v1 * 0xfa0 + 0x800) >> 12);
        mScaleY = w1;
        signed short even = data_02082214[idx * 2];
        int t2 = (int)(((long long)even * fdivResult + 0x800) >> 12);
        int u2 = (int)(((long long)t2 * 0x332 + 0x800) >> 12);
        int v2 = u2 + 0xffa;
        int w2 = (int)(((long long)v2 * 0xfa0 + 0x800) >> 12);
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
 * plays sound 0x7d, drops the effect handle, takes mAngleY from the halfword at
 * data_0209f318 + 0x17c and starts mWobbleTimer at 0x1b. */
// @symbol _ZN18daObjPowerUpItem_c12BeginOpeningEv
void daObjPowerUpItem_c::BeginOpening()
{
    Vector3 pos;
    int x = mPosX;
    int y = mPosY + 0x82000;
    int z = mPosZ;
    ((int *)&pos)[0] = x;
    ((int *)&pos)[1] = y;
    ((int *)&pos)[2] = z;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
        0x102, ((int *)&pos)[0], ((int *)&pos)[1], ((int *)&pos)[2]);
    func_02012694(0x7d, (const Vector3 *)&mCamSpacePosX);
    mEffectHandle = 0;
    mAngleY = *(short *)((char *)data_0209f318 + 0x17c);
    mWobbleTimer = 0x1b;
}

/* Falling update: emits particle 0x103, turns the flower about Y by 0x250 plus
 * an amount that grows with its vertical speed (towards the speed's sign), and
 * moves it with the mesh collision. Touching water either dissolves it in a
 * puff (when the ground probe under it is missing or more than 0x64000 away) or
 * stops the water detection. Landing calls func_0200fccc(this, 1) and enters
 * FLOWER_STATE_OPENING. */
// @symbol _ZN18daObjPowerUpItem_c13StateLaunchedEv
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

    mEffectHandle = (u32)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        mEffectHandle, 0x103, mPosX, mPosY, mPosZ, 0, 0);

    a = mVertSpeed;
    mag = (a < 0) ? -a : a;
    y = (int)(((s64)mag * 0x120000 + 0x800) >> 12);
    X = (s16)(y / 4096);
    delta = 0x250;
    if (X > 0)
        delta += X;
    if (a > 0)
        mAngleY += delta;
    else
        mAngleY -= delta;

    func_0203568c((int*)&mWithMeshClsn, 0x3c000);
    func_02035684((int*)&mWithMeshClsn, 0x3c000);
    UpdatePos((dCc_c*)&mdCcAc_c);
    dBgCh_Actr_UpdateContinuous_Veneer((void*)&mWithMeshClsn);

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
    func_0200fccc((char*)this, 1);
    SetState(FLOWER_STATE_OPENING);
}

/* Enter action for the launch: plays sound 0x7c, drops the effect handle and
 * sets gravity (-0x668), terminal velocity (-0xf000) and vertical speed
 * (0xd000). */
// @symbol _ZN18daObjPowerUpItem_c13BeginLaunchedEv
void daObjPowerUpItem_c::BeginLaunched()
{
    func_02012694(0x7c, (const Vector3*)&mCamSpacePosX);
    mEffectHandle = 0;
    mVertAccel = 0xfffff998;
    mTerminalVelocity = -0xf000;
    mVertSpeed = 0xd000;
}

/* Switches to state `i` and runs that state's enter action. */
// @symbol _ZN18daObjPowerUpItem_c8SetStateEi
void daObjPowerUpItem_c::SetState(int i) {
  mState = i;
  int j = mState;
  (this->*data_ov002_021097bc[j].pmf[0])();
}

/* Runs the current state's per-frame update. */
// @symbol _ZN18daObjPowerUpItem_c11UpdateStateEv
void daObjPowerUpItem_c::UpdateState() {
  int j = mState;
  (this->*data_ov002_021097bc[j].pmf[1])();
}

/* Pickup. Looks up the actor whose id sits in the flower's collider
 * (mdCcAc_c.otherOwner) and, if it is a Player that is not collecting a cap,
 * not holding a MONKEY_THIEF and (as Yoshi) not holding a MONKEY_STAR in its
 * mouth, gives it the power-up for its character and destroys the flower:
 * Mario gets wing feathers when this flower's param1 is 1 and a balloon
 * otherwise, Luigi vanishes, Wario turns to metal, Yoshi breathes fire. With
 * the 0x20000 yoshi-mouth bit set on the flower it only resets mLifeTimer to
 * 0x64. */
// @symbol _ZN18daObjPowerUpItem_c11CheckPickupEv
void daObjPowerUpItem_c::CheckPickup() {
    Player* player;
    u32 id = mdCcAc_c.otherOwner;
    if (id == 0) return;

    player = (Player*)dActor_c::FindWithID(id);
    if (player == 0) return;

    {
        int b = (int)(player->actorID == ACTOR_PLAYER);
        if (b == 0) return;
    }

    if (player->IsCollectingCap() != 0) return;

    {
        dActor_c* held = (dActor_c*)player->mHeldObj;
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
        if ((dActor_c*)player->mObjInMouth != 0) {
            dActor_c* inMouth = (dActor_c*)player->mObjInMouth;
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
 * 0x3c000); otherwise it is 0x78000. Returns 1 without doing anything while the
 * 0x40000 yoshi-mouth bit is set. */
// @symbol _ZN18daObjPowerUpItem_c10DropShadowEv
int daObjPowerUpItem_c::DropShadow()
{
    int r3;
    int b = (int)((mFlags & 0x40000) != 0);
    if (b != 0) return b;
    *(struct Matrix4x3*)&mShadowMat = *(struct Matrix4x3*)&mOpenModel.mat4x3;
    mShadowMat.m[10] = mGroundY >> 3;
    r3 = 0x78000;
    if (mState == FLOWER_STATE_LAUNCHED) {
        int d = mPosY - mGroundY;
        if (d <= 0x1000) d = 0x1000;
        r3 = 0x64000 - (int)(((s64)d * 0x180 + 0x800) >> 12);
        if (r3 < 0x3c000) r3 = 0x3c000;
    }
    return _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j((char*)this, &mShadowModel, &mShadowMat, r3, 0x3c000, 0xf);
}

/* Model matrices: rotates mOpenModel's matrix by mAngleY, sets its translation
 * to the position >> 3 and copies it to mCloseModel. */
// @symbol _ZN18daObjPowerUpItem_c14UpdateMatricesEv
void daObjPowerUpItem_c::UpdateMatrices(){
  Matrix4x3_FromRotationY(&mOpenModel.mat4x3, mAngleY);
  mOpenModel.mat4x3.m[9] = mPosX >> 3;
  mOpenModel.mat4x3.m[10] = mPosY >> 3;
  mOpenModel.mat4x3.m[11] = mPosZ >> 3;
  *(struct Matrix4x3*)&mCloseModel.mat4x3 = *(struct Matrix4x3*)&mOpenModel.mat4x3;
}

// @symbol _ZN18daObjPowerUpItem_c16CleanupResourcesEv
s32 daObjPowerUpItem_c::CleanupResources()
{
    ((SharedFilePtr *)gPFlowerCloseModelFile)->Release();
    ((SharedFilePtr *)gPFlowerOpenModelFile)->Release();
    return 1;
}

// @symbol _ZN18daObjPowerUpItem_c6RenderEv
int daObjPowerUpItem_c::Render()
{
  int f = (int)((mFlags & 0x40000) != 0);
  if (f != 0) return 1;
  /* blinks while mLifeTimer is below 0x2d */
  unsigned char st = mLifeTimer;
  if (st < 0x2d && (st & 1)) return 1;
  switch (mState) {
  case FLOWER_STATE_LAUNCHED: mCloseModel.Render((const Vector3 *)&mScaleX); break;
  case FLOWER_STATE_OPENING: mOpenModel.Render((const Vector3 *)&mScaleX); break;
  case FLOWER_STATE_RESTING: mOpenModel.Render((const Vector3 *)&mScaleX); break;
  }
  return 1;
}

// @symbol _ZN18daObjPowerUpItem_c8BehaviorEv
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

// @symbol _ZN18daObjPowerUpItem_c13InitResourcesEv
int daObjPowerUpItem_c::InitResources()
{
    struct Vector3 pos;

    Model::LoadFile(*(SharedFilePtr *)gPFlowerCloseModelFile);
    Model::LoadFile(*(SharedFilePtr *)gPFlowerOpenModelFile);
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

    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char *)this) + 0x1cc, ((char *)this), 0x32000, 0x64000, 0x800002, 0x8000);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char *)this) + 0x200, ((char *)this), 0x3c000, 0x3c000, 0, 0);
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
        if (ClosestPlayer()->param1 == CHARACTER_LUIGI && _ZN8SaveData16HasPlayerLostCapEv() == 0) {
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

// @symbol _ZN18daObjPowerUpItem_c13OnYoshiTryEatEv
s32 daObjPowerUpItem_c::OnYoshiTryEat()
{
    return 5;
}

/* Reconstructed source-style name: SM64DS proves daObjPowerUpItem_c through
 * RTTI, allocation size, vtable identity, and the POWER_UP_ITEM registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical alias: PowerFlower_Spawn. */
// @symbol daObjPowerUpItem_c_classInit
extern "C" daObjPowerUpItem_c *daObjPowerUpItem_c_classInit()
{
    return new daObjPowerUpItem_c();
}
