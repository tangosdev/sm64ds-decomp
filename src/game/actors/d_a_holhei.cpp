//cpp
/**
 * d_a_holhei.cpp
 * Object - Chuckya (HOLHEI 190)
 *
 * ov062/daHolhei_c, 36 functions, .text 0x02115ee0..0x021174cc.
 * RTTI ov062:0x0211d9ac names daHolhei_c. The tree used the coined
 * English name Chuckya; that spelling is gone from identifiers.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S -- mwccarm 2004/b56 emits
 * one .text section per function in the reverse of source order.
 *
 * The destructor is inline in include/daHolhei_c.h so D1 then D0 match
 * the cartridge and no D2 is emitted. Factory is `return new daHolhei_c()`.
 *
 * deslop leftovers (this TU):
 * - dCcAc_c::Init / dBgCh_Actr::Init stay mangled: Fix12<int> by value, wall 6az.
 *   InitResources is the caller.
 * - ModelAnim::SetAnim stays mangled (same 6az). State handlers in this TU call it.
 * - dActor_c::DropShadowRadHeight stays mangled (same 6az). func_ov062_02116dbc
 *   and func_ov062_02116d28 are the callers.
 * - dActor_c::SpawnCoins stays mangled (same 6az). func_ov062_021163b0 calls it.
 * - dEnemyBase_c::KillByInvincibleChar (ov002:0x020ada40) stays mangled (6az).
 *   func_ov062_02116010 is the caller.
 * - data_ov062_0211ddf0 / dde8 / dde0 / de00 / de08 / ddf8 are this overlay's
 *   BMD/BCA SharedFilePtr handles. InitResources LoadFile/SetFile and
 *   CleanupResources Release them; SetAnim reads [1] as the BCA pointer.
 *   symbols.txt has no recovered names.
 * - data_ov062_0211dea0 / dec0 / de70 / ded0 / dee0 / de90 / df00 / deb0 /
 *   de80 / def0 are the state records Behavior compares by address and
 *   daHolhei_c_ChangeState writes. Overlay .bss owns them.
 * - data_ov062_0211df10 / df14 / df18 are the carry-offset table
 *   func_ov062_02116edc homes toward.
 * - S14: g_profile_HOLHEI stays outside this text-only TU.
 * - helpers stay func_ov062_* offset soup.
 * - held actor +0xc8 / +8, Player::TryGrab, func_ov002_020db5f4 / 020db54c.
 * - data_0209f2f8 is the level id Behavior fences X/Z against.
 * - daHolhei_c_ChangeState is coined; ROM does not preserve that spelling.
 * - volatile Vector3 in func_ov062_021165e8 (frame-slot).
 * - Render calls Model::Render, not ModelAnim::Render: ModelAnim overrides
 *   the slot and an unqualified mModel.Render(0) misses.
 */

#include "common.h"
#include "daHolhei_c.h"
#include "SharedFilePtr.h"

struct C; typedef int (C::*PMF)();
struct C { char pad[0x364]; PMF *pp; };
struct Klass; typedef void (Klass::*KPMF)();
struct M { char pad[8]; KPMF pmf; };

extern "C" {
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void* p);
extern char* _ZNK10dBgCh_Actr14GetFloorResultEv(void* p);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* p, Vector3* v);
extern short func_02010844(void* unused, Vector3* v, short angle);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void* p);
extern void *_ZN8dActor_c10FindWithIDEj(u32 id);
extern void _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(void *c, void *v, void *a, int flag);
extern void func_02012694(int a, void *p);
extern int _ZN8dActor_c18HorzAngleToCPlayerEv(void *c);
extern int _ZN6Player7TryGrabER8dActor_c(void *p, void *a);
extern int daHolhei_c_ChangeState(void *c, void *p);
extern int func_ov002_020db5f4(void *c, void *arg);
extern int data_ov062_0211dea0[];
extern int _ZNK9Animation12WillHitFrameEi(char *anim, int f);
namespace cv {
  extern "C" int func_ov002_020db54c(int p, int a, int b, int s);
}
extern int _ZN9Animation8FinishedEv(char *anim);
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *m, void *f, int a, int b, unsigned int e);
extern int *data_ov062_0211de08[];
extern int data_ov062_0211deb0[];
extern int data_ov062_0211dde8[];
extern int _ZN8dActor_c24KillAndTrackInDeathTableEv(void* c);
extern int _ZN8dActor_c14TriplePoofDustEv(void* c);
extern void func_0200d8c8(void* cam, void* v, int strength);
extern int _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void* a, Vector3* v, unsigned n, int f, short s);
extern int data_02092138;
extern void* data_0209f318;
extern void *data_ov062_0211de00[];
extern void* _ZN8dActor_c13ClosestPlayerEv(void* self);
extern int ApproachAngle(void* angle, int target, int a, int b, int c);
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int c, const void* v, unsigned int e);
extern int Vec3_Dist(const void* a, const void* b);
extern int data_ov062_0211ddf8[];
extern s16 Vec3_HorzAngle(const void* v0, const void* v1);
extern char data_ov062_0211df00[];
extern char data_ov062_0211de70[];
extern char data_ov062_0211de90[];
extern char data_ov062_0211dee0[];
extern void _Z14ApproachLinearRiii(int* x, int target, int step);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *, ShadowModel &sm, Matrix4x3 &mtx, int a, int b, unsigned int c);
extern Matrix4x3 IDENTITY_MATRIX4X3;
extern void Matrix4x3_FromRotationY(struct Matrix4x3 *mF, s16 angY);
extern void MulMat4x3Mat4x3(const struct Matrix4x3 *m1, const struct Matrix4x3 *m0, struct Matrix4x3 *mF);
extern int data_ov062_0211ddf0[];
extern int data_ov062_0211dde0[];
extern void func_ov062_02116edc(void *c);
extern int Math_Function_0203b14c(void*, int, int, int, int);
extern void* _ZN8dActor_c11UpdateCarryER6PlayerRK7Vector3(void*, int, void*);
extern int data_ov062_0211df10[];
extern int data_ov062_0211df14[];
extern int data_ov062_0211df18[];
unsigned short DecIfAbove0_Short(unsigned short* p);
extern int func_ov062_02115f84(char* c);
void func_ov062_02116010(void* self);
void func_ov062_02116d28(char* c);
void func_ov062_02116e80(void* c);
void func_ov062_02116dbc(char* c);
extern signed char data_0209f2f8;
extern char data_ov062_0211dec0[];
extern char data_ov062_0211ded0[];
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* self, dActor_c* a, int r, int h, unsigned int e, unsigned int g);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, dActor_c* a, int r, int h, Vector3_16* p, Vector3_16* q);
extern int AngleDiff(int a, int b);
extern unsigned int RandomIntInternal(void* s);
extern int data_0209e650[];
extern int data_ov062_0211de80[];
extern int data_ov062_0211def0[];
}

// @symbol daHolhei_c_classInit
/* Reconstructed source-style name: SM64DS proves daHolhei_c through RTTI,
 * allocation size 0x438, vtable identity, and the HOLHEI registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Chuckya_Spawn.
 *
 * Every instruction the cartridge has here falls out of the one `new`.
 * 0x0211747c loads 1080 = 0x438 into fBase_c::operator new; the implicit
 * constructor calls dEnemyBase_c's C2, stores &_ZTV10daHolhei_c[2] because
 * this TU emits the table, then the four member C1s at 0x110 / 0x144 /
 * 0x300 / 0x368. The null check is the one `new` itself emits. */
extern "C" daHolhei_c *daHolhei_c_classInit(void)
{
    return new daHolhei_c();
}

// @symbol _ZN10daHolhei_c16OnAimedAtWithEggEv
s32 daHolhei_c::OnAimedAtWithEgg() {
    return 827392;
}

// @symbol _ZN10daHolhei_c13InitResourcesEv
/* Claims six shared files -- one model and five animations -- builds the
 * shadow and both collision volumes, seeds mHomePos and mPrevPos from mPos,
 * and enters the starting state. Does not check the loads.
 *
 * dCcAc_c::Init and dBgCh_Actr::Init keep scalar-slot wrappers: both carry
 * Fix12<int> by value in the ROM mangled name (wall 6az). */
int daHolhei_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(*(SharedFilePtr *)data_ov062_0211ddf0), 1, -1);
    mShadowModel.InitCylinder();
    Animation::LoadFile(*(SharedFilePtr *)data_ov062_0211dde8);
    Animation::LoadFile(*(SharedFilePtr *)data_ov062_0211dde0);
    Animation::LoadFile(*(SharedFilePtr *)data_ov062_0211de00);
    Animation::LoadFile(*(SharedFilePtr *)data_ov062_0211de08);
    Animation::LoadFile(*(SharedFilePtr *)data_ov062_0211ddf8);
    mVertAccel = -0x3000;
    mTerminalVelocity = -0x1e000;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCc_c, (dActor_c*)this, 0xc8000, 0xfa000, 0x200004, 0x3010);
    mModel.speed = 0x1000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mMeshClsn, (dActor_c*)this, 0x118000, 0x118000, 0, 0);
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    unk_108 = 1;
    unk_10a = 4;
    mPrevPosX = mPosX;
    mPrevPosY = mPosY;
    mPrevPosZ = mPosZ;
    ::daHolhei_c_ChangeState(this, data_ov062_0211dee0);
    return 1;
}

// @symbol _ZN10daHolhei_c8BehaviorEv
int daHolhei_c::Behavior()
{
    char* c = (char*)this;

    if (UpdateKillByInvincibleChar(mMeshClsn, mModel, 3))
        return 1;

    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    DecIfAbove0_Short(&unk_3e6);
    DecIfAbove0_Short(&unk_3e8);

    {
        M* m = (M*)mState;
        if (m->pmf != 0)
            (((Klass*)c)->*(m->pmf))();
    }

    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    UpdatePos((dCc_c*)&mdCc_c);

    if (mState != (void*)data_ov062_0211dea0) {
        if (mState != (void*)data_ov062_0211dec0
            && mState != (void*)data_ov062_0211de70) {
            int r2 = 0;
            signed char t = data_0209f2f8;
            if (t == 0x16) {
                if (mPosX > (int)0xff95c000)
                    r2 = 1;
            } else if (t == 0x15) {
                if (mPosZ < (int)0xff2f4000)
                    r2 = 1;
            }
            if (r2 != 0
                || (mHorzSpeed != 0
                    && IsGoingOffCliff(mMeshClsn, 0x3c000, (s16)0x2888, 0, 1, 0x32000))) {
                mHorzSpeed = 0;
                mEdgeStop = 1;
                mPosX = mPrevPosX;
                mPosY = mPrevPosY;
                mPosZ = mPrevPosZ;
            } else {
                if (mEdgeStop == 1)
                    mEdgeStop = 0;
            }
        }
        mPrevPosX = mPosX;
        mPrevPosY = mPosY;
        mPrevPosZ = mPosZ;
        UpdateWMClsn(mMeshClsn, 3);
    }

    if (mState == (void*)data_ov062_0211ded0
        || mState == (void*)data_ov062_0211dee0
        || mState == (void*)data_ov062_0211de90
        || mState == (void*)data_ov062_0211df00) {
        func_ov062_02116010(c);
    }

    mdCc_c.Clear();
    mModel.Advance();
    mModel.UpdateVerts();

    {
        char* p3f8 = (char*)mHeld;
        if (p3f8 != 0) {
            int flag = (mFlags & 0x4000) != 0;
            if (flag) {
                if (*(int*)(p3f8 + 0xc8) != 0) {
                    func_ov062_02116d28(c);
                    goto ret;
                }
            }
        }
    }

    mdCc_c.Update();
    mCarryOffsX = 0;
    mCarryOffsY = 0;
    mCarryOffsZ = 0;
    func_ov062_02116e80(c);
    func_ov062_02116dbc(c);

ret:
    return 1;
}

// @symbol _ZN10daHolhei_c6RenderEv
int daHolhei_c::Render()
{
    void *r1 = mHeld;
    if (r1 != 0) {
        int r0 = mFlags;
        int flag = (r0 & 0x4000) ? 1 : 0;
        if (flag != 0) {
            if (*(int*)((char*)r1 + 0xc8) != 0) {
                func_ov062_02116edc(this);
            }
        }
    }
    /* ModelAnim overrides Render; the ROM calls Model::Render on this member. */
    mModel.Model::Render((const Vector3 *)0);
    return 1;
}

// @symbol _ZN10daHolhei_c16OnPendingDestroyEv
void daHolhei_c::OnPendingDestroy()
{
}

// @symbol _ZN10daHolhei_c16CleanupResourcesEv
int daHolhei_c::CleanupResources()
{
    ((SharedFilePtr *)data_ov062_0211ddf0)->Release();
    ((SharedFilePtr *)data_ov062_0211dde8)->Release();
    ((SharedFilePtr *)data_ov062_0211dde0)->Release();
    ((SharedFilePtr *)data_ov062_0211de00)->Release();
    ((SharedFilePtr *)data_ov062_0211de08)->Release();
    ((SharedFilePtr *)data_ov062_0211ddf8)->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- func_ov062_02116edc, 0x02116edc, size 0xe4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02116edc
/* recovered: shared common types, declarations from a shared header */
extern "C" {
int Math_Function_0203b14c(void*, int, int, int, int);
void* _ZN8dActor_c11UpdateCarryER6PlayerRK7Vector3(void*, int, void*);

void func_ov062_02116edc(void* c_){
    char* c = (char*)c_;
  int idx = 0;
  if(*(int*)(*(int*)(c+0x3f8) + 8) == 2) idx = 1;
  int k = idx * 0xc;
  Math_Function_0203b14c(c+0x42c, *(int*)((char*)data_ov062_0211df10 + k), 0x800, 0x3e8000, 4);
  Math_Function_0203b14c(c+0x430, *(int*)((char*)data_ov062_0211df14 + k), 0x800, 0x3e8000, 4);
  Math_Function_0203b14c(c+0x434, *(int*)((char*)data_ov062_0211df18 + k), 0x800, 0x3e8000, 4);
  void* r = _ZN8dActor_c11UpdateCarryER6PlayerRK7Vector3(c, *(int*)(c+0x3f8), c+0x42c);
  { struct M12w { int w[12]; };  /* array-wrapper copy */
    *(M12w*)(c+0x31c) = *(M12w*)r; }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- func_ov062_02116e80, 0x02116e80, size 0x5c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol func_ov062_02116e80
void func_ov062_02116e80(void *c) {
    Matrix4x3_FromRotationY((struct Matrix4x3*)((char*)c + 0x31c), *(s16*)((char*)c + 0x8e));
    *(Fix12i*)((char*)c + 0x340) = *(Fix12i*)((char*)c + 0x5c) >> 3;
    *(Fix12i*)((char*)c + 0x344) = *(Fix12i*)((char*)c + 0x60) >> 3;
    *(Fix12i*)((char*)c + 0x348) = *(Fix12i*)((char*)c + 0x64) >> 3;
    MulMat4x3Mat4x3((struct Matrix4x3*)(*(int*)((char*)c + 0x314) + 0x90),
                    (struct Matrix4x3*)((char*)c + 0x31c),
                    (struct Matrix4x3*)((char*)c + 0x3fc));
    *(int*)((char*)c + 0xc8) = (int)((char*)c + 0x3fc);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov062_02116dbc, 0x02116dbc, size 0xc4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02116dbc
/* recovered: shared common types */
extern "C" void func_ov062_02116dbc(char* thiz)
{
    char* c = thiz;
    { struct M12w { int w[12]; };  /* array-wrapper copy: keeps C's block copy under -lang c++ */
      *(M12w*)(c + 0x390) = *(M12w*)&IDENTITY_MATRIX4X3; }
    *(int*)(c + 0x3b4) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x3b8) = *(int*)(c + 0x60) >> 3;
    *(int*)(c + 0x3bc) = *(int*)(c + 0x64) >> 3;
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x144) != 0) {
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, *(ShadowModel*)(c + 0x368), *(Matrix4x3*)(c + 0x390), 0x12c000, 0x32000, 0xf);
    } else {
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, *(ShadowModel*)(c + 0x368), *(Matrix4x3*)(c + 0x390), 0x12c000, 0x3e8000, 0xf);
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov062_02116d28, 0x02116d28, size 0x94 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02116d28
/* recovered: shared common types */
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void func_ov062_02116d28(char *c)
{
    { struct M12w { int w[12]; };  /* array-wrapper copy: keeps C's block copy under -lang c++ */
      *(M12w*)(c + 0x390) = *(M12w*)&IDENTITY_MATRIX4X3; }
    char *o = *(char **)(c + 0x3f8);
    *(int *)(c + 0x3b4) = *(int *)(o + 0x5c) >> 3;
    o = *(char **)(c + 0x3f8);
    *(int *)(c + 0x3b8) = *(int *)(o + 0x60) >> 3;
    o = *(char **)(c + 0x3f8);
    *(int *)(c + 0x3bc) = *(int *)(o + 0x64) >> 3;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j((dActor_c *)c, *(ShadowModel *)(c + 0x368), *(Matrix4x3 *)(c + 0x390), 0x12c000, 0x32000, 0xf);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- daHolhei_c_ChangeState, 0x02116cd8, size 0x50 */
/* -------------------------------------------------------------------------- */
namespace tu {  /* namespaced: the file-scope view above is (void*, void*) */
// @symbol daHolhei_c_ChangeState
/* Coined C name. ROM does not preserve the English spelling; the (C*, PMF*)
 * parameter types are a reconstruction the bytes cannot distinguish from
 * (void*, void*). */
extern "C" int daHolhei_c_ChangeState(C *c, PMF *p) { c->pp = p; PMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(); }
}  /* namespace tu */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov062_02116c78, 0x02116c78, size 0x60 */
/* -------------------------------------------------------------------------- */
extern "C" {
extern int data_ov062_0211ddf8[];
// @symbol func_ov062_02116c78
int func_ov062_02116c78(char *c){
    daHolhei_c *self = (daHolhei_c *)c;
  self->unk_3e6 = 0x3c;
  *(int*)(c+0x3f0) = 0;
  *(int*)(c+0x35c) = 0x1000;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void*)(c+0x300), (void*)data_ov062_0211ddf8[1], 0, 0x1000, 0);
  *(short*)(c+0x3f4) = Vec3_HorzAngle((Vector3*)(c+0x5c), (Vector3*)(c+0x3c0));
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov062_02116bf8, 0x02116bf8, size 0x80 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol func_ov062_02116bf8
int func_ov062_02116bf8(char* c){
    daHolhei_c *self = (daHolhei_c *)c;
  _Z14ApproachLinearRiii((int*)(c+0x98), 0, 0x2000);
  if(self->unk_3e6 == 0){
    ApproachAngle((s16*)(c+0x94), *(s16*)(c+0x3f4), 0xa, 0x200, 0x100);
    if(AngleDiff(*(s16*)(c+0x3f4), *(s16*)(c+0x8e)) < 0x100){
      ::daHolhei_c_ChangeState(c, data_ov062_0211dee0);
    }
  }
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov062_02116b80, 0x02116b80, size 0x78 */
/* -------------------------------------------------------------------------- */
extern "C" {
extern unsigned int RandomIntInternal(void* s);
extern int data_ov062_0211ddf8[];
extern int data_0209e650[];
// @symbol func_ov062_02116b80
int func_ov062_02116b80(char* c){
  *(short*)(c+0x300+0xf4) = (short)((RandomIntInternal(data_0209e650) >> 8) << 0xc);
  *(short*)(c+0x100) = (short)(((RandomIntInternal(data_0209e650) >> 8) & 0x3f) + 0x64);
  *(int*)(c+0x35c) = 0x1000;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x300, (void*)data_ov062_0211ddf8[1], 0, 0x1000, 0);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov062_02116a08, 0x02116a08, size 0x178 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol func_ov062_02116a08
    int func_ov062_02116a08(char* c)
    {
    daHolhei_c *self = (daHolhei_c *)c;
        struct { int vd0[3]; int vd1[3]; } L;
        int r;
        char* pl;
        int* pos;
        char* p0;
        char* p1;

        p0 = c + 0x5c;
        p1 = c + 0x3c0;

        L.vd0[0] = 0;
        L.vd0[1] = 0;
        L.vd0[2] = 0;

        Vec3_Dist(p0, p1);

        r = func_ov062_02115f84(c);
        if (r != 0 || self->mEdgeStop == 1) {
            if (r != 2) {
                ::daHolhei_c_ChangeState(c, data_ov062_0211df00);
            } else {
                ::daHolhei_c_ChangeState(c, data_ov062_0211de70);
            }
            return 1;
        }

        ApproachAngle(c + 0x94, *(short *)(c + 0x3f4), 0xa, 0x200, 0x100);

        *(int *)(c + 0x98) = 0xa000;
        pl = (char *)_ZN8dActor_c13ClosestPlayerEv(c);

        *(int *)(c + 0x3ec) =
            _ZN5Sound8PlayLongEjjjRK7Vector3s(
                *(unsigned int *)(c + 0x3ec), 3, 0x18a, c + 0x74, 0);

        if (pl != 0 && self->unk_3e8 == 0) {
            pos = (int *)(((int)pl + 0x5c));
            L.vd1[0] = pos[0];
            L.vd1[1] = pos[1];
            L.vd1[2] = pos[2];

            if (Vec3_Dist(c + 0x3c0, L.vd1) < 0x3e8000) {
                *(int *)(c + 0x98) = 0;
                ::daHolhei_c_ChangeState(c, data_ov062_0211de90);
            }
            return 1;
        }

        if (*(unsigned short *)(c + 0x100) == 0)
            ::daHolhei_c_ChangeState(c, data_ov062_0211dee0);

        return 1;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov062_02116980, 0x02116980, size 0x88 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol func_ov062_02116980
int func_ov062_02116980(char *c) {
    daHolhei_c *self = (daHolhei_c *)c;
    *(int*)(c+0x5c) = self->mPrevPosX;
    *(int*)(c+0x60) = self->mPrevPosY;
    *(int*)(c+0x64) = self->mPrevPosZ;
    *(int*)(c+0x98) = 0;
    if (AngleDiff(*(short*)(c+0x3f4), Vec3_HorzAngle(c+0x5c, c+0x3c0)) <= 0x2000) {
        *(short*)(c+0x3f4) = *(short*)(c+0x94) - 0x1000;
    } else {
        *(short*)(c+0x3f4) = Vec3_HorzAngle(c+0x5c, c+0x3c0);
    }
    *(short*)(c+0x100) = 0x46;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov062_02116894, 0x02116894, size 0xec */
/* -------------------------------------------------------------------------- */
extern "C" {
// @symbol func_ov062_02116894
int func_ov062_02116894(char* c){
    daHolhei_c *self = (daHolhei_c *)c;
  if(*(unsigned short*)(c+0x100) != 0) return 1;
  if(*(int*)(c+0x98) == 0) goto angle;
  if(func_ov062_02115f84(c) != 0 || self->mEdgeStop == 1){
    *(short*)((c+0x300)+0xf4) = *(short*)(c+0x94) - 0x2000;
    goto angle;
  }
  *(short*)(c+0x94) = *(short*)((c+0x300)+0xf4);
  ::daHolhei_c_ChangeState(c, data_ov062_0211dee0);
  *(short*)((c+0x300)+0xe8) = 0x1e;
  *(short*)((c+0x300)+0xf4) = *(short*)(c+0x94);
  return 1;
angle:
  if(AngleDiff(*(short*)((c+0x300)+0xf4), *(short*)(c+0x8e)) < 0x100){
    *(int*)(c+0x98) = 0xa000;
  }
  ApproachAngle(c+0x94, *(short*)((c+0x300)+0xf4), 0xa, 0x200, 0x100);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov062_02116850, 0x02116850, size 0x44 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol func_ov062_02116850
s16 func_ov062_02116850(void* c) {
    *(int*)((char*)c + 0xa8) = 0;
    s16 angle = Vec3_HorzAngle((char*)c + 0x5c, (char*)c + 0x3c0);
    *(s16*)((char*)c + 0x3f4) = angle;
    *(s16*)((char*)c + 0x100) = 0x14;
    *(int*)((char*)c + 0x98) = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov062_021167c0, 0x021167c0, size 0x90 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_021167c0
extern "C" int func_ov062_021167c0(char* c){
    daHolhei_c *self = (daHolhei_c *)c;
    ApproachAngle(c + 0x94, *(short*)(c + 0x3f4), 0xa, 0x200, 0x100);
    if (AngleDiff(*(short*)(c + 0x3f4), *(short*)(c + 0x8e)) < 0x100) {
        *(int*)(c + 0x98) = 0xa000;
    } else {
        *(short*)(c + 0x100) = 0x14;
    }
    if (*(unsigned short*)(c + 0x100) == 0) {
        self->unk_3e8 = 0x1e;
        ::daHolhei_c_ChangeState(c, data_ov062_0211dee0);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov062_02116784, 0x02116784, size 0x3c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol func_ov062_02116784
int func_ov062_02116784(char *c) {
    *(int*)(c+0x3f0) = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x300, (void*)data_ov062_0211ddf8[1], 0, 0x1000, 0);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov062_021165e8, 0x021165e8, size 0x19c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_021165e8
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov062_021165e8(char* c)
{
    daHolhei_c *self = (daHolhei_c *)c;
    char* player;
    int r;
    volatile struct Vector3 pos;
    struct Vector3* pp;

    player = (char*)_ZN8dActor_c13ClosestPlayerEv(c);
    r = func_ov062_02115f84(c);
    if (r != 0 || self->mEdgeStop == 1) {
        if (r != 2)
            ::daHolhei_c_ChangeState(c, data_ov062_0211df00);
        else
            ::daHolhei_c_ChangeState(c, data_ov062_0211de70);
        return 1;
    }

    if (player == 0)
        return 1;

    pp = (struct Vector3*)(int)(player + 0x5c);
    pos.x = pp->x;
    pos.y = pp->y;
    pos.z = pp->z;

    if (*(int*)(c + 0x3f0) == 0) {
        *(short*)(c + 0x3f4) = (short)_ZN8dActor_c18HorzAngleToCPlayerEv(c);
        ApproachAngle((short*)(c + 0x94), *(short*)(c + 0x3f4), 0x80, 0x200, 0x400);
        if (AngleDiff(*(short*)(c + 0x3f4), *(short*)(c + 0x8e)) < 0x200) {
            *(int*)(c + 0x98) = 0x1e000;
            *(int*)(c + 0x3f0) = 1;
            self->mChasePosX = pos.x;
            self->mChasePosY = pos.y;
            self->mChasePosZ = pos.z;
        }
    }

    *(unsigned int*)(c + 0x3ec) = _ZN5Sound8PlayLongEjjjRK7Vector3s(*(unsigned int*)(c + 0x3ec), 3, 0x18a, (const void*)(c + 0x74), 0);

    {
        int s = *(int*)(c + 0x3f0);
        if (s != 0) {
            if (s == 1) {
                if (Vec3_Dist((const void*)(c + 0x5c), (const void*)(c + 0x3cc)) < 0x3c000)
                    *(int*)(c + 0x3f0) = 2;
            } else {
                if (Vec3_Dist((const void*)(c + 0x5c), (const void*)(c + 0x3cc)) > 0xc8000)
                    ::daHolhei_c_ChangeState(c, data_ov062_0211ded0);
            }
        }
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov062_021165e0, 0x021165e0, size 0x8 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol func_ov062_021165e0
int func_ov062_021165e0(void)
{
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov062_021164e8, 0x021164e8, size 0xf8 */
/* -------------------------------------------------------------------------- */
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
// @symbol func_ov062_021164e8
extern "C" int func_ov062_021164e8(char *c)
{
    daHolhei_c *self = (daHolhei_c *)c;
    int flag;
    int t;
    if (self->unk_3e5 == 0) {
        t = (*(int *)(c + 0xb0) & 0x4000) != 0;
        if (t) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void *)(c + 0x300), (void *)data_ov062_0211de00[1], 0, 0x1000, 0);
            self->unk_3e5 = 1;
        }
    }
    flag = *(int *)(c + 0xb0);
    t = (flag & 0x400) != 0;
    if (t) goto do_block;
    t = (flag & 0x2000) != 0;
    if (t) goto do_block;
    t = (flag & 0x100) != 0;
    if (t) goto done;
do_block:
    {
        void *m = *(void **)(c + 0x3f8);
        *(u16 *)(c + 0x94) = *(s16 *)((char *)m + 0x8e);
        *(int *)(c + 0x128) |= 2;
        ::daHolhei_c_ChangeState(c, data_ov062_0211dec0);
        if (*(int *)((char *)*(void **)(c + 0x3f8) + 8) == 2) {
            *(int *)(c + 0xa8) = 0x50000;
            *(int *)(c + 0x98) = 0x14000;
        }
        *(void **)(c + 0x3f8) = 0;
    }
done:
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov062_02116498, 0x02116498, size 0x50 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol func_ov062_02116498
int func_ov062_02116498(char* c){
  *(int*)(c+0x9c)=-0x4000;
  *(int*)(c+0xa8)=0x3c000;
  *(int*)(c+0x98)=0xa000;
  *(int*)(c+0xa0)=-0x64000;
  func_02012694(0xf7, c+0x74);
  *(int*)(c+0xb0)=0;
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov062_021163b0, 0x021163b0, size 0xe8 */
/* -------------------------------------------------------------------------- */
/* func_ov062_021163b0 at 0x021163b0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov062).
 */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol func_ov062_021163b0
int func_ov062_021163b0(char* c)
{
    Vector3 v[2];
    if (data_02092138 > *(int*)(c + 0x60)) {
        _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
        return 1;
    }
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x144) != 0) {
        _ZN8dActor_c14TriplePoofDustEv(c);
        _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
        func_02012694(0x125, c + 0x74);
        func_0200d8c8(data_0209f318, c + 0x5c, 0x7d0000);
        v[0].x = *(int*)(c + 0x5c);
        v[0].y = *(int*)(c + 0x60);
        v[0].z = *(int*)(c + 0x64);
        v[0].y += 0x32000;
        v[1].x = *(int*)(c + 0x5c);
        v[1].y = *(int*)(c + 0x60);
        v[1].z = *(int*)(c + 0x64);
        _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(c, &v[1], *(u8*)(c + 0x10a) + 1, 0xa000, 0);
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov062_02116368, 0x02116368, size 0x48 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol func_ov062_02116368
int func_ov062_02116368(void* c) {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0x300, (void *)data_ov062_0211dde8[1], 0x40000000, 0x1000, 0);
    *(int*)((char*)c + 0x3f0) = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov062_0211632c, 0x0211632c, size 0x3c */
/* -------------------------------------------------------------------------- */
extern "C" {
extern int data_ov062_0211de80[];
// @symbol func_ov062_0211632c
int func_ov062_0211632c(void* c){
  if(_ZN9Animation8FinishedEv((char*)c+0x350)){ *(int*)((char*)c+0x3f0)=0; ::daHolhei_c_ChangeState(c, data_ov062_0211de80); }
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov062_021162b8, 0x021162b8, size 0x74 */
/* -------------------------------------------------------------------------- */
extern "C" {
extern unsigned int RandomIntInternal(void* s);
extern int data_ov062_0211dde0[];
extern int data_0209e650[];
// @symbol func_ov062_021162b8
int func_ov062_021162b8(char* c){
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x300, (void*)data_ov062_0211dde0[1], 0, 0x1000, 0);
  *(short*)(c+0x100) = (short)(((RandomIntInternal(data_0209e650) >> 8) & 0x1f) + 0x14);
  *(short*)(c+0x300+0xf4) = (short)((RandomIntInternal(data_0209e650) >> 8) << 0xc);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov062_02116274, 0x02116274, size 0x44 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol func_ov062_02116274
int func_ov062_02116274(unsigned char *c)
{
    short *p94 = (short *)((int)c + 0x94);
    short val = *p94;
    unsigned short *p100 = (unsigned short *)((int)c + 0x100);
    val += 0x500;
    *p94 = val;
    if (*p100 == 0)
    {
        ::daHolhei_c_ChangeState(c, data_ov062_0211deb0);
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov062_02116238, 0x02116238, size 0x3c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol func_ov062_02116238
int func_ov062_02116238(char *c){
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c+0x300, data_ov062_0211de08[1], 0x40000000, 0x1000, 0);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov062_021161a8, 0x021161a8, size 0x90 */
/* -------------------------------------------------------------------------- */
/* func_ov062_021161a8 at 0x021161a8 (ov062), size 0x90
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol func_ov062_021161a8
int func_ov062_021161a8(char *c)
{
  if (*(int *)(c + 0x3f8) != 0 && _ZNK9Animation12WillHitFrameEi(c + 0x350, 0x14))
  {
    cv::func_ov002_020db54c(*(int *)(c + 0x3f8), 0x28000, 0x50000, *(short *)(c + 0x8e));
    *(int *)(c + 0x3f8) = 0;
    func_02012694(0x126, c + 0x74);
  }
  if (_ZN9Animation8FinishedEv(c + 0x350))
  {
    int *q = (int *)(c + 0x128);
    *q &= ~2;
    ::daHolhei_c_ChangeState(c, data_ov062_0211ded0);
  }
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov062_02116010, 0x02116010, size 0x198 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02116010
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
namespace tu {  /* namespaced: a (void*) view of this symbol is in scope */
void func_ov062_02116010(char *c)
{
    u32 id;
    void *a;
    int b;
    int angle;
    struct Vector3_16 v;

    id = *(u32 *)(c + 0x134);
    if (id != 0 &&
        (a = _ZN8dActor_c10FindWithIDEj(id)) != 0 &&
        (*(int *)(c + 0x130) & 0x10) != 0) {
        v.x = 0x1000;
        v.y = 0;
        v.z = 0;
        _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(c, &v, a, 0);
        func_02012694(0x125, c + 0x74);
        return;
    }

    *(int *)(((long long)(int)(c + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x80;

    id = *(u32 *)(c + 0x134);
    if (id == 0)
        return;
    a = _ZN8dActor_c10FindWithIDEj(id);
    if (a == 0)
        return;
    b = (int)(*(u16 *)((char *)a + 0xc) == 0xbf);
    if (b == 0)
        return;
    angle = _ZN8dActor_c18HorzAngleToCPlayerEv(c);
    angle = AngleDiff(angle, *(s16 *)(c + 0x8e));
    if (angle > 0x2000) {
        if ((*(int *)(c + 0x130) & 0x1000) == 0)
            return;
        *(int *)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFFULL) |= 0x80;
        if (_ZN6Player7TryGrabER8dActor_c(a, c) == 0)
            return;
        *(int *)(c + 0x3f8) = (int)a;
        *(int *)(((long long)(int)(c + 0x128)) & 0xFFFFFFFFFFFFFFFFLL) |= 2;
        *(int *)(c + 0x98) = 0;
        ::daHolhei_c_ChangeState(c, data_ov062_0211dea0);
        return;
    }
    if (func_ov002_020db5f4(a, c) == 0)
        return;
    *(int *)(c + 0x3f8) = (int)a;
    *(int *)(c + 0x98) = 0;
    ::daHolhei_c_ChangeState(c, data_ov062_0211def0);
}
}
}  /* namespace tu */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov062_02115f84, 0x02115f84, size 0x8c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
namespace tu {  /* namespaced: a (void*) view of this symbol is in scope */
// @symbol func_ov062_02115f84
int func_ov062_02115f84(char* c) {
    Vector3 v;
    short r4 = 0;
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c+0x144)) {
        char* fr = _ZNK10dBgCh_Actr14GetFloorResultEv(c+0x144);
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3(fr+4, &v);
        r4 = func_02010844(c, &v, *(short*)(c+0x8e));
    }
    if (_ZNK10dBgCh_Actr8IsOnWallEv(c+0x144))
        return 1;
    if (r4 < 0) r4 = -r4;
    if (r4 > 0x1000) return 2;
    return 0;
}
}
}  /* namespace tu */

