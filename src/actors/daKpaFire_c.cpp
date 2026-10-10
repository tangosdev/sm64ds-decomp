//cpp
/* daKpaFire_c: Bowser's Koopa-fire (KOOPAFIRE profile, ov060).
 *
 * The whole unit, ov060 .text 0x02116484..0x02117938, 26 functions: the
 * destructor pair, twenty helpers, CleanupResources, Render, Behavior and
 * InitResources. Eight variants from param1 & 7 (plus a sub-variant at
 * >> 4 & 3): gravity-falling flames with mesh and cylinder collision,
 * drawn by the particle system (Render only reports success). Behavior
 * runs the variant's handler from the data_ov060_0211afb4 PMF table;
 * InitResources runs its data_ov060_0211af74 ActorFn, then finishes setup.
 *
 * The two tables, read out of the ROM (__sinit_ov060_02119df0 fills them from
 * the records at 0x0211a734..0x0211a7ac); each entry is a plain function
 * address:
 *
 *   mVariant  Behavior handler        init              what the handler does
 *   0         func_ov060_0211747c     func_ov060_021167c8  rides Bowser's mouth, spawns variant 5
 *   1         func_ov060_021169f8     func_ov060_02116b18  grows, then splits into variant 2 and dies
 *   2         func_ov060_02116b68     func_ov060_02116c68  flies, lands into variant 7 or 3
 *   3         func_ov060_021167ec     func_ov060_021167c8  one frame: spawns three variant 4
 *   4         func_ov060_021168c4     func_ov060_021169b0  bounces; vanishes near Bowser
 *   5         func_ov060_02116d78     func_ov060_02116f74  sprays from a pitch/heading, lands into variant 6
 *   6         func_ov060_02116f90     func_ov060_0211722c  falls, burns on the ground, ends in a burst
 *   7         func_ov060_02116f90     func_ov060_021171e8  same handler as 6; tossed straight up, scale 6.0, default gravity
 *
 * (The right-hand column is a reading of each handler, not ROM data.) The
 * `FIRE_PARAM(variant, sub)` macro below spells the param1 words the handlers
 * hand to dActor_c::Spawn.
 *
 * The tree used to call this class BowserFire (coined): the cartridge's
 * _ZTS11daKpaFire_c at ov060 0x0211a7c0, _ZTI at 0x0211a7b4 and _ZTV at
 * 0x0211a7f4 name it daKpaFire_c, and daKpaFire_c_classInit builds it
 * for the KOOPAFIRE registry profile.
 *
 * The out-of-line destructor is the key function, so this TU emits the
 * vtable and RTTI (manifest: deadstrip-data against the homed triple).
 * The file is ROM-descending: codegen is deferred, so .text comes out in
 * reverse source order. The destructor alone is bracketed by
 * `#pragma defer_codegen off/on`, so it is generated as it is parsed --
 * ahead of every deferred function -- and comes out D1, D0, then a D2 the
 * cartridge has no home for (manifest: deadstrip). Left deferred it comes
 * out D2, D0, D1 and the ROM's D1-before-D0 pair at 0x02116484 inverts.
 *
 * Known limits:
 * - The twenty func_ov060_* helpers stay free extern "C" functions, but they
 *   now take daKpaFire_c * and read named members instead of bytes (the three
 *   whose signatures decl_common.h fixes -- func_ov060_02116740, _02117624 and
 *   _021172c8 -- keep their char * / unsigned char * parameter and cast on
 *   entry). Several are reached through the variant PMF/ActorFn tables rather
 *   than direct calls, so method conversion still needs per-helper review --
 *   verify alone cannot catch a wrong-this (bytes still match).
 * - func_ov060_02117624 stays free AND parses as C: its Matrix4x3 block
 *   store scalarizes under C++ (same wall as Bullet 020fed7c and ov062
 *   ba84). `#pragma cplusplus off/on` around the definition only.
 * - func_ov060_02116740 and func_ov060_021172c8 stay free: decl_common.h
 *   declares them for other users.
 * - dCcAc_c::Init, dBgCh_Actr::Init, DropShadowRadHeight, Particle::New
 *   and SaveData helpers stay mangled scalar externs (Fix12-by-value
 *   member form is the 6az wall).
 * - Leftover raw offsets, each commented where it occurs: two daKpa_c
 *   fields this class reaches into (+0x410 and the dExtFrameCtrl_c at +0x124) and
 *   the Particle fields at +0x44 / +0x4c / +0x50 (Particle__System.h names
 *   +0x4c callbackVelocity and +0x50 callbackScale; +0x44 is unnamed padding
 *   there; they are reached through a void *, so they stay raw). The
 *   Matrix4x3 scratch at 0x32c is still the header's pad_32c.
 * - Not decoded: the meaning of daKpa_c's mState value 0xf and its +0x410
 *   word beyond what daKpa_c.cpp already says, the sound id 0x180 and the
 *   `3` passed beside it, what DropShadowRadHeight's opacity argument 0xf means (every caller passes it), the 0x380
 *   divisor of variant 5's velocity, what player param1 == 3 means, and
 *   the fire ids in the two particle id pairs (a kind and kind + 1).
 * - The per-variant labels in the table above are descriptions of the
 *   handlers, not recovered names.
 * - data_ov060_0211af74 / 0211afb4 / 0211934c / 02119358 / 02119364 and
 *   g_profile_KOOPAFIRE are not this TU's data.
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "daKpaFire_c.h"
#include "types.h"
#include "common.h"
#include "decl_common.h"
#include "dBgCh_Actr.h"
#include "decl_dBgCh_Actr.h"
#include "dBgCh_Gnd.h"
#include "dActor_c.h"
#include "daKpa_c.h"
#include "Player.h"

/* Local shadow declarations carried from the legacy files verbatim.
 * NOT reconciled against real project headers -- check include/*.h for
 * each of these before compiling; a real header should usually win. */
/* PMF-table views. Behavior calls data_ov060_0211afb4[mVariant].pmf and
 * InitResources calls data_ov060_0211af74[mVariant] as pointer-to-member
 * on this; the legacy shards proved swapping their empty stand-ins for
 * the real dActor_c byte-identical under the pin (same survivor shape as
 * Bullet's 020fed2c keeper). */
struct Vector3_16f;
struct dCc_c;
typedef void (dActor_c::*PMF)();
struct Entry { PMF pmf; };
typedef void (dActor_c::*ActorFn)();

/* Actor ids, symbols/actor_debug_names.tsv. */
enum {
    ACTOR_PLAYER = 191,
    ACTOR_OBJ_MARIO_CAP = 269,
    ACTOR_KOOPA = 279,        /* daKpa_c, the registry profile that classInit names KOOPA */
    ACTOR_KOOPAFIRE = 280,    /* this class */
    ACTOR_COIN = 288
};

/* mVariant values the code compares against directly: 0 (InitResources) and 4
   (Behavior) have names here; func_ov060_02116f90 also tests for 7 and still
   spells it as a bare literal. The rest are told apart only by which handler
   the tables pick (see the banner). */
enum {
    VARIANT_MOUTH = 0,        /* cylinder disabled, no shadow, rides Bowser's mouth (its handler starts no particle) */
    VARIANT_BOUNCING = 4      /* the one whose Behavior keeps its gravity after touching ground */
};

/* The param1 word a spawner hands to a new fire: variant in bits 0..2,
   sub-variant in bits 4..5 (what InitResources reads back out). */
#define FIRE_PARAM(variant, sub) (((sub) << 4) | (variant))

/* Particle ids, named by where they are used, not recovered names. 0x9a, 0x9c
   and 0xa6 are passed to func_ov060_02116518, which also starts id + 1 in the
   second handle. 0x9e goes straight to NewUnkCallback818 (one handle only);
   0x9f is a one-shot NewSimple. */
enum {
    PARTICLE_LANDED_BURN = 0x9c,   /* variants 6/7 once mLanded is set */
    PARTICLE_FALLING = 0x9e,       /* variants 6/7 while airborne */
    PARTICLE_SCALED_9A = 0x9a,     /* variants 1 and 5; the one id that also gets the extra +0x44 / +0x4c sizing */
    PARTICLE_VARIANT_4 = 0xa6,     /* variant 4 */
    PARTICLE_END_PUFF = 0x9f       /* one-shot at the end of variant 6/7 (NewSimple, not a handle) */
};

/* Sound id variant 0 plays through its mSoundHandle; named by use site, not decoded. */
enum { SOUND_ID_MOUTH = 0x180 };

extern "C" {
extern u32 _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f( u32 a, u32 b, Fix12i c, Fix12i d, Fix12i e, const struct Vector3_16f* f);
extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE( u32 a, u32 b, Fix12i c, Fix12i d, Fix12i e, const void* f, void* g);
extern void* _ZN8Particle6System12FromUniqueIDEj(u32 id);
extern void* _ZN8dActor_c13ClosestPlayerEv(void* self);
extern short Vec3_HorzAngle(const struct Vector3* a, const struct Vector3* b);
extern char* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(int a, int b, const void *pos, const void *rot, int e, int f);
extern void _ZN7fBase_c18MarkForDestructionEv(void* a);
extern "C" void func_ov060_02116518(daKpaFire_c *self, u32 kind, int a2, int a3);
extern Fix12i Vec3_HorzDist(const Vector3* a, const Vector3* b);
extern int RandomIntInternal(int* seed);
extern int data_0209e650[];
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(char* self, void* c);
extern void func_ov060_0211712c(daKpaFire_c *p);
extern int func_ov060_021172c8(unsigned char *p, unsigned int n);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *c);
extern int data_ov060_02119358[];
extern int data_ov060_0211934c[];
extern short data_02082214[];
extern void func_ov060_021172e0(daKpaFire_c *self);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, Fix12i x, Fix12i y, Fix12i z);
void* _ZN8dActor_c15FindWithActorIDEjPS_(u32 id, void* prev);
int _ZN8SaveData19IsCharacterUnlockedEj(u32 c);
extern char* _ZN8dActor_c10FindWithIDEj(unsigned int id);
/* local extern: the call passes an untyped this, so it cannot use the header method. */
extern int _ZNK15dExtFrameCtrl_c13GetFrameCountEv(void *anim);
extern u32 _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 a, u32 b, u32 c, void *pos, u32 d);
extern u16 data_ov060_02119364[];
extern "C" Entry data_ov060_0211afb4[];
extern "C" void dBgCh_Actr_UpdateDiscreteNoLava_veneer(void *p);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *actor, int a, int b, unsigned int c, unsigned int d);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, int a, int b, void *v, int c);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(dBgCh_Gnd *self);
extern ActorFn data_ov060_0211af74[];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- daKpaFire_c_classInit, 0x02117938, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol daKpaFire_c_classInit
/* recovered: vtable identified, globals resolved */
/* Reconstructed source-style name: SM64DS proves daKpaFire_c through RTTI,
 * allocation size, vtable identity, and the KOOPAFIRE registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: BowserFire_Spawn.
 *
 * `new daKpaFire_c` is the whole sequence the loose factory spelled by hand:
 * fBase_c::operator new(0x38c), dEnemyBase_c's base constructor, the vptr
 * store, then the three member constructors in declaration order. Highest
 * address in the unit, so first in source (codegen is deferred). */
extern "C" daKpaFire_c *daKpaFire_c_classInit(void)
{
    return new daKpaFire_c;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- _ZN11daKpaFire_c13InitResourcesEv, 0x02117790, size 0x1a8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daKpaFire_c13InitResourcesEv
/* Sets up collision and state (no files of its own; daKpa_c owns the fight's).
 * The ground probe uses the real dBgCh_Gnd (clsnY is seed on entry, hit on
 * exit); the variant dispatch through data_ov060_0211af74 measured
 * byte-identical with the real dActor_c. The `|= 1` sets dCc_c::flags bit 0
 * inside mdCcAc_c when the variant is mouth. The doubled pos.y store is the
 * ROM's own shape. dCcAc_c/dBgCh_Actr Init stay mangled: Fix12-by-value (6az). */
int daKpaFire_c::InitResources()
{
    Vector3 pos;

    if (this->mShadowModel.InitCylinder() == 0)
        return 0;

    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &this->mdCcAc_c, this, 0x28000, 0x50000, 0x200002, 0);   /* radius 40, height 80 units; flags 0x200002, vulnFlags 0 */
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &this->mWithMeshClsn, this, 0x32000, 0x32000, 0, 0);     /* radius and height 50 units; the two Vector3_16 * args are null */

    /* Fix12: gravity -4.0 units per frame per frame, fall speed floor -30.0
       units per frame. The variant's init (below) may overwrite both. */
    this->mVertAccel = -0x4000;
    this->mTerminalVelocity = -0x1e000;
    this->mVariant = this->param1 & 7;
    this->mFrameCount = 0;
    if (this->mVariant == VARIANT_MOUTH)
        this->mDropsShadow = 0;
    else
        this->mDropsShadow = 1;
    this->mLanded = 0;
    this->mSubVariant = ((unsigned int)this->param1 >> 4) & 3;
    /* flags bit 0 set = this cylinder is disabled (dCc_c::flags). */
    if (this->mVariant == VARIANT_MOUTH)
        this->mdCcAc_c.flags |= 1;
    this->mFireScale = 0x2000;      /* 2.0 */
    this->mParticleHandle_380 = 0;
    this->mParticleHandle_37c = this->mParticleHandle_380;
    this->mKpaUniqueID = 0;

    /* constructed here (not at function top: the ROM constructs after the
       collider setup), destroyed at the single exit below -- both synthesized */
    dBgCh_Gnd rc;
    {
        int p60;
        pos.x = this->mPosX;
        p60 = this->mPosY;
        pos.y = p60;
        pos.z = this->mPosZ;
        pos.y = p60 + 0x32000;      /* search seed: 50 units above the fire */
    }
    rc.SetObjAndPos(pos, 0);
    if (rc.DetectClsn())
        this->mGroundY = rc.clsnY;
    else
        this->mGroundY = this->mPosY;

    /* the variant's init, from the ROM-filled table */
    (((dActor_c *)this)->*data_ov060_0211af74[this->mVariant])();

    this->mSoundHandle = 0;
    this->mSoundID = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- _ZN11daKpaFire_c8BehaviorEv, 0x021176d4, size 0xbc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daKpaFire_c8BehaviorEv
/* Once per frame: bump mTickCount, run the variant's handler from the
 * data_ov060_0211afb4 table, bump mFrameCount, then -- while gravity is still
 * on -- step the mesh collision and, for every variant but the bouncing one,
 * stop the fall (speed and accel zero) the first frame it is on the ground.
 * Then the Player-burn check, the shadow update, and the cylinder's Clear /
 * Update. Always returns 1. */
int daKpaFire_c::Behavior()
{
    mTickCount += 1;
    (this->*data_ov060_0211afb4[mVariant].pmf)();
    mFrameCount += 1;
    if (mVertAccel != 0) {
        dBgCh_Actr_UpdateDiscreteNoLava_veneer(&mWithMeshClsn);
        if (mVariant != VARIANT_BOUNCING) {
            if (mWithMeshClsn.IsOnGround() != 0) {
                mVertSpeed = 0;
                mVertAccel = 0;
            }
        }
    }
    func_ov060_02116740((char *)this);
    func_ov060_02117624((char *)this);
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- _ZN11daKpaFire_c6RenderEv, 0x021176cc, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daKpaFire_c6RenderEv
/* recovered: shared header, real C++ method
 *
 * `return 1` and nothing else -- the whole ROM body is `mov r0,#1; bx lr`.
 * The flame is drawn by the particle system, so the render slot only has to
 * report success.
 */
int daKpaFire_c::Render()
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- _ZN11daKpaFire_c16CleanupResourcesEv, 0x021176c4, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daKpaFire_c16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * `return 1` with no releases, which is the finding rather than a stub:
 * daKpaFire_c holds no SharedFilePtr of its own. daKpa_c loads and frees the
 * whole fight's files -- 0x1c models, six more, and three singles -- and the
 * fire it breathes borrows from that set without taking a reference.
 */
int daKpaFire_c::CleanupResources()
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov060_02117624, 0x02117624, size 0xa0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov060_02117624
/* C block store (same wall as Bullet 020fed7c and ov062 ba84): the Matrix4x3
 * copy scalarizes under C++. Parsed as C; the TU returns to C++ after it. */
/* Per-frame drop shadow. Does nothing when mDropsShadow is 0. Otherwise it
 * builds a translation matrix in the shared scratch matrix data_020a0e68 at
 * the fire's x, its mGroundY and its z (each >> 3), copies it into the
 * Matrix4x3 at 0x32c of the actor, and draws the dExtShadowModel_c at 0x304 with
 * radius mShadowRadiusMul * mFireScale, the second Fix12 argument (dActor_c.h
 * calls it `depth`) 30 units (0x1e000), and the `opacity` argument 0xf (what
 * that value means is not decoded; every caller passes 0xf).
 * Parsed as C, so the class is named `struct daKpaFire_c`; the Matrix4x3 at
 * 0x32c is still the header's pad_32c bytes, reached through a cast. */
/* recovered: shared common types */
#include "common.h"
extern "C" {

void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(void *thisp, void *sm, void *mtx, int rad, int t, unsigned int j);
extern Matrix4x3 data_020a0e68;
}
#pragma cplusplus off
void func_ov060_02117624(char *c) {
    struct daKpaFire_c *self = (struct daKpaFire_c *)c;
    if (self->mDropsShadow == 0) return;
    Matrix4x3_FromTranslation(&data_020a0e68, self->mPosX>>3, self->mGroundY>>3, self->mPosZ>>3);
    *(Matrix4x3*)self->pad_32c = data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(self, &self->mShadowModel, self->pad_32c, self->mShadowRadiusMul * self->mFireScale, 0x1e000, 0xf);
}
#pragma cplusplus on

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov060_0211747c, 0x0211747c, size 0x1a8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov060_0211747c
/* Variant 0 Behavior: ride Bowser's mouth.
 *
 * Looks up the daKpa_c by mKpaUniqueID; if it is gone, destroys this fire.
 * Otherwise it does nothing unless Bowser's mState is 0xf and
 * func_ov060_02111c68 returns a row number n -- from the first of two known
 * animations only once its frame is 0x31 or later (n = frame - 0x31), from the
 * second always (n = frame + 0xb) -- or -1 when neither is playing. n wraps to 0 when it reaches the
 * frame count of the dExtFrameCtrl_c at Bowser +0x124.
 *
 * Row n of the u16 table data_ov060_02119364 (five halfwords per row, t[0..4])
 * gives the mouth pose. In whole units, turned by Bowser's yaw (mAngleY >> 4,
 * times two, indexes the sine table; s0 and s1 are the two entries there):
 * t[0] sideways, t[2] + 20 forward, and t[1] - 90 up. t[4] + 0x818 becomes
 * the fire's pitch word (0x92) and t[3] + yaw - 0x4700 its heading word
 * (0x94). The fire moves to Bowser's position plus that offset and plays
 * sound SOUND_ID_MOUTH on mSoundHandle (the handle is reset when the id
 * changes). On every second row (n even) it also spawns a variant 5 fire at
 * its own position, passing its own pitch/heading words as the spawn rotation.
 *
 * Bowser's position goes through v[] first (volatile, so the reads keep their
 * ROM order). Raw offsets left: Bowser +0x124 (the dExtFrameCtrl_c inside his
 * mModelAnim); his yaw is read unsigned through mAngleY. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov060_0211747c(daKpaFire_c *self)
{
    daKpa_c *o;
    int n;
    int ang;
    int i;
    u16 *t;
    int s1;
    int s0;
    volatile int v[3];
    int a;
    int b;
    int *p;

    o = (daKpa_c *)_ZN8dActor_c10FindWithIDEj(self->mKpaUniqueID);
    if (o == 0) {
        _ZN7fBase_c18MarkForDestructionEv(self);
        return;
    }

    if (o->mState != 0xf)
        return;

    n = o->func_ov060_02111c68();
    if (n < 0)
        return;

    ang = *(u16 *)&o->mAngleY;
    i = (ang >> 4) * 2;
    p = &o->mPosX;
    v[0] = p[0];
    v[1] = p[1];
    s1 = data_02082214[i + 1];
    s0 = data_02082214[i];
    v[2] = p[2];
    t = data_ov060_02119364;

    if (n == _ZNK15dExtFrameCtrl_c13GetFrameCountEv((char *)o + 0x124))
        n = 0;

    t += n * 5;

    if (self->mSoundID != SOUND_ID_MOUTH)
        self->mSoundHandle = 0;
    self->mSoundID = SOUND_ID_MOUTH;
    self->mSoundHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(
        self->mSoundHandle, 3, self->mSoundID, &self->mCamSpacePosX, 0);

    a = t[0];
    b = t[2] + 0x14;
    self->mPosX = v[0] + (b * s0 + a * s1);
    self->mPosY = v[1] + ((t[1] - 0x5a) << 12);
    self->mPosZ = v[2] + (b * s1 - a * s0);
    self->mPrevAngleX = (short)(t[4] + 0x818);
    self->mPrevAngleY = (short)(t[3] + ang - 0x4700);

    if (n & 1)
        return;

    _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(ACTOR_KOOPAFIRE, FIRE_PARAM(5, 0), &self->mPosX, &self->mPrevAngleX,
                                                 self->mAreaId, -1);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov060_021172e0, 0x021172e0, size 0x19c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov060_021172e0
/* End of a variant 6/7 fire, called once its mFireScale has burnt down to 0 or
 * below: plays the one-shot particle PARTICLE_END_PUFF at the fire's position
 * and destroys the fire. Then, with a 2 in 10 chance (random >> 16, % 10 < 2),
 * it leaves something behind:
 *   - if the nearest player's param1 is 3 (not decoded; daFPknBall_c and
 *     daJango_c make the same test), a daKpa_c exists and its mCapActorAlive
 *     is 0: it spawns a Mario-cap object (param = character << 8 | 0xb, the
 *     character drawn at random among those of the first three that are
 *     unlocked), sets the daKpa_c's mCapActorAlive to 1 if the spawn worked,
 *     and stops -- this branch returns whether or not the spawn worked;
 *   - otherwise it spawns a coin and zeroes the coin's three velocity words. */
/* recovered: shared common types */
extern "C" void func_ov060_021172e0(daKpaFire_c* self)
{
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
        PARTICLE_END_PUFF, *(Fix12i*)&self->mPosX, *(Fix12i*)&self->mPosY, *(Fix12i*)&self->mPosZ);
    _ZN7fBase_c18MarkForDestructionEv(self);

    if ((((u32)RandomIntInternal(&data_0209e650[0]) >> 0x10) % 10) >= 2) {
        return;
    }

    {
        void* p = _ZN8dActor_c13ClosestPlayerEv(self);
        daKpa_c* sb;
        if (p != 0 && ((dActor_c*)p)->param1 == 3 &&
            (sb = (daKpa_c*)_ZN8dActor_c15FindWithActorIDEjPS_(ACTOR_KOOPA, 0)) != 0 &&
            sb->mCapActorAlive == 0) {
            int c;
            int idx;
            int mask = 0;
            for (c = 0; c < 3; c++) {
                if (_ZN8SaveData19IsCharacterUnlockedEj(c) != 0) {
                    mask = (mask | (1 << c)) & 0xff;
                }
            }
            do {
                idx = ((u32)RandomIntInternal(&data_0209e650[0]) >> 0x10) % 3;
            } while ((mask & (1 << idx)) == 0);
            {
                void* r = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                    ACTOR_OBJ_MARIO_CAP, (idx << 8) | 0xb, &self->mPosX,
                    &self->mAngleX, self->mAreaId, -1);
                if (r != 0) {
                    sb->mCapActorAlive = 1;
                }
            }
            return;
        }
    }

    {
        dActor_c* r = (dActor_c*)_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
            ACTOR_COIN, 0, &self->mPosX, 0, self->mAreaId, -1);
        if (r != 0) {
            r->unk_0a4 = 0;
            r->mVertSpeed = 0;
            r->unk_0ac = 0;
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov060_021172c8, 0x021172c8, size 0x18 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov060_021172c8
/* 1 if more than n Behavior frames have passed (mFrameCount > n), else 0.
 * Keeps the unsigned char * parameter decl_common.h declares. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov060_021172c8(unsigned char *p, unsigned int n){
  return ((daKpaFire_c *)p)->mFrameCount > n;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov060_0211722c, 0x0211722c, size 0x9c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov060_0211722c
/* Variant 6 init: random heading (16 random bits); vertical speed 30.0 units
 * with a 2 in 10 chance, else 10.0; horizontal speed 5.0 (0x5000); gravity
 * -1.0 (-0x1000) per frame per frame; mFireScale 1.0 plus a random 0..0xfff
 * (so from 1.0 to just under 2.0); shadow multiplier 0x10. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov060_0211722c(daKpaFire_c *c)
{
    unsigned int r;
    c->mPrevAngleY = (short)((unsigned int)RandomIntInternal(&data_0209e650[0]) >> 0x10);
    r = (unsigned int)RandomIntInternal(&data_0209e650[0]) >> 0x10;
    if (r % 10 < 2)
        c->mVertSpeed = 0x1e000;
    else
        c->mVertSpeed = 0xa000;
    c->mHorzSpeed = 0x5000;
    c->mVertAccel = -0x1000;
    c->mFireScale = (int)(((unsigned int)RandomIntInternal(&data_0209e650[0]) >> 16) & 0xfff) + 0x1000;
    c->mShadowRadiusMul = 0x10;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov060_021171e8, 0x021171e8, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov060_021171e8
/* Variant 7 init: random heading, vertical speed 10.0 units (0xa000), no
 * horizontal speed, mFireScale 6.0 (0x6000), shadow multiplier 0x10. Gravity
 * is left at the InitResources default. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov060_021171e8(daKpaFire_c *c)
{
    unsigned int r = RandomIntInternal(data_0209e650);
    c->mPrevAngleY = r >> 0x10;
    c->mVertSpeed = 0xa000;
    c->mHorzSpeed = 0;
    c->mFireScale = 0x6000;
    c->mShadowRadiusMul = 0x10;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov060_0211712c, 0x0211712c, size 0xbc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov060_0211712c
/* Sideways wobble, called by the variant 2 handler and the variant 6/7
 * handler. Forms a phase angle ((mTickCount + mPhaseOffset) & 0x3f) << 10 --
 * 64 steps of a full turn (0x10000) -- and nudges x and z by products of two
 * sine-table entries each, `<< 2` then `/ 0x1000`:
 *   x += table[2h]     * table[2p]
 *   z += table[2h + 1] * table[2p + 1]
 * with h = mPrevAngleY >> 4 (the heading, read unsigned) and p = phase >> 4. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov060_0211712c(daKpaFire_c *p)
{
    short ang = (short)(((p->mTickCount + p->mPhaseOffset) & 0x3f) << 10);
    int bi = (unsigned short)ang >> 4;
    int *px = &p->mPosX;
    int *pz;
    *px = *px + ((data_02082214[((int)*(unsigned short *)&p->mPrevAngleY >> 4) * 2]
                  * data_02082214[bi * 2]) << 2) / 0x1000;
    pz = &p->mPosZ;
    *pz = *pz + ((data_02082214[((int)*(unsigned short *)&p->mPrevAngleY >> 4) * 2 + 1]
                  * data_02082214[bi * 2 + 1]) << 2) / 0x1000;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov060_02116f90, 0x02116f90, size 0x19c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov060_02116f90
/* Variant 6 and 7 Behavior: a flame that falls, lands, burns down and bursts.
 *
 * Every frame it moves (UpdatePos), clamps its vertical speed to at least
 * -4.0 (-0x4000), and destroys itself once its y is below 0.
 *
 * While mLanded is 0 (airborne): keeps the PARTICLE_FALLING particle at the
 * fire's position, 55 units up (0x37000); sets cylinder flags bit 0 (cylinder
 * disabled); runs the wobble. On the first frame the mesh collision reports the
 * ground it sets mLanded, picks mFireScale (6.0 for variant 7, else 6.0 plus
 * a random even number from 0 to 0x1ffe, so up to just under 8.0), zeroes the
 * horizontal speed, vertical speed and gravity, and forgets both particle
 * handles.
 *
 * Once mLanded is set: shows PARTICLE_LANDED_BURN scaled by mFireScale (y
 * offset 12 * scale), clears flags bit 0 (cylinder enabled), and once
 * mFrameCount exceeds mFireScale * 10 / 0x1000 + 5 it shrinks mFireScale by
 * 0x266 (about 0.15) per frame; at 0 or below it ends in func_ov060_021172e0. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov060_02116f90(daKpaFire_c *self) {
    _ZN8dActor_c9UpdatePosEP5dCc_c((char *)self, 0);
    if (self->mVertSpeed < -0x4000)
        self->mVertSpeed = -0x4000;

    if (self->mLanded == 0) {
        self->mParticleHandle_37c = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
            self->mParticleHandle_37c, PARTICLE_FALLING, self->mPosX, self->mPosY + 0x37000, self->mPosZ, 0);
        self->mdCcAc_c.flags |= 1;
        func_ov060_0211712c(self);
        if (_ZNK10dBgCh_Actr10IsOnGroundEv(&self->mWithMeshClsn)) {
            self->mLanded++;
            if (self->mVariant == 7) {
                self->mFireScale = 0x6000;
            } else {
                self->mFireScale = ((u32)RandomIntInternal(&data_0209e650[0]) >> 16 & 0xfff) * 2 + 0x6000;
            }
            self->mHorzSpeed = 0;
            self->mVertSpeed = 0;
            self->mVertAccel = 0;
            self->mParticleHandle_380 = 0;
            self->mParticleHandle_37c = self->mParticleHandle_380;
        }
    } else {
        func_ov060_02116518(self, PARTICLE_LANDED_BURN, 1, self->mFireScale * 0xc);
        self->mdCcAc_c.flags &= ~1;
        if (self->mFrameCount > self->mFireScale * 0xa / 0x1000 + 5) {
            self->mFireScale -= 0x266;
            if (self->mFireScale <= 0)
                func_ov060_021172e0(self);
        }
    }
    if (self->mPosY < 0)
        _ZN7fBase_c18MarkForDestructionEv(self);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov060_02116f74, 0x02116f74, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov060_02116f74
/* Variant 5 init: horizontal speed is stored as the raw integer 30 (not
 * 30.0 units; variant 5's handler multiplies it straight into the sine
 * table values), mFireScale 2.0 (8192 = 0x2000), shadow multiplier 16. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov060_02116f74(daKpaFire_c *p)
{
    p->mHorzSpeed = 30;
    p->mFireScale = 8192;
    p->mShadowRadiusMul = 16;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov060_02116d78, 0x02116d78, size 0x1fc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov060_02116d78
/* Variant 5 Behavior: a spray flame that is thrown along a pitch and heading
 * (the words spawners leave in 0x92 and 0x94, which dActor_c names
 * mPrevAngleX/Y). It never calls UpdatePos; it moves itself by the three
 * velocity words it writes (the mesh collision step in Behavior is separate).
 *
 * Each frame: mFireScale grows by 0x255 (about 0.146) while below 5.0 (0x5000),
 * and the pitch word (mPrevAngleX, signed) loses 0x200 while above 0x800.
 * The three velocity words come from the speed (mHorzSpeed), the pitch and the
 * heading through the sine table: unk_0a4 = x, mVertSpeed = y (-speed times
 * the pitch's table[2p]), unk_0ac = z; the x and z values are divided by 896
 * (0x380, not decoded). Position moves by those velocities. Shows
 * PARTICLE_SCALED_9A scaled by mFireScale (y offset 12 * scale). When the mesh
 * collision reports ground it spawns a variant 6 fire with a 16 in 17 chance
 * (random >> 16, % 17 != 0) and destroys this one; after 60 frames
 * (mFrameCount >= 0x3c) it destroys itself as well. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov060_02116d78(daKpaFire_c *c)
{

    if (c->mFireScale < 0x5000) c->mFireScale += 0x255;
    if (c->mPrevAngleX > 0x800) c->mPrevAngleX -= 0x200;

    {
        int i0 = (*(unsigned short*)&c->mPrevAngleX) >> 4;
        int i1 = (*(unsigned short*)&c->mPrevAngleY) >> 4;
        i0 = i0 * 2;
        i1 = i1 * 2 + 1;
        int s0 = data_02082214[i0];
        int speed = c->mHorzSpeed;
        int s1 = data_02082214[i1];
        c->unk_0a4 = (s1 * (speed * s0)) / 896;
    }
    {
        int speed = c->mHorzSpeed;
        int s0 = data_02082214[((*(unsigned short*)&c->mPrevAngleX)>>4)*2];
        c->mVertSpeed = (-speed) * s0;
    }
    {
        int s0 = data_02082214[((*(unsigned short*)&c->mPrevAngleX)>>4)*2];
        int s1 = data_02082214[((*(unsigned short*)&c->mPrevAngleY)>>4)*2];
        int speed = c->mHorzSpeed;
        c->unk_0ac = (s1 * ((-speed) * s0)) / 896;
    }
    c->mPosX += c->unk_0a4;
    c->mPosY += c->mVertSpeed;
    c->mPosZ += c->unk_0ac;

    func_ov060_02116518(c, PARTICLE_SCALED_9A, 1, c->mFireScale * 0xc);
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(&c->mWithMeshClsn) != 0) {
        int r = RandomIntInternal(&data_0209e650[0]);
        unsigned int r3 = (unsigned int)r >> 16;
        if (r3 % 17 != 0)
            _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(ACTOR_KOOPAFIRE, FIRE_PARAM(6, 0), &c->mPosX, 0, c->mAreaId, -1);
        _ZN7fBase_c18MarkForDestructionEv(c);
    }
    if (c->mFrameCount < 0x3c) return;
    _ZN7fBase_c18MarkForDestructionEv(c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov060_02116c68, 0x02116c68, size 0x110 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov060_02116c68
/* Variant 2 init: random heading (16 random bits) and phase; horizontal speed
 * random (r % 10 in 0.5-unit steps, up to 4.5, when mSubVariant is non-zero;
 * in 3.5-unit steps (0x3800), up to 31.5, when it is 0); vertical speed random 0..9
 * whole units; gravity -1.0 (-0x1000); terminal velocity from
 * data_ov060_0211934c[mSubVariant] (-8.0, -6.0, -6.0 for sub-variants 0..2);
 * mFireScale 2.0 (0x2000); shadow multiplier 0x10. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov060_02116c68(daKpaFire_c *c)
{
  unsigned int r;
  c->mPrevAngleY = (short)(((unsigned int)RandomIntInternal(&data_0209e650[0])) >> 0x10);
  if (c->mSubVariant != 0)
  {
    r = ((unsigned int)RandomIntInternal(&data_0209e650[0])) >> 0x10;
    c->mHorzSpeed = (int)((r % 10) << 0xc) >> 1;
  }
  else
  {
    r = ((unsigned int)RandomIntInternal(&data_0209e650[0])) >> 0x10;
    c->mHorzSpeed = (int)((r % 10) * 0x7000) >> 1;
  }

  r = ((unsigned int)RandomIntInternal(&data_0209e650[0])) >> 0x10;
  c->mVertSpeed = (r % 10) << 0xc;
  c->mVertAccel = -0x1000;

  c->mPhaseOffset = (short)(((unsigned int)RandomIntInternal(&data_0209e650[0])) >> 0x10);
  c->mTerminalVelocity = data_ov060_0211934c[c->mSubVariant];
  c->mFireScale = 0x2000;
  c->mShadowRadiusMul = 0x10;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov060_02116b68, 0x02116b68, size 0x100 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov060_02116b68
/* Variant 2 Behavior: a flame in flight. Shows the particle id that
 * data_ov060_02119358[mSubVariant] picks (0xa2, 0xa4, 0xa4 for sub-variants 0..2)
 * 70 units up (0x46000), unscaled; wobbles; destroys itself once mFrameCount
 * exceeds 450 (0x1c2); moves (UpdatePos). When the mesh collision reports
 * ground it spawns a variant 7 fire (mSubVariant 0) or a variant 3 fire
 * (otherwise) at its position and destroys itself; while still airborne it
 * destroys itself once its y is below 0. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov060_02116b68(daKpaFire_c *c)
{
    func_ov060_02116518(c, data_ov060_02119358[c->mSubVariant], 0, 0x46000);
    func_ov060_0211712c(c);
    if (func_ov060_021172c8((unsigned char *)c, 0x1c2))
        _ZN7fBase_c18MarkForDestructionEv(c);
    _ZN8dActor_c9UpdatePosEP5dCc_c((char *)c, 0);
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(&c->mWithMeshClsn)) {
        if (c->mSubVariant == 0)
            _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(ACTOR_KOOPAFIRE, FIRE_PARAM(7, 0), &c->mPosX, 0, c->mAreaId, -1);
        else
            _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(ACTOR_KOOPAFIRE, FIRE_PARAM(3, 0), &c->mPosX, 0, c->mAreaId, -1);
        _ZN7fBase_c18MarkForDestructionEv(c);
    } else {
        if (c->mPosY >= 0) return;
        _ZN7fBase_c18MarkForDestructionEv(c);
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov060_02116b18, 0x02116b18, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov060_02116b18
/* Variant 1 init: vertical speed 7.0 units (0x7000), horizontal speed 17.5
 * (0x11800), gravity +1.0 (0x1000, upward -- unlike the InitResources
 * default), mFireScale 2.0, random phase, shadow multiplier 0x10. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov060_02116b18(daKpaFire_c* c){
  c->mVertSpeed = 0x7000;
  c->mHorzSpeed = 0x11800;
  c->mFireScale = 0x2000;
  c->mVertAccel = 0x1000;
  c->mPhaseOffset = (unsigned int)RandomIntInternal(data_0209e650) >> 0x10;
  c->mShadowRadiusMul = 0x10;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov060_021169f8, 0x021169f8, size 0x120 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov060_021169f8
/* Variant 1 Behavior: grows, then splits and vanishes. mFireScale grows by
 * 0x200 per frame while below 5.0 (0x5000); it moves (UpdatePos) and shows
 * PARTICLE_SCALED_9A at a 120 unit (0x78000) y offset. Once mFrameCount is
 * above 20 (0x14): with mSubVariant 0 it spawns three variant 2 fires
 * (param 2), each with mFireScale set to 5.0; otherwise two, with sub-variants 1
 * and 2 (params 0x12 and 0x22), each with mFireScale 8.0 (0x8000). The
 * children get this fire's pitch/heading words as their spawn rotation and
 * their mFireScale is set after the spawn returns. Then it destroys itself.
 * Before frame 21 it only grows and moves. */
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov060_021169f8(daKpaFire_c* sl)
{
    int i;
    daKpaFire_c* a;
    if (sl->mFireScale < 0x5000) {
        sl->mFireScale += 0x200;
    }
    _ZN8dActor_c9UpdatePosEP5dCc_c((char *)sl, 0);
    func_ov060_02116518(sl, PARTICLE_SCALED_9A, 1, 0x78000);
    if (sl->mFrameCount <= 0x14)
        return;
    if (sl->mSubVariant == 0) {
        for (i = 0; i < 3; i++) {
            a = (daKpaFire_c *)_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(ACTOR_KOOPAFIRE, FIRE_PARAM(2, 0), &sl->mPosX, &sl->mPrevAngleX, sl->mAreaId, -1);
            a->mFireScale = 0x5000;
        }
    } else {
        a = (daKpaFire_c *)_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(ACTOR_KOOPAFIRE, FIRE_PARAM(2, 1), &sl->mPosX, &sl->mPrevAngleX, sl->mAreaId, -1);
        a->mFireScale = 0x8000;
        a = (daKpaFire_c *)_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(ACTOR_KOOPAFIRE, FIRE_PARAM(2, 2), &sl->mPosX, &sl->mPrevAngleX, sl->mAreaId, -1);
        a->mFireScale = 0x8000;
    }
    _ZN7fBase_c18MarkForDestructionEv(sl);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov060_021169b0, 0x021169b0, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov060_021169b0
// recovered name: Bowser_Kill
/* recovered: renamed to Class_Method, declarations from a shared header */
/* recovered: renamed to Class_Method */
/* daKpa_c::Kill - recovered from vtable slot identity */
/* The two labels above do not fit what the function does: data_ov060_0211af74
 * makes it the INIT of variant 4. Vertical speed 30.0 units (0x1e000),
 * horizontal speed 15.0 (0xf000), random phase, SetLimMovFlag on the mesh
 * collision, shadow multiplier 0x10. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov060_021169b0(daKpaFire_c* thiz) {
    thiz->mVertSpeed = 0x1e000;
    thiz->mHorzSpeed = 0xf000;
    thiz->mPhaseOffset = (unsigned int)RandomIntInternal(&data_0209e650[0]) >> 16;
    _ZN10dBgCh_Actr13SetLimMovFlagEv(&thiz->mWithMeshClsn);
    thiz->mShadowRadiusMul = 0x10;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov060_021168c4, 0x021168c4, size 0xec */
/* -------------------------------------------------------------------------- */
// @symbol func_ov060_021168c4
/* Variant 4 Behavior: a bouncing flame that is called off near Bowser.
 *
 * On its first frame it finds the daKpa_c by actor id and remembers its
 * uniqueID in mKpaUniqueID; later frames look it up by that id. Whenever the
 * mesh collision reports it has just hit the ground its vertical speed is
 * set to 30.0 units (0x1e000) -- the bounce. Moves (UpdatePos), shows
 * PARTICLE_VARIANT_4 at a 50 unit (0x32000) y offset, destroys itself once
 * mFrameCount exceeds 150 (0x96). It also destroys itself when the daKpa_c
 * was found, its +0x410 word (daKpa_c.cpp uses it as an index into a
 * pointer-to-member table) is 0, and the horizontal distance to it is under
 * 150 units (0x96000).
 * Raw offset left: daKpa_c +0x410. */
extern "C" void func_ov060_021168c4(daKpaFire_c* c)
{
    daKpa_c* r4;
    if (c->mFrameCount == 0) {
        dActor_c* a = dActor_c::FindWithActorID(ACTOR_KOOPA, 0);
        r4 = (daKpa_c*)a;
        c->mKpaUniqueID = r4->uniqueID;
    } else {
        r4 = (daKpa_c*)dActor_c::FindWithID(c->mKpaUniqueID);
    }
    if (c->mWithMeshClsn.JustHitGround() != 0) {
        c->mVertSpeed = 0x1e000;
    }
    c->UpdatePos((dCc_c*)0);
    func_ov060_02116518(c, PARTICLE_VARIANT_4, 0, 0x32000);
    if (func_ov060_021172c8((unsigned char*)c, 0x96) != 0) {
        c->MarkForDestruction();
    }
    if (r4 == 0) return;
    if (*(int*)((char*)r4 + 0x410) != 0) return;
    if (Vec3_HorzDist((Vector3*)&c->mPosX, (Vector3*)&r4->mPosX) >= 0x96000) return;
    c->MarkForDestruction();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov060_021167ec, 0x021167ec, size 0xd8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov060_021167ec
/* Variant 3 Behavior: acts on its first frame only (mFrameCount 0; after that
 * it does nothing, and it also does nothing if there is no player). Aims at
 * the nearest player (heading = Vec3_HorzAngle to the player), sets mFireScale
 * to 5.0 (0x5000), spawns three variant 4 fires at its position, each with
 * heading = that heading + 0, 0x5555, 0xaaaa (a third of a turn apart, 0x10000
 * being a full turn) and mFireScale copied from this one, then destroys itself.
 * The children are spawned with no rotation and have their pitch/heading words
 * written afterwards. */
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov060_021167ec(daKpaFire_c* c)
{
    dActor_c* p;
    if (c->mFrameCount != 0) return;
    p = (dActor_c*)_ZN8dActor_c13ClosestPlayerEv(c);
    if (p == 0) return;
    c->mPrevAngleY = Vec3_HorzAngle((struct Vector3*)&c->mPosX, (struct Vector3*)&p->mPosX);
    c->mFireScale = 0x5000;
    {
        int i = 0;
        int ang = 0;
        do {
            daKpaFire_c* a = (daKpaFire_c*)_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(ACTOR_KOOPAFIRE, FIRE_PARAM(4, 0), &c->mPosX, (const struct Vector3_16*)0, c->mAreaId, -1);
            short v = c->mPrevAngleY + ang;
            a->mPrevAngleX = 0;
            a->mPrevAngleY = v;
            a->mPrevAngleZ = 0;
            a->mFireScale = c->mFireScale;
            i++;
            ang += 0x5555;
        } while (i < 3);
    }
    _ZN7fBase_c18MarkForDestructionEv(c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov060_021167c8, 0x021167c8, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov060_021167c8
/* Init shared by variants 0 and 3: cylinder flags bit 0 set (cylinder
 * disabled), mFireScale 2.0 (0x2000), shadow multiplier 0x10. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov060_021167c8(daKpaFire_c *c)
{
    c->mdCcAc_c.flags |= 1;
    c->mFireScale = 0x2000;
    c->mShadowRadiusMul = 0x10;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov060_02116740, 0x02116740, size 0x88 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov060_02116740
/* Burn the player the fire's cylinder touched. Reads mdCcAc_c.otherOwner (the
 * dCc_c field Clear zeroes; here it is used as an actor uniqueID, 0 for none)
 * and looks that actor up; if it is the Player (actor id 0xbf = ACTOR_PLAYER)
 * and neither Player::mIsMetal nor Player::mIsVanish is set it calls
 * Player::Burn.
 * Keeps the char * parameter decl_common.h declares. */
extern "C" {
extern void _ZN6Player4BurnEv(void* p);
void func_ov060_02116740(char* c){
  unsigned int id = ((daKpaFire_c *)c)->mdCcAc_c.otherOwner;
  if(id==0) return;
  Player* p = (Player *)_ZN8dActor_c10FindWithIDEj(id);
  if(p==0) return;
  int b = (int)(p->actorID == ACTOR_PLAYER);
  if(b==0) return;
  if(p->mIsMetal!=0) return;
  if(p->mIsVanish!=0) return;
  _ZN6Player4BurnEv(p);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov060_02116518, 0x02116518, size 0x228 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov060_02116518
/* Particle helper. Keeps two particles alive at the fire's position, y raised
 * by a3 (Fix12): `kind` in mParticleHandle_37c and `kind + 1` in
 * mParticleHandle_380 (each handle is passed back in, so a live particle is
 * presumably refreshed rather than duplicated). Kinds 0xa2 and 0xa4 start the first one
 * through the NewUnkCallback818 form of Particle::System::New; the rest use
 * the plain New.
 *
 * With a2 non-zero each live particle is then resized: its word at +0x50
 * is set to mFireScale as a halfword value (0x7fff when mFireScale is 0x8000
 * or more). For kind 0x9a only, its words at +0x44 and +0x4c are also set from
 * mFireScale * 0x2800 and mFireScale * 0xa66, each rounded (+0x800) and shifted
 * down 12 (about * 2.5 and * 0.65; the second is truncated to a halfword
 * first). +0x50 and +0x4c are Particle::System's callbackScale and
 * callbackVelocity in Particle__System.h and +0x44 is unnamed there; all three
 * stay raw because they are reached through a void *. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov060_02116518(daKpaFire_c* self, u32 kind, int a2, int a3)
{
    void* o;

    if (kind == 0xa2 || kind == 0xa4) {
        self->mParticleHandle_37c = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
            self->mParticleHandle_37c, kind, self->mPosX, self->mPosY + a3, self->mPosZ, 0);
    } else {
        self->mParticleHandle_37c = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            self->mParticleHandle_37c, kind, self->mPosX, self->mPosY + a3, self->mPosZ, 0, 0);
    }

    if (self->mParticleHandle_37c != 0 && a2 != 0) {
        o = _ZN8Particle6System12FromUniqueIDEj(self->mParticleHandle_37c);
        if (self->mFireScale >= 0x8000) { *(int*)((char*)o + 0x50) = 0x7fff; } else { *(int*)((char*)o + 0x50) = (short)self->mFireScale; }

        if (kind == PARTICLE_SCALED_9A) {
            *(int*)((char*)o + 0x44) = (int)(((long long)(self->mFireScale) * 0x2800 + 0x800) >> 12);
            *(int*)((char*)o + 0x4c) = (short)(Fix12i)(((long long)(self->mFireScale) * 0xa66 + 0x800) >> 12);
        }
    }

    self->mParticleHandle_380 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        self->mParticleHandle_380, kind + 1, self->mPosX, self->mPosY + a3, self->mPosZ, 0, 0);
    if (self->mParticleHandle_380 == 0)
        return;
    if (a2 == 0)
        return;

    o = _ZN8Particle6System12FromUniqueIDEj(self->mParticleHandle_380);
    if (self->mFireScale >= 0x8000) { *(int*)((char*)o + 0x50) = 0x7fff; } else { *(int*)((char*)o + 0x50) = (short)self->mFireScale; }

    if (kind != PARTICLE_SCALED_9A)
        return;

    *(int*)((char*)o + 0x44) = (int)(((long long)(self->mFireScale) * 0x2800 + 0x800) >> 12);
    *(int*)((char*)o + 0x4c) = (short)(Fix12i)(((long long)(self->mFireScale) * 0xa66 + 0x800) >> 12);
}
}

/* -------------------------------------------------------------------------- */
/* D1 then D0 from one out-of-line definition; the homeless D2 is deadstripped. */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daKpaFire_cD1Ev
// @symbol _ZN11daKpaFire_cD0Ev
/* The compiler writes both bodies: store this vtable, destroy the members in
 * reverse declaration order, chain to dEnemyBase_c, then (D0) the inline
 * operator delete. */
#pragma defer_codegen off
daKpaFire_c::~daKpaFire_c()
{
}
#pragma defer_codegen on

/* ---- the static initializer's objects. __sinit_daKpaFire_c.cpp, emitted from
 * these definitions, copies the 16 .data PMF descriptors at 0x0211a734..
 * 0x0211a7ac into the two 8-slot .bss tables. A member-pointer initializer is
 * not static here, so mwccarm emits a runtime copy for each slot; the
 * descriptors keep their ROM bytes. First word is the handler, second the
 * pointer-adjustor (0). The descriptor order and the copy order are the ROM's
 * own. */
extern PMF data_ov060_0211a734;
extern PMF data_ov060_0211a73c;
extern PMF data_ov060_0211a744;
extern PMF data_ov060_0211a74c;
extern PMF data_ov060_0211a754;
extern PMF data_ov060_0211a75c;
extern PMF data_ov060_0211a764;
extern PMF data_ov060_0211a76c;
extern PMF data_ov060_0211a774;
extern PMF data_ov060_0211a77c;
extern PMF data_ov060_0211a784;
extern PMF data_ov060_0211a78c;
extern PMF data_ov060_0211a794;
extern PMF data_ov060_0211a79c;
extern PMF data_ov060_0211a7a4;
extern PMF data_ov060_0211a7ac;

/* Behavior handler table (mVariant 0..7); one Entry{PMF} per slot. */
extern "C" Entry data_ov060_0211afb4[8] = {
    {data_ov060_0211a794}, {data_ov060_0211a78c}, {data_ov060_0211a76c}, {data_ov060_0211a77c},
    {data_ov060_0211a764}, {data_ov060_0211a774}, {data_ov060_0211a744}, {data_ov060_0211a784}
};
/* InitResources handler table (mVariant 0..7); one ActorFn per slot. */
extern "C" ActorFn data_ov060_0211af74[8] = {
    data_ov060_0211a75c, data_ov060_0211a754, data_ov060_0211a73c, data_ov060_0211a74c,
    data_ov060_0211a734, data_ov060_0211a7ac, data_ov060_0211a7a4, data_ov060_0211a79c
};
