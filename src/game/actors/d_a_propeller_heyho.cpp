//cpp
/* HAND-ASSEMBLED translation unit -- ov070/daPropeller_Heyho_c (27 function(s)).
 * tubuild create refused this TU (legacy bodies wrapped in extern "C" { }),
 * so this is a raw concatenation of the complete legacy files in REVERSE
 * ROM order (mwccarm emits one .text section per function in the reverse
 * of source order). Conflicting declarations were reconciled by hand; see
 * the manifest notes.
 *
 *
 * deslop
 * Leftover: dCcAc_c::Init / dBgCh_Actr::Init stay mangled (Fix12-by-value, 6az;
 *   dBgCh Init header Fix12i mangles as int -- this TU's InitResources call).
 *   ModelAnim::SetAnim, DropShadowRadHeight, SpawnCoins, SpawnFireball,
 *   Particle::System::New / NewUnkCallback818, Player::SpinBounce / Hurt stay
 *   mangled (Fix12-by-value, 6az -- this TU). Animation::Finished / WillHitFrame
 *   in func_ov070_0211f48c / 0211f62c / 0211f6e0 keep c+0x350 (ModelAnim MI
 *   +0x50 -- this TU, measured: `(char *)&mModelAnim + 0x50` DIFFs).
 *   data_ov070_* SharedFilePtr handles (Init LoadFile / Cleanup Release) and
 *   state records (FlyGuy_ChangeState) are sinit-owned BSS, not this TU's data
 *   claim. FlyGuy_ChangeState keeps its C-ABI name. Helpers stay func_ov070_*
 *   (cartridge addresses, no identifiers). ApproachAngle int-target vs short
 *   via block-scope extern (this TU). V3w/V3h array-wrapper for struct copy
 *   (this TU). func_ov070_0211f48c / 0211f62c / 0211f6e0 keep char* for the
 *   inherited dActor/Player offsets and the V3w copy (this TU, measured).
 */

#include "daPropeller_Heyho_c.h"

/* The cartridge preserves this descriptor's C-ABI symbol but not its source
 * spelling.  Keep that evidence-bounded alias while giving the table its real
 * field layout; all seven scalar words and the factory relocation are covered
 * by the TU's data claim. */
extern "C" daPropeller_Heyho_c *daPropeller_Heyho_c_classInit(void);

struct PropellerHeyhoSpawnInfo {
    daPropeller_Heyho_c *(*classInit)();
    short behaviorPriority;
    short renderPriority;
    unsigned flags;
    int rangeOffsetY;
    int range;
    int drawDistance;
    int untrackDistance;
};

typedef char PropellerHeyhoSpawnInfo_size_must_be_0x1c[
    sizeof(PropellerHeyhoSpawnInfo) == 0x1c ? 1 : -1];

/* Reconstructed source-style name: SM64DS proves this descriptor through
 * its registry role, the PROPELLER_HEYHO literal ROM profile ID, and the
 * factory relocation it carries; later EAD lineage supplies g_profile_.
 * Exact original spelling is not preserved. Historical alias:
 * FlyGuy_SpawnInfo. */
extern "C" PropellerHeyhoSpawnInfo g_profile_PROPELLER_HEYHO = {
    daPropeller_Heyho_c_classInit,
    0x00e8,
    0x0057,
    0x10000003,
    0x00064000,
    0x000c8000,
    0x01000000,
    0x01000000
};

struct V3w { int w[3]; };  /* array-wrapper: C++ scalarizes a plain struct copy; this form keeps the C front end's ldm/stm block copy */
struct V3h { short h[3]; };

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daPropeller_Heyho_c_classInit
/* The registry factory behind the PROPELLER_HEYHO profile.
 * `return new daPropeller_Heyho_c()` MATCHES (size 0x50); the synthesized
 * ctor stores `_ZTV19daPropeller_Heyho_c + 2`. Historical alias: FlyGuy_Spawn. */
extern "C" daPropeller_Heyho_c *daPropeller_Heyho_c_classInit(void)
{
    return new daPropeller_Heyho_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daPropeller_Heyho_c13OnYoshiTryEatEv
#include "daPropeller_Heyho_c.h"
/* recovered: renamed to Class_Method */
s32 daPropeller_Heyho_c::OnYoshiTryEat() {
    return 5;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daPropeller_Heyho_c13OnTurnIntoEggER6Player
// recovered name: FlyGuy_OnTurnIntoEgg
/* daPropeller_Heyho_c::OnTurnIntoEgg -- vtable slot 19, verified against ov070
 * relocs.txt: _ZTV19daPropeller_Heyho_c (0x02123168) + 0x4c -> 0x021204ec, exactly this
 * placeholder's former address (former name func_ov070_021204ec).
 * Matched byte-for-byte with mwccarm 2004/b56 (ov070).
 */
#include "daPropeller_Heyho_c.h"
#include "Player.h"

void daPropeller_Heyho_c::OnTurnIntoEgg(Player &player)
{
    GivePlayerCoins(player, (u8)(unk_10a + 1), 0);
    KillAndTrackInDeathTable();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daPropeller_Heyho_c16OnAimedAtWithEggEv
#include "daPropeller_Heyho_c.h"
// recovered name: FlyGuy_OnAimedAtWithEgg
/* recovered: renamed to Class_Method */
/* daPropeller_Heyho_c::OnAimedAtWithEgg - recovered from vtable slot identity */
s32 daPropeller_Heyho_c::OnAimedAtWithEgg() {
    return 0x2b000; /* Fix12 egg-aim HEIGHT added to pos.y, per dEnemyBase_c.h slot-29 */
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daPropeller_Heyho_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "daPropeller_Heyho_c.h"
#include "SharedFilePtr.h"
extern SharedFilePtr data_ov070_02123530;
extern SharedFilePtr data_ov070_02123520;
extern SharedFilePtr data_ov070_02123518;
extern SharedFilePtr data_ov070_02123510;
extern SharedFilePtr data_ov070_02123528;
extern SharedFilePtr data_ov070_02123508;
extern SharedFilePtr data_ov070_02123500;
extern daPropeller_Heyho_c::State data_ov070_0212359c;
extern "C" {
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* self, dActor_c* a, int r, int h, unsigned int e, unsigned int g);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, dActor_c* a, int r, int h, Vector3_16* p, Vector3_16* q);
extern int FlyGuy_ChangeState(daPropeller_Heyho_c* c, daPropeller_Heyho_c::State* p);
}

int daPropeller_Heyho_c::InitResources()
{
    mModelAnim.SetFile((BMD_File *)Model::LoadFile(data_ov070_02123530), 1, -1);
    mShadowModel.InitCylinder();
    Animation::LoadFile(data_ov070_02123520);
    Animation::LoadFile(data_ov070_02123518);
    Animation::LoadFile(data_ov070_02123510);
    Animation::LoadFile(data_ov070_02123528);
    Animation::LoadFile(data_ov070_02123508);
    Animation::LoadFile(data_ov070_02123500);
    mCanSpitFire = param1 & 0xff;
    if (mCanSpitFire == 0xff) mCanSpitFire = 0;
    mTerminalVelocity = -0x1e000;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x3c000, 0x32000, 0x200000, 0x7eff0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x50000, 0x3c000, 0, 0);
    unk_108 = 1;
    unk_10a = 1;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    FlyGuy_ChangeState(this, &data_ov070_0212359c);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daPropeller_Heyho_c8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "daPropeller_Heyho_c.h"

#include "Player.h"
extern daPropeller_Heyho_c::State data_ov070_021235cc;
extern daPropeller_Heyho_c::State data_ov070_021235bc;
extern "C" {
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void func_ov070_02120070(daPropeller_Heyho_c *c);
extern void func_ov070_0211f100(daPropeller_Heyho_c *c);
}

int daPropeller_Heyho_c::Behavior()
{
    if (UpdateYoshiEat(mWithMeshClsn) != 0) {
        mdCcAc_c.Clear();
        if (mEatenByYoshi != 0) {
            if (unk_104 == 0) {
                mdCcAc_c.Update();
            }
        }
        func_ov070_02120070(this);
        return 1;
    }

    if (mDeathState != 0) {
        UpdateDeath(mWithMeshClsn);
        func_ov070_02120070(this);
        return 1;
    }

    if (mCurrentState != &data_ov070_021235cc) {
        DecIfAbove0_Short((unsigned short *)&mStateTimer);
    }
    DecIfAbove0_Short(&mCooldown);

    {
        State *q = mCurrentState;
        /* Reads the handler's pointer word directly rather than as `&q->mMain`:
           taking the ADDRESS of a pointer-to-member makes mwcc materialise the
           whole 8-byte pmf. Reading one to CALL it is free. */
        if (*(int *)((char *)q + 8) != 0) {
            (this->*(q->mMain))();
        }
    }

    {
        /* Gravity, clamped at terminal velocity. unk_0ac is read and written
           back unchanged -- the ROM really does reload and restore it here. */
        int v = mVertSpeed + mVertAccel;
        int hi = mTerminalVelocity;
        if (v >= hi) {
            hi = v;
        }
        int tmp = unk_0ac;
        mVertSpeed = hi;
        unk_0ac = tmp;
    }

    UpdatePosWithOnlySpeed(&mdCcAc_c);
    UpdateWMClsn(mWithMeshClsn, 0);

    if (mCurrentState != &data_ov070_021235bc) {
        mAngleY = mPrevAngleY;
        mAngleZ = mPrevAngleZ;
    }

    func_ov070_02120070(this);

    if (mCurrentState != &data_ov070_021235bc) {
        func_ov070_0211f100(this);
    }

    mdCcAc_c.Clear();
    {
        Player *p = ClosestPlayer();
        if (p != 0 && p->mIsVanish == 0) {
            mdCcAc_c.Update();
        }
    }

    mModelAnim.speed = 0x1000;
    mModelAnim.Advance();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daPropeller_Heyho_c6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "daPropeller_Heyho_c.h"

int daPropeller_Heyho_c::Render()
{
    int b = ((mFlags & 0x40000) != 0);
    if (b) return 1;
    mModelAnim.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daPropeller_Heyho_c16OnPendingDestroyEv
/* recovered: shared header, real C++ method
 *
 * fBase_c slot 12. Empty in the ROM: four bytes, `bx lr`.
 */
#include "daPropeller_Heyho_c.h"

void daPropeller_Heyho_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daPropeller_Heyho_c16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Releases the 7 shared file(s) InitResources claimed.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free.
 */
#include "daPropeller_Heyho_c.h"
#include "SharedFilePtr.h"

extern "C" {
}

int daPropeller_Heyho_c::CleanupResources()
{
    data_ov070_02123530.Release();
    data_ov070_02123520.Release();
    data_ov070_02123518.Release();
    data_ov070_02123510.Release();
    data_ov070_02123528.Release();
    data_ov070_02123508.Release();
    data_ov070_02123500.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_02120070
/* recovered: shared common types */
#include "common.h"
extern "C" {

extern void Vec3_Asr(void* d, void* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* thiz, void* sm, void* mtx, int f, int g, unsigned int h);
extern int data_020a0e68[];

typedef struct { int w[12]; } M48;

void func_ov070_02120070(daPropeller_Heyho_c* c)
{
    Vector3 v;
    Vec3_Asr(&v, &c->mPosX, 3);
    Matrix4x3_FromTranslation(data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(data_020a0e68, c->mAngleX, c->mAngleY, c->mAngleZ);
    *(M48*)&c->mModelAnim.mat4x3 = *(M48*)data_020a0e68;
    Matrix4x3_FromTranslation(data_020a0e68, c->mPosX >> 3, (c->mPosY - 0xa000) >> 3, c->mPosZ >> 3);
    *(M48*)c->mShadowMatrix = *(M48*)data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, &c->mShadowModel, c->mShadowMatrix, 0x76000, 0x320000, 0xf);
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol FlyGuy_ChangeState
extern "C" int FlyGuy_ChangeState(daPropeller_Heyho_c *c, daPropeller_Heyho_c::State *p)
{
    c->mCurrentState = p;
    daPropeller_Heyho_c::State *q = c->mCurrentState;
    if (q->mInit == 0)
        return 1;
    return (c->*(q->mInit))();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211ffa8
extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int fix, unsigned int j);
extern unsigned int RandomIntInternal(void* s);
extern int data_0209e650[];
int func_ov070_0211ffa8(daPropeller_Heyho_c* c){
  c->mTargetAngY = (short)(((RandomIntInternal(data_0209e650) >> 8) & 0xf) << 0xc);
  c->mStateTimer = (short)(((RandomIntInternal(data_0209e650) >> 8) & 0x1f) + 0x32);
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&c->mModelAnim, (void*)((int *)&data_ov070_02123520)[1], 0, 0x1000, 0);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211fd98
extern "C" {  /* .c-derived member: C linkage for the whole block */
typedef short s16;

extern int Vec3_Dist(void *a, void *b);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void *p);
extern short Vec3_HorzAngle(void *a, void *b);
extern void ApproachAngle(s16 *dst, s16 target, int a, int b, int c);
extern short Vec3_VertAngle(void *a, void *b);
extern void _Z14ApproachLinearRsss(void *dst, short a, short b);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, short ang);
extern void MulVec3Mat4x3(void *in, void *m, void *out);
extern void _Z14ApproachLinearRiii(void *dst, int a, int b);
extern int FlyGuy_ChangeState(daPropeller_Heyho_c *c, daPropeller_Heyho_c::State *p);
extern Player *_ZN8dActor_c22ClosestNonVanishPlayerEv(daPropeller_Heyho_c *c);

extern int data_020a0e68[];
extern daPropeller_Heyho_c::State data_ov070_0212359c;
extern daPropeller_Heyho_c::State data_ov070_021235ac;

int func_ov070_0211fd98(daPropeller_Heyho_c *c)
{
    int in[3];
    int out[3];
    Vector3 t;
    Player *p;

    in[0] = 0; in[1] = 0; in[2] = 0;
    out[0] = 0; out[1] = 0; out[2] = 0;

    if (Vec3_Dist(&c->mPosX, &c->mHomePosX) > 0x1f4000 ||
        c->mWithMeshClsn.IsOnWall()) {
        c->mTargetAngY = Vec3_HorzAngle(&c->mPosX, &c->mHomePosX);
        if ((u16)c->mStateTimer < 0x14)
            c->mStateTimer = 0x14;
    }
    ApproachAngle(&c->mPrevAngleY, c->mTargetAngY, 0xa, 0x200, 0x100);

    _Z14ApproachLinearRsss(&c->mPrevAngleX, Vec3_VertAngle(&c->mPosX, &c->mHomePosX), 0x100);

    ApproachAngle(&c->mPrevAngleZ,
                  (c->mPrevAngleY - c->mTargetAngY) / 2,
                  0xa, 0x100, 0x50);

    in[2] = 0xa000;
    Matrix4x3_FromRotationY(data_020a0e68, c->mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, c->mPrevAngleX);
    MulVec3Mat4x3(in, data_020a0e68, out);

    _Z14ApproachLinearRiii(&c->unk_0a4, out[0], 0x1000);
    _Z14ApproachLinearRiii(&c->mVertSpeed, out[1], 0x800);
    _Z14ApproachLinearRiii(&c->unk_0ac, out[2], 0x1000);

    if ((u16)c->mStateTimer == 0) {
        FlyGuy_ChangeState(c, &data_ov070_0212359c);
        return 1;
    }
    if (c->mCooldown != 0)
        return 1;
    if (Vec3_Dist(&c->mPosX, &c->mHomePosX) < 0x5dc000) {
        p = _ZN8dActor_c22ClosestNonVanishPlayerEv(c);
        if (p) {
            int *pos = (int *)&p->mPosX;
            t.x = pos[0];
            t.y = pos[1];
            t.z = pos[2];
            if (Vec3_Dist(&c->mPosX, &t) < 0x3e8000)
                FlyGuy_ChangeState(c, &data_ov070_021235ac);
        }
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211fd60
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int fix, unsigned int j);
/* (data_ov070_02123520: SharedFilePtr view declared earlier in this TU) */
int func_ov070_0211fd60(daPropeller_Heyho_c *p) {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&p->mModelAnim, ((void**)&data_ov070_02123520)[1], 0, 0x1000, 0);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211fae4
extern "C" {  /* .c-derived member: C linkage for the whole block */

extern Player *_ZN8dActor_c22ClosestNonVanishPlayerEv(daPropeller_Heyho_c *self);
extern short Vec3_HorzAngle(void *v0, void *v1);
extern short Vec3_VertAngle(void *v1, void *v0);
extern void ApproachAngle(s16 *cur, s16 target, int a, int b, int c);
extern void Matrix4x3_FromRotationY(void *m, int ang);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, short ang);
extern void MulVec3Mat4x3(void *a, void *m, void *out);
extern int Vec3_Dist(void *a, void *b);
extern unsigned int RandomIntInternal(void *seed);
extern int FlyGuy_ChangeState(daPropeller_Heyho_c *c, daPropeller_Heyho_c::State *p);

extern signed char data_0209f2f8;
extern daPropeller_Heyho_c::State data_ov070_021235dc;
extern int data_020a0e68[];
extern daPropeller_Heyho_c::State data_ov070_0212359c;
extern int data_0209e650[];
extern daPropeller_Heyho_c::State data_ov070_021235cc;
extern daPropeller_Heyho_c::State data_ov070_0212358c;

int func_ov070_0211fae4(daPropeller_Heyho_c *c)
{
    Player *player;
    Vector3 vin;
    Vector3 vb;
    Vector3 vc;
    Vector3 vd;

    player = _ZN8dActor_c22ClosestNonVanishPlayerEv(c);
    if (player == 0) {
        if (data_0209f2f8 != 0x16) {
            c->mHomePosX = c->mPosX;
            c->mHomePosY = c->mPosY;
            c->mHomePosZ = c->mPosZ;
            c->mHomePosY += 0xc8000;
        }
        c->mPosX = c->mPrevPosX;
        c->mPosY = c->mPrevPosY;
        c->mPosZ = c->mPrevPosZ;
        c->mPrevAngleX = 0;
        c->mStateTimer = 0;
        c->mVertSpeed = 0;
        FlyGuy_ChangeState(c, &data_ov070_021235dc);
        return 1;
    }

    vin.x = 0;
    vin.y = 0;
    vin.z = 0;

    {
    int *q = (int *)&player->mPosX;
    vb.x = q[0];
    vb.y = q[1];
    vb.z = q[2];
    }
    vb.y += 0xc8000;
    vc.x = vb.x;
    vc.y = vb.y;
    vc.z = vb.z;

    c->mTargetAngY = Vec3_HorzAngle(&c->mPosX, &vc);
    ApproachAngle(&c->mPrevAngleY, c->mTargetAngY, 1, 0x500, 0x500);

    vd.x = vb.x;
    vd.y = vb.y;
    vd.z = vb.z;
    ApproachAngle(&c->mPrevAngleX, Vec3_VertAngle(&c->mPosX, &vd), 1, 0x500, 0x500);

    ApproachAngle(&c->mPrevAngleZ,
                  (c->mPrevAngleY - c->mTargetAngY) / 2,
                  0xa, 0x100, 0x50);

    vin.z = 0xf000;
    Matrix4x3_FromRotationY(data_020a0e68, c->mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, c->mPrevAngleX);
    MulVec3Mat4x3(&vin, data_020a0e68, &c->unk_0a4);

    if (Vec3_Dist(&c->mPosX, &c->mHomePosX) > 0x5dc000) {
        FlyGuy_ChangeState(c, &data_ov070_0212359c);
        return 1;
    }

    {
    int dist = Vec3_Dist(&c->mPosX, &vb);
    int thresh = 0x258000;
    if (data_0209f2f8 == 0x16) thresh = 0x384000;
    if (dist < thresh) {
        c->unk_0a4 = 0;
        c->mVertSpeed = 0;
        c->unk_0ac = 0;
        if (c->mCanSpitFire == 0 ||
            (((unsigned int)RandomIntInternal(&data_0209e650) >> 8) & 1) == 0) {
            FlyGuy_ChangeState(c, &data_ov070_021235cc);
        } else {
            FlyGuy_ChangeState(c, &data_ov070_0212358c);
        }
    }
    }

    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211fa80
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern int _ZN8dActor_c23HorzAngleToCPlayerOrAngEv(void *);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int fix, unsigned int j);
int func_ov070_0211fa80(daPropeller_Heyho_c *c) {
    c->mHitDuringAttack = 0;
    c->mStateTimer = 0x3f;
    c->mStateStep = 0;
    c->mTargetAngY = c->HorzAngleToCPlayerOrAng();
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&c->mModelAnim, (void*)((int *)&data_ov070_02123518)[1], 0x40000000, 0x1000, 0);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211f6e0
extern "C" {  /* .c-derived member: C linkage for the whole block */
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef signed char s8;


/* (data_ov070_02123510: SharedFilePtr view declared earlier in this TU) */
extern s8 data_0209f2f8;
extern daPropeller_Heyho_c::State data_ov070_0212359c;
extern daPropeller_Heyho_c::State data_ov070_021235dc;
extern s32 data_0209f32c;
extern int data_020a0e68[];

/* (ApproachAngle: this file's own int-target view, declared inside the function body) */
extern int _ZN9Animation8FinishedEv(void* thiz);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int fix, unsigned int j);
extern int FlyGuy_ChangeState(daPropeller_Heyho_c* c, daPropeller_Heyho_c::State* p);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void* thiz);
extern Player *_ZN8dActor_c22ClosestNonVanishPlayerEv(daPropeller_Heyho_c* thiz);
extern short Vec3_VertAngle(void* v1, void* v0);
extern int Vec3_Dist(void* a, void* b);
extern u16 DecIfAbove0_Short(u16* p);
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(void* mF, s16 angX);
extern void MulVec3Mat4x3(void* v, void* m, void* res);

int func_ov070_0211f6e0(char* c)
{
    extern int ApproachAngle(s16* angle, int target, int step, int maxDelta, int minDelta); /* byte-load-bearing: int target */
    char* player;
    Vector3 tmp;
    Vector3 v;
    Vector3 aim;
    s16 vAngle;
    s16 half;
    s32 z;
    daPropeller_Heyho_c* self = (daPropeller_Heyho_c*)c;

    ApproachAngle((s16*)(c + 0x94), self->mTargetAngY, 0x100, 0x1000, 0x1000);
    ApproachAngle((s16*)(c + 0x96), 0, 0x100, 0x1000, 0x1000);

    if (_ZN9Animation8FinishedEv(c + 0x350)) {
        if (self->mStateStep == 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, (void*)((int *)&data_ov070_02123510)[1], 0, 0x1000, 0);
            self->mStateStep = 1;
        }
        if (self->mHitDuringAttack == 1) {
            if (data_0209f2f8 != 0x16)
                self->mHomePosY += 0x12c000;
            self->mStateStep = 0;
            self->mCooldown = 0x5a;
            FlyGuy_ChangeState(self, &data_ov070_0212359c);
            return 1;
        }
    }

    if (*(u16*)(c + 0x100) == 0 || _ZNK10dBgCh_Actr8IsOnWallEv(c + 0x144)) {
        if (data_0209f2f8 != 0x16) {
            self->mHomePosX = *(s32*)(c + 0x5c);
            self->mHomePosY = *(s32*)(c + 0x60);
            self->mHomePosZ = *(s32*)(c + 0x64);
        }
        *(u16*)(c + 0x92) = 0;
        FlyGuy_ChangeState(self, &data_ov070_021235dc);
        return 1;
    }

    player = (char*)_ZN8dActor_c22ClosestNonVanishPlayerEv((daPropeller_Heyho_c*)c);
    if (player == 0) {
        if (data_0209f2f8 != 0x16) {
            self->mHomePosX = *(s32*)(c + 0x5c);
            self->mHomePosY = *(s32*)(c + 0x60);
            self->mHomePosZ = *(s32*)(c + 0x64);
        }
        *(u16*)(c + 0x92) = 0;
        FlyGuy_ChangeState(self, &data_ov070_021235dc);
        return 1;
    }

    if (*(u8*)(player + 0x706) != 0 && data_0209f32c > *(s32*)(c + 0x60)) {
        if (data_0209f2f8 != 0x16) {
            self->mHomePosX = *(s32*)(c + 0x5c);
            self->mHomePosY = *(s32*)(c + 0x60);
            self->mHomePosZ = *(s32*)(c + 0x64);
            self->mHomePosY += 0xc8000;
        }
        *(s32*)(c + 0x5c) = *(s32*)(c + 0x68);
        *(s32*)(c + 0x60) = *(s32*)(c + 0x6c);
        *(s32*)(c + 0x64) = *(s32*)(c + 0x70);
        *(u16*)(c + 0x92) = 0;
        *(u16*)(c + 0x100) = 0;
        *(u32*)(c + 0xa8) = 0;
        FlyGuy_ChangeState(self, &data_ov070_021235dc);
        return 1;
    }

    *(V3w*)&tmp = *(V3w*)(player + 0x5c);  /* array-wrapper keeps the ldm/stm block copy under -lang c++ */
    z = 0;
    v.x = z;
    v.y = z;
    v.z = z;
    tmp.y = *(int *)(player + 0x644);
    if (data_0209f2f8 == 0x16)
        tmp.y += 0x32000;
    else
        tmp.y += 0x47000;
    {
        int tx = tmp.x;
        int ty = tmp.y;
        int tz = tmp.z;
        aim.x = tx;
        aim.y = ty;
        aim.z = tz;
    }
    vAngle = Vec3_VertAngle((Vector3*)(c + 0x5c), &aim);
    ApproachAngle((s16*)(c + 0x92), vAngle, 0xa, 0x200, 0x100);

    v.z = 0x11000;
    if (*(s32*)(c + 0x60) <= *(s32*)((char*)&tmp + 4) + 0x5000 ||
        *(s32*)(c + 0x60) <= *(s32*)(player + 0x60) + 0x5000 ||
        Vec3_Dist((Vector3*)(c + 0x5c), &self->mHomePosX) > 0x5dc000) {
        DecIfAbove0_Short((u16*)(c + 0x100));
        v.z = 0x9000;
    }

    half = (*(s16*)(c + 0x94) - self->mTargetAngY) / 2;
    ApproachAngle((s16*)(c + 0x96), half, 0xa, 0x100, 0x50);

    Matrix4x3_FromRotationY(data_020a0e68, *(s16*)(c + 0x8e));
    Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, *(s16*)(c + 0x92));
    MulVec3Mat4x3(&v, data_020a0e68, (Vector3*)(c + 0xa4));

    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211f694
extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int fix, unsigned int j);
int func_ov070_0211f694(daPropeller_Heyho_c *c) {
    c->mStateStep = 0;
    if (c->mHitDuringAttack == 0) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&c->mModelAnim, (void*)((int *)&data_ov070_02123508)[1], 0x40000000, 0x1000, 0);
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211f62c
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern int _ZN9Animation8FinishedEv(void *p);
extern signed char data_0209f2f8;
extern int FlyGuy_ChangeState(daPropeller_Heyho_c *c, daPropeller_Heyho_c::State *p);
extern daPropeller_Heyho_c::State data_ov070_0212359c;

int func_ov070_0211f62c(char *c)
{
    if (_ZN9Animation8FinishedEv(c + 0x350) != 0) {
        if (data_0209f2f8 != 0x16)
            ((daPropeller_Heyho_c *)c)->mHomePosY += 0x12c000;
        ((daPropeller_Heyho_c *)c)->mStateStep = 0;
        ((daPropeller_Heyho_c *)c)->mCooldown = 0x5a;
        FlyGuy_ChangeState((daPropeller_Heyho_c *)c, &data_ov070_0212359c);
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211f5f0
struct BCA_File;
/* (ModelAnim: real header type in scope) */
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int fix, unsigned int j);


/* (data_ov070_02123500: SharedFilePtr view declared earlier in this TU) */

extern "C" int func_ov070_0211f5f0(daPropeller_Heyho_c *c) {
    unsigned int flags = 0;
    BCA_File *file = (BCA_File *)(((int *)&data_ov070_02123500)[1]);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&c->mModelAnim, file, 0x40000000, 0x1000, flags);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol func_ov070_0211f48c
/* recovered: shared common types */
#include "common.h"
char* _ZN8dActor_c13ClosestPlayerEv(void* self);
short Vec3_HorzAngle(void* a, void* b);
/* (ApproachAngle: this file's own int-target view, declared inside the function body) */
int _ZNK9Animation12WillHitFrameEi(void* a, int f);
short Vec3_VertAngle(void* a, void* b);
void* _ZN8dActor_c13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j(void* self, void* pos, void* vel, int a, int b, unsigned int d);
void func_02012694(int a, void* p);
int _ZN9Animation8FinishedEv(void* a);
int FlyGuy_ChangeState(daPropeller_Heyho_c* c, daPropeller_Heyho_c::State* p);
extern daPropeller_Heyho_c::State data_ov070_0212359c;

#define M(p) (p)

int func_ov070_0211f48c(char* c) {
    extern void ApproachAngle(short* p, int target, int a, int b, int limit); /* byte-load-bearing: int target */
    char* pl;
    struct Vector3_16 vel;
    struct Vector3 posbuf;
    struct Vector3 fp;
    struct Vector3 tmp;
    daPropeller_Heyho_c* self = (daPropeller_Heyho_c*)c;

    pl = _ZN8dActor_c13ClosestPlayerEv(c);
    if ((unsigned)(*(int*)(c+0x358) << 4) >> 0x10 >= 0xd)
        goto hitframe;

    if (pl != 0) {
        *(V3w*)&posbuf = *(V3w*)(pl+0x5c);
        tmp.x = posbuf.x;
        tmp.y = posbuf.y;
        tmp.z = posbuf.z;
        self->mTargetAngY = Vec3_HorzAngle(c+0x5c, &tmp);
    }
    ApproachAngle((short*)(c+0x94), self->mTargetAngY, 0xa, 0x400, 0x200);

hitframe:
    if (_ZNK9Animation12WillHitFrameEi(c+0x350, 0xd) != 0) {
        *(V3h*)&vel = *(V3h*)(c+0x8c);
        if (pl != 0) {
            int *base = (int *)(int)M(pl + 0x5c);
            fp.x = base[0];
            fp.y = base[1];
            fp.z = base[2];
            vel.x = Vec3_VertAngle(c+0x5c, &fp);
        }
        _ZN8dActor_c13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j(c, c+0x5c, &vel, 0x1e000, 0xa000, 1);
        func_02012694(0x105, c+0x74);
    }
    if (_ZN9Animation8FinishedEv(c+0x350) != 0) {
        *(int*)(c+0x358) = 0;
        self->mCooldown = 0x5a;
        FlyGuy_ChangeState(self, &data_ov070_0212359c);
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211f450
extern "C" {  /* .c-derived member: C linkage for the whole block */
short func_ov070_0211f450(daPropeller_Heyho_c *c) {
    c->unk_0a4 = 0;
    c->mVertSpeed = 0;
    c->unk_0ac = 0;
    c->mVertSpeed = 0x32000;
    c->mVertAccel = -0x5000;
    c->mStateTimer = 3;
    c->mFlags = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211f368
/* recovered: shared common types */
#include "common.h"
extern "C" int func_ov070_0211f0a4(daPropeller_Heyho_c *c);
typedef int Fix12i;

extern "C" unsigned _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned a, unsigned b, Fix12i c, Fix12i d, Fix12i e, void* f, void* g);
extern "C" unsigned _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(unsigned a, unsigned b, Fix12i c, Fix12i d, Fix12i e, void* f);
extern "C" void ApproachAngle(short* v, short a, int b, int c, int d);
extern "C" void _Z14ApproachLinearRsss(void* v, short a, short b);

extern "C" int func_ov070_0211f368(daPropeller_Heyho_c* c)
{
    if (c->mStateStep != 0) {
        Vector3 v;
        int x, y, z;
        x = c->mPosX;
        z = c->mPosZ;
        y = c->mPosY + 0x50000;
        ((int*)&v)[0] = x;
        ((int*)&v)[1] = y;
        ((int*)&v)[2] = z;
        c->mParticle0 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            c->mParticle0, 0x13a, v.x, v.y, v.z, 0, 0);
        c->mParticle1 = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
            c->mParticle1, 0x13b, v.x, v.y, v.z, 0);
    }
    ApproachAngle(&c->mAngleX, -0x4000, 0xa, 0x200, 0x100);
    _Z14ApproachLinearRsss(&c->mAngleX, -0x4000, 0x200);
    if ((u16)c->mStateTimer == 0)
        func_ov070_0211f0a4(c);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211f100
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

struct BCA_File;

extern daPropeller_Heyho_c::State data_ov070_021235bc;
extern daPropeller_Heyho_c::State data_ov070_021235cc;
/* (data_ov070_02123528: SharedFilePtr view declared earlier in this TU) */

extern "C" {
extern dActor_c* _ZN8dActor_c10FindWithIDEj(u32 id);
extern int FlyGuy_ChangeState(daPropeller_Heyho_c* c, daPropeller_Heyho_c::State* p);
extern int func_ov002_020aea30(void* c, void* p, int a, int b);
extern int _ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(void* c, void* clsn, void* player);
extern void _ZN6Player10SpinBounceE5Fix12IiE(void* p, s32 f);
extern void _ZN12dEnemyBase_c22SpawnMegaCharParticlesER8dActor_cPc(void* enemy, void* actor, char* s);
extern void _ZN6Player16IncMegaKillCountEv(void* p);
extern void func_02012694(int a, void* b);
extern int _ZN6Player9IsOnShellEv(void* p);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, const Vector3* v, u32 a, s32 f, u32 b, u32 c, u32 d);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int fix, unsigned int j);
}

extern "C" void func_ov070_0211f100(daPropeller_Heyho_c* c)
{
    Player* r5;
    s32 r4;

    if (c->mdCcAc_c.otherOwner == 0)
        return;
    r5 = (Player *)_ZN8dActor_c10FindWithIDEj(c->mdCcAc_c.otherOwner);
    if (!r5)
        return;

    r4 = (s32)c->mdCcAc_c.hitFlags;
    if (r4 & 0x40000) {
        c->mStateStep = 1;
        FlyGuy_ChangeState(c, &data_ov070_021235bc);
        return;
    }
    if (r4 & 0x20) {
        c->mDeathState = 1;
        func_ov002_020aea30(c, r5, 0, 1);
        return;
    }
    if (r4 & 0x67c0) {
        c->mStateStep = 0;
        FlyGuy_ChangeState(c, &data_ov070_021235bc);
        return;
    }

    {
        int isBf = (int)(r5->actorID == 0xbf);
        if (!isBf)
            return;
    }
    if (r5->mIsVanish != 0)
        return;

    if (_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(c, &c->mdCcAc_c, r5)) {
        _ZN6Player10SpinBounceE5Fix12IiE(r5, 0x28000);
        c->mDeathState = 1;
        func_ov002_020aea30(c, r5, 0, 1);
        return;
    }

    if (r4 & 0x10) {
        _ZN12dEnemyBase_c22SpawnMegaCharParticlesER8dActor_cPc(c, r5, (char*)0);
        r5->IncMegaKillCount();
        func_02012694(0x1d, &c->mCamSpacePosX);
        FlyGuy_ChangeState(c, &data_ov070_021235bc);
        return;
    }

    if (r5->mIsMetal == 1 || r5->IsOnShell() == 1) {
        c->mStateStep = 0;
        FlyGuy_ChangeState(c, &data_ov070_021235bc);
        return;
    }

    {
        Vector3 v;
        v.x = c->mPosX;
        v.y = c->mPosY;
        v.z = c->mPosZ;
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r5, &v, 2, 0xc000, 1, 0, 1);
    }
    if (c->mHitDuringAttack != 0)
        return;
    if (c->mCurrentState != &data_ov070_021235cc)
        return;

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&c->mModelAnim, (void*)((int *)&data_ov070_02123528)[1], 0x40000000, 0x1000, 0);
    c->mStateStep = 1;
    c->mHitDuringAttack = 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211f0a4
/* recovered: shared common types */
#include "common.h"


/* (dActor_c: real header type in scope; SpawnCoins goes through the mangled extern below) */
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void *, Vector3 const &, unsigned int, int, short);


extern "C" int func_ov070_0211f0a4(daPropeller_Heyho_c *c) {
    c->SmallPoofDust();
    Vector3 pos;
    pos.x = c->mPosX;
    pos.y = c->mPosY;
    pos.z = c->mPosZ;
    unsigned int coins = (unsigned char)c->unk_10a + 1;
    _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(c, pos, coins, 0xa000, 0);
    c->KillAndTrackInDeathTable();
}
