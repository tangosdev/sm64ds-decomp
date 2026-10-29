//cpp
struct Vector3_16f;
namespace Particle {
struct Callback;
struct System {
    static int New(unsigned, unsigned, int, int, int, const ::Vector3_16f *, Callback *);
    static void NewSimple(unsigned int, int, int, int);
    static unsigned NewUnkCallback818(unsigned, unsigned, int, int, int, const ::Vector3_16f *);
};
}
/**
 * Shared base for enemies and other actors that use the enemy collision,
 * death and Yoshi-eat handling.
 *
 * IsGoingOffCliff probes straight down from the actor and stores the
 * result in mCliffState. AngleAwayFromWallOrCliff turns the actor when
 * that probe says the floor is gone or too steep.
 *
 * The constructor and destructor bodies are empty. The compiler supplies
 * the base call and the vptr store (see the constructor note). Source
 * order is the reverse of the ROM because codegen is deferred.
 * decl_common.h is left out: its helper spellings disagree with this file.
 *
 * The out-of-line destructor is the class's key function. It emits D2, D0
 * and D1 in cartridge order (0x020aed18, 0x020aed3c, 0x020aed74), so this
 * object carries _ZTV/_ZTI/_ZTS12dEnemyBase_c and the inherited RTTI. The
 * manifest's compiler_only_output licenses them.
 *
 * This TU also owns the module's second static initializer: the two
 * non-const PMF tables at the end of the file make mwcc emit
 * __sinit_d_enemy_base.cpp, byte-identical to retail __sinit_ov002_02100938
 * (0x1a4, eighteen relocations), plus the .ctor word and the sixteen
 * pointer-to-member descriptors. See notes/sinit-ownership-pilot-ov002.md.
 *
 * The name is the cartridge's: ov002 holds _ZTI12dEnemyBase_c/
 * _ZTS12dEnemyBase_c at 0x021081c0/0x021081cc, and da1up_c's
 * __si_class_type_info reaches them through its +8 word.
 *
 * leftovers:
 *   - Aea30C and EnemyDeathC are derived stand-in receivers for the two
 *     state tables (eight start handlers dispatched by func_ov002_020aea30,
 *     five per-frame handlers dispatched by UpdateDeath). They belong on
 *     dEnemyBase_c, but a member declaration in dEnemyBase_c.h renumbers the
 *     @NNN temporaries of every promoted TU that includes it, so the move
 *     waits for those TUs to stop recording per-TU numbers.
 *   - Each named local draws one @NNN number, so dropping one renumbers this
 *     TU's pointer-to-member records (measured). The manifest's
 *     defined_symbol_renames rows pair the numbers mwcc emits now with the
 *     pinned names; a change that adds or drops a named local has to shift
 *     their `from` side to match.
 *   - func_ov002_020ae5c8 stays a free `extern "C"` callee -- callers pass
 *     its unused second argument; the one-argument form comes out 0x50
 *     against the ROM's 0x5c (measured note in place).
 *   - Computed-spelling externs stay where a member form changes codegen:
 *     the Particle::System wrappers take Fix12<int> by value (runbook
 *     section 7). dActor_c::ReflectAngle is a member call since batch 5.
 *     cstd::atan2 is the scalar call.
 *   - func_ov002_020ae4cc passes its particle position as
 *     `*(int *)((char *)v + N)`; indexing v[] directly comes out 0xd4 against
 *     the ROM's 0xfc (measured). SpawnParticlesIfHitOtherObj keeps its
 *     coin tests as nested int compares: neither bool nor a single `&&`
 *     matches (the `&&` form is 0xa0 against 0xb8).
 *   - Address-derived member names stand until the handlers' semantics are
 *     recovered.
 */
#include "dEnemyBase_c.h"
#include "nitro/fx/fx.h"
#include "decl_dBgPi.h"
#include "dBgCh_Lin.h"
#include "Player.h"
#include "SurfaceInfo.h"
#include "dBgCh_Actr.h"
#include "Sound.h"
#include "cstd.h"

/* Written as the real constructor, which is the form the legacy shard recovered
   and byte-matched. Two steps, and neither is spelled below: the dActor_c
   base-subobject call, then this class's own vptr store. `dEnemyBase_c : dActor_c`
   supplies the first and `virtual ~dEnemyBase_c()` the second, in the order mwcc
   always uses (notes/ctor-migration.md sec 6: base calls -> derived vptr store ->
   members -> body).

   The base call is a real `bl` only because include/dActor_c.h DECLARES dActor_c()
   and never defines it; left undeclared the compiler would synthesise and inline
   the base construction here and the candidate would land short. That is
   notes/ctor-migration.md sec 2, and it is why dEnemyBase_c.h declares
   dEnemyBase_c() the same way for the classes below it.

   One definition emits C1 AND C2 (sec 1). There is no C1 file and no C1 in the ROM
   -- nothing constructs a standalone dEnemyBase_c, so the linker never kept one;
   the manifest licenses the C1 this TU emits as compiler-only output. */
// @symbol _ZN12dEnemyBase_cC2Ev
dEnemyBase_c::dEnemyBase_c()
{
}

/* One definition, three emitted sections: D2 0x020aed18, D0 0x020aed3c, D1 0x020aed74. */
// @symbol _ZN12dEnemyBase_cD2Ev
// @symbol _ZN12dEnemyBase_cD0Ev
// @symbol _ZN12dEnemyBase_cD1Ev
/* Store this class's vtable over the one dActor_c's constructor left, then run the
   dActor_c subobject destructor. D0 additionally returns the object to the actor
   heap through dEnemyBase_c's own inline operator delete (see include/dEnemyBase_c.h). The
   three legacy files each spelled one variant; the compiler emits all three from
   this. */
dEnemyBase_c::~dEnemyBase_c()
{
}

/* Runs the mesh collision (sel picks which of four update flavours), then caches
   whichever surface normals came back: the floor result's into mFloorNormal*,
   the wall result's into mWallNormal*. Those two CopyNormalTo calls are what
   evidence both triples as Vector3s.

   On a floor that does not limit movement, the vertical velocity is re-derived
   so the motion stays in the floor plane. */
// @symbol _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj
/* local extern: func_02038414, the tail-call veneer to
   dBgCh_Actr::UpdateDiscreteNoLava_2, is the one veneer dBgCh_Actr.h does
   not declare -- decl_common.h already spells it with a void *. */
extern "C" {
extern void func_02038414(dBgCh_Actr *);
}

void dEnemyBase_c::UpdateWMClsn(dBgCh_Actr &clsn, unsigned int sel) {
    switch (sel) {
    case 1:
        func_020383f0(&clsn);
        break;
    case 2:
        dBgCh_Actr_UpdateDiscreteNoLava_veneer(&clsn);
        break;
    case 3:
        func_02038414(&clsn);
        break;
    default:
        dBgCh_Actr_UpdateContinuous_Veneer(&clsn);
        break;
    }
    if (clsn.IsOnGround()) {
        clsn.GetFloorResult()->surface.CopyNormalTo(*(Vector3 *)&mFloorNormalX);
        if (clsn.GetLimMovFlag() == 0) {
            int dz = mFloorNormalY;
            if (dz != 0) {
                int nx = mFloorNormalX;
                int vx = mSpeedX;
                int nz = mFloorNormalZ;
                int vz = mSpeedZ;
                int a = FX_MUL(nx, vx);
                int b = FX_MUL(nz, vz);
                int num = a + b;
                int q = cstd::fdiv(num, dz);
                mVertSpeed = -(q + 0x8000);
            }
        }
    }
    if (clsn.IsOnWall()) {
        clsn.GetWallResult()->surface.CopyNormalTo(*(Vector3 *)&mWallNormalX);
    }
}

/* Its legacy file built this from a local `struct dEnemyBase_c : dActor_c` shadow with a
   local POD `struct Vector3`; both are the real ones now. Measured: using
   types.h's Vector3 -- which declares ~Vector3(){} and is therefore NOT a POD --
   for the local `v` costs nothing here, all 0x13c bytes reproduce, which is what
   lets dActor_c::Spawn take it by const reference as its real signature says. */
extern "C" int RandomIntInternal(int *seed);
extern u16 data_ov002_020ff014[]; /* coin actor IDs: 0x120, 0x121, 0x122 */
extern int data_0209e650;

// @symbol _ZN12dEnemyBase_c9SpawnCoinEv
void dEnemyBase_c::SpawnCoin()
{
    int i;
    Vector3 v;
    if (mCoinType != 0) {
        int tz = mPosZ;
        int ty = mPosY + 0x78000;
        int tx = mPosX;
        v.x = tx;
        v.y = ty;
        v.z = tz;
        if (mCoinType >= 4)
            mCoinType = 1;
        for (i = 0; i < mExtraCoinCount + 1; i++) {
            dActor_c *coin =
                dActor_c::Spawn(data_ov002_020ff014[mCoinType - 1], 0xf2, v, 0, mAreaId, -1);
            if (coin != 0) {
                int rnd1 = RandomIntInternal(&data_0209e650);
                int rnd2 = RandomIntInternal(&data_0209e650);
                int a = (int)((u32)rnd1 >> 16 << 27) >> 16;
                u32 b = (u32)rnd2 >> 16;
                coin->mPrevAngleX = 0;
                coin->mPrevAngleY = (s16)(a * i);
                coin->mPrevAngleZ = 0;
                coin->mHorzSpeed = (((b % 50) + 100) << 15) / 100;
            }
        }
    }
    PoofDust();
}

// @symbol func_ov002_020aea30
/* Start the selected enemy death sequence. This reconstructed interface uses
   receiver, attacker and nullable collision pointers, matching all callers.
   The handler table forwards the latter two registers; current handlers use
   the attacker and ignore collision. The exact original prototype is unknown.
   Incoming r3 is overwritten before use. Prior fourth-argument claims confused
   death-state stores with argument setup; three-argument compiler probes match.
   ov004 has a different function at this address and remains separately named.

   If mDeathState is set: clears mFlags bit 0x10000000, zeroes mDeathTimer,
   invokes the pointer-to-member-function from data_ov002_0210db80[state-1]
   (forwarding both pointer args), then sets mVertAccel to -0x2000 and clears
   the mFlags bit again.

   The eight targets are declared on `Aea30C`, a derived stand-in receiver like
   EnemyDeathC below and for the same reason: they are dEnemyBase_c methods, but
   a member declaration in dEnemyBase_c.h renumbers @NNN in its consumers. It
   used to be a non-polymorphic shadow struct of padN/fNN fields; deriving it
   from dEnemyBase_c measured byte-identical, PMF calls included. */
struct Aea30C;
typedef void (Aea30C::*Aea30PMF)(dActor_c*, dBgCh_Actr*);
extern Aea30PMF data_ov002_0210db80[];
struct Aea30C : dEnemyBase_c {
    /* The eight state-table targets. Member spellings are what let the table
     below be written as real pointer-to-member initializers. */
    void func_ov002_020ae9f8(dActor_c *, dBgCh_Actr *);
    void func_ov002_020ae954(dActor_c *, dBgCh_Actr *);
    void func_ov002_020ae890(dActor_c *, dBgCh_Actr *);
    void func_ov002_020ae87c(dActor_c *, dBgCh_Actr *);
    void func_ov002_020ae844(dActor_c *, dBgCh_Actr *);
    void func_ov002_020ae80c(dActor_c *, dBgCh_Actr *);
    void func_ov002_020ae73c(dActor_c *, dBgCh_Actr *);
    void func_ov002_020aea2c(dActor_c *, dBgCh_Actr *);
};
extern "C" void func_ov002_020aea30(Aea30C* c, dActor_c* a, dBgCh_Actr* b) {
    if (c->mDeathState == 0)
        return;
    c->mFlags &= ~0x10000000;
    c->mDeathTimer = 0;
    (c->*data_ov002_0210db80[c->mDeathState - 1])(a, b);
    c->mVertAccel = -0x2000;
    c->mFlags &= ~0x10000000;
}

/* The five death-state handlers UpdateDeath dispatches through
   data_ov002_0210dbc0. They are dEnemyBase_c methods in all but name, but a
   member declaration added to dEnemyBase_c.h renumbers mwcc's @NNN
   temporaries in every TU that includes it, and eleven promoted TUs record
   theirs. So they are declared here on a derived stand-in receiver with no
   fields or virtuals of its own; the table entries static_cast back to
   dEnemyBase_c's pointer-to-member type, which is a zero adjustment. */
struct EnemyDeathC : dEnemyBase_c {
  int func_ov002_020ae64c(dBgCh_Actr & clsn_);
  int func_ov002_020ae608(dBgCh_Actr & clsn_);
  int func_ov002_020ae4cc(dBgCh_Actr & clsn_);
  int func_ov002_020ae454(dBgCh_Actr & clsn_);
  int func_ov002_020aea24(dBgCh_Actr & clsn_);
};

// @symbol _ZN6Aea30C19func_ov002_020aea2cEP8dActor_cP10dBgCh_Actr
void Aea30C::func_ov002_020aea2c(dActor_c*, dBgCh_Actr*)
{
}

// @symbol _ZN11EnemyDeathC19func_ov002_020aea24ER10dBgCh_Actr
int EnemyDeathC::func_ov002_020aea24(dBgCh_Actr&)
{
    return 0;
}

// @symbol _ZN6Aea30C19func_ov002_020ae9f8EP8dActor_cP10dBgCh_Actr
/* Stop the enemy (zero mHorzSpeed and mVertSpeed), start a 0xf-frame
   mDeathTimer, then clear mFlags bit 0x1. */
void Aea30C::func_ov002_020ae9f8(dActor_c*, dBgCh_Actr*)
{
    mHorzSpeed = 0;
    mVertSpeed = 0;
    mDeathTimer = 0xf;
    mFlags &= ~0x1;
}

// @symbol func_ov002_020ae968
/* Knock-back strengths, indexed by the attacking player's param1. */
extern "C" {
extern u16 data_ov002_020ff01c[];
}

/* Actor ID 0xbf is the player. Int-returning, as the 0/1 the ROM
   materialises before testing it requires. */
static inline int IsPlayerActor(dActor_c *actor) {
    return actor->actorID == 0xbf;
}

extern "C" void func_ov002_020ae968(dEnemyBase_c *c, dActor_c *arg) {
    c->mVertSpeed = 0x14000;
    if (IsPlayerActor(arg)) {
        u16 t = data_ov002_020ff01c[(u16)arg->param1];
        c->mHorzSpeed = ((int)t << 0xf) / 50;
    } else {
        c->mHorzSpeed = 0x14000;
    }
    c->mFlags &= ~1;
    Sound::PlayBank0(9, c->mCamSpacePos);
}

// @symbol _ZN6Aea30C19func_ov002_020ae954EP8dActor_cP10dBgCh_Actr
void Aea30C::func_ov002_020ae954(dActor_c* a, dBgCh_Actr*)
{
    mPrevAngleY = a->mAngleY;
    func_ov002_020ae968(this, a);
}

// @symbol func_ov002_020ae8b8
extern "C" void func_ov002_020ae8b8(dEnemyBase_c *c, dActor_c *arg) {
    if (IsPlayerActor(arg)) {
        u16 t = data_ov002_020ff01c[(u16)arg->param1];
        int scaled = (int)t << 3;
        int ip = scaled << 12;
        int d1 = ip / 100;
        int d2 = scaled / 100;
        c->mHorzSpeed = d1;
        c->mVertSpeed = (d2 + 0x20) << 12;
    } else {
        c->mHorzSpeed = 0xa000;
        c->mVertSpeed = 0x28000;
    }
    c->mFlags &= ~1;
}

// @symbol _ZN6Aea30C19func_ov002_020ae890EP8dActor_cP10dBgCh_Actr
void Aea30C::func_ov002_020ae890(dActor_c* a, dBgCh_Actr*)
{
    mPrevAngleY = a->mAngleY;
    func_ov002_020ae8b8(this, a);
    Sound::PlayBank0(0xa, mCamSpacePos);
}

// @symbol _ZN6Aea30C19func_ov002_020ae87cEP8dActor_cP10dBgCh_Actr
void Aea30C::func_ov002_020ae87c(dActor_c* a, dBgCh_Actr*)
{
    mPrevAngleY = a->mAngleY;
    func_ov002_020ae8b8(this, a);
}

extern "C" s16 Vec3_HorzAngle(const void *v0, const void *v1);

// @symbol _ZN6Aea30C19func_ov002_020ae844EP8dActor_cP10dBgCh_Actr
void Aea30C::func_ov002_020ae844(dActor_c* a, dBgCh_Actr*)
{
    s16 angle = Vec3_HorzAngle(&a->mPos, &mPos);
    mPrevAngleY = angle;
    func_ov002_020ae968(this, a);
}

// @symbol _ZN6Aea30C19func_ov002_020ae80cEP8dActor_cP10dBgCh_Actr
void Aea30C::func_ov002_020ae80c(dActor_c* a, dBgCh_Actr*)
{
    mPrevAngleY = Vec3_HorzAngle(&a->mPos, &mPos);
    func_ov002_020ae8b8(this, a);
}

// @symbol _ZN6Aea30C19func_ov002_020ae73cEP8dActor_cP10dBgCh_Actr
void Aea30C::func_ov002_020ae73c(dActor_c* a, dBgCh_Actr*)
{
    mPrevAngleY = Vec3_HorzAngle(&a->mPos, &mPos);
    mVertSpeed = 0xa000;

    if (IsPlayerActor(a))
        mHorzSpeed = (data_ov002_020ff01c[(unsigned short)a->param1] << 15) / 50;
    else
        mHorzSpeed = 0x14000;

    mHorzSpeed /= 6;
    mDeathTimer = 8;
    mFlags &= ~1;
    Sound::PlayBank0(0xa, mCamSpacePos);
}

/* mDeathState selects which death handler from a table of POINTERS TO MEMBER
   FUNCTION, then the position and mesh collision are updated regardless. */
typedef int (dEnemyBase_c::*DeathPMF)(dBgCh_Actr &);
extern DeathPMF data_ov002_0210dbc0[];

extern "C" {
extern void DecIfAbove0_Short(unsigned short *p);
}

// @symbol _ZN12dEnemyBase_c11UpdateDeathER10dBgCh_Actr
int dEnemyBase_c::UpdateDeath(dBgCh_Actr &clsn) {
    int ret;
    if (mDeathState == 0)
        return 0;
    DecIfAbove0_Short(&mDeathTimer);
    ret = (this->*data_ov002_0210dbc0[mDeathState - 1])(clsn);
    UpdatePos(0);
    UpdateWMClsn(clsn, 0);
    return ret;
}

/* Forward declaration, because func_ov002_020ae5c8 is written LOWER in this file (the whole
   TU runs highest ROM address first).

   THE SECOND PARAMETER IS REAL AND IS LOAD-BEARING, and merging is what proved
   it. func_ov002_020ae5c8's own file defines it as `(void *c)` and never reads a
   second argument; this file's declared it `(char *, int)` and passed one. Built
   the one-argument way, this function comes out 0x50 against the ROM's 0x5c --
   twelve bytes and three instructions short, because `x` no longer has to
   survive the body in a callee-saved register. So both observations are true at
   once: the caller passes it, the callee ignores it, and the definition below
   carries the parameter unused. */
extern "C" int func_ov002_020ae5c8(dEnemyBase_c *c, int x);

// @symbol _ZN11EnemyDeathC19func_ov002_020ae64cER10dBgCh_Actr
int EnemyDeathC::func_ov002_020ae64c(dBgCh_Actr &clsn) {
    int x = (int)&clsn;
    if (mDeathTimer >= 5) {
        int v = mScaleY - 0x23d;
        if (v < 0x4cc)
            v = 0x4cc;
        mScaleY = v;
        mScaleZ = 0x2000 - mScaleY;
        mScaleX = mScaleZ;
    }
    return func_ov002_020ae5c8(this, x);
}

// @symbol _ZN11EnemyDeathC19func_ov002_020ae608ER10dBgCh_Actr
int EnemyDeathC::func_ov002_020ae608(dBgCh_Actr &clsn) {
    if (clsn.IsOnGround() == 0)
        return 0;
    SpawnCoin();
    KillAndTrackInDeathTable();
    mDeathState = 0;
    return 1;
}

// @symbol func_ov002_020ae5c8
/* `x` is unused on purpose -- see the note on func_ov002_020ae64c, which passes it. */
extern "C" int func_ov002_020ae5c8(dEnemyBase_c *c, int x) {
    if (c->mDeathTimer != 0)
        return 0;
    c->SpawnCoin();
    c->KillAndTrackInDeathTable();
    c->mDeathState = 0;
    return 1;
}

struct Vector3_16f;
extern "C" {
extern void *d_enemy_baseNewHold(unsigned int a, unsigned int b, int c, int d, int e, const void *f,
                                 void *g);
extern u32 d_enemy_baseNewUnkCallback818Hold(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y,
                                             Fix12i z, const Vector3_16f *dir);
}

// @symbol _ZN11EnemyDeathC19func_ov002_020ae4ccER10dBgCh_Actr
int EnemyDeathC::func_ov002_020ae4cc(dBgCh_Actr &clsn) {
    int v[3];
    int n = mScaleY;
    if (n == 0) {
        int z = mPosZ;
        int y = mPosY + 0x50000;
        int x = mPosX;
        v[0] = x;
        v[1] = y;
        v[2] = z;
    } else {
        int z = mPosZ;
        int y = mPosY + n * 0x50;
        int x = mPosX;
        v[0] = x;
        v[1] = y;
        v[2] = z;
    }
    if (clsn.IsOnGround()) {
        SpawnCoin();
        KillAndTrackInDeathTable();
        mDeathState = 0;
        return 1;
    }
    mDeathParticleID0 =
        (u32)Particle::System::New(mDeathParticleID0, 0x13a, *(int *)((char *)v + 0),
                                   *(int *)((char *)v + 4), *(int *)((char *)v + 8), 0, 0);
    mDeathParticleID1 =
        Particle::System::NewUnkCallback818(mDeathParticleID1, 0x13b, *(int *)((char *)v + 0),
                                            *(int *)((char *)v + 4), *(int *)((char *)v + 8), 0);
    return 0;
}

// @symbol _ZN11EnemyDeathC19func_ov002_020ae454ER10dBgCh_Actr
int EnemyDeathC::func_ov002_020ae454(dBgCh_Actr &clsn) {
    if (mDeathTimer == 0 || clsn.IsOnGround() != 0 || clsn.IsOnWall() != 0) {
        SpawnCoin();
        KillAndTrackInDeathTable();
        mDeathState = 0;
        return 1;
    }
    return 0;
}

/* Written as the real method over the real dBgCh_Lin/dBgPi classes, which is the
   form the legacy shard recovered and byte-matched. The two by-value Fix12i
   parameters the mangled name claims are spelled as themselves. */
extern "C" {
extern short data_02082214[];
}

// @symbol _ZN12dEnemyBase_c15IsGoingOffCliffER10dBgCh_Actrisbbi
int dEnemyBase_c::IsGoingOffCliff(dBgCh_Actr &clsn, Fix12i down, s16 maxSlope, bool detectWater,
                                  bool allowObjFloor, Fix12i up) {
    Vector3 start;
    Vector3 end;
    mCliffState = 0;
    if (clsn.IsOnGround() != 0) {
        dBgCh_Lin line;
        start.x = mPosX;
        start.y = mPosY;
        start.z = mPosZ;
        start.y += up;
        end.x = mPosX;
        end.y = mPosY;
        end.z = mPosZ;
        end.y -= down;
        line.SetObjAndLine(start, end, this);
        if (detectWater)
            line.StartDetectingWater();
        if (line.DetectClsn()) {
            if (line.clsnDist - up >= down)
                mCliffState = 1;
            if (!allowObjFloor) {
                dBgPi result;
                line.CopyTo(result);
                if (result.GetClsnID() != -1) {
                    mCliffState = 1;
                    return 1;
                }
            }
            Vector3 normal;
            line.surface.CopyNormalTo(normal);
            int idx = ((unsigned short)maxSlope >> 4) * 2;
            short cosMax = data_02082214[idx + 1];
            if (normal.y < cosMax)
                mCliffState = 2;
        } else {
            mCliffState = 1;
        }
    }
    return mCliffState != 0;
}

/* On a wall, reflect the heading off it; on a cliff edge (mCliffState), turn
   around; otherwise report that nothing was done. */
extern "C" {
/* ReflectAngle takes Fix12<int> by value -- the mwccarm 6az wall, runbook
   section 7 -- so it stays extern "C" with scalars in those slots. */
}

// @symbol _ZN12dEnemyBase_c24AngleAwayFromWallOrCliffER10dBgCh_ActrRs
int dEnemyBase_c::AngleAwayFromWallOrCliff(dBgCh_Actr &clsn, short &outAngle) {
    if (clsn.IsOnWall()) {
        outAngle = ReflectAngle(mWallNormalX, mWallNormalZ, outAngle);
    } else if (mCliffState) {
        outAngle = (short)(mPrevAngleY + 0x8000);
    } else {
        return 0;
    }
    return 1;
}

/* One frame of "this enemy is in Yoshi's mouth / has just been spat out". The
   three mFlags bits pick the phase: 0x40000 = held in the mouth (track the
   eater's position and return 2), 0x20000 = swallowed (return 1), 0x80000 = just
   spat out, in which case the launch velocity is built from the eater's facing
   angle and then the body flies under mesh collision until it lands.

   The launch block was the last residue: the cartridge fills the load-use slot
   after `ldr r0,[r6,#0xd0]` with the rounding constant (mov r3,#0x800) while every
   spelling tried here filled it with the position base (add r1,r6,#0x5c). Writing
   the eater test in its POSITIVE sense -- `if (eater != 0) { launch } else v =
   0x14000;` rather than `if (eater == 0) v = 0x14000; else { launch }` -- puts the
   launch block first in the linearisation and the slot falls the cartridge's way.
   Same branch, same semantics; only the order the two arms are visited in.

   The eater is mEatingPlayer, read as a dActor_c for its position, facing and
   speed. On landing, the floor normal's X/Z (scaled by 8) is added into
   mSpeedX/mSpeedZ and mHorzSpeed becomes the horizontal length of the
   mSpeedX/mVertSpeed/mSpeedZ triple. mSpitOutTimer is set to 5 at the spit
   and counted down here; the subclasses read 5 and 0 off it. */
extern "C" {
extern s32 Vec3_HorzLen(const Vector3 *v);
}

/* The three dActor_c::mFlags bits Yoshi's mouth drives. */
enum { YOSHI_SWALLOWED = 0x20000, YOSHI_HOLDING = 0x40000, YOSHI_SPAT_OUT = 0x80000 };

/* Int-returning, as the 0/1 the ROM materialises before each test requires. */
static inline int HasFlag(int flags, int bit) {
    return (flags & bit) != 0;
}

// @symbol _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr
int dEnemyBase_c::UpdateYoshiEat(dBgCh_Actr &clsn) {
    int flags;
    int v;
    int lim;

    flags = mFlags;
    if (HasFlag(flags, YOSHI_HOLDING)) {
        if (mEatingPlayer == 0) {
            mFlags &= ~YOSHI_HOLDING;
        } else {
            Vector3 *o = &mEatingPlayer->mPos;
            mPosX = o->x;
            mPosY = o->y;
            mPosZ = o->z;
            mPosY += 0x30000;
        }
        mEatenByYoshi = 0;
        return 2;
    }
    if (HasFlag(flags, YOSHI_SWALLOWED)) {
        mEatenByYoshi = 0;
        return 1;
    }
    if (HasFlag(flags, YOSHI_SPAT_OUT) || mEatenByYoshi != 0) {
        if (mSpitOutTimer != 0)
            mSpitOutTimer -= 1;
        if (HasFlag(mFlags, YOSHI_SPAT_OUT)) {
            mEatenByYoshi = 1;
            mSpitOutTimer = 5;
            mFlags &= ~YOSHI_SPAT_OUT;
            if (mEatingPlayer != 0) {
                Vector3 *o2 = &mEatingPlayer->mPos;
                mPosX = o2->x;
                mPosY = o2->y;
                mPosZ = o2->z;
                mPosY += 0x32000;
                {
                    dActor_c *o3 = mEatingPlayer;
                    u16 ang = o3->mAngleY;
                    int idx = (ang >> 4) << 1;
                    int dist = 0x32000;
                    mPosX += FX_MUL(data_02082214[idx], dist);
                    mPosZ += FX_MUL(data_02082214[idx + 1], dist);
                }
                {
                    Vector3_16 *p = &mEatingPlayer->mAngle;
                    mPrevAngleX = p->x;
                    mPrevAngleY = p->y;
                    mPrevAngleZ = p->z;
                }
                v = mEatingPlayer->mHorzSpeed;
            } else {
                v = 0x14000;
            }
            if (v >= 0x14000)
                mHorzSpeed = v + 0xa000;
            else
                mHorzSpeed = FX_MUL(v, 0x800) + 0x14000;
            mHorzSpeed = FX_MUL(mHorzSpeed, 0x14cc);
            mVertSpeed = 0xc000;
            mEatingPlayer = 0;
        }
        lim = clsn.GetLimMovFlag();
        clsn.SetLimMovFlag();
        UpdatePos(0);
        dBgCh_Actr_UpdateContinuous_Veneer(&clsn);
        if (clsn.IsOnGround() != 0) {
            dBgPi *fr = clsn.GetFloorResult();
            fr->surface.CopyNormalTo(*(Vector3 *)&mFloorNormalX);
            if (clsn.JustHitGround() == 0) {
                mVertSpeed = 0;
                mEatenByYoshi = 0;
            } else {
                fr->surface.CopyNormalTo(*(Vector3 *)&mFloorNormalX);
                mVertSpeed = cstd::fdiv((mVertSpeed * -50) / 100, mFloorNormalY);
            }
            mSpeedX += mFloorNormalX << 3;
            mSpeedZ += mFloorNormalZ << 3;
            mHorzSpeed = Vec3_HorzLen((const Vector3 *)&mSpeedX);
        }
        mAngleY = mPrevAngleY;
        if (lim == 0)
            clsn.ClearLimMovFlag();
        return 3;
    }
    mEatingPlayer = 0;
    mFlags &= ~(YOSHI_SWALLOWED | YOSHI_HOLDING | YOSHI_SPAT_OUT);
    mEatenByYoshi = 0;
    return 0;
}

/* While mEatenByYoshi is set, a cylinder collision against anything other than actor
   IDs 0x120/0x121 spawns the mega-character particles; otherwise bit 0x20000 on
   the collision is raised. Clearing mEatenByYoshi clears that bit instead. */

// @symbol _ZN12dEnemyBase_c27SpawnParticlesIfHitOtherObjER5dCc_c
int dEnemyBase_c::SpawnParticlesIfHitOtherObj(dCc_c &clsn) {
    if (mEatenByYoshi != 0) {
        unsigned int id = clsn.otherOwner;
        if (id != 0) {
            dActor_c *other = dActor_c::FindWithID(id);
            if (other != 0) {
                unsigned short otherID = other->actorID;
                int isCoin = otherID == 0x120;
                if (!isCoin) {
                    int isRedCoin = otherID == 0x121;
                    if (!isRedCoin) {
                        SpawnMegaCharParticles(*other, (char *)&clsn);
                        return 1;
                    }
                }
            }
        }
        clsn.flags |= 0x20000;
    } else {
        clsn.flags &= ~0x20000;
    }
    return 0;
}

// @symbol _ZN12dEnemyBase_c22SpawnMegaCharParticlesER8dActor_cPc
extern "C" {
extern void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);
extern s16 atan2_hold_d_enemy_base_1(s32 y, s32 x);
extern s32 *Vec3_AsrInPlace(s32 *v, s32 sh);
}

void dEnemyBase_c::SpawnMegaCharParticles(dActor_c &other, char *p) {
    s16 dirvec[3];
    Vector3 v;
    Vector3 dst, src;

    dst.x = mPosX;
    dst.y = mPosY;
    {
        Vector3 *pv = &other.mPos;
        dst.z = mPosZ;
        src.x = pv->x;
        src.y = pv->y;
        src.z = pv->z;
    }

    {
        int t = ((int)(u16)mAngleY) >> 4;
        dirvec[0] = data_02082214[t * 2];
        dirvec[1] = 0;
        t = ((int)(u16)mAngleY) >> 4;
        dirvec[2] = data_02082214[t * 2 + 1];
    }

    if (p != 0) {
        Vector3 delta;
        s32 w = ((dCc_c *)p)->radius;
        s32 aX, aY;
        s32 iX, iY;
        s32 s;

        Vec3_Sub(&delta, &src, &dst);

        v.x = delta.x;
        v.y = delta.y;
        v.z = delta.z;

        aX = cstd::atan2(v.x, v.z);
        aY = cstd::atan2(v.y, Vec3_HorzLen(&v));

        iX = (u16)aX >> 4;
        iY = (u16)aY >> 4;

        s = FX_MUL(w, data_02082214[iY * 2 + 1]);
        dst.x += FX_MUL(s, data_02082214[iX * 2]);
        dst.y += FX_MUL(w, data_02082214[iY * 2]);
        dst.z += FX_MUL(s, data_02082214[iX * 2 + 1]);

        dst.y += ((dCc_c *)p)->height >> 1;
    } else {
        Vector3 delta;
        Vec3_Sub(&delta, &src, &dst);
        dst.x = delta.x;
        dst.y = delta.y;
        dst.z = delta.z;
        Vec3_AsrInPlace(&dst.x, 1);
        dst.y += 0x46000;
    }

    Particle::System::New(0, 0x43, dst.x, dst.y, dst.z, 0, 0);
    Particle::System::New(0, 0x44, dst.x, dst.y, dst.z, 0, 0);
    Particle::System::New(0, 0xbf, dst.x, dst.y, dst.z, (const Vector3_16f *)(dirvec), 0);
    Particle::System::New(0, 0xc0, dst.x, dst.y, dst.z, 0, 0);
}

/* What happens when a mega/invincible Mario runs an enemy over. The enemy is
   turned to face the player, given the launch velocity the caller passed, put into
   death state 8 with a 30-frame timer, and two particle systems are spawned at its
   position raised by whatever OnAimedAtWithEgg (vtable slot 29) reports.

   The third parameter is genuinely unused -- every caller materialises it and no
   body reads it. include/dEnemyBase_c.h carries the census that proves both the
   arity and this address's overlay; this member spent a long time spelled
   func_ov002_020ada40 because the mangled name was sitting on ov004's unrelated
   function at the same address. */
extern "C" {
namespace Particle {
struct SystemHold {
    static void NewSimple(unsigned int a, int x, int y, int z);
};
}
}

// @symbol _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Playeri
void dEnemyBase_c::KillByInvincibleChar(const Vector3_16 &spin, Player &player, Fix12i) {
    mFlags &= ~0x10000001;
    mPrevAngleY = Vec3_HorzAngle(&player.mPos, &mPos);
    mHorzSpeed = 0xa000;
    mVertSpeed = 0x28000;
    mDeathTimer = 0x1e;
    mSpinRateX = spin.x;
    mSpinRateY = spin.y;
    mSpinRateZ = spin.z;
    mDeathState = 8;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x32000;
    Sound::PlayBank0(9, mCamSpacePos);
    volatile int v[3];
    v[0] = mPosX;
    v[1] = mPosY;
    v[2] = mPosZ;
    int ret = OnAimedAtWithEgg();
    int vy = v[1];
    int vx = v[0];
    vy = vy + ret;
    v[1] = vy;
    Particle::System::NewSimple(0x43, vx, vy, v[2]);
    Particle::System::NewSimple(0x44, v[0], v[1], v[2]);
    player.IncMegaKillCount();
}

/* One frame of the death an invincible (mega) character inflicts. `flags` is a
   two-bit selector, not a boolean: bit 0 drops a coin, bit 1 books the kill in
   the death table. Both reference parameters are null-checked -- the compiler
   emits the test rather than folding it away, so `&ref == 0` is how the ROM's
   check is spelled. The model spins about the height OnAimedAtWithEgg (vtable
   slot 29) reports, the point Yoshi's egg aims at: it is lifted to that height,
   rotated, and dropped back. */
extern "C" {
extern void Vec3_Asr(void *dst, const void *src, int sh);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationZXYExt(void *m, int x, int y, int z);
extern char data_020a0e68;
}

// @symbol _ZN12dEnemyBase_c26UpdateKillByInvincibleCharER10dBgCh_ActrR14dExtAnmModel_cj
int dEnemyBase_c::UpdateKillByInvincibleChar(dBgCh_Actr &clsn, dExtAnmModel_c &anim,
                                             unsigned int flags) {
    int v[3];

    if (mDeathState != 8)
        return 0;

    if (mDeathTimer != 0)
        mDeathTimer -= 1;

    if (mDeathTimer == 0 || (&clsn != 0 && clsn.IsOnGround() != 0 && mVertSpeed < 0)) {
        if (flags & 1)
            SpawnCoin();
        if (flags & 2)
            KillAndTrackInDeathTable();
        mDeathState = 0;
        return 2;
    }

    UpdatePos(0);
    if (&clsn != 0) {
        UpdateWMClsn(clsn, 0);
        if (clsn.IsOnWall() != 0)
            mPrevAngleY = ReflectAngle(mWallNormalX, mWallNormalZ, mPrevAngleY);
    }

    if (&anim != 0) {
        mAngleX += mSpinRateX;
        mAngleY += mSpinRateY;
        mAngleZ += mSpinRateZ;
        Vec3_Asr(v, &mPosX, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, v[0], v[1], v[2]);
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, OnAimedAtWithEgg() >> 3, 0);
        Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68,
            mAngleX, mAngleY, mAngleZ);
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, (-OnAimedAtWithEgg()) >> 3, 0);
        anim.mat4x3 = *(Matrix4x3 *)&data_020a0e68;
    }
    return 1;
}

/* The death-state-8 sibling of UpdateKillByInvincibleChar above: the same
   vtable-slot-29 spin over data_020a0e68, but reached as a free helper taking
   the enemy, its collision and its dExtSimpleModel_c as raw pointers plus the
   coin/death-table flags, and running the wall-reflect variant of the
   collision update. Lowest address in the TU, so it is defined last: this
   file runs highest ROM address first. */
/* Vec3_Asr, the Matrix4x3 helpers and data_020a0e68 reuse the declarations
   above UpdateKillByInvincibleChar. The parameters stay untyped: the four
   callers' declarations spell them as void * and char *. */

// @symbol func_ov002_020ad660
extern "C" int func_ov002_020ad660(void *enemy, void *clsnArg, void *modelArg, int flags) {
    dEnemyBase_c *c = (dEnemyBase_c *)enemy;
    dBgCh_Actr *clsn = (dBgCh_Actr *)clsnArg;
    dExtSimpleModel_c *model = (dExtSimpleModel_c *)modelArg;
    int v[3];

    if (c->mDeathState != 8)
        return 0;

    if (c->mDeathTimer == 0 || (clsn != 0 && (clsn->IsOnGround() || clsn->IsOnWall()))) {
        if (flags & 1)
            c->SpawnCoin();
        if (flags & 2)
            c->KillAndTrackInDeathTable();
        c->mDeathState = 0;
        return 2;
    }

    c->mDeathTimer -= 1;
    c->UpdatePos(0);
    if (clsn != 0)
        c->UpdateWMClsn(*clsn, 0);

    if (model != 0) {
        c->mAngleX += c->mSpinRateX;
        c->mAngleY += c->mSpinRateY;
        c->mAngleZ += c->mSpinRateZ;
        Vec3_Asr(v, &c->mPos, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, v[0], v[1], v[2]);
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, c->OnAimedAtWithEgg() >> 3, 0);
        Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68, c->mAngleX, c->mAngleY, c->mAngleZ);
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, (-c->OnAimedAtWithEgg()) >> 3, 0);
        model->mat4x3 = *(Matrix4x3 *)&data_020a0e68;
    }
    return 1;
}

/* The two state-handler tables. func_ov002_020aea30 dispatches the first on
   mDeathState; UpdateDeath dispatches the second on mDeathState. Non-const
   pointer-to-member arrays live in .bss, so mwcc generates this TU's static
   initializer to copy the descriptor records in at overlay load. */
Aea30PMF data_ov002_0210db80[8] = {
    &Aea30C::func_ov002_020ae9f8,
    &Aea30C::func_ov002_020ae954,
    &Aea30C::func_ov002_020ae890,
    &Aea30C::func_ov002_020ae87c,
    &Aea30C::func_ov002_020ae844,
    &Aea30C::func_ov002_020ae80c,
    &Aea30C::func_ov002_020ae73c,
    &Aea30C::func_ov002_020aea2c,
};

DeathPMF data_ov002_0210dbc0[8] = {
    static_cast<DeathPMF>(&EnemyDeathC::func_ov002_020ae64c),
    static_cast<DeathPMF>(&EnemyDeathC::func_ov002_020ae608),
    static_cast<DeathPMF>(&EnemyDeathC::func_ov002_020ae608),
    static_cast<DeathPMF>(&EnemyDeathC::func_ov002_020ae4cc),
    static_cast<DeathPMF>(&EnemyDeathC::func_ov002_020ae608),
    static_cast<DeathPMF>(&EnemyDeathC::func_ov002_020ae608),
    static_cast<DeathPMF>(&EnemyDeathC::func_ov002_020ae454),
    static_cast<DeathPMF>(&EnemyDeathC::func_ov002_020aea24),
};
