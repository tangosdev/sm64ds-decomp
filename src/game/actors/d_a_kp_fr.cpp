//cpp
/* daKpFr_c -- the chasing flame (KERONPA_FIRE), ov070.
 *
 * It runs at 10.0 (0xa000) toward the closest visible player, turning 0x180
 * a frame, and goes out in a poof (func_ov070_02121c8c) after 105 frames or
 * against a wall. It will not run off a ledge: a step that finds no ground
 * within 50.0 below is undone (func_ov070_02121be4). A Mario it touches is
 * burnt (daPly_c::Burn) unless he is vanished.
 *
 * _ZTS8daKpFr_c 0x0212340c. _ZTI8daKpFr_c 0x02123418.
 * _ZTV8daKpFr_c 0x02123448. The inline destructor in the header is the key
 * function: D1 at 0x02121b48, D0 at 0x02121b88, no D2.
 *
 * DO NOT "TIDY" THESE -- each one is load-bearing:
 *   mwcc emits one .text section per ordinary definition in reverse source
 *   order; the inline destructor group comes out retail D1 then D0, no D2.
 *   Mtx43, the flat type for the IDENTITY_MATRIX4X3 copy: the nested
 *   math/Matrix.h spelling (via dExtShadowModel_c.h) scalarizes it.
 *   fx32[3] locals instead of Vector3, which declares its own
 *   destructor.
 *   FX_MUL's 64-bit product (one smull) in func_ov070_02121d50 is the
 *   matching form.
 *   func_ov070_02121d50 calls dBgCh_Actr_UpdateContinuous_Veneer, as the
 *   ROM does, not UpdateContinuous.
 */

// leftovers:
// - `return new` synthesizes a vague-linkage _ZN9Matrix4x3D1Ev over mMatrix
//   (deadstripped; no ROM symbol).
#include "daKpFr_c.h"
#include "nitro/fx/fx.h"
#include "SpawnInfo.h"
#include "dBgCh_Gnd.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "daPly_c.h"
#include "SurfaceInfo.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "nitro/fx/fx_cp.h"

/* The registry factory behind the KERONPA_FIRE profile.
 * `return new daKpFr_c()` emits 0x48 with the synthesized ctor storing
 * `_ZTV8daKpFr_c + 2`. Historical aliases: daKpFr_c_Spawn, FlameChompFire_Spawn. */
daKpFr_c *daKpFr_c_classInit(void) {
    return new daKpFr_c();
}

/* Reconstructed source-style names: SM64DS proves the daKpFr_c RTTI identity,
 * KERONPA_FIRE registry ID, descriptor/factory relationship, and object shape;
 * later EAD lineage supplies the spelling prior. Exact original SM64DS
 * spellings are not preserved. Historical aliases: daKpFr_c_Spawn and
 * daKpFr_c_SpawnInfo. The descriptor is at ov070:0x02123424. */
SpawnInfo<daKpFr_c> g_profile_KERONPA_FIRE = {
    daKpFr_c_classInit, 0x010f, 0x0082, 0x00000006, 0x0001e000, 0x0003c000, 0x01000000, 0x01000000};

#include "decl_common.h"
#include "dBgCh_Actr.h"
#include "private/mtx43.h"

extern Matrix4x3 IDENTITY_MATRIX4X3;

int daKpFr_c::InitResources()
{
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    mdCcAc_c.Init(this, 0x37000, 0x78000, 0x200002, 0x8000);
    mWithMeshClsn.Init(this, 0x32000, 0x32000, 0, 0);
    mVertAccel = -0x400;
    mTerminalVelocity = -0x5000;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    func_ov070_02122044(0);
    *(Mtx43 *)&mMatrix = *(Mtx43 *)&IDENTITY_MATRIX4X3;
    return 1;
}

int daKpFr_c::Behavior()
{
    func_ov070_02121fd0();
    func_ov070_02121e14();
    return 1;
}

/* Particle__System.h owns Vector3_16f but does not yet declare this member;
 * retain its typed, exact-spelling ABI import rather than widening that
 * shared header in a class-source lane. */
int daKpFr_c::Render()
{
  int hidden = (mFlags & 0x40000) != 0;
  if (hidden) return 1;
  mParticle1 = Particle::System::NewCleanParticle(mParticle1, kPtclKeronpaFireA, mPosX,
                                                  mPosY + 0x4b000, mPosZ, 0);
  mParticle2 = Particle::System::NewCleanParticle(mParticle2, kPtclKeronpaFireB, mPosX,
                                                  mPosY + 0x4b000, mPosZ, 0);
  return 1;
}


void daKpFr_c::OnPendingDestroy()
{
}


int daKpFr_c::CleanupResources()
{
    return 1;
}

extern daKpFrState data_ov070_021236ec[];
/* The ROM PMF constants the state table copies from (unlicensed .data):
 * the init/behavior halves of the two states, in retail copy order. */
extern daKpFrStateMethod data_ov070_021233ec;
extern daKpFrStateMethod data_ov070_021233fc;
extern daKpFrStateMethod data_ov070_021233f4;
extern daKpFrStateMethod data_ov070_02123404;

void daKpFr_c::func_ov070_02122044(int idx)
{
    mStateMethods = &data_ov070_021236ec[idx];
    func_ov070_0212200c();
}

void daKpFr_c::func_ov070_0212200c() {
    daKpFrStateMethod *method = &mStateMethods->init;
    (this->**method)();
}

void daKpFr_c::func_ov070_02121fd0() {
    daKpFrStateMethod *method = &mStateMethods->behavior;
    (this->**method)();
}

int daKpFr_c::func_ov070_02121fb0()
{
    mHorzSpeed = 40960;
    mStateTimer = 105;
    mStateResult = 0;
    return 1;
}


int daKpFr_c::func_ov070_02121f18() {
    daPly_c *player;
    if (DecIfAbove0_Byte(&mStateTimer) == 0)
        func_ov070_02121c8c();
    player = ClosestNonVanishPlayer();
    if (player) {
        short angleToPlayer = Vec3_HorzAngle(&mPos, &player->mPos);
        ApproachLinear(mAngleY, angleToPlayer, 0x180);
        mPrevAngleY = mAngleY;
    }
  UpdatePos(&mdCcAc_c);
  func_ov070_02121be4();
  func_ov070_02121d50(&mWithMeshClsn);
  func_ov070_02121cbc();
  mdCcAc_c.Clear();
  mdCcAc_c.Update();
  return 1;
}

int daKpFr_c::func_ov070_02121ef8()
{
    mdCcAc_c.Clear();
    mStateResult = 1;
    return 1;
}

int daKpFr_c::func_ov070_02121eb0() {
    int flags = mFlags;
    int isSet = (flags & 0x20000) ? 1 : 0;
    if (isSet == 0) {
        isSet = (flags & 0x40000) ? 1 : 0;
        if (isSet == 0)
            func_ov070_02121c8c();
    }
    return 1;
}

/* Fix12-by-value, notes/mwccarm-codegen.md 6az -- header method form homes the
   class args. */
void daKpFr_c::func_ov070_02121e14() {
  int shadowDepth;
  mMatrix.t.x = mPosX >> 3;
  mMatrix.t.y = mPosY >> 3;
  mMatrix.t.z = mPosZ >> 3;
  dBgCh_Gnd ground;
  ground.SetObjAndPos(mPos, this);
  if (ground.DetectClsn() != 0)
    shadowDepth = (mPosY - ground.clsnY) + 0x1e000;
  else
    shadowDepth = 0x12c000;
  DropShadowRadHeight(mShadowModel, mMatrix, 0x64000, shadowDepth, 0xf);
}

/* GetFloorResult is not declared in the shared header yet; keep only that
 * proven typed ABI seam. ROM calls the UpdateContinuous veneer, not the
 * method. FX_MUL's 64-bit product is the smull the ROM uses. */

void daKpFr_c::func_ov070_02121d50(void* vclsn) {
    dBgCh_Actr* clsn = (dBgCh_Actr*)vclsn;
    fx32 normal[3];
    dBgCh_Actr_UpdateContinuous_Veneer(clsn);
    if (clsn->IsOnGround()) {
        clsn->GetFloorResult()->surface.CopyNormalTo(*(Vector3 *)normal);
        if (normal[1] != 0) {
            int a = FX_MUL(normal[0], mSpeedX);
            int b = FX_MUL(normal[2], mSpeedZ);
            mVertSpeed = -(FX_Div(a + b, normal[1]) + 0x8000);
        }
    }
    if (clsn->IsOnWall())
        func_ov070_02121c8c();
}

void func_02012694(unsigned int id, const Vector3 *pos);

void daKpFr_c::func_ov070_02121cbc(){
  dActor_c* found;
  daPly_c *player;
  int b;
  unsigned int id = mdCcAc_c.otherOwner;
  if (id == 0) return;
  found = dActor_c::FindWithID(id);
  if (found == 0) return;
  b = (int)(found->actorID == 0xbf);
  if (b == 0) return;
  player = (daPly_c *)found;
  b = (int)((mFlags & 0x20000) != 0);
  if (b != 0) {
    func_ov070_02122044(1);
    return;
  }
  if (player->mIsVanish != 0) return;
  player->Burn();
  func_ov070_02121c8c();
}

void daKpFr_c::func_ov070_02121c8c()
{
    func_02012694(0x166, (const Vector3 *)(&mCamSpacePosX));
    SmallPoofDust();
    MarkForDestruction();
}

void daKpFr_c::func_ov070_02121be4()
{
    fx32 pos[3];

    if (!mWithMeshClsn.IsOnGround()) return;

    dBgCh_Gnd ground;
    {
        fx32 z = mPosZ;
        fx32 y = mPosY + 0x1e000;
        fx32 x = mPosX;
        pos[0] = x;
        pos[1] = y;
        pos[2] = z;
    }
    ground.SetObjAndPos(*(Vector3*)pos, this);
    if (ground.DetectClsn() == 0 ||
        ground.clsnY < mPosY - 0x32000) {
        mPosX = mPrevPosX;
        mPosY = mPrevPosY;
        mPosZ = mPrevPosZ;
    }
}


int daKpFr_c::OnYoshiTryEat()
{
    return 5;
}


/* No separate body: the inline class destructor plus vtable instantiation
 * makes mwcc emit the retail deleting variant after D1. */


/* No separate body: the inline class destructor emits this complete variant
 * first, through the class vtable instantiated in this TU. */

/* Static-init globals (was the handwritten __sinit_ov070_02123030 shard).
 * The two-entry state table plain-copied from the ROM PMF constants, in
 * retail copy order: state 0's init/behavior halves, then state 1's. */
daKpFrState data_ov070_021236ec[2] = {
    {data_ov070_021233ec, data_ov070_021233fc},
    {data_ov070_021233f4, data_ov070_02123404},
};
