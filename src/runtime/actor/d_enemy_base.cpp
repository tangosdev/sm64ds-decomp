//cpp
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
 *   - The handlers keep their named pointer locals (`Aea30C *self = this`,
 *     `dEnemyBase_c *c = this`): each named local draws one @NNN number, so
 *     dropping one renumbers this TU's pointer-to-member records (measured).
 *   - UpdateYoshiEat keeps a `char *self` for the fields the headers name
 *     only as unk_ (0x0a4, 0x0ac, 0x104); see the note there.
 *   - func_ov002_020ae5c8 stays a free `extern "C"` callee -- callers pass
 *     its unused second argument; the one-argument form comes out 0x50
 *     against the ROM's 0x5c (measured note in place).
 *   - UpdateKillByInvincibleChar keeps the KbicVB vtable-slot-29 stand-in
 *     (real slot name unresolved) and the KbicM48 48-byte matrix copy: a
 *     plain Matrix4x3 assignment there renumbers one @NNN (measured).
 *   - Computed-spelling externs stay where a member form changes codegen:
 *     _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s (Fix12<int> by value) and the
 *     Particle::System::New wrapper, both noted at their declarations.
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
extern "C" {
extern void func_020383f0(dBgCh_Actr *);
extern void dBgCh_Actr_UpdateDiscreteNoLava_veneer(dBgCh_Actr *);
extern void func_02038414(dBgCh_Actr *);
extern void dBgCh_Actr_UpdateContinuous_Veneer(dBgCh_Actr *);
extern int _ZN4cstd4fdivEii(int, int);
}

void dEnemyBase_c::UpdateWMClsn(dBgCh_Actr & clsn_, unsigned int sel)
{
    dBgCh_Actr *clsn = &clsn_;

    switch (sel) {
    case 1: func_020383f0(clsn); break;
    case 2: dBgCh_Actr_UpdateDiscreteNoLava_veneer(clsn); break;
    case 3: func_02038414(clsn); break;
    default: dBgCh_Actr_UpdateContinuous_Veneer(clsn); break;
    }
    if (clsn->IsOnGround()) {

        ((dBgPi *)clsn->GetFloorResult())->surface.CopyNormalTo(*(Vector3 *)&mFloorNormalX);
        if (clsn->GetLimMovFlag() == 0) {
            int dz = mFloorNormalY;
            if (dz != 0) {
                int nx = mFloorNormalX;
                int vx = unk_0a4;
                int nz = mFloorNormalZ;
                int vz = unk_0ac;
                int a = FX_MUL(nx, vx);
                int b = FX_MUL(nz, vz);
                int num = a + b;
                int q = _ZN4cstd4fdivEii(num, dz);
                mVertSpeed = -(q + 0x8000);
            }
        }
    }
    if (clsn->IsOnWall()) {
        ((dBgPi *)clsn->GetWallResult())->surface.CopyNormalTo(*(Vector3 *)&mWallNormalX);
    }

}

/* Its legacy file built this from a local `struct dEnemyBase_c : dActor_c` shadow with a
   local POD `struct Vector3`; both are the real ones now. Measured: using
   types.h's Vector3 -- which declares ~Vector3(){} and is therefore NOT a POD --
   for the local `v` costs nothing here, all 0x13c bytes reproduce, which is what
   lets dActor_c::Spawn take it by const reference as its real signature says. */
extern "C" int RandomIntInternal(int *seed);
extern u16 data_ov002_020ff014;
extern int data_0209e650;

// @symbol _ZN12dEnemyBase_c9SpawnCoinEv
void dEnemyBase_c::SpawnCoin()
{
    dEnemyBase_c *t = this;
    int i;
    Vector3 v;
    if (t->unk_108 != 0) {
        int tz = t->mPosZ;
        int ty = t->mPosY + 0x78000;
        int tx = t->mPosX;
        v.x = tx;
        v.y = ty;
        v.z = tz;
        if (t->unk_108 >= 4)
            t->unk_108 = 1;
        for (i = 0; i < t->unk_10a + 1; i++) {
            dActor_c *coin = dActor_c::Spawn(
                (&data_ov002_020ff014)[t->unk_108 - 1],
                0xf2, v, 0, t->mAreaId, -1);
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
    this->PoofDust();
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
    Aea30C *self = this;
    self->mHorzSpeed = 0;
    self->mVertSpeed = 0;
    self->mDeathTimer = 0xf;
    self->mFlags &= ~0x1;
}

// @symbol func_ov002_020ae968
extern "C" {
extern int _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int id, void *v);
extern u16 data_ov002_020ff01c[];
}

extern "C" void func_ov002_020ae968(dEnemyBase_c *c, dActor_c *arg) {
    int b;
    c->mVertSpeed = 0x14000;
    b = arg->actorID;
    b = b == 0xbf;
    if (b != 0) {
        u16 t = data_ov002_020ff01c[(u16)arg->param1];
        c->mHorzSpeed = ((int)t << 0xf) / 50;
    } else {
        c->mHorzSpeed = 0x14000;
    }
    c->mFlags &= ~1;
    _ZN5Sound9PlayBank0EjRK7Vector3(9, &c->mCamSpacePosX);
}

// @symbol _ZN6Aea30C19func_ov002_020ae954EP8dActor_cP10dBgCh_Actr
void Aea30C::func_ov002_020ae954(dActor_c* a, dBgCh_Actr*)
{
    mPrevAngleY = a->mAngleY;
    func_ov002_020ae968(this, a);
}

// @symbol func_ov002_020ae8b8
extern "C" void func_ov002_020ae8b8(dEnemyBase_c *c, dActor_c *arg) {
    int b;
    b = arg->actorID;
    b = b == 0xbf;
    if (b != 0) {
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
    _ZN5Sound9PlayBank0EjRK7Vector3(0xa, &mCamSpacePosX);
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
    s16 angle = Vec3_HorzAngle(&a->mPos, &mPosX);
    mPrevAngleY = angle;
    func_ov002_020ae968(this, a);
}

// @symbol _ZN6Aea30C19func_ov002_020ae80cEP8dActor_cP10dBgCh_Actr
void Aea30C::func_ov002_020ae80c(dActor_c* a, dBgCh_Actr*)
{
    mPrevAngleY = Vec3_HorzAngle(&a->mPos, &mPosX);
    func_ov002_020ae8b8(this, a);
}

// @symbol _ZN6Aea30C19func_ov002_020ae73cEP8dActor_cP10dBgCh_Actr
void Aea30C::func_ov002_020ae73c(dActor_c* a, dBgCh_Actr*)
{
    Aea30C *c = this;
    dActor_c *arg = a;
    c->mPrevAngleY = Vec3_HorzAngle(&arg->mPos, &c->mPosX);
    c->mVertSpeed = 0xa000;

    if ((int)(arg->actorID == 0xbf) != 0)
        c->mHorzSpeed = (data_ov002_020ff01c[(unsigned short)arg->param1] << 15) / 50;
    else
        c->mHorzSpeed = 0x14000;

    {
        int *p = &c->mHorzSpeed;
        *p = *p / 6;
    }
    c->mDeathTimer = 8;
    c->mFlags &= ~1;
    _ZN5Sound9PlayBank0EjRK7Vector3(0xa, &c->mCamSpacePosX);
}

/* mDeathState selects which death handler from a table of POINTERS TO MEMBER
   FUNCTION, then the position and mesh collision are updated regardless. */
typedef int (dEnemyBase_c::*DeathPMF)(dBgCh_Actr &);
extern DeathPMF data_ov002_0210dbc0[];

extern "C" {
extern void DecIfAbove0_Short(unsigned short *p);
}

// @symbol _ZN12dEnemyBase_c11UpdateDeathER10dBgCh_Actr
int dEnemyBase_c::UpdateDeath(dBgCh_Actr & clsn_)
{
    dBgCh_Actr *clsn = &clsn_;
    int ret;
    if (mDeathState == 0)
        return 0;
    DecIfAbove0_Short(&mDeathTimer);
    ret = (this->*data_ov002_0210dbc0[mDeathState - 1])(*clsn);
    this->UpdatePos(0);
    UpdateWMClsn(*clsn, 0);
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
int EnemyDeathC::func_ov002_020ae64c(dBgCh_Actr& clsn){
    dEnemyBase_c *c = this;
    int x = (int)&clsn;
    if (c->mDeathTimer >= 5) {
        int v = c->mScaleY - 0x23d;
        if (v < 0x4cc)
            v = 0x4cc;
        c->mScaleY = v;
        c->mScaleZ = 0x2000 - c->mScaleY;
        c->mScaleX = c->mScaleZ;
    }
  return func_ov002_020ae5c8(c, x);
}

// @symbol _ZN11EnemyDeathC19func_ov002_020ae608ER10dBgCh_Actr
int EnemyDeathC::func_ov002_020ae608(dBgCh_Actr& clsn){
    dEnemyBase_c *c = this;
    dBgCh_Actr *a = &clsn;
    if (a->IsOnGround() == 0)
        return 0;
    c->SpawnCoin();
    c->KillAndTrackInDeathTable();
    c->mDeathState = 0;
    return 1;
}

// @symbol func_ov002_020ae5c8
/* `x` is unused on purpose -- see the note on func_ov002_020ae64c, which passes it. */
extern "C" int func_ov002_020ae5c8(dEnemyBase_c *c, int x) {
    unsigned short *p = &c->mDeathTimer;
    if (*p != 0)
        return 0;
    c->SpawnCoin();
    c->KillAndTrackInDeathTable();
    c->mDeathState = 0;
    return 1;
}

struct Vector3_16f;
extern "C" {
extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
  unsigned int a, unsigned int b, int c, int d, int e, const void* f, void* g);
extern u32 _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
  u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, const Vector3_16f* dir);
}

// @symbol _ZN11EnemyDeathC19func_ov002_020ae4ccER10dBgCh_Actr
int EnemyDeathC::func_ov002_020ae4cc(dBgCh_Actr &clsn_) {
    dEnemyBase_c *self = this;
    dBgCh_Actr *clsn = &clsn_;
    int v[3];
    int n = self->mScaleY;
    if (n == 0) {
        int z = self->mPosZ;
        int y = self->mPosY + 0x50000;
        int x = self->mPosX;
        v[0] = x;
        v[1] = y;
        v[2] = z;
    } else {
        int z = self->mPosZ;
        int y = self->mPosY + n * 0x50;
        int x = self->mPosX;
        v[0] = x;
        v[1] = y;
        v[2] = z;
    }
    if (clsn->IsOnGround()) {
        self->SpawnCoin();
        self->KillAndTrackInDeathTable();
        self->mDeathState = 0;
        return 1;
    }
    self->mDeathParticleID0 =
        (u32)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            self->mDeathParticleID0, 0x13a, *(int *)((char *)v + 0), *(int *)((char *)v + 4),
            *(int *)((char *)v + 8), 0, 0);
    self->mDeathParticleID1 =
        _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
            self->mDeathParticleID1, 0x13b, *(int *)((char *)v + 0), *(int *)((char *)v + 4),
            *(int *)((char *)v + 8), 0);
    return 0;
}

// @symbol _ZN11EnemyDeathC19func_ov002_020ae454ER10dBgCh_Actr
int EnemyDeathC::func_ov002_020ae454(dBgCh_Actr& clsn){
    dEnemyBase_c *c = this;
    dBgCh_Actr *a = &clsn;
    if (c->mDeathTimer == 0 || a->IsOnGround() != 0 || a->IsOnWall() != 0) {
        c->SpawnCoin();
        c->KillAndTrackInDeathTable();
        c->mDeathState = 0;
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
int dEnemyBase_c::IsGoingOffCliff(dBgCh_Actr &clsn_, Fix12i fix2, s16 a3,
                                  bool a4, bool a5, Fix12i fix6)
{
  void *clsn = &clsn_;
  Vector3 v1;
  Vector3 v2;
  mCliffState = 0;
  if (((dBgCh_Actr *)clsn)->IsOnGround() != 0) {
    dBgCh_Lin line;
    v1.x = this->mPosX;
    v1.y = this->mPosY;
    v1.z = this->mPosZ;
    v1.y += fix6;
    v2.x = this->mPosX;
    v2.y = this->mPosY;
    v2.z = this->mPosZ;
    v2.y -= fix2;
    line.SetObjAndLine(v1, v2, this);
    if (a4 != 0)
      line.StartDetectingWater();
    if (line.DetectClsn()) {
      if (line.clsnDist - fix6 >= fix2)
        mCliffState = 1;
      if (a5 == 0) {
        dBgPi result;
        line.CopyTo(result);
        if (result.GetClsnID() != -1) {
          mCliffState = 1;
          return 1;
        }
      }
      Vector3 normal;
      line.surface.CopyNormalTo(normal);
      int idx = ((unsigned short)a3 >> 4) * 2;
      short s = data_02082214[idx + 1];
      if (normal.y < s)
        mCliffState = 2;
    } else {
      mCliffState = 1;
    }
  }
  return this->mCliffState != 0;
}

/* On a wall, reflect the heading off it; on a cliff edge (mCliffState), turn
   around; otherwise report that nothing was done. */
extern "C" {
/* ReflectAngle takes Fix12<int> by value -- the mwccarm 6az wall, runbook
   section 7 -- so it stays extern "C" with scalars in those slots. */
extern short _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(void *actor, int a, int b, short c);
}

// @symbol _ZN12dEnemyBase_c24AngleAwayFromWallOrCliffER10dBgCh_ActrRs
int dEnemyBase_c::AngleAwayFromWallOrCliff(dBgCh_Actr & clsn_, short & outAngle_)
{
    void *clsn = &clsn_;
    short *outAngle = &outAngle_;
    if (((dBgCh_Actr *)clsn)->IsOnWall()) {
        *outAngle = _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(this,
            mWallNormalX, mWallNormalZ, *outAngle);
    } else if (mCliffState) {
        *outAngle = (short)(mPrevAngleY + 0x8000);
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
   speed. Member spellings throughout are byte-identical to the old `self +
   offset` ones (measured). `self` survives for two fields only, which the
   headers name only as unk_: dEnemyBase_c's 0x104 (set to 5 when Yoshi spits
   the enemy out and counted down here; subclasses test it for 5 and 0) and
   dActor_c's 0x0a4/0x0ac (the X/Z partners of mVertSpeed in a velocity vector:
   Vec3_HorzLen of the triple at 0x0a4 becomes mHorzSpeed). Naming them is a
   header rename across their other consumers. */
extern "C" {
extern s32 Vec3_HorzLen(const Vector3 *v);
extern short data_02082214[];
}


// @symbol _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr
int dEnemyBase_c::UpdateYoshiEat(dBgCh_Actr & clsn_)
{
    char *self = (char *)this;
    dBgCh_Actr *clsn = &clsn_;
    int flags;
    int t;
    int v;
    int lim;

    flags = mFlags;
    t = flags & 0x40000;
    t = t != 0;
    if (t != 0) {
        if (mEatingPlayer == 0) {
            mFlags &= ~0x40000;
        } else {
            Vector3 *o = &((dActor_c *)mEatingPlayer)->mPos;
            mPosX = o->x;
            mPosY = o->y;
            mPosZ = o->z;
            mPosY += 0x30000;
        }
        mEatenByYoshi = 0;
        return 2;
    }
    t = flags & 0x20000;
    t = t != 0;
    if (t != 0) {
        mEatenByYoshi = 0;
        return 1;
    }
    t = flags & 0x80000;
    t = t != 0;
    if (t != 0 || mEatenByYoshi != 0) {
        if (*(u16 *)(self + 0x104) != 0)
            *(u16 *)(self + 0x104) -= 1;
        t = mFlags & 0x80000;
        t = t != 0;
        if (t != 0) {
            mEatenByYoshi = 1;
            *(u16 *)(self + 0x104) = 5;
            mFlags &= ~0x80000;
            if (mEatingPlayer != 0) {
                Vector3 *o2 = &((dActor_c *)mEatingPlayer)->mPos;
                mPosX = o2->x;
                mPosY = o2->y;
                mPosZ = o2->z;
                mPosY += 0x32000;
                {
                    dActor_c *o3 = (dActor_c *)mEatingPlayer;
                    u16 ang = o3->mAngleY;
                    int idx = (ang >> 4) << 1;
                    int dist = 0x32000;
                    mPosX += FX_MUL(data_02082214[idx], dist);
                    mPosZ += FX_MUL(data_02082214[idx + 1], dist);
                }
                {
                    Vector3_16 *p = &((dActor_c *)mEatingPlayer)->mAngle;
                    mPrevAngleX = p->x;
                    mPrevAngleY = p->y;
                    mPrevAngleZ = p->z;
                }
                v = ((dActor_c *)mEatingPlayer)->mHorzSpeed;
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
        lim = clsn->GetLimMovFlag();
        clsn->SetLimMovFlag();
        UpdatePos(0);
        dBgCh_Actr_UpdateContinuous_Veneer(clsn);
        if (clsn->IsOnGround() != 0) {
            dBgPi *fr = (dBgPi *)clsn->GetFloorResult();
            fr->surface.CopyNormalTo(*(Vector3 *)&mFloorNormalX);
            if (clsn->JustHitGround() == 0) {
                mVertSpeed = 0;
                mEatenByYoshi = 0;
            } else {
                fr->surface.CopyNormalTo(*(Vector3 *)&mFloorNormalX);
                mVertSpeed = _ZN4cstd4fdivEii((mVertSpeed * -50) / 100, mFloorNormalY);
            }
            {
                int *pa = (int *)(self + 0xa4);
                *pa += mFloorNormalX << 3;
                *(int *)(self + 0xac) += mFloorNormalZ << 3;
                mHorzSpeed = Vec3_HorzLen((const Vector3 *)pa);
            }
        }
        mAngleY = mPrevAngleY;
        if (lim == 0)
            clsn->ClearLimMovFlag();
        return 3;
    }
    mEatingPlayer = 0;
    mFlags &= ~0xe0000;
    mEatenByYoshi = 0;
    return 0;
}

/* While mEatenByYoshi is set, a cylinder collision against anything other than actor
   IDs 0x120/0x121 spawns the mega-character particles; otherwise bit 0x20000 on
   the collision is raised. Clearing mEatenByYoshi clears that bit instead. */
extern "C" {
}

// @symbol _ZN12dEnemyBase_c27SpawnParticlesIfHitOtherObjER5dCc_c
int dEnemyBase_c::SpawnParticlesIfHitOtherObj(dCc_c & clsn_)
{
    dCc_c *clsn = &clsn_;
    u32 *f;
    if (mEatenByYoshi != 0) {
        unsigned int id = clsn->otherOwner;
        if (id != 0) {
            void* a = dActor_c::FindWithID(id);
            if (a != 0) {
                unsigned short t = ((dActor_c *)a)->actorID;
                int e1 = (t == 0x120);
                if (e1 == 0) {
                    int e2 = (t == 0x121);
                    if (e2 == 0) {
                        SpawnMegaCharParticles(*(dActor_c *)a, (char *)clsn);
                        return 1;
                    }
                }
            }
        }
        f = &clsn->flags;
        *f = *f | 0x20000;
        goto done;
    }
    f = &clsn->flags;
    *f = *f & ~0x20000;
done:
    return 0;
}

// @symbol _ZN12dEnemyBase_c22SpawnMegaCharParticlesER8dActor_cPc
extern "C" {
extern void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);
extern s32 Vec3_HorzLen(const Vector3 *v);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
extern s32 *Vec3_AsrInPlace(s32 *v, s32 sh);
}

void dEnemyBase_c::SpawnMegaCharParticles(dActor_c &a, char *p)
{
    dEnemyBase_c *self = this;
    dActor_c *ap = &a;
    s16 dirvec[3];
    Vector3 v;
    Vector3 dst, src;

    dst.x = self->mPosX;
    dst.y = self->mPosY;
    {
        Vector3 *pv = &ap->mPos;
        dst.z = self->mPosZ;
        src.x = pv->x;
        src.y = pv->y;
        src.z = pv->z;
    }

    {
        int t = ((int)(u16)self->mAngleY) >> 4;
        dirvec[0] = data_02082214[t * 2];
        dirvec[1] = 0;
        t = ((int)(u16)self->mAngleY) >> 4;
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

        aX = _ZN4cstd5atan2E5Fix12IiES1_(v.x, v.z);
        aY = _ZN4cstd5atan2E5Fix12IiES1_(v.y, Vec3_HorzLen(&v));

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

    _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        0, 0x43, dst.x, dst.y, dst.z, 0, 0);
    _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        0, 0x44, dst.x, dst.y, dst.z, 0, 0);
    _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        0, 0xbf, dst.x, dst.y, dst.z, dirvec, 0);
    _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        0, 0xc0, dst.x, dst.y, dst.z, 0, 0);
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
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int a, int x, int y, int z);
}

// @symbol _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE
void dEnemyBase_c::KillByInvincibleChar(const Vector3_16 & vel_, Player & player_, Fix12<int>)
{
    mFlags &= ~0x10000001;
    mPrevAngleY = Vec3_HorzAngle(&player_.mPos, &mPos);
    mHorzSpeed = 0xa000;
    mVertSpeed = 0x28000;
    mDeathTimer = 0x1e;
    mSpinRateX = vel_.x;
    mSpinRateY = vel_.y;
    mSpinRateZ = vel_.z;
    mDeathState = 8;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x32000;
    _ZN5Sound9PlayBank0EjRK7Vector3(9, (void *)&mCamSpacePosX);
    volatile int v[3];
    v[0] = mPosX;
    v[1] = mPosY;
    v[2] = mPosZ;
    int ret = OnAimedAtWithEgg();
    int vy = v[1];
    int vx = v[0];
    vy = vy + ret;
    v[1] = vy;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x43, vx, vy, v[2]);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x44, v[0], v[1], v[2]);
    player_.IncMegaKillCount();
}


/* One frame of the death an invincible (mega) character inflicts. `flags` is a
   two-bit selector, not a boolean: bit 0 drops a coin, bit 1 books the kill in
   the death table. Both reference parameters are null-checked -- the compiler
   emits the test rather than folding it away, so `&ref == 0` is how the ROM's
   check is spelled. */
extern "C" {
extern void Vec3_Asr(void *dst, const void *src, int sh);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationZXYExt(void *m, int x, int y, int z);
extern char data_020a0e68;
}

/* Reaches dActor_c vtable slot 29 -- vtable+0x74 -- whose return value, arithmetic
   shifted right by 3, is the height of the point the model spins about. Kept as
   a stand-in rather than dActor_c::OnAimedAtWithEgg() because that slot's NAME is
   imported from a different hierarchy (dScMgBase_c) and does not fit this use;
   flagged, not resolved. */
struct KbicVB {
    virtual void d00();
    virtual void d01();
    virtual void d02();
    virtual void d03();
    virtual void d04();
    virtual void d05();
    virtual void d06();
    virtual void d07();
    virtual void d08();
    virtual void d09();
    virtual void d10();
    virtual void d11();
    virtual void d12();
    virtual void d13();
    virtual void d14();
    virtual void d15();
    virtual void d16();
    virtual void d17();
    virtual void d18();
    virtual void d19();
    virtual void d20();
    virtual void d21();
    virtual void d22();
    virtual void d23();
    virtual void d24();
    virtual void d25();
    virtual void d26();
    virtual void d27();
    virtual void d28();
    virtual int m29();
};

struct KbicM48 { int w[12]; };

// @symbol _ZN12dEnemyBase_c26UpdateKillByInvincibleCharER10dBgCh_ActrR9ModelAnimj
int dEnemyBase_c::UpdateKillByInvincibleChar(dBgCh_Actr & ww_, ModelAnim & mm_, unsigned int flags)
{
    dBgCh_Actr *clsn = &ww_;
    ModelAnim *anim = &mm_;
    int v[3];

    if (mDeathState != 8)
        return 0;

    if (mDeathTimer != 0)
        mDeathTimer -= 1;

    if (mDeathTimer == 0 ||
        (clsn != 0 && clsn->IsOnGround() != 0 && mVertSpeed < 0)) {
        if (flags & 1)
            SpawnCoin();
        if (flags & 2)
            KillAndTrackInDeathTable();
        mDeathState = 0;
        return 2;
    }

    UpdatePos(0);
    if (clsn != 0) {
        UpdateWMClsn(*clsn, 0);
        if (clsn->IsOnWall() != 0)
            mPrevAngleY = _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(
                this, mWallNormalX, mWallNormalZ, mPrevAngleY);
    }

    if (anim != 0) {
        mAngleX += mSpinRateX;
        mAngleY += mSpinRateY;
        mAngleZ += mSpinRateZ;
        Vec3_Asr(v, &mPosX, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, v[0], v[1], v[2]);
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0,
            ((KbicVB *)(void *)this)->m29() >> 3, 0);
        Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68,
            mAngleX, mAngleY, mAngleZ);
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0,
            (-((KbicVB *)(void *)this)->m29()) >> 3, 0);
        *(KbicM48 *)&anim->mat4x3 = *(KbicM48 *)&data_020a0e68;
    }
    return 1;
}

/* The death-state-8 sibling of UpdateKillByInvincibleChar above: the same
   vtable-slot-29 spin over data_020a0e68, but reached as a free helper taking
   the enemy, its collision and its model anim as raw pointers plus the
   coin/death-table flags, and running the wall-reflect variant of the
   collision update. Lowest address in the TU, so it is defined last: this
   file runs highest ROM address first. The slot-29 spin reuses KbicVB. */
extern "C" {
/* local extern: free-call spelling of the dBgCh_Actr const member */
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *o);
/* local extern: free-call spelling of the dBgCh_Actr const member */
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void *o);
/* local extern: free-call spelling of the dEnemyBase_c member */
extern void _ZN12dEnemyBase_c9SpawnCoinEv(void *o);
/* local extern: free-call spelling of the dActor_c member */
extern void _ZN8dActor_c24KillAndTrackInDeathTableEv(void *o);
/* local extern: free-call spelling of the dActor_c member */
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *o, void *cc);
/* local extern: free-call spelling of the dEnemyBase_c member */
extern void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void *o, void *w, unsigned int j);
}

/* Vec3_Asr, the Matrix4x3 helpers and data_020a0e68 reuse the declarations
   above UpdateKillByInvincibleChar. */

// @symbol func_ov002_020ad660
extern "C" int func_ov002_020ad660(void *cc, void *pp, void *r5p, int flags)
{
    dEnemyBase_c *c = (dEnemyBase_c *)cc;
    dBgCh_Actr *p = (dBgCh_Actr *)pp;
    ModelAnim *r5 = (ModelAnim *)r5p;
    int v[3];

    if (c->mDeathState != 8)
        return 0;

    if (c->mDeathTimer == 0 ||
        (p != 0 && (_ZNK10dBgCh_Actr10IsOnGroundEv(p) || _ZNK10dBgCh_Actr8IsOnWallEv(p)))) {
        if (flags & 1)
            _ZN12dEnemyBase_c9SpawnCoinEv(c);
        if (flags & 2)
            _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
        c->mDeathState = 0;
        return 2;
    }

    c->mDeathTimer -= 1;
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, 0);
    if (p != 0)
        _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(c, p, 0);

    if (r5 != 0) {
        c->mAngleX += c->mSpinRateX;
        c->mAngleY += c->mSpinRateY;
        c->mAngleZ += c->mSpinRateZ;
        Vec3_Asr(v, &c->mPos, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, v[0], v[1], v[2]);
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0,
            ((KbicVB *)(void *)c)->m29() >> 3, 0);
        Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68, c->mAngleX, c->mAngleY, c->mAngleZ);
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0,
            (-((KbicVB *)(void *)c)->m29()) >> 3, 0);
        r5->mat4x3 = *(Matrix4x3 *)&data_020a0e68;
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
