//cpp
/* daKpFr_c -- the chasing flame (KERONPA_FIRE), ov070.
 *
 * It runs at 10.0 (0xa000) toward the closest visible player, turning 0x180
 * a frame, and goes out in a poof (func_ov070_02121c8c) after 105 frames or
 * against a wall. It will not run off a ledge: a step that finds no ground
 * within 50.0 below is undone (func_ov070_02121be4). A Mario it touches is
 * burnt (Player::Burn) unless he is vanished.
 *
 * One TU, 21 functions. It began as the old one-function files
 * concatenated in reverse ROM order: their bodies were wrapped in
 * extern "C" { }, which the merge tool could not take apart.
 *
 * DO NOT "TIDY" THESE -- each one is load-bearing:
 *   mwcc emits one .text section per ordinary definition in reverse source
 *   order; the inline destructor group comes out retail D1 then D0, no D2.
 *   M48, the array wrapper for the IDENTITY_MATRIX4X3 copy: the nested
 *   math/Matrix.h spelling (via ShadowModel.h) scalarizes it.
 *   Fix12i[3] locals instead of Vector3, which declares its own
 *   destructor.
 *   The (long long) smull in func_ov070_02121d50 is the matching form.
 *   func_ov070_02121d50 calls dBgCh_Actr_UpdateContinuous_Veneer, as the
 *   ROM does, not UpdateContinuous.
 *
 * WHY SOME CALLS ARE SPELLED AS MANGLED SYMBOLS (Fix12<int> by value, see
 * notes/mwccarm-codegen.md 6az, unless noted):
 *   dCcAc_c::Init, DropShadowRadHeight and
 *   Particle::System::NewUnkCallback818.
 *   dBgCh_Actr::Init: its header's Fix12i mangles as int.
 *   dBgCh_Actr::GetFloorResult: not in dBgCh_Actr.h yet.
 *
 * Known limits:
 *   data_ov070_021236ec, the state table, is sinit-owned BSS, not this TU's
 *   data claim.
 *   `return new` synthesizes a vague-linkage _ZN9Matrix4x3D1Ev over mMatrix
 *   (deadstripped; no ROM symbol).
 */
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

/* The registry factory behind the KERONPA_FIRE profile.
 * `return new daKpFr_c()` MATCHES (size 0x48); the synthesized ctor stores
 * `_ZTV8daKpFr_c + 2`. Historical aliases: daKpFr_c_Spawn, FlameChompFire_Spawn. */
// @symbol daKpFr_c_classInit
extern "C" daKpFr_c *daKpFr_c_classInit(void)
{
    return new daKpFr_c();
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

// @symbol _ZN8daKpFr_c13InitResourcesEv
#include "decl_common.h"
/* Array-only wrapper preserves retail's ldm/stm matrix copy in C++ mode.
 * Nested math/Matrix.h spelling (via ShadowModel.h) scalarizes otherwise. */
struct M48 { int w[12]; };
extern "C" {
/* Fix12-by-value, notes/mwccarm-codegen.md 6az. dBgCh_Actr::Init header Fix12i
   mangles as int. */
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

// @symbol _ZN8daKpFr_c8BehaviorEv
int daKpFr_c::Behavior()
{
    func_ov070_02121fd0(((char*)this));
    func_ov070_02121e14(((char*)this));
    return 1;
}

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
  int hidden = (mFlags & 0x40000) != 0;
  if (hidden) return 1;
  mParticle1 = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
      mParticle1, 0x7f, mPosX, mPosY + 0x4b000, mPosZ, 0);
  mParticle2 = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
      mParticle2, 0x80, mPosX, mPosY + 0x4b000, mPosZ, 0);
  return 1;
}

// @symbol _ZN8daKpFr_c16OnPendingDestroyEv

void daKpFr_c::OnPendingDestroy()
{
}

// @symbol _ZN8daKpFr_c16CleanupResourcesEv

int daKpFr_c::CleanupResources()
{
    return 1;
}

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

extern "C" void func_ov070_0212200c(void *raw) {
    daKpFr_c *self = (daKpFr_c *)raw;
    daKpFrStateMethod *method = &self->mStateMethods->init;
    (self->**method)();
}

extern "C" void func_ov070_02121fd0(char *raw) {
    daKpFr_c *self = (daKpFr_c *)raw;
    daKpFrStateMethod *method = &self->mStateMethods->behavior;
    (self->**method)();
}

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
    short angleToPlayer = Vec3_HorzAngle(&self->mPosX, &player->mPosX);
    ApproachLinear(self->mAngleY, angleToPlayer, 0x180);
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

extern "C" {  /* Unresolved func_ placeholder; retain its current C ABI spelling. */
int func_ov070_02121ef8(char *raw)
{
    daKpFr_c *self = (daKpFr_c *)raw;
    self->mdCcAc_c.Clear();
    self->mStateResult = 1;
    return 1;
}
}

extern "C" {  /* Unresolved func_ placeholder; retain its current C ABI spelling. */
extern void func_ov070_02121c8c(void *t);
int func_ov070_02121eb0(void *c) {
    daKpFr_c *self = (daKpFr_c *)c;
    int flags = self->mFlags;
    int isSet = (flags & 0x20000) ? 1 : 0;
    if (isSet == 0) {
        isSet = (flags & 0x40000) ? 1 : 0;
        if (isSet == 0)
            func_ov070_02121c8c(c);
    }
    return 1;
}
}

extern "C" {
/* Fix12-by-value, notes/mwccarm-codegen.md 6az -- header method form homes the
   class args. */
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    dActor_c *actor, ShadowModel *shadow, Matrix4x3 *matrix,
    Fix12i radius, Fix12i depth, u32 opacity);
void func_ov070_02121e14(char *raw) {
  daKpFr_c *self = (daKpFr_c *)raw;
  int shadowDepth;
  self->mMatrix.t.x = self->mPosX >> 3;
  self->mMatrix.t.y = self->mPosY >> 3;
  self->mMatrix.t.z = self->mPosZ >> 3;
  dBgCh_Gnd ground;
  ground.SetObjAndPos(*(Vector3*)&self->mPosX, self);
  if (ground.DetectClsn() != 0)
    shadowDepth = (self->mPosY - ground.clsnY) + 0x1e000;
  else
    shadowDepth = 0x12c000;
  _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
      self, &self->mShadowModel, &self->mMatrix, 0x64000, shadowDepth, 0xf);
}
}

// @symbol func_ov070_02121d50
/* GetFloorResult is not declared in the shared header yet; keep only that
 * proven typed ABI seam. ROM calls the UpdateContinuous veneer, not the
 * method. (long long) smull is the MATCH addressing form. */
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

// @symbol _ZN8daKpFr_c13OnYoshiTryEatEv

int daKpFr_c::OnYoshiTryEat()
{
    return 5;
}

// @symbol _ZN8daKpFr_cD0Ev

/* No separate body: the inline class destructor plus vtable instantiation
 * makes mwcc emit the retail deleting variant after D1. */

// @symbol _ZN8daKpFr_cD1Ev

/* No separate body: the inline class destructor emits this complete variant
 * first, through the class vtable instantiated in this TU. */
