//cpp
/* daYegg_c -- the Yoshi egg (registry profile YOSHI_EGG).
 *
 * The egg a Yoshi carries and throws. The state table at 0x02110a5c holds
 * two member-function pointers per state, enter and execute (see daYegg_State
 * in daYegg_c.h), and the state number is mState: func_ov002_020ed63c installs
 * a state and runs its enter, func_ov002_020ed684 runs the current execute
 * from Behavior. An egg in state 0 trails the player (mPlayer;
 * func_ov002_020ed0d4, func_ov002_020ed5b0); Player::St_YoshiPower_Init takes
 * one from the Player's held-object queue, sets its 0x100 flag bit and enters
 * state 1, where it homes on the nearest actor not yet targeted
 * (func_ov002_020edb3c), and on contact or
 * after five payouts it bursts (func_ov002_020edca4), paying out coins, a
 * blue coin or the tracked star as param1 asks (func_ov002_020ec610 /
 * 020ec628 / 020ec640 read the bits of mParamHigh, which is param1 >> 4).
 * param1 bits 0-1 are the starting state (and the egg's size), bits 4-5 the
 * variant func_ov002_020ec654 tests, bits 6-9 the coin count, bit 10 the blue
 * coin and bit 11 the star.
 *
 * This file is the whole linker unit 0x020ec56c..0x020ee42c, 33 functions:
 * D1 and D0 (src/game/actors/d_a_warpkun.cpp ends exactly at 0x020ec56c
 * below them), the twenty-six helpers func_ov002_020ec610 through
 * func_ov002_020eddc4, CleanupResources, Render, Behavior, InitResources,
 * and last the registry factory daYegg_c_classInit (0x020ee3d8);
 * src/actors/dBgActor_c.cpp starts exactly at 0x020ee42c above it. The
 * out-of-line destructor is the key function, so this TU also emits the
 * vtable and the RTTI.
 *
 * It replaces the one-function sources for _ZN8daYegg_cD1Ev,
 * _ZN8daYegg_cD0Ev, func_ov002_020ec610 .. func_ov002_020eddc4,
 * _ZN8daYegg_c16CleanupResourcesEv, _ZN8daYegg_c6RenderEv,
 * _ZN8daYegg_c8BehaviorEv, _ZN8daYegg_c13InitResourcesEv and
 * daYegg_c_classInit. Each member keeps the provenance notes its source
 * carried.
 *
 * `#pragma defer_codegen off` keeps this file in ROM order. It also scopes
 * opt_strength_reduction off to func_ov002_020edb3c and InitResources, the
 * two members whose sources carried it; with codegen deferred the pragma is
 * last-wins over the whole file.
 *
 * The twenty-four self-first helpers are real daYegg_c members; the two
 * called from Player code (func_ov002_020ed63c by St_YoshiPower_Init,
 * func_ov002_020edca4 by Player.cpp) stay free. All keep their address-based
 * names; the comments say what each does, not what the original called it.
 *
 * deslop leftovers:
 * - the callees with Fix12<int> parameters (ModelAnim::SetAnim,
 *   dCcAc_c::Init, dBgCh_Actr::Init, the shadow drops, Particle::System::New)
 *   stay spelled as mangled extern-C free functions. A real method call homes
 *   a class-typed by-value argument and size-DIFFs the caller
 *   (notes/mwccarm-codegen.md 6az).
 * - still raw offsets: the words written into each spawned coin in
 *   func_ov002_020ec80c (+0x92..+0x98), the DOSUN byte at +0x3a2 in
 *   func_ov002_020ec670, the word at Player+0xc8 in func_ov002_020ed998 and
 *   the dBgCh_Gnd result at +0x44 in func_ov002_020ed7f8.
 * - mSpawnPos is named in daYegg_c.h but its purpose is not known
 *   (written once, never read here); unk_41e and the 4-byte gap at 0x3f8 are
 *   unnamed. dActor_c::unk_0a4/unk_0ac and Player::unk_37c are read through
 *   their unk_ names, and func_ov002_020ed738 reads the floor result +4 raw.
 * - this list is representative, not exhaustive: the flag and mask
 *   literals (mFlags 0x40000/0x10000000/0x8, the hit flags 0x8000/0x26fe0,
 *   the cylinder words 0x202000/0x200002/0xa08000) are also left numeric, and
 *   the extern-C Fix12 callees include SpawnCoins, Player::Hurt,
 *   Particle::NewSimple and RunningSlidingDustAt as well.
 * - the particle, sound, shadow and animation-file arguments are
 *   numeric (0x3f..0x42, 0x2c, 0x93, 0x103, the two shared animation files)
 *   and the egg's flag bits 0x100/0x400/0x2000 are named by use only. The IDs'
 *   names are not recovered. 0x100 is set by Player::St_YoshiPower_Init (on the
 *   egg it takes from the held-object queue); who sets 0x400 and 0x2000 is not
 *   recovered. The "idle" and "moving" animation names say when the code
 *   picks each file, not recovered names.
 */

#pragma defer_codegen off

#include "daYegg_c.h"
#include "types.h"
#include "common.h"
#include "SharedFilePtr.h"
#include "Player.h"

struct dBgPi;

/* Three plain words: the stack vectors the C helpers build, which carry
   none of Vector3's empty destructor. */
struct Vec3i { s32 x, y, z; };
struct Vec3_16f { s16 x, y, z; };

/* The scratch matrix at 0x020a0e68, as twelve words. */
struct M48 { int w[12]; };
struct Spawn { struct Vec3_16f rot; struct Vec3i pos; };

/* func_ov002_020ecd18's vector, copied by hand: the by-value result of the
   followed actor's slot 30 comes back through it. */
struct Vec3 { s32 x, y, z; Vec3() {} Vec3(const Vec3 &o) { x = o.x; y = o.y; z = o.z; } };

/* The followed actor seen through its vtable, as far as func_ov002_020ecd18
   reaches: slot 30 returns a position by value. */
struct VObj {
    virtual void v00();
    virtual void v01();
    virtual void v02();
    virtual void v03();
    virtual void v04();
    virtual void v05();
    virtual void v06();
    virtual void v07();
    virtual void v08();
    virtual void v09();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
    virtual void v25();
    virtual void v26();
    virtual void v27();
    virtual void v28();
    virtual void v29();
    virtual struct Vec3 v30();
};

/* The same actor as func_ov002_020edb3c asks it: slot 20 is a yes/no. */
struct VObjQuery {
    virtual int s0(); virtual int s1(); virtual int s2(); virtual int s3();
    virtual int s4(); virtual int s5(); virtual int s6(); virtual int s7();
    virtual int s8(); virtual int s9(); virtual int s10(); virtual int s11();
    virtual int s12(); virtual int s13(); virtual int s14(); virtual int s15();
    virtual int s16(); virtual int s17(); virtual int s18(); virtual int s19();
    virtual int s20();
};

/* A ground probe on the stack, opaque; its constructor and destructor are
   called by name. */
typedef struct dBgCh_Gnd { char buf[0x68 - 0x18]; } dBgCh_Gnd;

/* Actor registry IDs this file tests or spawns (symbols/actor_debug_names.tsv). */
enum {
    ACTOR_DOSUN       = 0xa1,
    ACTOR_BATAN       = 0xa4,
    ACTOR_BATANKING   = 0xa5,
    ACTOR_SILVER_STAR = 0xb3,
    ACTOR_BOMBKING    = 0xbd,
    ACTOR_HOLHEI      = 0xbe,
    ACTOR_PLAYER      = 0xbf,
    ACTOR_KURIKING    = 0xc6,
    ACTOR_KURIBO_L    = 0xca,
    ACTOR_WANWAN      = 0xdb,
    ACTOR_BLUE_COIN   = 0x122,
    ACTOR_WANWAN2     = 0x151
};

/* The state table at 0x02110a5c: two member-function pointers per state,
   enter and execute, indexed by mState. __sinit_ov002_02107118 fills it. */
typedef void (daYegg_c::*PMF)();
struct Entry { PMF pmf[2]; };
extern Entry data_ov002_02110a5c[];

extern "C" {
/* Defined below. */
void func_ov002_020ed63c(daYegg_c *c, int i);
void func_ov002_020edca4(daYegg_c *self);

/* Data. 0x0210e6b0 and 0x0210eb78 are the two shared animation files; +4 of
   each is the file the shared loader put there. */
extern char data_ov002_0210e6b0[];
extern char data_ov002_0210eb78[];
extern s16 data_ov002_021000a8[];
extern s16 data_02082214[];
extern int data_0209e650;
extern M48 data_020a0e68;

extern int func_0203567c(int p);
extern int func_02035638(u8 *p);
/* local extern: daStar_c::func_ov002_020e7218. This TU does not include daStar_c.h. */
extern void _ZN8daStar_c19func_ov002_020e7218EPci(char *a, char *b, int c);
extern char *data_ov002_021000a0[];
extern void Matrix4x3_FromRotationZXYExt(void *m, int x, int y, int z);
extern int _ZNK5dBgPi9GetClsnIDEv(struct dBgPi *thiz);
extern char *_ZN8dActor_c10FindWithIDEj(u32 id);
extern char *_ZN8dActor_c4NextEPKS_(const void *prev);
extern char *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, const void *v, const void *w, int e, int f);
/* Was spelled `func_02123804`, a name no symbols.txt defines. ov002's relocs
   record the call at 0x020ec718 as `overlays(77,78,79,80)`: four same-base
   overlays each put a function at 0x02123804. The call is gated on actor
   types 0xa4 and 0xa5, BATAN and BATANKING, both daBtn_c in ov079, and
   ov079's function there is daBtn_c's hit reaction, which takes the body and
   the actor that hit it -- the call's (actor, egg). */
/* local extern: daBtn_c::func_ov079_02123804. This TU does not include daBtn_c.h. */
extern void _ZN7daBtn_c19func_ov079_02123804EP8dActor_c(struct daBtn_c *self, dActor_c *other);
extern void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void *thiz, const void *v, unsigned int n, Fix12i f, short s);
extern int RandomIntInternal(int *seed);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *anim, void *file, int a, int b, unsigned int u);
extern void _Z15ApproachLinear2Rsss(s16 *p, short a, short b);
extern int func_ov002_020d5f98(void *c, unsigned char *a, unsigned char *b);
extern int func_ov002_020d6048(void *c);
extern void _ZN7fBase_c18MarkForDestructionEv(void *c);
extern void _ZN5dCc_c5ClearEv(void *p);
extern void _ZN5dCc_c6UpdateEv(void *p);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *p);
extern s32 _ZNK10dBgCh_Actr8IsOnWallEv(void *self);
extern s32 _ZNK10dBgCh_Actr14GetResultFlag1Ev(void *self);
extern void *_ZNK10dBgCh_Actr14GetFloorResultEv(void *self);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *c, void *cc);
extern void dBgCh_Actr_UpdateContinuous_Veneer(void *p);
extern void dBgCh_Actr_UpdateDiscreteNoLava_veneer(void *self);
extern s32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 a, u32 b, s32 x, s32 y, s32 z, struct Vec3_16f *rot, void *cb);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 a, int b, int c, int d);
extern void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(s32 x, s32 y, s32 z);
extern s32 _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 a, u32 b, u32 c, void *v, u32 d);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, const void *pos);
extern s32 _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(void *dst, const void *src, s32 step);
extern s16 Vec3_HorzAngle(const void *a, const void *b);
extern int Vec3_HorzDist(const void *a, const void *b);
extern int Vec3_Dist(const void *a, const void *b);
extern void Vec3_Add(void *out, const void *a, const void *b);
extern void MulVec3Mat4x3(const void *in, const void *m, void *out);
extern void func_0203568c(void *p, int v);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *mc, void *a, Fix12i r, unsigned int h, unsigned int x, unsigned int y);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, int radius, int height, void *a, void *b);
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void Math_Function_0203b14c(void *ptr, int target, int rate, int limit, int step);
extern void ApproachAngle(void *cur, short target, int divisor, int band, int maxStep);
extern void _Z11UpdateAngleRssis(short *a, int b, int c, short d);
extern int func_02010844(void *unused, void *v, short angle);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *self, void *out);
extern void *_ZN9dBgCh_GndC1Ev(dBgCh_Gnd *self);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(dBgCh_Gnd *self, const void *v, void *actor);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(dBgCh_Gnd *self);
extern void _ZN9dBgCh_GndD1Ev(dBgCh_Gnd *self);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void Matrix4x3_FromTranslation(M48 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToTranslation(M48 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationZXYExt(M48 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(M48 *m, int x, int y, int z);
extern int func_ov002_020cf700(void *g);
extern int func_ov002_020d0d2c(void *g);
extern void _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(void *self, void *shadow, void *mtx, int fix, int t, unsigned int n);
extern void _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(void *self, void *shadow, void *mtx, int fix, int t1, int t2, unsigned int n);
extern void _ZN8dActor_c11UntrackStarERa(void *self, signed char *r);
extern unsigned char _ZN8dActor_c9TrackStarEjj(void *self, u32 star, u32 kind);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *self, const void *pos, u32 a, int fix, u32 b, u32 c, u32 d);
extern void LoadBlueCoinModel(void *self);
extern void UnloadBlueCoinModel(void *self);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinals 0 and 1 -- _ZN8daYegg_cD1Ev, 0x020ec56c, size 0x48;
 *                         _ZN8daYegg_cD0Ev, 0x020ec5b4, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_cD1Ev
// @symbol _ZN8daYegg_cD0Ev
/* One native destructor definition emits both ROM variants. D1 is one vtable
 * store and five destructor calls, every one a consequence of
 * `struct daYegg_c : dEnemyBase_c` and the four members that declaration
 * types, destroyed in reverse declaration order, then dEnemyBase_c; the
 * class adds no member with a destructor of its own -- a Player pointer and
 * scalars. D0 is the same destruction followed by the inherited actor-heap
 * operator delete. Being the first out-of-line virtual, it makes this file the
 * key function's home, so the vtable and RTTI are emitted here too. */
daYegg_c::~daYegg_c()
{
}

#ifdef _MSC_VER
/* The host uses the flat ROM D0 name, which MSVC never emits: it folds the
 * Itanium destructor variants into the one ~daYegg_c() above. This arm
 * spells out what the deleting destructor does -- the D1 body, called
 * qualified so it is a direct call, then the class-specific operator delete.
 * Nothing here reaches mwccarm. */
extern "C" daYegg_c *_ZN8daYegg_cD0Ev(daYegg_c *thiz)
{
    thiz->daYegg_c::~daYegg_c();
    daYegg_c::operator delete(thiz);
    return thiz;
}
#endif

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov002_020ec610, 0x020ec610, size 0x18 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_c19func_ov002_020ec610Ev
/* Reads mParamHigh bit 7 (param1 bit 11): nonzero when the egg pays out the
 * tracked star when it bursts. */
int daYegg_c::func_ov002_020ec610(){
  return ((mParamHigh >> 7) & 1) != 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov002_020ec628, 0x020ec628, size 0x18 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_c19func_ov002_020ec628Ev
/* Reads mParamHigh bit 6 (param1 bit 10): nonzero when the egg pays out a
 * blue coin. InitResources loads the blue-coin model for it and
 * CleanupResources unloads it. */
int daYegg_c::func_ov002_020ec628(){
  return ((mParamHigh >> 6) & 1) != 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov002_020ec640, 0x020ec640, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_c19func_ov002_020ec640Ev
/* Reads mParamHigh bits 2-5 (param1 bits 6-9): the number of coins to pay out,
 * 0 to 15. */
unsigned char daYegg_c::func_ov002_020ec640(){
  return (mParamHigh >> 2) & 0xf;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov002_020ec654, 0x020ec654, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_c19func_ov002_020ec654Ev
/* The egg's variant bit: reads mParamHigh bits 0-1 (param1 bits 4-5) and is
 * nonzero when that value is 0 or 2, zero when it is 1 or 3. It chooses the
 * model, the shadow shape (cylinder when zero, cuboid when nonzero), the
 * flight handler in state 1 and the state-1 enter's gravity, target-search
 * range and ground-collision setup (the cylinder it builds is the same for
 * both). What the two variants are in the game is not recovered. */
int daYegg_c::func_ov002_020ec654() {
    return (((mParamHigh & 3) - 1) & 1) != 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov002_020ec670, 0x020ec670, size 0xb8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_c19func_ov002_020ec670Ei
/* What the egg did to the wall it hit. `arg` is the egg's dBgCh_Actr as an
 * integer; func_0203567c turns it into a dBgPi, whose collision ID names the
 * actor owning the surface hit. Returns without doing anything if there is no
 * such ID or actor. A DOSUN gets its byte at +0x3a2 set to 1; a BATAN or
 * BATANKING is handed the egg through daBtn_c's hit reaction. Any other actor
 * is left alone. */
void daYegg_c::func_ov002_020ec670(int arg)
{
    struct dBgPi* cr;
    struct dActor_c* actor;
    u16 type;

    cr = (struct dBgPi*)func_0203567c(arg);
    if (_ZNK5dBgPi9GetClsnIDEv(cr) == -1) return;

    actor = (struct dActor_c*)_ZN8dActor_c10FindWithIDEj(_ZNK5dBgPi9GetClsnIDEv(cr));
    if (actor == 0) return;

    type = actor->actorID;
    {
        int t = (int)(type == ACTOR_DOSUN);
        if (t != 0) {
            *(u8*)((char*)actor + 0x3a2) = 1;
            return;
        }
    }
    {
        int t = (int)(type == ACTOR_BATAN);
        if (t != 0) goto docall;
    }
    {
        int t = (int)(type == ACTOR_BATANKING);
        if (t == 0) return;
    }
docall:
    _ZN7daBtn_c19func_ov079_02123804EP8dActor_c((struct daBtn_c *)actor, this);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov002_020ec728, 0x020ec728, size 0xe4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_c19func_ov002_020ec728Ev
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
/* Pays out once for the current payout slot (mPayoutIdx). Does nothing if the
 * index is already 5 or the slot is already marked paid. Otherwise it spawns
 * the coin count from func_ov002_020ec640 as coins, and one blue coin if
 * func_ov002_020ec628 says so, both at the egg's position raised by 0x28000
 * (40 units), then marks the slot paid. The 0x2000 handed on is 2.0 in
 * Fix12; what that argument controls is not recovered here. */
void daYegg_c::func_ov002_020ec728()
{
    unsigned int idx = mPayoutIdx;
    unsigned int n;
    struct Vector3 vec;
    if (idx >= 5) return;
    if (mPayoutDone[idx] != 0) return;
    n = func_ov002_020ec640();
    {
        int tx = mPosX;
        int tz = mPosZ;
        int ty = mPosY + 0x28000;
        vec.x = tx;
        vec.y = ty;
        vec.z = tz;
    }
    if (n != 0) {
        struct Vector3 v2;
        v2.x = vec.x;
        v2.y = vec.y;
        v2.z = vec.z;
        _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(this, &v2, n, 0x2000, 0);
    }
    if (func_ov002_020ec628()) {
        struct Vector3 v3;
        v3.x = *(int*)&vec.x;
        v3.y = *(int*)&vec.y;
        v3.z = *(int*)&vec.z;
        func_ov002_020ec80c(&v3, 1, 0x2000, 0);
    }
    mPayoutDone[mPayoutIdx] = 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov002_020ec80c, 0x020ec80c, size 0x12c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_c19func_ov002_020ec80cEPviis
/* Spawns `count` blue coins (actor BLUE_COIN, param1 2) at *b in the egg's
 * area. When more than one is asked for, the speed `sl` is raised to at least
 * 0x4000 (4 units). Each coin gets a heading of `arg5` plus a random multiple
 * of 0x800 (a 32nd of a turn) that differs from the previous coin's, and a
 * speed of `sl` scaled by a random 100 to 149 percent. The scaling compounds,
 * because the product is stored back into `sl`. The four fields written at
 * +0x92, +0x94, +0x96 and +0x98 of each new actor (three halfwords and a
 * word) are dActor_c's
 * mPrevAngleX/Y/Z and mHorzSpeed; whether the coin reads them as spawn
 * arguments is not shown here. */
void daYegg_c::func_ov002_020ec80c(void* b, int count, int sl, short arg5)
    {
        unsigned int r;
        int zero;
        char* n;
        int rv;
        int prev;
        int i;
        prev = 0xff;
        if (count > 1) {
            if (sl < 0x4000) sl = 0x4000;
        }
        i = 0;
        if (count <= 0) return;
        zero = i;
        do {
            n = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(ACTOR_BLUE_COIN, 2, b, 0, mAreaId, -1);
            if (n != 0) {
                do {
                    rv = (int)(((unsigned int)RandomIntInternal(&data_0209e650) >> 16) << 27) >> 16;
                } while (rv == prev);
                r = (unsigned int)RandomIntInternal(&data_0209e650);
                r = r >> 16;
                *(short*)(n + 0x92) = zero;
                sl = sl * ((r % 50) + 100) / 100;
                prev = rv;
                *(short*)(n + 0x94) = arg5 + rv;
                *(short*)(n + 0x96) = zero;
                *(int*)(n + 0x98) = sl;
            }
            i++;
        } while (i < count);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov002_020ec938, 0x020ec938, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_c19func_ov002_020ec938Ev
/* State 3 (DROP), execute: moves the egg by its speed and gravity, bursts it
 * if it is on the ground, then runs the ground collision and clears the
 * cylinder's hit record. */
void daYegg_c::func_ov002_020ec938(){
    _ZN8dActor_c9UpdatePosEP5dCc_c(this, &mdCcAc_c);
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(&mWithMeshClsn))
        func_ov002_020edca4(this);
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
    _ZN5dCc_c5ClearEv(&mdCcAc_c);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov002_020ec978, 0x020ec978, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_c19func_ov002_020ec978Ev
/* State 3 (DROP), enter: plays the idle animation (file 0x0210eb78) at normal
 * speed, zeroes the horizontal speed and sets gravity to -0x2000 (-2 units
 * per frame squared). */
void daYegg_c::func_ov002_020ec978() {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(void **)(data_ov002_0210eb78 + 4), 0, 0x1000, 0);
    mHorzSpeed = 0;
    mVertAccel = -0x2000;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov002_020ec9c4, 0x020ec9c4, size 0x110 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_c19func_ov002_020ec9c4Ev
/* State 2 (WOBBLE), execute. While mWobbling is set it steps the X and Z tilt
 * (mAngleX, mAngleZ) toward 0 by 0x400 a frame (a 64th of a turn) and clears
 * mWobbling once the X tilt and the Y angle are both 0. Otherwise it asks
 * func_ov002_020d5f98 about the Player; on a yes, it sets the tilt from
 * the answer (b times two entries of the s16 table at 0x021000a8, indexed by
 * a*2) and sets mWobbling. Then, every frame: destroys the egg if
 * func_ov002_020d6048 says no for the Player, clears and updates the
 * cylinder, moves the egg when it is not on the ground, and runs the ground
 * collision. */
void daYegg_c::func_ov002_020ec9c4(){
  if (mWobbling != 0){
    _Z15ApproachLinear2Rsss(&mAngleX, 0, 0x400);
    _Z15ApproachLinear2Rsss(&mAngleZ, 0, 0x400);
    if (mAngleX == 0){
      if (mAngleY == 0)
        mWobbling = 0;
    }
  } else {
    unsigned char a, b;
    if (func_ov002_020d5f98(mPlayer, &a, &b)){
      unsigned char ip;
      a = a * 2;
      ip = a;
      mAngleX = b * data_ov002_021000a8[ip];
      mAngleZ = b * data_ov002_021000a8[ip + 1];
      mWobbling = 1;
    }
  }
  if (!func_ov002_020d6048(mPlayer))
    _ZN7fBase_c18MarkForDestructionEv(this);
  _ZN5dCc_c5ClearEv(&mdCcAc_c);
  _ZN5dCc_c6UpdateEv(&mdCcAc_c);
  if (!_ZNK10dBgCh_Actr10IsOnGroundEv(&mWithMeshClsn))
    _ZN8dActor_c9UpdatePosEP5dCc_c(this, &mdCcAc_c);
  dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov002_020ecad4, 0x020ecad4, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_c19func_ov002_020ecad4Ev
/* State 2 (WOBBLE), enter: zeroes the horizontal speed and plays the idle
 * animation (file 0x0210eb78) at normal speed. */
void daYegg_c::func_ov002_020ecad4() {
    mHorzSpeed = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(void **)(data_ov002_0210eb78 + 4), 0, 0x1000, 0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov002_020ecb0c, 0x020ecb0c, size 0x20c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_c19func_ov002_020ecb0cEv
/* State 1 (SEEK), execute, for the egg variant func_ov002_020ec654 calls
 * nonzero (see ecf94). If the actor it is seeking (mTargetId) exists and no
 * longer has profile bit 0x10000000, it pays out and re-enters state 1 so a
 * new target is picked. Otherwise, unless daYegg_FLAG_ATTACHED is set: with
 * daYegg_FLAG_DROPPED it enters state 3; with daYegg_FLAG_FLYING it feeds
 * its saved handles back into Particle::System::New (0x2c) and
 * Sound::PlayLong (3, 0x93), runs func_ov002_020ed738 (tilt), returns if
 * func_ov002_020eddc4 handled a contact (nonzero), and bursts if IsOnWall is
 * set or bit 0x10 of the same result byte is. The particle's rotation pair comes from the sine
 * table indexed by mAngleY >> 4 (sin then cos). Every pass that gets this far
 * bursts if func_ov002_020ed6cc reports the egg stalled, and otherwise clears
 * and, if flying, updates the cylinder, moves the egg and runs the ground
 * collision. */
void daYegg_c::func_ov002_020ecb0c()
{
    dActor_c *o;
    u32 id;
    s32 flag;
    s32 b;
    u32 fb0;
    s32 onwall;
    struct Spawn sp;
    s16 rx;
    s16 rz;
    u16 ang;
    struct Vec3_16f *pr;
    s32 pz;
    s32 px;

    id = mTargetId;
    o = 0;
    if (id != 0)
        o = (dActor_c *)_ZN8dActor_c10FindWithIDEj(id);
    if (o != 0) {
        b = (s32)((o->mFlags & 0x10000000) != 0);
        if (b == 0) {
            func_ov002_020ec728();
            func_ov002_020ed63c(this, daYegg_STATE_SEEK);
            return;
        }
    }
    fb0 = mFlags;
    flag = 0;
    b = (s32)((fb0 & daYegg_FLAG_ATTACHED) != 0);
    if (b == 0) {
        b = (s32)((fb0 & daYegg_FLAG_DROPPED) != 0);
        if (b != 0) {
            func_ov002_020ed63c(this, daYegg_STATE_DROP);
        } else {
            b = (s32)((fb0 & daYegg_FLAG_FLYING) != 0);
            if (b != 0) {
                s32 py = *(volatile s32 *)&mPosY + 0x28000;
                pz = mPosZ;
                px = mPosX;
                sp.pos.y = py;
                sp.pos.z = pz;
                sp.pos.x = px;
                ang = *(u16 *)&mAngleY;
                onwall = data_02082214[(ang >> 4) * 2];
                rx = (s16)onwall;
                sp.rot.y = 0;
                pr = &sp.rot;
                sp.rot.x = rx;
                ang = *(u16 *)&mAngleY;
                rz = data_02082214[(ang >> 4) * 2 + 1];
                sp.rot.z = rz;
                mParticleHandle = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                    mParticleHandle, 0x2c, sp.pos.x, sp.pos.y, pz, pr, 0);
                mSoundHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(
                    mSoundHandle, 3, 0x93, &mCamSpacePosX, 0);
                flag = 1;
                func_ov002_020ed738();
                if (func_ov002_020eddc4() != 0)
                    return;
                onwall = _ZNK10dBgCh_Actr8IsOnWallEv(&mWithMeshClsn);
                if ((onwall | func_02035638((u8 *)&mWithMeshClsn)) != 0) {
                    func_ov002_020edca4(this);
                    return;
                }
            }
        }
    }
    if (func_ov002_020ed6cc() != 0) {
        func_ov002_020edca4(this);
        return;
    }
    _ZN5dCc_c5ClearEv(&mdCcAc_c);
    if (flag == 1)
        _ZN5dCc_c6UpdateEv(&mdCcAc_c);
    _ZN8dActor_c9UpdatePosEP5dCc_c(this, &mdCcAc_c);
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov002_020ecd18, 0x020ecd18, size 0x27c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_c19func_ov002_020ecd18Ev
/* State 1 (SEEK), execute, for the other egg variant (see ecf94). `o` is the
 * actor being sought (mTargetId). If it is gone, or has lost profile bit
 * 0x10000000, the egg re-enters state 1 to pick again. Unless
 * daYegg_FLAG_ATTACHED is set: with daYegg_FLAG_DROPPED it enters state 3; with
 * daYegg_FLAG_FLYING it checks the wall hit record first (func_ov002_020ec670,
 * then bursts), then, if it has a target, moves toward the position that
 * actor's vtable slot 30 returns, at mHorzSpeed per step. Having arrived, it
 * bursts when the target is one of the six burst actors (BOMBKING, HOLHEI,
 * KURIKING, KURIBO_L, WANWAN, WANWAN2) and otherwise pays out and re-enters
 * state 1; if not yet there it turns to face the target. Flying then kicks up
 * sliding dust at its position and runs func_ov002_020eddc4 (a nonzero result
 * ends the frame). Every pass bursts if func_ov002_020ed6cc reports the egg
 * stalled, otherwise clears and, if flying, updates the cylinder, moves the
 * egg only when it has no target, and runs the ground collision. */
void daYegg_c::func_ov002_020ecd18()
{
    dActor_c *o;
    s32 flag;
    s32 b;
    u32 fb0;

    o = 0;
    if (mTargetId != 0)
        o = (dActor_c *)_ZN8dActor_c10FindWithIDEj(mTargetId);
    if (o != 0) {
        b = (s32)((o->mFlags & 0x10000000) != 0);
        if (b == 0) {
            func_ov002_020ed63c(this, daYegg_STATE_SEEK);
            return;
        }
    }
    if (mTargetId != 0 && o == 0) {
        func_ov002_020ed63c(this, daYegg_STATE_SEEK);
        return;
    }
    fb0 = mFlags;
    flag = 0;
    b = (s32)((fb0 & daYegg_FLAG_ATTACHED) != 0);
    if (b == 0) {
        b = (s32)((fb0 & daYegg_FLAG_DROPPED) != 0);
        if (b != 0) {
            func_ov002_020ed63c(this, daYegg_STATE_DROP);
        } else {
            b = (s32)((fb0 & daYegg_FLAG_FLYING) != 0);
            if (b != 0) {
                if (_ZNK10dBgCh_Actr14GetResultFlag1Ev(&mWithMeshClsn) != 0) {
                    func_ov002_020ec670((int)&mWithMeshClsn);
                    func_ov002_020edca4(this);
                    return;
                }
                if (o != 0) {
                    struct Vec3 tmp = ((struct VObj *)o)->v30();
                    if (_Z14ApproachLinearR7Vector3RKS_5Fix12IiE(
                            &mPosX, &tmp, mHorzSpeed) != 0) {
                        switch (o->actorID) {
                        case ACTOR_BOMBKING:
                        case ACTOR_HOLHEI:
                        case ACTOR_KURIKING:
                        case ACTOR_KURIBO_L:
                        case ACTOR_WANWAN:
                        case ACTOR_WANWAN2:
                            func_ov002_020edca4(this);
                            return;
                        default:
                            func_ov002_020ec728();
                            func_ov002_020ed63c(this, daYegg_STATE_SEEK);
                            return;
                        }
                    }
                    mAngleY = Vec3_HorzAngle(&mPosX, &tmp);
                }
                _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(
                    mPosX, mPosY, mPosZ);
                flag = 1;
                if (func_ov002_020eddc4() != 0)
                    return;
            }
        }
    }
    if (func_ov002_020ed6cc() != 0) {
        func_ov002_020edca4(this);
        return;
    }
    _ZN5dCc_c5ClearEv(&mdCcAc_c);
    if (flag == 1)
        _ZN5dCc_c6UpdateEv(&mdCcAc_c);
    if (o == 0)
        _ZN8dActor_c9UpdatePosEP5dCc_c(this, &mdCcAc_c);
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov002_020ecf94, 0x020ecf94, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_c19func_ov002_020ecf94Ev
/* State 1 (SEEK), execute: picks the variant's handler. */
void daYegg_c::func_ov002_020ecf94()
{
    if (func_ov002_020ec654())
        func_ov002_020ecb0c();
    else
        func_ov002_020ecd18();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov002_020ecfc8, 0x020ecfc8, size 0x10c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_c19func_ov002_020ecfc8Ev
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
/* State 1 (SEEK), enter. Clears the target, plays the moving animation (file
 * 0x0210e6b0) at normal speed and sets the horizontal speed to 0x64000 (100
 * units). For the variant func_ov002_020ec654 calls zero: no gravity, a
 * target search within 0x7d0000 (2000 units) and func_0203568c on the ground
 * collision with 0x2a000 (42 units; what that sets is not recovered). For the other variant: gravity -0xa000
 * (-10 units per frame squared) and a search within 0xfa0000 (4000 units). If
 * a target is found the egg turns to face it; if not, and it has already
 * targeted at least one actor, the zero variant bursts at once. Finally the
 * cylinder is re-initialised with radius 0x64000 (100 units), height 0xc8000
 * (200 units), own flags 0x202000 and hit mask 0 -- in dCc_c.h's bit table
 * that is the enemy and egg bits, with nothing able to hit it. */
void daYegg_c::func_ov002_020ecfc8(){
  dActor_c* r5;
  mTargetId = 0;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(void **)(data_ov002_0210e6b0 + 4), 0, 0x1000, 0);
  mHorzSpeed = 0x64000;
  if (!func_ov002_020ec654()){
    mVertAccel = 0;
    r5 = (dActor_c*)func_ov002_020edb3c(0, 0x7d0000);
    func_0203568c(&mWithMeshClsn, 0x2a000);
  } else {
    mVertAccel = -0xa000;
    r5 = (dActor_c*)func_ov002_020edb3c(0, 0xfa0000);
  }
  if (r5){
    mAngleY = Vec3_HorzAngle((struct Vector3*)&mPosX, (struct Vector3*)&r5->mPosX);
  } else {
    if (mTargetedCount){
      if (!func_ov002_020ec654())
        func_ov002_020edca4(this);
    }
  }
  _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x64000, 0xc8000, 0x202000, 0);
  mPrevAngleY = mAngleY;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov002_020ed0d4, 0x020ed0d4, size 0x4dc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_c19func_ov002_020ed0d4Ev
// obsolete provenance (this is daYegg_c state 0 execute): recovered name: daWarpkun_c_Kill
/* recovered: shared common types, renamed to Class_Method, declarations from a shared header */
/* recovered: shared common types, renamed to Class_Method */
/* (obsolete provenance) daWarpkun_c::Kill - recovered from vtable slot identity */
/* State 0 (FOLLOW_PLAYER), execute. `com` is the followed Player. Each call:
 *  - copies the Player's 0xa4/0xa8/0xac words and its horizontal speed;
 *  - while unk_41e is zero (nothing in this file sets it nonzero, so every
 *    call) rebuilds the hold target and the goal angles. The target is the
 *    Player's position plus the vector (0, 0, mHoldOffsetZ = -104 units)
 *    rotated by the egg's eased angles. If the Player's word at +0x37c or
 *    func_ov002_020cf700 or func_ov002_020d0d2c is nonzero, the goal X/Z
 *    angles are the Player's when the egg is on the ground and -0x4000 (a
 *    quarter turn) and 0 when it is not. Otherwise they are the Player's, and
 *    the goal Y angle becomes the direction from the Player's previous to its
 *    current position when those are at least 0x5000 (5 units) apart
 *    horizontally;
 *  - eases mPos toward the hold target (Math_Function_0203b14c: a third of
 *    the remaining distance per call, at most 1000 units) and each eased
 *    angle toward its goal (ApproachAngle: an eighth of the remaining angle,
 *    at most 0x4000 a call, at least 0x100), then copies the eased Y angle
 *    into mAngleY;
 *  - sets the animation: the moving one (file 0x0210e6b0) when mHorzSpeed is
 *    nonzero, else the idle one (0x0210eb78), at a speed of 1.0 to 2.0 that
 *    grows with mHorzSpeed and saturates at 0x20000 (32 units); the speed is
 *    0 while the Player's mIsAirborne is set;
 *  - for the variant func_ov002_020ec654 calls zero, rewrites the 0xa4/0xac
 *    words as absolute values capped at 0x20000 (32 units) and tilts mAngleX
 *    (negative) and mAngleZ by up to 0xe39 (20 degrees) in proportion to
 *    them; for the other variant it runs func_ov002_020ed738 instead;
 *  - clears the cylinder, and runs the discrete ground collision unless the
 *    Player's mIsNoControl is set or the Player is 0x190000 (400 units) or
 *    more away. */
void daYegg_c::func_ov002_020ed0d4()
{
    struct Vector3 in, out;
    Player* com;
    int* p;
    volatile struct Vector3 t1, t2;
    struct Vector3 v1, v2;
    struct Vector3 sumA, sumB, sumC;
    int *pt1, *pt2, *pv1, *pv2;

    com = mPlayer;
    mHoldOffsetZ = -0x68000;
    p = &com->unk_0a4;
    unk_0a4 = p[0];
    mVertSpeed = p[1];
    unk_0ac = p[2];
    mHorzSpeed = com->mHorzSpeed;

    if (DecIfAbove0_Byte(&unk_41e) == 0) {
        in.x = 0; in.y = 0; in.z = 0;
        out.x = 0; out.y = 0; out.z = 0;
        in.z = mHoldOffsetZ;

        Matrix4x3_FromRotationZXYExt(&data_020a0e68, mEasedAngleX, mEasedAngleY, mEasedAngleZ);
        MulVec3Mat4x3(&in, &data_020a0e68, &out);

        if (*(int*)&mPlayer->unk_37c != 0
            || func_ov002_020cf700(mPlayer) != 0
            || func_ov002_020d0d2c(mPlayer) != 0) {
            if (_ZNK10dBgCh_Actr10IsOnGroundEv(&mWithMeshClsn) != 0) {
                mGoalAngleX = mPlayer->mAngleX;
                mGoalAngleZ = mPlayer->mAngleZ;
                pt1 = &com->mPrevPosX;
                pt2 = &com->mPosX;
                t1.x = pt1[0];
                t1.y = pt1[1];
                t1.z = pt1[2];
                t2.x = pt2[0];
                t2.y = pt2[1];
                t2.z = pt2[2];
                Vec3_Add(&sumA, &com->mPosX, &out);
                mHoldTargetX = sumA.x;
                mHoldTargetY = sumA.y;
                mHoldTargetZ = sumA.z;
            } else {
                mGoalAngleX = -0x4000;
                mGoalAngleZ = 0;
                Vec3_Add(&sumB, &com->mPosX, &out);
                mHoldTargetX = sumB.x;
                mHoldTargetY = sumB.y;
                mHoldTargetZ = sumB.z;
            }
        } else {
            mGoalAngleX = com->mAngleX;
            mGoalAngleZ = com->mAngleZ;
            pv1 = &com->mPrevPosX;
            pv2 = &com->mPosX;
            v1.x = pv1[0];
            v1.y = pv1[1];
            v1.z = pv1[2];
            v2.x = pv2[0];
            v2.y = pv2[1];
            v2.z = pv2[2];
            if (Vec3_HorzDist(&v1, &v2) >= 0x5000) {
                mGoalAngleY = Vec3_HorzAngle(&v1, &v2);
            }
            Vec3_Add(&sumC, &com->mPosX, &out);
            mHoldTargetX = sumC.x;
            mHoldTargetY = sumC.y;
            mHoldTargetZ = sumC.z;
        }
        unk_41e = 0;
    }

    {
        /* 0x1000 / 0x3000 as Fix12: a third. */
        int fd = _ZN4cstd4fdivEii(0x1000, 0x3000);
        Math_Function_0203b14c(&mPosX, mHoldTargetX, fd, 0x3e8000, 4);
        Math_Function_0203b14c(&mPosY, mHoldTargetY, fd, 0x3e8000, 4);
        Math_Function_0203b14c(&mPosZ, mHoldTargetZ, fd, 0x3e8000, 4);
    }

    ApproachAngle(&mEasedAngleX, mGoalAngleX, 8, 0x4000, 0x100);
    ApproachAngle(&mEasedAngleY, mGoalAngleY, 8, 0x4000, 0x100);
    ApproachAngle(&mEasedAngleZ, mGoalAngleZ, 8, 0x4000, 0x100);

    mAngleY = mEasedAngleY;

    {
        int v98 = mHorzSpeed;
        int t = v98 >> 5;
        if (t > 0x1000) t = 0x1000;
        if (t < 0) t = 0;
        t = (t + 0x1000) >> 8;
        unsigned char byteVal = (unsigned char)t;
        int idx = byteVal & 0x3f;
        int speed = idx << 8;
        if (mPlayer->mIsAirborne != 0) speed = 0;
        if (v98 != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(void **)(data_ov002_0210e6b0 + 4), 0, speed, 0);
        } else {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(void **)(data_ov002_0210eb78 + 4), 0, speed, 0);
        }
    }

    if (func_ov002_020ec654() == 0) {
        int a4 = unk_0a4;
        if (a4 < 0) a4 = -a4;
        unk_0a4 = a4;
        int ac = unk_0ac;
        if (ac < 0) ac = -ac;
        unk_0ac = ac;
        a4 = unk_0a4;
        if (a4 > 0x20000) { a4 = 0x20000; unk_0a4 = a4; }
        a4 = unk_0a4;
        {
            int fd1 = _ZN4cstd4fdivEii(a4, 0x20000);
            int r1 = (int)(((long long)fd1 * 0xe39 + 0x800) >> 12);
            short a4ang = (short)(-r1);
            ac = unk_0ac;
            if (ac > 0x20000) { ac = 0x20000; unk_0ac = ac; }
            ac = unk_0ac;
            {
                int fd2 = _ZN4cstd4fdivEii(ac, 0x20000);
                int r2 = (int)(((long long)fd2 * 0xe39 + 0x800) >> 12);
                mAngleX = a4ang;
                mAngleZ = (short)r2;
            }
        }
    } else {
        func_ov002_020ed738();
    }

    _ZN5dCc_c5ClearEv(&mdCcAc_c);
    {
        int dist = Vec3_Dist(&mPosX, &com->mPosX);
        if (mPlayer->mIsNoControl != 0) return;
        if (dist >= 0x190000) return;
        dBgCh_Actr_UpdateDiscreteNoLava_veneer(&mWithMeshClsn);
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov002_020ed5b0, 0x020ed5b0, size 0x8c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_c19func_ov002_020ed5b0Ev
// State 0 (FOLLOW_PLAYER), enter. Snaps this object to the actor it follows
// (mPlayer): copies its position (mPosX/Y/Z) and Y angle (mAngleY), mirrors
// the position into mHoldTarget, and copies its rotation triple (mAngleX/Y/Z)
// into both the goal angles and the eased angles.
void daYegg_c::func_ov002_020ed5b0()
{
    Player* src;
    int* sp;
    short* m;
    src = mPlayer;
    sp = &src->mPosX;
    mPosX = sp[0];
    mPosY = sp[1];
    mPosZ = sp[2];
    src = mPlayer;
    mAngleY = src->mAngleY;
    mHoldTargetX = mPosX;
    mHoldTargetY = mPosY;
    mHoldTargetZ = mPosZ;
    src = mPlayer;
    m = &src->mAngleX;
    mGoalAngleX = m[0];
    mGoalAngleY = m[1];
    mGoalAngleZ = m[2];
    src = mPlayer;
    m = &src->mAngleX;
    mEasedAngleX = m[0];
    mEasedAngleY = m[1];
    mEasedAngleZ = m[2];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov002_020ed63c, 0x020ed63c, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020ed63c
/* Switches to state `i` and runs that state's enter handler. Player's
 * St_YoshiPower_Init calls it with state 1 on the egg it holds. */
extern "C" void func_ov002_020ed63c(daYegg_c *c, int i) { c->mState = i; int j = c->mState; (c->*data_ov002_02110a5c[j].pmf[0])(); }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov002_020ed684, 0x020ed684, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_c19func_ov002_020ed684Ev
/* Runs the current state's execute handler. */
void daYegg_c::func_ov002_020ed684() { int j = mState; (this->*data_ov002_02110a5c[j].pmf[1])(); }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov002_020ed6cc, 0x020ed6cc, size 0x6c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_c19func_ov002_020ed6ccEv
/* recovered: shared common types */
/* Stall detector. Measures the horizontal distance from the position saved
 * on the previous call (mLastPos) to the current one, then saves the current
 * position. A move of 0x32000 (50 units) or more reloads mStallTimer to 0xf
 * and returns 0. A shorter move counts the timer down, and the function
 * returns 1 whenever the timer is zero after the countdown (first reached
 * after 15 shorter moves in a row; it stays 1 while the egg stays put), else
 * 0. */
int daYegg_c::func_ov002_020ed6cc() {
  Fix12i d = Vec3_HorzDist((struct Vector3*)&mLastPosX, (struct Vector3*)&mPosX);
  mLastPosX = mPosX;
  mLastPosY = mPosY;
  mLastPosZ = mPosZ;
  if (d >= 0x32000) goto fail;
  if (DecIfAbove0_Byte(&mStallTimer)) goto ret0;
  return 1;
fail:
  mStallTimer = 0xf;
ret0:
  return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov002_020ed738, 0x020ed738, size 0xc0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_c19func_ov002_020ed738Ev
/* recovered: shared common types */
/* Eases the X and Z tilt (mAngleX, mAngleZ) toward a target pair, each by a
 * quarter of the remaining angle, at most 0x1000 (a 16th of a turn) a call
 * (UpdateAngle). On the ground the pair comes from the floor normal
 * (func_02010844 evaluated at the Y angle and at the Y angle minus 0x4000, a
 * quarter turn). In the air it is the Player's own X and Z angles when mState
 * is 0 and there is a Player, and 0 otherwise. */
void daYegg_c::func_ov002_020ed738() {
    int e4 = 0;
    int e6 = 0;
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(&mWithMeshClsn)) {
        struct Vector3 n;
        void* fr = _ZNK10dBgCh_Actr14GetFloorResultEv(&mWithMeshClsn);
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)fr+4, &n);
        e4 = func_02010844(this, &n, mAngleY);
        e6 = func_02010844(this, &n, (short)(mAngleY - 0x4000));
    } else {
        if (mState == 0) {
            Player* p = mPlayer;
            if (p != 0) {
                e4 = p->mAngleX;
                e6 = p->mAngleZ;
            }
        }
    }
    _Z11UpdateAngleRssis(&mAngleX, e4, 4, 0x1000);
    _Z11UpdateAngleRssis(&mAngleZ, e6, 4, 0x1000);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov002_020ed7f8, 0x020ed7f8, size 0x1a0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_c19func_ov002_020ed7f8Ev
/* Drop shadow, called from Behavior. Skipped when the followed Player's
 * mOpacity is below 1. It probes the floor with a dBgCh_Gnd from 0x28000 (40
 * units) above the egg (its +0x44 is the floor height, dBgCh_Gnd's clsnY);
 * r5 is the egg's height over that floor when one was found (the probe's start
 * point otherwise), at least 0x1000 (1 unit), and r4 the
 * shadow size, 0x50000 (80 units) less 3/32 of r5 and never below 0xa000 (10
 * units). It fills mShadowMtx with a Y rotation and the position >> 3, then
 * draws the shadow, cylinder or cuboid by the variant bit, unless the egg's
 * flags have 0x40000 (dActor_c.h lists 0x020000 / 0x040000 as yoshi-mouth
 * states written by actor code), or the Player's word at +0x37c is nonzero, or either of
 * func_ov002_020cf700 / func_ov002_020d0d2c says so for the Player. */
void daYegg_c::func_ov002_020ed7f8()
{
    dBgCh_Gnd rg;
    struct Vec3i v;
    int r5;
    int r4;
    int b;

    if (mPlayer->mOpacity < 1)
        return;

    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    v.y += 0x28000;
    _ZN9dBgCh_GndC1Ev(&rg);
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, &v, 0);
    r4 = v.y;
    if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg))
        r4 = *(int*)((char*)&rg + 0x44);
    r5 = mPosY - r4;
    if (r5 <= 0x1000) r5 = 0x1000;
    r4 = 0x50000 - (int)(((long long)r5 * 0x180 + 0x800) >> 12);
    if (r4 < 0xa000) r4 = 0xa000;
    Matrix4x3_FromRotationY(mShadowMtx, mAngleY);
    mShadowMtx[9] = mPosX >> 3;
    mShadowMtx[10] = mPosY >> 3;
    mShadowMtx[11] = mPosZ >> 3;
    b = (mFlags & 0x40000) ? 1 : 0;
    if (b == 0
        && *(int*)&mPlayer->unk_37c == 0
        && !func_ov002_020cf700(mPlayer)
        && !func_ov002_020d0d2c(mPlayer))
    {
        if (func_ov002_020ec654() == 0) {
            _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
                this, &mShadowModel, mShadowMtx, r4, r5 + 0x28000, 0xf);
        } else {
            _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(
                this, &mShadowModel, mShadowMtx, r4, r5 + 0x28000, r4, 0xf);
        }
    }
    _ZN9dBgCh_GndD1Ev(&rg);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov002_020ed998, 0x020ed998, size 0x1a4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_c19func_ov002_020ed998Ev
/* Places the egg's model matrix, called from Behavior. With
 * daYegg_FLAG_ATTACHED set and a Player, it copies the 12-word matrix the
 * word at Player+0xc8 points to (that field is padding in dActor_c.h), shifts
 * its translation by (0x3f, 9, 0xb) and applies a ZXY rotation of
 * (-0x49f5, -0xc17, -0x293f), then moves the egg's mPos to the resulting
 * translation << 3 and returns; matrix units are mPos >> 3 throughout.
 * Otherwise it builds the matrix from the egg's own pose: the translation is
 * mPos with 0x14000 (20 units) added to Y, scaled by 1/8 (x << 9 >> 12, rounded),
 * rotated by (mAngleX, mAngleY, mAngleZ), and stored into the model's own
 * matrix (mModelAnim.mat4x3). */
void daYegg_c::func_ov002_020ed998()
{
    int on = (mFlags & daYegg_FLAG_ATTACHED) != 0;
    if (on && *(int *)&mPlayer) {
        volatile struct Vec3i v;
        int t9, t10, t11;
        v.x = 0;
        v.y = 0;
        v.z = 0;
        data_020a0e68 = *(M48 *)(*(char **)((char *)mPlayer + 0xc8));
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0x3f, 9, 0xb);
        Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68, 0xffffb60b, 0xfffff3e9, 0xffffd6c1);
        t9  = ((volatile M48 *)&data_020a0e68)->w[9];
        t10 = ((volatile M48 *)&data_020a0e68)->w[10];
        t11 = ((volatile M48 *)&data_020a0e68)->w[11];
        v.y = t10;
        v.z = t11;
        v.x = t9;
        mPosX = t9 << 3;
        mPosY = v.y << 3;
        mPosZ = v.z << 3;
        return;
    }
    Matrix4x3_FromTranslation(&data_020a0e68,
        (int)((((long long)mPosX << 9) + 0x800) >> 12),
        (int)((((long long)(mPosY + 0x14000) * 0x200) + 0x800) >> 12),
        (int)((((long long)mPosZ << 9) + 0x800) >> 12));
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        mAngleX, mAngleY, mAngleZ);
    *(M48 *)&mModelAnim.mat4x3 = data_020a0e68;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov002_020edb3c, 0x020edb3c, size 0x168 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_c19func_ov002_020edb3cEii
/* Target search. If five actors have already been targeted it bursts the egg
 * (for the variant func_ov002_020ec654 calls zero) and returns 0. Otherwise it
 * walks the actor list for the nearest actor, closer than `best` (the
 * caller's range), that is not the egg or the followed Player, has profile
 * bit 0x10000000 and not the off-screen bit 0x8, is not in mTargetedIds and,
 * for the other variant, answers nonzero to vtable slot 20. The winner's
 * uniqueID is appended to mTargetedIds, becomes mTargetId and bumps
 * mTargetedCount; the winner is returned, or 0 if none qualified. `a1` is
 * not used. */
#pragma opt_strength_reduction off
int daYegg_c::func_ov002_020edb3c(int a1, int best)
{
    dActor_c *found;
    dActor_c *actor;
    int matched;
    int b;
    struct P { int a, b, c, d; };
    volatile struct P _p;
    struct P *_q = (struct P *)&_p;

    if (mTargetedCount >= 5) {
        if (func_ov002_020ec654() == 0) {
            func_ov002_020edca4(this);
        }
        return 0;
    }

    found = 0;
    actor = (dActor_c *)_ZN8dActor_c4NextEPKS_(0);
    if (actor == 0) goto end;

loop:
    if (actor == this) goto next;
    if (actor == (dActor_c *)mPlayer) goto next;
    b = (actor->mFlags & 0x10000000) != 0;
    if (!b) goto next;
    b = (actor->mFlags & 8) != 0;
    if (b) goto next;
    matched = 0;
    for (int i = 0; i < 5; i++) {
        int bv = mTargetedIds[i];
        if (bv == actor->uniqueID) matched = 1;
    }
    if (matched != 0) goto next;
    if (func_ov002_020ec654() != 0) {
        if (((VObjQuery *)actor)->s20() == 0) goto next;
    }
    {
        int d = Vec3_Dist(&mPosX, &actor->mPosX);
        if (d < best) {
            best = d;
            found = actor;
        }
    }
next:
    actor = (dActor_c *)_ZN8dActor_c4NextEPKS_(actor);
    if (actor != 0) goto loop;

end:
    if (found != 0) {
        int idx = mTargetedCount;
        mTargetedIds[idx] = found->uniqueID;
        mTargetId = found->uniqueID;
        mTargetedCount += 1;
    }
    return (int)found;
}
#pragma opt_strength_reduction on

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov002_020edca4, 0x020edca4, size 0x120 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020edca4
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
/* The burst. Does nothing if mBurstDone is already set. Otherwise it pays out
 * (func_ov002_020ec728), and when the star bit is set gives up the tracked
 * star slot and spawns a SILVER_STAR (param1 0x10) 40 units above the egg,
 * which func_ov002_020e7218 then takes together with the Player. It then
 * starts four particle effects (ids 0x3f to 0x42) at the egg, marks the egg
 * for destruction, plays Sound::PlayCharVoice id 0x103 at its camera-space
 * position and sets mBurstDone. Stays a free function: Player.cpp calls it
 * on its held object, and Player is not deslopped here. */
extern "C" {
void func_ov002_020edca4(daYegg_c* self)
{
    struct Vector3 pos;
    char* spawned;
    int xv, zv, yv, sy;

    if (self->mBurstDone != 0) return;

    self->func_ov002_020ec728();

    yv = self->mPosY;
    zv = self->mPosZ;
    sy = yv + 0x28000;
    xv = self->mPosX;
    pos.x = xv;
    pos.y = sy;
    pos.z = zv;
    if (self->func_ov002_020ec610() != 0) {
        _ZN8dActor_c11UntrackStarERa((struct dActor_c*)self, (signed char*)&self->mStarSlot);

        spawned = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(ACTOR_SILVER_STAR, 0x10,
            &pos, 0, self->mPlayer->mAreaId, -1);
        if (spawned != 0) {
            _ZN8daStar_c19func_ov002_020e7218EPci((char*)spawned, (char*)self->mPlayer, 1);
        }
    }

    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x3f, self->mPosX, self->mPosY, self->mPosZ);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x40, self->mPosX, self->mPosY, self->mPosZ);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x41, self->mPosX, self->mPosY, self->mPosZ);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x42, self->mPosX, self->mPosY, self->mPosZ);
    _ZN7fBase_c18MarkForDestructionEv((struct dActor_c*)self);
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(0, 0x103, (const struct Vector3*)&self->mCamSpacePosX);

    self->mBurstDone = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- func_ov002_020eddc4, 0x020eddc4, size 0x190 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_c19func_ov002_020eddc4Ev
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
/* func_ov002_020eddc4 at 0x020eddc4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */
/* Contact handling, run each frame the egg is flying. The actor that touched
 * the egg's cylinder is looked up from the cylinder's otherOwner
 * (mdCcAc_c.otherOwner, +0x134); the answer is 0 and nothing happens if
 * there is none or it is the followed Player. When it is some other actor of
 * type PLAYER (any Player-type actor except the followed one):
 * if the cylinder's hit flags (mdCcAc_c.hitFlags, +0x130) have bit 0x8000 the
 * egg enters state 3 and returns 1; if none of the bits 0x26fe0 are set, the
 * Player is hurt (Player::Hurt at the egg's position with 1, 12 units in
 * Fix12, 1, 0, 1). When the touched actor is one of the six burst actors
 * (BOMBKING, HOLHEI, KURIKING, KURIBO_L, WANWAN, WANWAN2) the egg bursts and
 * returns 1. When it is the actor currently sought (mTargetId) the egg pays
 * out, re-enters state 1 and returns 1. Any other case returns 0. */
int daYegg_c::func_ov002_020eddc4()
{
    struct dActor_c* actor;
    struct dActor_c* other;
    u32 id;
    u32 flags;
    u16 type;

    id = mdCcAc_c.otherOwner;
    if (id == 0) goto fail;

    actor = (struct dActor_c*)_ZN8dActor_c10FindWithIDEj(id);
    if (actor == 0) goto fail;

    if (actor == (struct dActor_c*)mPlayer) goto fail;

    {
        int t = (int)(actor->actorID == ACTOR_PLAYER);
        if (t != 0) {
            flags = mdCcAc_c.hitFlags;
            if (flags & 0x8000) {
                func_ov002_020ed63c(this, daYegg_STATE_DROP);
                return 1;
            }
            if (!(flags & 0x26fe0)) {
                struct Vector3 pos;
                pos.x = mPosX;
                pos.y = mPosY;
                pos.z = mPosZ;
                _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(actor, &pos, 1, 0xc000, 1, 0, 1);
            }
        }
    }

    type = actor->actorID;
    switch (type) {
    case ACTOR_BOMBKING:
    case ACTOR_HOLHEI:
    case ACTOR_KURIKING:
    case ACTOR_KURIBO_L:
    case ACTOR_WANWAN:
    case ACTOR_WANWAN2:
        func_ov002_020edca4(this);
        return 1;
    }

    other = 0;
    id = mTargetId;
    if (id != 0) {
        other = (struct dActor_c*)_ZN8dActor_c10FindWithIDEj(id);
    }
    if (other != actor) goto fail;

    func_ov002_020ec728();
    func_ov002_020ed63c(this, daYegg_STATE_SEEK);
    return 1;

fail:
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- _ZN8daYegg_c16CleanupResourcesEv, 0x020edf54, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
/* Releases the two shared animation files and, if the egg pays a blue coin,
   unloads the blue-coin model InitResources loaded. */
int daYegg_c::CleanupResources()
{
  ((SharedFilePtr *)(&data_ov002_0210e6b0))->Release();
  ((SharedFilePtr *)(&data_ov002_0210eb78))->Release();
  if (func_ov002_020ec628() != 0)
    UnloadBlueCoinModel(((void*)this));
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- _ZN8daYegg_c6RenderEv, 0x020edf98, size 0x78 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_c6RenderEv
/* recovered: named members + shared header, real C++ method -- vtable slot 9 */
/* Draws the egg's model unless it is hidden: skipped when mFlags has 0x40000,
   when the followed Player is inside a cannon, or when the Player's opacity
   is below 1. */
int daYegg_c::Render()
{
    /* The temporary is load-bearing and must not be folded into the `if`, the
       same way it is in daBakubaku_c::Render: `if (mFlags & 0x40000)` tests the
       masked word directly, while the ROM materialises the 0/1 first. Folding it
       changes the function's SIZE, which is what a `999 word(s) differ` says. */
    int b = (int)((mFlags & 0x40000) != 0);
    if (b) return 1;

    if (mPlayer->IsInsideOfCannon()) return 1;
    if (mPlayer->mOpacity < 1) return 1;
    mModelAnim.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- _ZN8daYegg_c8BehaviorEv, 0x020ee010, size 0x134 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
/* Per-frame update: runs the current state's execute handler and advances the
 * model animation. In every state but 1 the egg takes the followed Player's
 * opacity; while that opacity is below 10 it is also pinned to the Player's
 * position plus the vector (0, 0, -104 units) rotated by the Player's angles.
 * In state 1 it is fully opaque (31). Then it places the model matrix
 * (func_ov002_020ed998), draws the drop shadow (func_ov002_020ed7f8), moves to
 * the next payout slot once the current one is paid, and bursts when all five
 * slots are used. */
int daYegg_c::Behavior()
{
    Vector3 vin;
    Vector3 vmid;
    Vector3 vout;
    func_ov002_020ed684();
    mModelAnim.Advance();
    if (mState != daYegg_STATE_SEEK) {
        if (mPlayer->mOpacity < 0xa) {
            /* BOTH oddities below are load-bearing, measured one at a time.

               `ang` must stay a pointer: reading the three angles as
               mPlayer->mAngleX/Y/Z instead reloads mPlayer per field and
               changes the function's size.

               `vin.z` really is written twice. Dropping the dead first store
               also changes the size, so the ROM's own source had it. */
            s16 *ang;
            vin.z = 0;
            vin.z = -0x68000;
            vin.x = 0;
            vin.y = 0;
            vmid.x = 0;
            vmid.y = 0;
            vmid.z = 0;
            ang = &mPlayer->mAngleX;
            Matrix4x3_FromRotationZXYExt(&data_020a0e68, ang[0], ang[1], ang[2]);
            MulVec3Mat4x3(&vin, &data_020a0e68, &vmid);
            Vec3_Add(&vout, &mPlayer->mPosX, &vmid);
            mPosX = vout.x;
            mPosY = vout.y;
            mPosZ = vout.z;
        }
        mModelAnim.ApplyOpacity(mPlayer->mOpacity, 0);
    } else {
        mModelAnim.ApplyOpacity(0x1f, 0);
    }
    func_ov002_020ed998();
    func_ov002_020ed7f8();
    if (mPayoutDone[mPayoutIdx] != 0)
        mPayoutIdx++;
    if (mPayoutIdx >= 5) {
        func_ov002_020edca4(this);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- _ZN8daYegg_c13InitResourcesEv, 0x020ee144, size 0x294 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daYegg_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * Vtable slot 0. This was the last daYegg_c function still written as a C free
 * function reaching every field through a raw offset -- it named not one member,
 * which is why it could not even include its own header once daYegg_c became a
 * real dEnemyBase_c subclass.
 *
 * Sets up the egg: mParamHigh from param1; the model by the variant
 * (func_ov002_020ec654 picks which of the two models at data_ov002_021000a0
 * and whether the shadow is a cylinder or a cuboid); the idle animation;
 * gravity -0x2000 (-2 units per frame squared) and a terminal velocity of
 * -0x3c000 (-60 units per frame); and the starting state from param1 & 3. The
 * same two bits choose the size: states 0 and 1 are normal size (scale 1.0,
 * collision cylinder 0x46000 by 0x8c000, i.e. 70 by 140 units, ground-collision
 * radius and height 0x28000 = 40 units), state 2 is the big egg (scale 2.0,
 * 0x78000 by 0xa0000 = 120 by 160 units, ground-collision radius and height
 * 0x64000 = 100 units), state 3 gets neither.
 * The cylinder's own flags 0x200002 and hit mask 0xa08000 are left as raw
 * numbers. It then clears the targeted-ID list, seeds both angle triples from
 * the actor's own angles, arms mStallTimer, loads the blue-coin model if the
 * egg pays one and tracks a star slot if it pays the star. */
#pragma opt_strength_reduction off
int daYegg_c::InitResources()
{
    int idx;
    int i;

    mParamHigh = (u8)(param1 >> 4);

    idx = 0;
    if (func_ov002_020ec654() != 0)
        idx = 1;

    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)data_ov002_0210e6b0);
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)data_ov002_0210eb78);
    if (mModelAnim.SetFile(*(BMD_File **)(data_ov002_021000a0[idx] + 4), 1, -1) == 0)
        return 0;

    /* The predicate is asked a SECOND time rather than reusing idx: the ROM calls
       0x020ec654 twice, and folding it into the index above loses a bl. */
    if (func_ov002_020ec654() == 0) {
        if (mShadowModel.InitCylinder() == 0)
            return 0;
    } else {
        if (mShadowModel.InitCuboid() == 0)
            return 0;
    }

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, *(void **)(data_ov002_0210eb78 + 4), 0, 0x1000, 0);

    mAreaId = -1;
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mState = param1 & 3;

    switch (mState) {
    case 0:
    case 1:
        mScaleX = 0x1000;
        mScaleY = 0x1000;
        mScaleZ = 0x1000;
        _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
            &mdCcAc_c, this, 0x46000, 0x8c000, 0x200002, 0xa08000);
        _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
            &mWithMeshClsn, this, 0x28000, 0x28000, 0, 0);
        break;
    case 2:
        mScaleX = 0x2000;
        mScaleY = 0x2000;
        mScaleZ = 0x2000;
        _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
            &mdCcAc_c, this, 0x78000, 0xa0000, 0x200002, 0xa08000);
        _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
            &mWithMeshClsn, this, 0x64000, 0x64000, 0, 0);
        break;
    default:
        break;
    }

    for (i = 0; i < 5; i++)
        mTargetedIds[i] = 0;

    mEasedAngleX = mAngleX;
    mEasedAngleY = mAngleY;
    mEasedAngleZ = mAngleZ;
    mGoalAngleX = mAngleX;
    mGoalAngleY = mAngleY;
    mGoalAngleZ = mAngleZ;
    mStallTimer = 0xf;

    if (func_ov002_020ec628() != 0)
        LoadBlueCoinModel((char *)this);
    if (func_ov002_020ec610() != 0)
        mStarSlot = _ZN8dActor_c9TrackStarEjj(this, 0, 1);
    return 1;
}
#pragma opt_strength_reduction on

/* -------------------------------------------------------------------------- */
/* ROM ordinal 32 -- daYegg_c_classInit, 0x020ee3d8, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol daYegg_c_classInit
/* recovered: vtable identified, globals resolved */
/* Reconstructed source-style name: SM64DS proves daYegg_c through RTTI,
 * allocation size, vtable identity, and the YOSHI_EGG registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: YoshiEgg_Spawn.
 *
 * `new daYegg_c` is the whole sequence the loose factory spelled by hand:
 * fBase_c::operator new(0x42c), dEnemyBase_c's base constructor, the vptr
 * store, then the four member constructors in declaration order. */
extern "C" daYegg_c *daYegg_c_classInit(void)
{
    return new daYegg_c;
}
