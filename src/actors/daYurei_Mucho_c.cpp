//cpp
/* daYurei_Mucho_c, the YUREI_MUCHO enemy (ov065) -- Snufit.
 * Class identity, layout evidence and matching experiments are recorded in
 * notes/agents/handoffs/daYurei_Mucho_c-ov065.md.
 *
 * deslop
 * Leftover: func_ov065_02115f84..0211696c are this TU's own state/helper
 *   bodies; the ROM symbols are still the func_ov labels. Naming them as
 *   C++ methods would emit _ZN15daYurei_Mucho_c* and miss those labels.
 * Leftover: ModelAnim::SetAnim / dCcAc_c::Init / dBgCh_Actr::Init /
 *   DropShadowRadHeight / KillByInvincibleChar / SpawnCoins / Player::Hurt /
 *   Player::Bounce stay mangled (Fix12-by-value, 6az; dBgCh Init header
 *   Fix12i mangles as int -- this TU's InitResources call).
 * Leftover: Mtx43 / V3A overlays on mModelAnim.mat4x3, mShadowMat, and
 *   player pos copies (Matrix4x3 / Vector3 assignment scalarizes).
 * Leftover: `(int)this + 0x3d8` / `(int)c + 0x3c0` integer-cast forms are
 *   load-bearing in Behavior and func_ov065_0211696c (`&mBobAngle` /
 *   `mShotPosX <<= 3` DIFFs). mStateTimer compares stay unsigned short
 *   (ldrh). SharedFilePtr +4 BCA loads stay raw (layout unrecovered).
 * Leftover: data_ov065_* SharedFilePtr handles (Init LoadFile / Cleanup
 *   Release) and State records (func_ov065_0211691c). S14 no
 *   g_profile_YUREI_MUCHO. pad_3e2 unobserved.
 */

#include "daYurei_Mucho_c.h"
#include "types.h"
#include "common.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "Player.h"

/* POD views preserve the retail aggregate copies. Matrix4x3 and Vector3 are
   available, but their non-POD copies change these two helpers' codegen; see
   the measured alternatives in the handoff. */
typedef struct Mtx43 { int w[12]; } Mtx43;
struct V3A { int w[3]; };

/* func_ov065_02116364 builds a five-Vector3 aggregate on the stack. Leaving it
   an unnamed local class works, but mwccarm then mangles its implicit
   destructor with a file-and-counter tag (_ZN29@class$NNNdaYurei_Mucho_c_cppD1Ev)
   that moves whenever this file does, so the manifest row licensing it would
   not survive the move out of src_tu/. Naming it at file scope pins the
   symbol; the emitted bytes are unchanged. */
struct V3Quint {
    Vector3 pp;
    Vector3 spv;
    Vector3 sout;
    Vector3 d;
    Vector3 tgt;
};

/* ABI declarations still used by the unreconstructed helpers. */
extern "C" {

/* arm9 -- 6az / unnamed helpers. Method forms of the Fix12-by-value
   group size-DIFF; see the deslop leftover list. */
extern int _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void *self, const Vector3 *pos,
                                                          unsigned int n, int f, short s);
extern void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int profile,
                                                             unsigned int param,
                                                             const void *pos, const void *rot,
                                                             int area, short id);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *self, void *shadow, void *mtx, int rad, int height, unsigned int flags);
extern void _ZN6Player6BounceE5Fix12IiE(void *p, int f);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *p, const void *v, unsigned int a,
                                                    int b, unsigned int d, unsigned int e,
                                                    unsigned int f);
extern int _ZN9Animation8FinishedEv(void *anim);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca, int a, int fix,
                                                        unsigned int j);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, dActor_c *a, int r, int h,
                                                      unsigned int e, unsigned int g);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void *self, dActor_c *a, int r, int h, Vector3_16 *p, Vector3_16 *q);
extern unsigned int RandomIntInternal(void *seed);
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void ApproachAngle(void *p, short target, int a, int b, int limit);
extern void _Z14ApproachLinearRsss(short *p, short target, short step);
extern void _Z14ApproachLinearRiii(int *x, int target, int step);
extern short Vec3_HorzAngle(const void *a, const void *b);
extern short Vec3_VertAngle(const void *a, const void *b);
extern int Vec3_Dist(const void *a, const void *b);
extern void Vec3_Asr(void *d, const void *s, int sh);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, short ax);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
extern void MulVec3Mat4x3(const void *in, const void *m, void *out);
extern void MulMat4x3Mat4x3(const int *a, const int *b, int *dst);
extern int func_02012694(int id, void *v);
extern int data_020a0e68[];
extern int data_0209e650[];
extern short data_02082214[];

/* ov002 -- 6az KillByInvincibleChar; unnamed particle helper. */
extern void _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(
    void *self, const void *v, void *p, int a);
extern void func_ov002_020aea30(void *self, void *p, int a, int b);

/* The four shared-file handles and four State records remain ROM-supplied BSS.
   __sinit_ov065_0211c110 initializes the handles and copies the eight PMF
   constants into the State records. This text-only TU does not own that storage
   or initializer; SharedFilePtr's complete layout is still unrecovered. */
extern SharedFilePtr data_ov065_0211d600;
extern SharedFilePtr data_ov065_0211d608;
extern SharedFilePtr data_ov065_0211d610;
extern SharedFilePtr data_ov065_0211d618;
/* These State objects remain ROM-supplied storage. Existing declarations in
   decl_common.h use raw arrays/words, so uses retain their local State casts. */
extern char data_ov065_0211d670[];

/* ov065 -- this TU's own free helpers, forward-declared because mwccarm emits
   .text in reverse source order and every caller is written above its callee. */
extern int func_ov065_02115f84(daYurei_Mucho_c *c);
extern void func_ov065_02115ff0(char *c);
extern int func_ov065_0211691c(daYurei_Mucho_c *c, daYurei_Mucho_c::State *s);
extern void func_ov065_0211696c(char *c);

}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daYurei_Mucho_c_classInit
/* The registry factory behind the YUREI_MUCHO / SNUFIT profile. `return new
   daYurei_Mucho_c()` MATCHES (size 0x50); the synthesized ctor stores
   `_ZTV15daYurei_Mucho_c + 2`. classInit is a reconstructed source-style
   name, not a preserved identifier. */
extern "C" daYurei_Mucho_c *daYurei_Mucho_c_classInit(void)
{
    return new daYurei_Mucho_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c13OnYoshiTryEatEv
/* dActor_c vtable slot 18. */
s32 daYurei_Mucho_c::OnYoshiTryEat()
{
    return 4;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c13OnTurnIntoEggER6Player
/* dActor_c vtable slot 19, confirmed by address: _ZTV15daYurei_Mucho_c
 * (0x0211cba4) + 0x4c -> 0x02116f14.
 *
 * The `R6Player` reference spelling in the mangled name is a coined guess: a
 * reference and a pointer generate identical ARM for this body, so the bytes
 * cannot distinguish them. */
void daYurei_Mucho_c::OnTurnIntoEgg(Player &player)
{
    GivePlayerCoins(player, (unsigned char)(unk_10a + 1), 0);
    KillAndTrackInDeathTable();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c16OnAimedAtWithEggEv
/* dActor_c vtable slot 29. */
s32 daYurei_Mucho_c::OnAimedAtWithEgg()
{
    return 0;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c13InitResourcesEv
/* Both Init methods are declared. The current dCcAc_c fixed-point aggregate
   arguments add 16 bytes here; dBgCh_Actr's scalar declaration emits a name
   absent from symbols.txt. Retain these ABI calls pending interface repair. */
int daYurei_Mucho_c::InitResources()
{
    mModelAnim.SetFile((BMD_File *)Model::LoadFile(data_ov065_0211d618), 1, -1);
    Model::LoadFile(data_ov065_0211d610);
    mShadowModel.InitCylinder();
    Animation::LoadFile(data_ov065_0211d600);
    Animation::LoadFile(data_ov065_0211d608);
    mTerminalVelocity = -0x1e000;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x38000, 0x7e000, 0x200000, 0x7eff0);
    mAngleY = mPrevAngleY;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x46000, 0, 0, 0);
    unk_108 = 1;
    unk_10a = 1;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mModelAnim.speed = 0x1000;
    func_ov065_0211691c(this, (State *)data_ov065_0211d670);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c8BehaviorEv
/* dActor_c vtable slot 6. */
int daYurei_Mucho_c::Behavior()
{
    if (UpdateYoshiEat(mWithMeshClsn) != 0) {
        mdCcAc_c.Clear();
        if (mEatenByYoshi != 0) {
            if (unk_104 == 0) {
                mdCcAc_c.Update();
            }
        }
        func_ov065_0211696c((char *)this);
        mHomePosX = mPosX;
        mHomePosY = mPosY;
        mHomePosZ = mPosZ;
        func_ov065_0211691c(this, (State *)data_ov065_0211d670);
        return 1;
    }
    if (UpdateKillByInvincibleChar(mWithMeshClsn, mModelAnim, 3) != 0) {
        return 1;
    }
    if (mDeathState != 0) {
        ApproachAngle(&mAngleX, -0x4000, 0xa, 0x200, 0x100);
        UpdateDeath(mWithMeshClsn);
        func_ov065_0211696c((char *)this);
        return 1;
    }
    DecIfAbove0_Short((unsigned short *)&mStateTimer);
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
        int fallSpeed = mVertSpeed + mVertAccel;
        int clamped = mTerminalVelocity;
        if (fallSpeed >= clamped) {
            clamped = fallSpeed;
        }
        int keep = unk_0ac;
        mVertSpeed = clamped;
        unk_0ac = keep;
    }
    if (mCurrentState != (State *)data_ov065_0211d650) {
        int *pAngle;
        int ang;
        int idx;
        short tbl;
        int result;
        /* The add sits INSIDE the integer cast, which is load-bearing here:
           `(int)this + 0x3d8` is not interchangeable with `&mBobAngle`. */
        pAngle = (int *)(((int)this + 0x3d8));
        *pAngle += 0x200;
        ang = mBobAngle;
        /* The shift must be LOGICAL so the angle wraps -- writing it on the
           signed s16 would read the wrong table entry for negative angles. */
        idx = ((unsigned short)(short)ang >> 4) * 2;
        tbl = data_02082214[idx];
        result = (int)(((long long)tbl * 0x46000 + 0x800) >> 12);
        _Z14ApproachLinearRiii(&mPosY, mHomePosY + (result + 0xb4000), 0x3000);
    }
    UpdatePosWithOnlySpeed(&mdCcAc_c);
    UpdateWMClsn(mWithMeshClsn, 0);
    func_ov065_0211696c((char *)this);
    if (mCurrentState != (State *)data_ov065_0211d660) {
        mAngleX = mPrevAngleX;
        mAngleY = mPrevAngleY;
        mAngleZ = mPrevAngleZ;
        func_ov065_02115ff0((char *)this);
    }
    mdCcAc_c.Clear();
    {
        Player *p = ClosestPlayer();
        if (p != 0 && p->mIsVanish == 0) {
            mdCcAc_c.Update();
        }
    }
    mModelAnim.Advance();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c6RenderEv
/* dActor_c vtable slot 9. */
int daYurei_Mucho_c::Render()
{
    int b = ((mFlags & 0x40000) != 0);
    if (b) return 1;
    mModelAnim.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c16OnPendingDestroyEv
/* fBase_c slot 12. Empty in the ROM: four bytes, `bx lr`. */
void daYurei_Mucho_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c16CleanupResourcesEv
/* dActor_c vtable slot 3. Releases the four files InitResources claimed.
 *
 * The body does not read the incoming object pointer or access instance
 * fields. Spelling it as the class method preserves the ROM bytes. */
int daYurei_Mucho_c::CleanupResources()
{
    data_ov065_0211d610.Release();
    data_ov065_0211d618.Release();
    data_ov065_0211d600.Release();
    data_ov065_0211d608.Release();
    return 1;
}

/* ========================================================================== */
/* The twelve retained free helpers use C linkage. Class member functions    */
/* retain C++ linkage, including when defined inside a linkage block.          */
/* ========================================================================== */
extern "C" {

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_0211696c
/* Rebuilds the model matrix from the actor's position and Z/X/Y angles, then
   drops the shadow. Called three times from Behavior. */
void func_ov065_0211696c(char *self)
{
    daYurei_Mucho_c *c = (daYurei_Mucho_c *)self;
    int v[3];

    Vec3_Asr(v, &c->mPosX, 3);
    Matrix4x3_FromTranslation(data_020a0e68, v[0], v[1], v[2]);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(data_020a0e68,
        c->mAngleX, c->mAngleY, c->mAngleZ);

    *(Mtx43 *)&c->mModelAnim.mat4x3 = *(Mtx43 *)data_020a0e68;
    c->mShotPosX = 0;
    c->mShotPosY = 0;
    c->mShotPosZ = 0;
    *(Mtx43 *)data_020a0e68 = *(Mtx43 *)&c->mModelAnim.mat4x3;

    MulMat4x3Mat4x3(reinterpret_cast<const int *>(
                        (char *)c->mModelAnim.data.transforms + 0xc0),
                     data_020a0e68, data_020a0e68);

    c->mShotPosX = data_020a0e68[9];
    c->mShotPosY = data_020a0e68[10];
    c->mShotPosZ = data_020a0e68[11];
    /* Integer-cast <<= is load-bearing: `mShotPosX <<= 3` DIFFs. */
    *(int *)(((int)c + 0x3c0)) <<= 3;
    *(int *)(((int)c + 0x3c4)) <<= 3;
    *(int *)(((int)c + 0x3c4)) -= 0xa000;
    *(int *)(((int)c + 0x3c8)) <<= 3;

    Matrix4x3_FromTranslation(data_020a0e68,
        c->mPosX >> 3,
        (c->mPosY - 0x18000) >> 3,
        c->mPosZ >> 3);

    *(Mtx43 *)c->mShadowMat = *(Mtx43 *)data_020a0e68;

    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, &c->mShadowModel, c->mShadowMat, 0x64000, 0x258000, 0xf);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_0211691c
/* The state setter. Stores the State into mCurrentState, RE-READS it -- the ROM
   really does reload the field it has just written -- null-tests the entry hook
   at +0x00 and calls it through the object. Called eight times inside this run.

   The pointer-to-member call is the ROM's own `ldr r2,[r3]; cmp r2,#0;
   ldr r1,[r3,#4]; add r0,r0,r1,asr #1; ands r1,r1,#1; ldrne r1,[r0];
   ldrne r1,[r1,r2]; ldreq r1,[r3]; blx r1` sequence, i.e. the Itanium
   {ptr_or_vtable_offset, adj*2|isVirtual} encoding. All eight records in
   ov065's .data carry adj word 0: non-virtual, no this-adjustment. */
int func_ov065_0211691c(daYurei_Mucho_c *c, daYurei_Mucho_c::State *s)
{
    c->mCurrentState = s;
    daYurei_Mucho_c::State *q = c->mCurrentState;
    if (q->mEnter == 0) {
        return 1;
    }
    return (c->*(q->mEnter))();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_021168a8
/* State entry hook at 0x0211d670 +0x00: randomise the facing angle and the
   timer, then start the wait animation. The BCA file is read straight out of
   the SharedFilePtr's second word -- the ROM does `ldr r1,[r0,#4]` off the
   literal at 0x02116918, not a call, so the raw read is what reproduces. */
int func_ov065_021168a8(daYurei_Mucho_c *c)
{
    c->mTargetAngle = (short)((RandomIntInternal(data_0209e650) >> 8) << 0xc);
    /* unsigned/signed timer store: a named s16 write DIFFs the ldrh sites. */
    *(short *)((char *)c + 0x100) = (short)(((RandomIntInternal(data_0209e650) >> 8) & 0x1f) + 0x32);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&c->mModelAnim,
        ((void **)&data_ov065_0211d600)[1], 0, 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_02116744
/* State main hook at 0x0211d670 +0x08: drift, and switch to the attack state
   when a player comes close enough. */
int func_ov065_02116744(daYurei_Mucho_c *c)
{
    int in[3];
    int dp[3];
    Player *pl;
    int *pos;

    in[0] = 0;
    in[1] = 0;
    in[2] = 0;

    if (Vec3_Dist(&c->mPosX, &c->mHomePosX) > 0x1f4000 ||
        c->mWithMeshClsn.IsOnWall() != 0) {
        c->mTargetAngle = Vec3_HorzAngle(&c->mPosX, &c->mHomePosX);
        if (*(unsigned short *)((char *)c + 0x100) < 0x14)
            *(unsigned short *)((char *)c + 0x100) = 0x14;
    }

    ApproachAngle(&c->mPrevAngleY, c->mTargetAngle, 0xa, 0x200, 0x100);
    ApproachAngle(&c->mPrevAngleX, 0, 1, 0x500, 0x500);

    in[2] = 0xa000;
    Matrix4x3_FromRotationY(data_020a0e68, c->mAngleY);
    MulVec3Mat4x3(in, data_020a0e68, &c->unk_0a4);

    if (*(unsigned short *)((char *)c + 0x100) == 0) {
        func_ov065_0211691c(c, (daYurei_Mucho_c::State *)data_ov065_0211d670);
        return 1;
    }

    pl = c->ClosestNonVanishPlayer();
    if (pl != 0) {
        pos = (int *)(((int)pl + 0x5c));
        dp[0] = pos[0];
        dp[1] = pos[1];
        dp[2] = pos[2];

        if (Vec3_Dist(&c->mPosX, dp) < 0x3e8000) {
            *(unsigned short *)((char *)c + 0x100) = 0x14;
            func_ov065_0211691c(c, (daYurei_Mucho_c::State *)&data_ov065_0211d680);
        }
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_0211672c
/* State entry hook at 0x0211d680 +0x00: stop the actor dead. */
int func_ov065_0211672c(daYurei_Mucho_c *p)
{
    p->unk_0a4 = 0;
    p->mVertSpeed = 0;
    p->unk_0ac = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_021165d8
/* State main hook at 0x0211d680 +0x08: steer toward the nearest non-vanishing
   player.

   dActor_c::ClosestNonVanishPlayer takes `this` in r0; the ROM emits no `mov`
   before the `bl` at 0x021165e8 because r0 still holds the incoming object. */
int func_ov065_021165d8(daYurei_Mucho_c *c)
{
    short r4 = 0;
    Player *p = c->ClosestNonVanishPlayer();
    if (p != 0) {
        Vector3 tmp = *(Vector3 *)&p->mPosX;
        Vector3 v;
        Vector3 a;
        a.x = tmp.x;
        a.y = tmp.y;
        a.z = tmp.z;
        c->mTargetAngle = Vec3_HorzAngle(&c->mPosX, &a);
        Vector3 b;
        b.x = tmp.x;
        b.y = tmp.y;
        b.z = tmp.z;
        r4 = Vec3_VertAngle(&c->mPosX, &b);
        if (Vec3_Dist(&c->mPosX, &tmp) >= 0x1f4000) {
            c->unk_0a4 = 0;
            c->mVertSpeed = 0;
            c->unk_0ac = 0;
        } else {
            v.z = 0;
            v.z = -0x1000;
            v.x = 0;
            v.y = 0;
            Matrix4x3_FromRotationY(data_020a0e68, c->mAngleY);
            MulVec3Mat4x3(&v, data_020a0e68, &c->unk_0a4);
        }
    } else {
        *(short *)((char *)c + 0x100) = r4;
    }
    ApproachAngle(&c->mPrevAngleY, c->mTargetAngle, 1, 0x500, 0x500);
    ApproachAngle(&c->mPrevAngleX, r4, 1, 0x500, 0x500);
    if (*(unsigned short *)((char *)c + 0x100) == 0)
        func_ov065_0211691c(c, (daYurei_Mucho_c::State *)data_ov065_0211d650);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_02116588
/* State entry hook at 0x0211d650 +0x00: start the attack animation. As at
   ordinal 11, the BCA file is the SharedFilePtr's second word, read directly
   (`ldr r1,[r0,#4]` at 0x021165a4 off the literal at 0x021165d4). */
short func_ov065_02116588(daYurei_Mucho_c *c)
{
    c->mShotCount = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&c->mModelAnim,
        ((void **)&data_ov065_0211d608)[1], 0x40000000, 0x1000, 0);
    *(short *)((char *)c + 0x100) = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_02116364
/* State main hook at 0x0211d650 +0x08: aim, and spawn the projectile (profile
   0xe9) up to three times before returning to a wait state. */
int func_ov065_02116364(daYurei_Mucho_c *c)
{
    V3Quint L;
    Player *pl = c->ClosestNonVanishPlayer();
    if (pl != 0) {
        *(V3A *)(int)(&L.pp) = *(V3A *)(int)(&pl->mPosX);
        L.tgt = L.pp;
        c->mTargetAngle = Vec3_HorzAngle(&c->mPosX, &L.tgt);
        ApproachAngle(&c->mPrevAngleY, c->mTargetAngle, 1, 0x500, 0x500);

        if (((*(u32 *)((char *)c + 0x358) << 4) >> 16) >= 0xf
            && *(u16 *)((char *)c + 0x100) == 0
            && c->mShotCount < 3) {
            void *spawned = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                0xe9, 1, &c->mShotPosX, 0, c->mAreaId, -1);
            if (spawned != 0) {
                u8 *sp2 = (u8 *)spawned;
                func_02012694(0xfb, &c->mCamSpacePosX);
                L.spv.x = 0;
                L.spv.y = 0;
                L.spv.z = 0x1e000;
                L.sout.x = 0;
                L.sout.y = 0;
                L.sout.z = 0;
                Matrix4x3_FromRotationY(&data_020a0e68, c->mAngleY);
                Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, c->mAngleX);
                MulVec3Mat4x3(&L.spv, &data_020a0e68, &L.sout);
                *(s32 *)(sp2 + 0xa4) = L.sout.x;
                *(s32 *)(sp2 + 0xa8) = L.sout.y;
                *(s32 *)(sp2 + 0xac) = L.sout.z;
                c->mShotCount += 1;
                *(u16 *)((char *)c + 0x100) = 4;
            }
        }
    }

    if (_ZN9Animation8FinishedEv((void *)((char *)c + 0x350)) != 0) {
        if (pl != 0) {
            s32 *dsrc = (s32 *)(int)(&pl->mPosX);
            L.d.x = dsrc[0];
            L.d.y = dsrc[1];
            L.d.z = dsrc[2];
            if (Vec3_Dist(&c->mPosX, &L.d) > 0x3e8000) {
                *(u16 *)((char *)c + 0x100) = 0;
                func_ov065_0211691c(c, (daYurei_Mucho_c::State *)data_ov065_0211d670);
            } else {
                *(u16 *)((char *)c + 0x100) = 0x32;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&c->mModelAnim,
                    ((void **)&data_ov065_0211d600)[1], 0, 0x1000, 0);
                func_ov065_0211691c(c, (daYurei_Mucho_c::State *)&data_ov065_0211d680);
            }
        } else {
            *(u16 *)((char *)c + 0x100) = 0;
            func_ov065_0211691c(c, (daYurei_Mucho_c::State *)data_ov065_0211d670);
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_02116328
/* State entry hook at 0x0211d660 +0x00: the bumped-from-below pop. */
int func_ov065_02116328(daYurei_Mucho_c *c)
{
    c->unk_0a4 = 0;
    c->mVertSpeed = 0;
    c->unk_0ac = 0;
    c->mVertSpeed = 0x32000;
    c->mVertAccel = -0x5000;
    *(short *)((char *)c + 0x100) = 0xa;
    c->mFlags = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_021162c0
/* State main hook at 0x0211d660 +0x08.
 *
 * CORRECTED HERE. The retired shard func_ov065_021162c0.c called `_ZN6EyerokD0Ev`, which is
 * ov066's name for 0x02115f84. This module's own name for that address is
 * func_ov065_02115f84, the death helper two ordinals below, and ov065 is the
 * module this branch links in. match.py wildcards every relocated word, so the
 * wrong callee still reproduced the bytes; the ROM's own `bl 0x02115f84` at
 * 0x02116314 decides it. */
int func_ov065_021162c0(daYurei_Mucho_c *t)
{
    ApproachAngle(&t->mAngleX, -0x4000, 0xa, 0x200, 0x100);
    _Z14ApproachLinearRsss(&t->mAngleX, -0x4000, 0x200);
    if (*(unsigned short *)((char *)t + 0x100) == 0)
        func_ov065_02115f84(t);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_02115ff0
/* The collision response, called once from Behavior. */
void func_ov065_02115ff0(char *self)
{
    daYurei_Mucho_c *c = (daYurei_Mucho_c *)self;
    short v[3];
    int hv[3];
    Player *p;
    int flags;
    unsigned int id;

    id = c->mdCcAc_c.otherOwner;
    if (id == 0) return;
    p = (Player *)dActor_c::FindWithID(id);
    if (p == 0) return;
    flags = (int)c->mdCcAc_c.hitFlags;

    if (flags & 0x40000) {
        c->mDeathState = 4;
        func_ov002_020aea30(c, p, 0, 4);
        func_02012694(0x11e, &c->mCamSpacePosX);
        return;
    }
    if (flags & 0x2400) {
        c->mDeathState = 2;
        func_ov002_020aea30(c, p, 0, 2);
        func_02012694(0x11e, &c->mCamSpacePosX);
        return;
    }
    if (flags & 0x4380) {
        c->mDeathState = 3;
        func_ov002_020aea30(c, p, 0, 3);
        func_02012694(0x11e, &c->mCamSpacePosX);
        return;
    }
    if (c->BumpedUnderneathByPlayer(*p) == 1) {
        func_02012694(0x11e, &c->mCamSpacePosX);
        func_ov065_0211691c(c, (daYurei_Mucho_c::State *)data_ov065_0211d660);
        return;
    }
    if (flags & 0x10) {
        v[0] = -0x2000;
        v[1] = 0;
        v[2] = 0;
        _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(c, v, p, 0);
        func_02012694(0x11e, &c->mCamSpacePosX);
        return;
    }
    if (flags & 0x40) {
        c->mDeathState = 2;
        func_ov002_020aea30(c, p, 0, 2);
        func_02012694(0x11e, &c->mCamSpacePosX);
        return;
    }
    if (flags & 0x20) {
        c->mDeathState = 1;
        func_ov002_020aea30(c, p, 0, 1);
        func_02012694(0x11e, &c->mCamSpacePosX);
        return;
    }
    {
        int b = (int)(((long long)(p->actorID == 0xbf)));
        if (b == 0) return;
    }
    if (p->mIsMetal == 1 || p->IsOnShell() == 1) {
        func_ov065_02115f84(c);
        return;
    }
    if (c->JumpedOnByPlayer(c->mdCcAc_c, *p)) {
        _ZN6Player6BounceE5Fix12IiE(p, 0x28000);
        c->mDeathState = 1;
        func_ov002_020aea30(c, p, 0, 1);
        func_02012694(0x11e, &c->mCamSpacePosX);
        return;
    }
    hv[0] = c->mPosX;
    hv[1] = c->mPosY;
    hv[2] = c->mPosZ;
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(p, hv, 2, 0xc000, 1, 0, 1);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_02115f84
/* The death helper: poof, drop the coins, unregister. */
int func_ov065_02115f84(daYurei_Mucho_c *c)
{
    Vector3 v;
    c->SmallPoofDust();
    v.x = c->mPosX;
    v.y = c->mPosY;
    v.z = c->mPosZ;
    _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(c, &v, c->unk_10a + 1, 0xa000, 0);
    c->KillAndTrackInDeathTable();
    return func_02012694(0x11e, &c->mCamSpacePosX);
}

}  /* extern "C" */

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* No separate body lives here. The inline virtual destructor in the directly
 * included class header makes mwccarm emit retail's D1 then D0 pair without
 * the otherwise homeless D2 variant an out-of-line definition produces.
 *
 * D1 stores the vptr, then destroys the ShadowModel at 0x364, the ModelAnim at
 * 0x300, the dBgCh_Actr at 0x144 and the dCcAc_c at 0x110 in reverse
 * declaration order, and tails into ov002 _ZN12dEnemyBase_cD2Ev. D0 repeats
 * that body verbatim -- it does NOT call D1 -- and then hands the object back
 * to the game heap. All of that is a consequence of the class declaration;
 * none of it is written out. */
