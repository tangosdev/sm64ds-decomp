//cpp
#include "daKpFr_c.h"
#include "dBgCh_Gnd.h"
#include "Particle__System.h"
#include "Player.h"
#include "SurfaceInfo.h"

/* Actor/process profile descriptor at ov070:0x02123424. Field roles are
 * recovered from fBase_c/dActor_c consumers; exact original member spellings
 * are not preserved. */
struct daKpFrSpawnInfo {
    daKpFr_c *(*classInit)();
    s16 profileIDAndExecuteOrder;
    s16 drawOrder;
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};

typedef char daKpFrSpawnInfo_size_must_be_0x1c[
    sizeof(daKpFrSpawnInfo) == 0x1c ? 1 : -1];

/* Manually curated translation unit -- ov070/daKpFr_c (21 function(s)).
 * tubuild create refused this TU (legacy bodies wrapped in extern "C" { }),
 * so it began as a reverse-ROM-order concatenation. It now uses the real
 * class, typed members, Player/collision headers, a typed retail factory seam,
 * and compiler-owned inline lifecycle. mwcc emits one .text section per
 * ordinary definition in reverse source order; the destructor variant group
 * is emitted first as retail D1 then D0, with no D2.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x02121b48  src/_ZN8daKpFr_cD1Ev.cpp
 *   [1] 0x02121b88  src/_ZN8daKpFr_cD0Ev.cpp
 *   [2] 0x02121bdc  src/_ZN8daKpFr_c13OnYoshiTryEatEv.cpp
 *   [3] 0x02121be4  src/func_ov070_02121be4.cpp
 *   [4] 0x02121c8c  src/func_ov070_02121c8c.c
 *   [5] 0x02121cbc  src/func_ov070_02121cbc.c
 *   [6] 0x02121d50  src/func_ov070_02121d50.cpp
 *   [7] 0x02121e14  src/func_ov070_02121e14.cpp
 *   [8] 0x02121eb0  src/func_ov070_02121eb0.c
 *   [9] 0x02121ef8  src/func_ov070_02121ef8.c
 *   [10] 0x02121f18  src/func_ov070_02121f18.cpp
 *   [11] 0x02121fb0  src/func_ov070_02121fb0.c
 *   [12] 0x02121fd0  src/func_ov070_02121fd0.cpp
 *   [13] 0x0212200c  src/func_ov070_0212200c.cpp
 *   [14] 0x02122044  src/func_ov070_02122044.c
 *   [15] 0x02122060  src/_ZN8daKpFr_c16CleanupResourcesEv.cpp
 *   [16] 0x02122068  src/_ZN8daKpFr_c16OnPendingDestroyEv.cpp
 *   [17] 0x0212206c  src/_ZN8daKpFr_c6RenderEv.cpp
 *   [18] 0x02122104  src/_ZN8daKpFr_c8BehaviorEv.cpp
 *   [19] 0x02122124  src/_ZN8daKpFr_c13InitResourcesEv.cpp
 *   [20] 0x021221fc  src/daKpFr_c_Spawn.c
 */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- class initializer, 0x021221fc, size 0x48 */
/* -------------------------------------------------------------------------- */
/* Natural `new daKpFr_c` targets `_Znwm`, not the retail actor allocator.
 * Keep this one typed C-ABI construction seam at the actor-table boundary. */
extern "C" {
extern void *_ZN7fBase_cnwEj(u32 size);
extern void _ZN8dActor_cC2Ev(dActor_c *actor);
extern void _ZN11ShadowModelC1Ev(ShadowModel *shadow);
extern void _ZN7dCcAc_cC1Ev(dCcAc_c *clsn);
extern void _ZN10dBgCh_ActrC1Ev(dBgCh_Actr *clsn);
extern int _ZTV8daKpFr_c[];

// @symbol daKpFr_c_classInit
daKpFr_c *daKpFr_c_classInit(void)
{
    daKpFr_c *actor = (daKpFr_c *)_ZN7fBase_cnwEj(sizeof(daKpFr_c));
    if (actor) {
        _ZN8dActor_cC2Ev(actor);
        *(int *)actor = (int)&_ZTV8daKpFr_c[2];
        _ZN11ShadowModelC1Ev(&actor->mShadowModel);
        _ZN7dCcAc_cC1Ev(&actor->mdCcAc_c);
        _ZN10dBgCh_ActrC1Ev(&actor->mWithMeshClsn);
    }
    return actor;
}
}

/* Reconstructed source-style names: SM64DS proves the daKpFr_c RTTI identity,
 * KERONPA_FIRE registry ID, descriptor/factory relationship, and object shape;
 * later EAD lineage supplies the spelling prior. Exact original SM64DS
 * spellings are not preserved. Historical aliases: daKpFr_c_Spawn and
 * daKpFr_c_SpawnInfo. */
extern "C" daKpFrSpawnInfo g_profile_KERONPA_FIRE = {
    daKpFr_c_classInit,
    0x010f,
    0x0082,
    0x00000006,
    0x0001e000,
    0x0003c000,
    0x01000000,
    0x01000000
};

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- _ZN8daKpFr_c13InitResourcesEv, 0x02122124, size 0xd8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKpFr_c13InitResourcesEv
#include "decl_common.h"
/* Array-only wrapper preserves retail's ldm/stm matrix copy in C++ mode. */
struct M48 { int w[12]; };
extern "C" {
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(dCcAc_c*, dActor_c*, Fix12i, Fix12i, unsigned int, unsigned int);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(dBgCh_Actr*, dActor_c*, Fix12i, Fix12i, void*, int);
extern int IDENTITY_MATRIX4X3[];
}

int daKpFr_c::InitResources()
{
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x37000, 0x78000, 0x200002, 0x8000);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x32000, 0x32000, 0, 0);
    mVertAccel = -0x400;
    mTerminalVelocity = -0x5000;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    func_ov070_02122044(((char*)this), 0);
    *(struct M48*)&mMatrix = *(struct M48*)IDENTITY_MATRIX4X3;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- _ZN8daKpFr_c8BehaviorEv, 0x02122104, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKpFr_c8BehaviorEv
int daKpFr_c::Behavior()
{
    func_ov070_02121fd0(((char*)this));
    func_ov070_02121e14(((char*)this));
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- _ZN8daKpFr_c6RenderEv, 0x0212206c, size 0x98 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKpFr_c6RenderEv
/* Particle__System.h owns Vector3_16f but does not yet declare this member;
 * retain its typed, exact-spelling ABI import rather than widening that
 * shared header in a class-source lane. */
extern "C" {
extern u32 _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
    u32 handle, u32 effectID, Fix12i x, Fix12i y, Fix12i z,
    const Vector3_16f *rotation);
}

int daKpFr_c::Render()
{
  int b = (mFlags & 0x40000) != 0;
  if (b) return 1;
  mParticle1 = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
      mParticle1, 0x7f, mPosX, mPosY + 0x4b000, mPosZ, 0);
  mParticle2 = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
      mParticle2, 0x80, mPosX, mPosY + 0x4b000, mPosZ, 0);
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- _ZN8daKpFr_c16OnPendingDestroyEv, 0x02122068, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKpFr_c16OnPendingDestroyEv

void daKpFr_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- _ZN8daKpFr_c16CleanupResourcesEv, 0x02122060, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKpFr_c16CleanupResourcesEv

int daKpFr_c::CleanupResources()
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov070_02122044, 0x02122044, size 0x1c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* Unresolved func_ placeholder; retain its current C ABI spelling. */
extern "C" daKpFrState data_ov070_021236ec[];
extern void func_ov070_0212200c(void *self);

void func_ov070_02122044(void *vself, int idx)
{
    daKpFr_c *self = (daKpFr_c *)vself;
    self->mStateMethods = &data_ov070_021236ec[idx];
    func_ov070_0212200c(self);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov070_0212200c, 0x0212200c, size 0x38 */
/* -------------------------------------------------------------------------- */
extern "C" void func_ov070_0212200c(void *raw) {
    daKpFr_c *self = (daKpFr_c *)raw;
    daKpFrStateMethod *method = &self->mStateMethods->init;
    (self->**method)();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov070_02121fd0, 0x02121fd0, size 0x3c */
/* -------------------------------------------------------------------------- */
extern "C" void func_ov070_02121fd0(char *raw) {
    daKpFr_c *self = (daKpFr_c *)raw;
    daKpFrStateMethod *method = &self->mStateMethods->behavior;
    (self->**method)();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov070_02121fb0, 0x02121fb0, size 0x20 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* Unresolved func_ placeholder; retain its current C ABI spelling. */
// @symbol func_ov070_02121fb0
int func_ov070_02121fb0(char *raw)
{
    daKpFr_c *self = (daKpFr_c *)raw;
    self->mHorzSpeed = 40960;
    self->mStateTimer = 105;
    self->mStateResult = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov070_02121f18, 0x02121f18, size 0x98 */
/* -------------------------------------------------------------------------- */
int ApproachLinear(short &value, short target, short step);

extern "C" {
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern void func_ov070_02121c8c(void* c);
extern short Vec3_HorzAngle(void* v0, void* v1);
extern void func_ov070_02121be4(void* c);
extern void func_ov070_02121d50(void* c, void* p);
extern void func_ov070_02121cbc(char* c);
int func_ov070_02121f18(char* raw) {
  daKpFr_c *self = (daKpFr_c*)raw;
  Player* player;
  if (DecIfAbove0_Byte(&self->mStateTimer) == 0)
    func_ov070_02121c8c(self);
  player = self->ClosestNonVanishPlayer();
  if (player) {
    short ang = Vec3_HorzAngle(&self->mPosX, &player->mPosX);
    ApproachLinear(self->mAngleY, ang, 0x180);
    self->mPrevAngleY = self->mAngleY;
  }
  self->UpdatePos(&self->mdCcAc_c);
  func_ov070_02121be4(self);
  func_ov070_02121d50(self, &self->mWithMeshClsn);
  func_ov070_02121cbc((char*)self);
  self->mdCcAc_c.Clear();
  self->mdCcAc_c.Update();
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov070_02121ef8, 0x02121ef8, size 0x20 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* Unresolved func_ placeholder; retain its current C ABI spelling. */
int func_ov070_02121ef8(char *raw)
{
    daKpFr_c *self = (daKpFr_c *)raw;
    self->mdCcAc_c.Clear();
    self->mStateResult = 1;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov070_02121eb0, 0x02121eb0, size 0x48 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* Unresolved func_ placeholder; retain its current C ABI spelling. */
extern void func_ov070_02121c8c(void *t);
int func_ov070_02121eb0(void *c) {
    daKpFr_c *self = (daKpFr_c *)c;
    int r2 = self->mFlags;
    int r1 = (r2 & 0x20000) ? 1 : 0;
    if (r1 == 0) {
        r1 = (r2 & 0x40000) ? 1 : 0;
        if (r1 == 0)
            func_ov070_02121c8c(c);
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov070_02121e14, 0x02121e14, size 0x9c */
/* -------------------------------------------------------------------------- */
extern "C" {
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    dActor_c *actor, ShadowModel *shadow, Matrix4x3 *matrix,
    Fix12i radius, Fix12i depth, u32 opacity);
void func_ov070_02121e14(char *raw) {
  daKpFr_c *self = (daKpFr_c *)raw;
  int f;
  self->mMatrix.t.x = self->mPosX >> 3;
  self->mMatrix.t.y = self->mPosY >> 3;
  self->mMatrix.t.z = self->mPosZ >> 3;
  dBgCh_Gnd rg;
  rg.SetObjAndPos(*(Vector3*)&self->mPosX, self);
  if (rg.DetectClsn() != 0)
    f = (self->mPosY - rg.clsnY) + 0x1e000;
  else
    f = 0x12c000;
  _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
      self, &self->mShadowModel, &self->mMatrix, 0x64000, f, 0xf);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov070_02121d50, 0x02121d50, size 0xc4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_02121d50
/* GetFloorResult is not declared in the shared header yet; keep only that
 * proven typed ABI seam and the exact UpdateContinuous veneer. */
namespace cstd { int fdiv(int a, int b); }

extern "C" void dBgCh_Actr_UpdateContinuous_Veneer(void* c);
extern "C" dBgPi* _ZNK10dBgCh_Actr14GetFloorResultEv(const dBgCh_Actr*);

extern "C" void func_ov070_02121d50(void* vself, void* vclsn) {
    daKpFr_c* self = (daKpFr_c*)vself;
    dBgCh_Actr* clsn = (dBgCh_Actr*)vclsn;
    Fix12i normal[3];
    dBgCh_Actr_UpdateContinuous_Veneer(clsn);
    if (clsn->IsOnGround()) {
        _ZNK10dBgCh_Actr14GetFloorResultEv(clsn)->surface.CopyNormalTo(*(Vector3*)normal);
        if (normal[1] != 0) {
            int a = (int)(((long long)normal[0] * self->unk_0a4 + 0x800) >> 12);
            int b = (int)(((long long)normal[2] * self->unk_0ac + 0x800) >> 12);
            self->mVertSpeed = -(cstd::fdiv(a + b, normal[1]) + 0x8000);
        }
    }
    if (clsn->IsOnWall())
        func_ov070_02121c8c(self);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov070_02121cbc, 0x02121cbc, size 0x94 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* Unresolved func_ placeholder; retain its current C ABI spelling. */
extern void func_ov070_02122044(void* c, int a);
extern void func_ov070_02121c8c(void* c);

void func_ov070_02121cbc(char* raw){
  daKpFr_c* self = (daKpFr_c*)raw;
  dActor_c* found;
  Player* player;
  int b;
  unsigned int id = self->mdCcAc_c.otherOwner;
  if (id == 0) return;
  found = dActor_c::FindWithID(id);
  if (found == 0) return;
  b = (int)(found->actorID == 0xbf);
  if (b == 0) return;
  player = (Player*)found;
  b = (int)((self->mFlags & 0x20000) != 0);
  if (b != 0) {
    func_ov070_02122044(self, 1);
    return;
  }
  if (player->mIsVanish != 0) return;
  player->Burn();
  func_ov070_02121c8c(self);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov070_02121c8c, 0x02121c8c, size 0x30 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* Unresolved func_ placeholder; retain its current C ABI spelling. */
extern void func_02012694(int id, void *pos);
void func_ov070_02121c8c(void *c)
{
    daKpFr_c *self = (daKpFr_c *)c;
    func_02012694(0x166, &self->mCamSpacePosX);
    self->SmallPoofDust();
    self->MarkForDestruction();
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov070_02121be4, 0x02121be4, size 0xa8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_02121be4
extern "C" void func_ov070_02121be4(void *raw)
{
    daKpFr_c *self = (daKpFr_c*)raw;
    Fix12i pos[3];

    if (!self->mWithMeshClsn.IsOnGround()) return;

    dBgCh_Gnd ground;
    {
        Fix12i z = self->mPosZ;
        Fix12i y = self->mPosY + 0x1e000;
        Fix12i x = self->mPosX;
        pos[0] = x;
        pos[1] = y;
        pos[2] = z;
    }
    ground.SetObjAndPos(*(Vector3*)pos, self);
    if (ground.DetectClsn() == 0 ||
        ground.clsnY < self->mPosY - 0x32000) {
        self->mPosX = self->mPrevPosX;
        self->mPosY = self->mPrevPosY;
        self->mPosZ = self->mPrevPosZ;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN8daKpFr_c13OnYoshiTryEatEv, 0x02121bdc, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKpFr_c13OnYoshiTryEatEv

int daKpFr_c::OnYoshiTryEat()
{
    return 5;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN8daKpFr_cD0Ev, 0x02121b88, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKpFr_cD0Ev

/* No separate body: the inline class destructor plus vtable instantiation
 * makes mwcc emit the retail deleting variant after D1. */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN8daKpFr_cD1Ev, 0x02121b48, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKpFr_cD1Ev

/* No separate body: the inline class destructor emits this complete variant
 * first, through the class vtable instantiated in this TU. */
