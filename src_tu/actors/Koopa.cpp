//cpp
/* Genuine production translation unit for ov062/Koopa (39 function(s)),
 * enrolled as one `complete` delinks span 0x021174cc..0x0211975c.
 *
 * THE CARTRIDGE CALLS THIS CLASS daNknk_c. `Koopa` is the decomp's coined name
 * and it is HOMED: config/arm9/overlays/ov062/symbols.txt carries both
 * `_ZTV5Koopa` and `_ZTV8daNknk_c` at 0x0211dab4, so this TU's vtable/RTTI
 * records can be word-compared against the ROM and a verifiable
 * compiler_only_output is possible. (`nknk` is NOKONOKO, the Japanese name;
 * the two registry profiles g_profile_NOKONOKO / g_profile_NOKONOKO_S belong
 * to the two factories folded in at the bottom of this file.)
 *
 * FUNCTION ORDER IS ROM-ASCENDING, NOT THE USUAL REVERSE. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order *only
 * while codegen is deferred*, which is the default. This TU sets
 * `#pragma defer_codegen off` (below), which emits in source order -- so the
 * source is laid out lowest-ROM-address first. The pragma and the layout are
 * ONE decision: flipping either alone puts `linkcheck [4b/8]` into its hard
 * `licensed .text functions are not emitted in ROM address order` refusal.
 *
 * THE DESTRUCTOR IS OUT OF LINE AND THAT IS DELIBERATE. The cartridge orders
 * D1 (0x021174cc) BELOW D0 (0x02117514) and emits no D2. Under default
 * (deferred) codegen an out-of-line destructor emits D2, D0, D1 -- the wrong
 * way round. Under `defer_codegen off` it emits D1, D0, D2, which is the
 * cartridge's order for the two variants that exist, and leaves a homeless
 * D2 that this file licenses as `deadstrip`. Keeping the destructor out of
 * line also keeps it the key function, which is what makes this TU emit
 * `_ZTV5Koopa` and the __si_class_type_info chain at all -- see
 * compiler_only_output in config/tu_manifest.d/ov062/Koopa.json.
 *
 * Assembled from these legacy one-function sources, all now deleted (ROM
 * address order):
 *   [0]  0x021174cc  src/_ZN5KoopaD1Ev.cpp
 *   [1]  0x02117514  src/_ZN5KoopaD0Ev.cpp
 *   [2]  0x02117570  src/func_ov062_02117570.cpp
 *   [3]  0x021175c0  src/func_ov062_021175c0.c
 *   [4]  0x02117724  src/func_ov062_02117724.c
 *   [5]  0x02117994  src/func_ov062_02117994.c
 *   [6]  0x021179e4  src/func_ov062_021179e4.cpp
 *   [7]  0x02117a3c  src/func_ov062_02117a3c.cpp
 *   [8]  0x02117acc  src/func_ov062_02117acc.c
 *   [9]  0x02117b48  src/func_ov062_02117b48.c
 *   [10] 0x02117b60  src/func_ov062_02117b60.c
 *   [11] 0x02117b9c  src/func_ov062_02117b9c.c
 *   [12] 0x02117bf4  src/func_ov062_02117bf4.cpp
 *   [13] 0x02117c98  src/func_ov062_02117c98.c
 *   [14] 0x02118004  src/func_ov062_02118004.cpp
 *   [15] 0x02118058  src/func_ov062_02118058.c
 *   [16] 0x021180d4  src/func_ov062_021180d4.c
 *   [17] 0x0211811c  src/func_ov062_0211811c.c
 *   [18] 0x021181a0  src/func_ov062_021181a0.c
 *   [19] 0x02118258  src/func_ov062_02118258.c
 *   [20] 0x02118334  src/func_ov062_02118334.c
 *   [21] 0x021183e0  src/func_ov062_021183e0.c
 *   [22] 0x02118588  src/func_ov062_02118588.c
 *   [23] 0x02118718  src/func_ov062_02118718.c
 *   [24] 0x02118a00  src/func_ov062_02118a00.cpp
 *   [25] 0x02118a50  src/func_ov062_02118a50.c
 *   [26] 0x02118b4c  src/func_ov062_02118b4c.c
 *   [27] 0x02118cdc  src/func_ov062_02118cdc.c
 *   [28] 0x02118de8  src/func_ov062_02118de8.cpp
 *   [29] 0x02118f04  src/_ZN5Koopa16CleanupResourcesEv.cpp
 *   [30] 0x02118f80  src/_ZN5Koopa16OnPendingDestroyEv.cpp
 *   [31] 0x02118f84  src/_ZN5Koopa6RenderEv.cpp
 *   [32] 0x021190ec  src/_ZN5Koopa8BehaviorEv.cpp
 *   [33] 0x02119420  src/_ZN5Koopa13InitResourcesEv.cpp
 *   [34] 0x02119608  src/_ZN5Koopa16OnAimedAtWithEggEv.cpp
 *   [35] 0x02119628  src/_ZN5Koopa13OnTurnIntoEggER6Player.cpp
 *   [36] 0x021196a8  src/_ZN5Koopa13OnYoshiTryEatEv.cpp
 *   [37] 0x021196bc  src/d_a_nknk_nokonoko_s.c
 *   [38] 0x0211970c  src/d_a_nknk_nokonoko.c
 *
 * [37] and [38] are NOT in the promotion queue's shard_count of 37. tu_map.py
 * cuts on symbol NAME, and these two are spelled `daNknk_c_classInit_*` while
 * every other member is spelled `_ZN5Koopa...` or `func_ov062_*`, so the run
 * was cut two functions short of its real end. They abut [36] with a ZERO-byte
 * gap, they allocate 976 bytes and store this class's vtable, and the sibling
 * oracle Chuckya absorbs its own `daHolhei_c_classInit` at the same relative
 * position. The real shard count is 39.
 */

/* Includes: hand-curated, NOT the generated union. The generated preamble was
 * discarded whole (37 members is well past the ~30 where it stops being
 * repairable): it nested `extern "C"` inside `extern "C"`, declared `Vec3` as
 * both a typedef and a struct tag, declared `data_ov062_0211cee8` three
 * mutually exclusive ways, and shadowed Vector3/SharedFilePtr/dActor_c against
 * the real headers this file has to include.
 *
 * ORDER IS LOAD-BEARING for Matrix4x3: Koopa.h reaches math/Matrix.h through
 * ModelAnim.h and ShadowModel.h, so the `{Matrix3x3 r; Vector3 t;}` spelling
 * wins TU-wide. func_ov062_02118334 whole-struct-assigns a matrix and was
 * written against common.h's flat `{s32 m[12]}`; it uses a local flat `Mat12`
 * rather than fighting the include order. */
#include "Koopa.h"
#include "types.h"
#include "common.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "dBgCh_Actr.h"
#include "decl_common.h"
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_Model.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"

/* mwccarm 2004/b56 defers codegen by default, which (a) reverses .text
 * emission order and (b) makes `#pragma opt_*` file-global last-wins so a
 * bracketed pair does not bind. Both matter here: this TU is written
 * ROM-ascending, and two of its members carry their own optimisation pragmas
 * that must not leak into the other 37. */
#pragma defer_codegen off

/* --- local shadow types -------------------------------------------------- */

/* A plain-old-data 3-vector. NOT `Vector3`: types.h's Vector3 declares an
 * (empty) destructor so the ROM's array destruction can exist, which makes it
 * non-POD. The legacy shards used a POD here, and keeping it POD also keeps
 * the function-local anonymous struct in ordinal 13 from emitting a destructor
 * of its own. */
struct Vec3 { s32 x, y, z; };

/* Layout-identical to Vector3, but with the three-argument constructor the
 * legacy shard for ordinal 6 used and types.h's Vector3 does not have. Only
 * that spelling reproduces the cartridge's y/z/x evaluation order there; the
 * object is handed to Spawn through a reference, so the cast costs nothing. */
struct V3ctor { s32 x, y, z; V3ctor(s32 X, s32 Y, s32 Z) : x(X), y(Y), z(Z) {} };

/* The flat Matrix4x3 spelling from common.h, renamed so it can coexist with
 * math/Matrix.h's `{Matrix3x3 r; Vector3 t;}`, which wins this TU through
 * Koopa.h. The array member is required: C++ scalarises a whole-struct
 * assignment word by word where C block-moved it. */
struct Mat12 { s32 m[12]; };

/* Entry into the file-handle tables. SharedFilePtr deliberately declares no
 * fields (see include/SharedFilePtr.h), but func_ov062_02117994 indexes +4, so
 * the tables are typed through this and cast at the SharedFilePtr call sites. */
struct Entry { char pad[4]; void *file; };

/* ModelAnim seen as a vtable only -- Koopa::Render dispatches slot 5 by hand. */
struct Mdl {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void slot5(void *p);
};

/* --- ROM symbols reached by their mangled spelling ------------------------ */
/* Mangled ROM names go INSIDE the block, where C linkage emits them verbatim.
 * A symbol whose ROM name is mangled but which is spelled UNMANGLED here would
 * have to stay outside it; nothing in this TU is in that class. */
extern "C" {

/* the vtable this TU owns, for the two factories at the bottom */
extern int _ZTV5Koopa[];

/* fBase_c / dEnemyBase_c construction, used by the factories */
extern void *_ZN7fBase_cnwEj(unsigned int size);
extern void _ZN12dEnemyBase_cC2Ev(void *self);
extern void _ZN7dCcAc_cC1Ev(void *self);
extern void _ZN10dBgCh_ActrC1Ev(void *self);
extern void _ZN9ModelAnimC1Ev(void *self);
extern void _ZN11ShadowModelC1Ev(void *self);

/* arm9 maths / helpers */
extern int Vec3_Dist(const void *a, const void *b);
extern short Vec3_HorzAngle(const void *a, const void *b);
extern void Vec3_Asr(struct Vec3 *d, struct Vec3 *s, int sh);
extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern s16 data_02082214[];
extern void func_0201267c(int a, void *p);
extern int SurfaceInfo_TestFlag0x20(int *p);
extern void LoadBlueCoinModel(void *c);
extern void UnloadBlueCoinModel(void *c);

/* ApproachLinear's ROM names are mangled; the pointer spelling is
 * ABI-identical to the reference one the real declaration uses. */
extern int _Z14ApproachLinearRiii(int *v, int target, int step);
extern int _Z14ApproachLinearRsss(short *v, short target, short step);

/* dActor_c / dEnemyBase_c / Player, reached mangled */
extern void *_ZN8dActor_c10FindWithIDEj(u32 id);
extern void *_ZN8dActor_c15FindWithActorIDEjPS_(u32 id, void *start);
extern void *_ZN8dActor_c13ClosestPlayerEv(void *self);
extern int _ZN8dActor_c14GetSubtractionEss(void *self, short a, short b);
extern s16 _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(void *self, int a, int b, s16 ang);
extern void _ZN8dActor_c8PoofDustEv(void *self);
extern void _ZN8dActor_c24KillAndTrackInDeathTableEv(void *self);
extern void _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(void *self, void *cc);
extern int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(void *self, int d);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *cc);
extern int _ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(void *self, void *cc, void *player);
extern void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 a, u32 b, const Vector3 &pos,
                                                             const void *dir, s32 e, s32 f);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *self, void *sm, struct Mat12 *m, int radHeight, int a, unsigned int b);

extern int _ZN12dEnemyBase_c26UpdateKillByInvincibleCharER10dBgCh_ActrR9ModelAnimj(
    void *self, void *wm, void *ma, unsigned int j);
extern int _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(void *self, void *wm);
extern int _ZN12dEnemyBase_c27SpawnParticlesIfHitOtherObjER5dCc_c(void *self, void *cc);
extern int _ZN12dEnemyBase_c15IsGoingOffCliffER10dBgCh_Actrisbbi(void *self, void *wm, int a,
                                                                 s16 b, int c, int d, int e);
extern void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void *self, void *wm, unsigned int j);
extern void _ZN12dEnemyBase_c11UpdateDeathER10dBgCh_Actr(void *self, void *wm);
extern unsigned char _ZN12dEnemyBase_c24AngleAwayFromWallOrCliffER10dBgCh_ActrRs(
    void *self, void *wm, short *a);
/* _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE is
 * declared by decl_Enemy.h with a real Vector3_16* second parameter; declaring
 * it again as void* here is `illegal function overloading`, so ordinal 13 uses
 * the header's spelling and common.h's Vector3_16 rather than the shard's
 * layout-identical local `V16`. */
extern void _ZN12dEnemyBase_c9SpawnCoinEv(void *self);

extern int _ZN6Player9IsOnShellEv(void *p);
extern void _ZN6Player6BounceE5Fix12IiE(void *p, int f);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *p, void *v, u32 a, int f,
                                                    u32 b, u32 c, u32 d);

/* ov002 helper with no source of its own */
extern void func_ov002_020aea30(void *self, void *a, int b);

/* dBgCh_Actr / SurfaceInfo / dCc_c / dCcAc_c */
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *self);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void *self);
extern void *_ZNK10dBgCh_Actr13GetWallResultEv(void *self);
extern void *_ZNK10dBgCh_Actr14GetFloorResultEv(void *self);
extern void _ZN10dBgCh_Actr19StartDetectingWaterEv(void *self);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void *self, void *actor, int r, int h, void *p, int q);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *s, Vector3 *v);
extern void _ZN5dCc_c5ClearEv(void *cc);
extern void _ZN5dCc_c6UpdateEv(void *cc);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *actor, int r, int h,
                                                      unsigned int e, unsigned int g);

/* Animation / Model / ModelAnim / ShadowModel / SharedFilePtr / fBase_c */
extern int _ZN9Animation8FinishedEv(void *anim);
extern int _ZNK9Animation13GetFrameCountEv(void *anim);
extern int _ZNK9Animation12WillHitFrameEi(void *anim, int frame);
extern void *_ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr *f);
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char *anim, void *file, int idx,
                                                       Fix12i speed, u32 flags);
extern void *_ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr *f);
extern void _ZN5Model12ShowMaterialEii(void *m, int a, int b);
extern void _ZN5Model12HideMaterialEii(void *m, int a, int b);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN13SharedFilePtr7ReleaseEv(SharedFilePtr *f);
extern void _ZN7fBase_c18MarkForDestructionEv(void *a);

/* Particle system */
extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 uniqueID, u32 effectID, s32 x, s32 y, s32 z, const void *dir, void *callback);
extern void _ZN8Particle19SetSelfDestructFlagEj(u32 id);
extern char *_ZN8Particle6System12FromUniqueIDEj(u32 id);
extern void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(int x, int y, int z);

/* matrix helpers, through the flat spelling -- see struct Mat12 above */
extern void Matrix4x3_FromTranslation(struct Mat12 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(struct Mat12 *m, short angY);
extern struct Mat12 data_020a0e68;

/* file-handle tables */
extern struct Entry *data_ov062_0211cee8[];
extern int data_ov062_0211cf0c[];
extern SharedFilePtr *data_ov062_0211ced8[];
extern SharedFilePtr *data_ov062_0211cee0[];

/* members of this TU that decl_common.h does NOT declare; ROM-ascending layout
 * still leaves plenty of forward calls */
extern void func_ov062_02117724(char *t, unsigned int a1, unsigned int a2,
                                unsigned int a3, unsigned short a4);
extern void func_ov062_02117b48(void *p);
extern int func_ov062_02117b60(void *c);
extern char *func_ov062_02117b9c(void *c);
extern void func_ov062_02117bf4(char *c);
extern int func_ov062_02118058(char *c);
extern void func_ov062_021180d4(void *c);
extern void func_ov062_0211811c(char *c);
extern void func_ov062_021181a0(char *c);
extern void func_ov062_021183e0(char *c);
extern void func_ov062_02118588(char *c);
extern void func_ov062_02118718(char *c);
extern void func_ov062_02118a00(void *c);
extern void func_ov062_02118a50(char *c);
extern void func_ov062_02118b4c(char *self);
extern void func_ov062_02118cdc(char *c);
extern void func_ov062_02118de8(char *c);

} /* extern "C" */

/* Two members disagree with decl_common.h about func_ov062_02117994, and each
 * disagreement is load-bearing -- see the call sites at ordinals 15 and 16. A
 * namespace gives a second view of the SAME linker symbol without overloading
 * the global-scope declaration decl_common.h supplies; the sibling oracle
 * Chuckya uses the same idiom for func_ov002_020db54c. */
namespace cv_ret {
extern "C" int func_ov062_02117994(void *c, int idx);
}
namespace cv_arg3 {
extern "C" void func_ov062_02117994(void *c, int idx, int extra);
}

/* ========================================================================== */
/* ROM ordinals 0 and 1 -- _ZN5KoopaD1Ev 0x021174cc, _ZN5KoopaD0Ev 0x02117514 */
/* ========================================================================== */
/* ONE definition; mwcc emits the variant group from it. Declared out of line
 * in Koopa.h, so this is also the class's key function and the reason this TU
 * emits _ZTV5Koopa and the RTTI chain.
 *
 * D1 destroys ShadowModel, ModelAnim, dBgCh_Actr and dCcAc_c in reverse
 * declaration order and then chains to dEnemyBase_c::~dEnemyBase_c -- all of
 * that is a consequence of the header's member list, not written here. D0 adds
 * the inline operator delete. The cartridge has no D2; the one this file emits
 * is homeless and is licensed `deadstrip` in the manifest. */
Koopa::~Koopa()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov062_02117570, 0x02117570, size 0x50                */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02117570
/* Signature taken from decl_common.h (`void(void*)`); the legacy shard spelled
 * the parameter `struct dActor_c *`, a local shadow that cannot survive the
 * real dActor_c this TU now includes. */
extern "C" void func_ov062_02117570(void *self)
{
    if (!_ZNK10dBgCh_Actr10IsOnGroundEv((char *)self + 0x144)) return;
    {
        void *floor = _ZNK10dBgCh_Actr14GetFloorResultEv((char *)self + 0x144);
        if (!SurfaceInfo_TestFlag0x20((int *)((char *)floor + 4))) return;
    }
    _ZN12dEnemyBase_c9SpawnCoinEv(self);
    _ZN8dActor_c24KillAndTrackInDeathTableEv(self);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov062_021175c0, 0x021175c0, size 0x164               */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_021175c0
/* Parameter widened to decl_common.h's `void*`; `c` keeps the shard's spelling
 * as a one-line alias so the body is unchanged. */
extern "C" void func_ov062_021175c0(void *self)
{
    char *c = (char *)self;
    volatile Vector3 pos;
    int t;
    u32 spawned;
    void *particle;

    if (*(unsigned char *)(c + 0x3ce) != 0) return;

    pos.x = *(int *)(c + 0x5c);
    pos.y = *(int *)(c + 0x60);
    pos.z = *(int *)(c + 0x64);

    t = *(unsigned short *)(c + 0xc);
    t = t == 0xcb;
    {
        int zArg = pos.z;
        if (t != 0) {
            pos.y = pos.y + 0xf000;
        } else {
            pos.y = pos.y + 0xa000;
        }

        spawned = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            0, 0xb2, pos.x, pos.y, zArg, (void *)0, (void *)0);
    }

    _ZN8Particle19SetSelfDestructFlagEj(0xb2);

    *(unsigned char *)(c + 0x3ce) = 0xa;
    if (spawned == 0) return;

    particle = _ZN8Particle6System12FromUniqueIDEj(spawned);
    if (particle == 0) return;

    {
        int t2 = *(unsigned short *)(c + 0xc);
        t2 = t2 == 0xcb;
        if (t2 != 0) {
            int v = *(int *)((char *)particle + 0x50);
            int r = (int)((((s64)v) * 0x800 + 0x800) >> 12);
            *(int *)((char *)particle + 0x50) = (short)r;
        } else {
            int v = *(int *)((char *)particle + 0x50);
            int r = (int)((((s64)v) * 0x500 + 0x800) >> 12);
            *(int *)((char *)particle + 0x50) = (short)r;
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov062_02117724, 0x02117724, size 0x270               */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02117724
/* 6f: keep constant live / flip coloring.
 * `opt_propagation off` came from the legacy shard. It is file-global
 * last-wins under deferred codegen, but this TU sets `defer_codegen off`, so
 * the off/on bracket really does bracket and cannot reach the other 38
 * members. */
#pragma opt_propagation off
extern "C" void func_ov062_02117724(char *t, unsigned int a1, unsigned int a2,
                                    unsigned int a3, unsigned short a4)
/* uxth vs a1-a3: ROM compares h<=a4 after a halfword arg */
{
    unsigned int h = (unsigned int)(*(int *)(t + 0x358) << 4) >> 16;
    if ((h >= a1 && h <= a2) || (h >= a3 && h <= a4)) {
        Vector3 pos;
        int k, iscb, idx, uid, iscb2;
        short ang;
        char *ps;
        if (*(unsigned char *)(t + 0x3cd) != 0) return;
        *(unsigned char *)(t + 0x3cd) = 1;
        func_0201267c(0xe4, t + 0x74);
        if (*(unsigned char *)(t + 0x398) == 1) return;
        ang = (short)(*(short *)(t + 0x8e) + 0x4000);
        pos.x = *(int *)(t + 0x5c);
        pos.y = *(int *)(t + 0x60);
        pos.z = *(int *)(t + 0x64);
        iscb = *(unsigned short *)(t + 0xc) == 0xcb;
        k = iscb ? 0xf : 0xa;
        *(volatile int *)&pos.y = pos.y + (k << 12); /* pin y add dest */
        if (h <= a2) {
            int i2, i1, v, w;
            idx = (unsigned short)ang >> 4;
            i2 = idx * 2;
            i1 = i2 + 1;
            v = data_02082214[i2];
            v = pos.x - k * v;
            w = data_02082214[i1];
            w = pos.z - k * w;
            pos.x = v;
            pos.z = w;
        } else {
            int i2, i1, ev, ew;
            idx = (unsigned short)ang >> 4;
            i2 = idx * 2;
            i1 = i2 + 1;
            ev = k * data_02082214[i2] + pos.x;
            ew = k * data_02082214[i1] + pos.z;
            pos.x = ev;
            pos.z = ew;
        }
        uid = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            0, 0xf9, pos.x, pos.y, pos.z, 0, 0);
        _ZN8Particle19SetSelfDestructFlagEj(0xf9);
        if (uid == 0) return;
        ps = _ZN8Particle6System12FromUniqueIDEj(uid);
        if (ps == 0) return;
        iscb2 = *(unsigned short *)(t + 0xc) == 0xcb;
        if (iscb2)
            *(int *)(ps + 0x50) = (short)(((long long)*(int *)(ps + 0x50) * 0x800 + 0x800) >> 12);
        else
            *(int *)(ps + 0x50) = (short)(((long long)*(int *)(ps + 0x50) * 0x500 + 0x800) >> 12);
    } else {
        *(unsigned char *)(t + 0x3cd) = 0;
    }
}
#pragma opt_propagation on

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov062_02117994, 0x02117994, size 0x50                */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02117994
extern "C" void func_ov062_02117994(char *c, int idx)
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        c + 0x300,
        data_ov062_0211cee8[idx]->file,
        data_ov062_0211cf0c[idx],
        *(Fix12i *)(c + 0x3bc),
        0
    );
    *(u8 *)(c + 0x398) = (u8)idx;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov062_021179e4, 0x021179e4, size 0x58                */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_021179e4
/* The legacy shard carried its own `struct Vector3` with a three-argument
 * constructor and its own `struct dActor_c`; both are shadows of real types
 * this TU includes. types.h's Vector3 has no such constructor, so the three
 * stores are written out and the actor is reached by offset, as every other
 * member here does. */
extern "C" void func_ov062_021179e4(void *self)
{
    char *c = (char *)self;
    /* Built through a three-argument constructor, which is how the shard
     * spelled it. The cartridge loads y, z, x in that order and stores them
     * in that order -- the constructor's argument-evaluation order. Neither
     * three separate member stores nor aggregate initialisation reproduces
     * it: both schedule x first, 14 words differing at the same 0x58 size. */
    V3ctor pos(*(s32 *)(c + 0x5c), *(s32 *)(c + 0x60) + 0x78000, *(s32 *)(c + 0x64));
    _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x122, 2, *(const Vector3 *)&pos, (const void *)0,
                                                    (s32)*(s8 *)(c + 0xcc), -1);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov062_02117a3c, 0x02117a3c, size 0x90                */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02117a3c
extern "C" void func_ov062_02117a3c(char *c)
{
    switch (*(int *)(c + 0x38c)) {
    case 0: func_ov062_02118a00(c); break;
    case 1: func_ov062_02118718(c); break;
    case 2: func_ov062_02118588(c); break;
    case 3: func_ov062_02118588(c); break;
    case 4: func_ov062_02118de8(c); break;
    case 5: func_ov062_021183e0(c); break;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov062_02117acc, 0x02117acc, size 0x7c                */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02117acc
extern "C" void func_ov062_02117acc(char *c)
{
    switch (*(int *)(c + 0x38c)) {
    case 0: func_ov062_02118a00(c); break;
    case 1: func_ov062_02118de8(c); break;
    case 2: func_ov062_02118cdc(c); break;
    case 3: func_ov062_02118b4c(c); break;
    case 4: func_ov062_02118a50(c); break;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov062_02117b48, 0x02117b48, size 0x18                */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02117b48
extern "C" void func_ov062_02117b48(void *p0)
{
    char *p = (char *)p0;
    *(int *)(p + 0x38c) = 0;
    *(int *)(p + 0xa8) = 81920;
    *(int *)(p + 0x98) = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov062_02117b60, 0x02117b60, size 0x3c               */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02117b60
extern "C" int func_ov062_02117b60(void *c)
{
    void *r1 = *(void **)((char *)c + 0x3b4);
    if (!r1) return 0x61a8000;
    return Vec3_Dist((char *)c + 0x5c, (char *)r1 + 0x5c);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov062_02117b9c, 0x02117b9c, size 0x58               */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02117b9c
/* Returns char* rather than void*: both call sites index off the result and
 * C++ will not implicitly convert a void* back to char*. */
extern "C" char *func_ov062_02117b9c(void *c)
{
    void *found = 0;
    void *best = 0;
    Fix12i bestDist = 0x7FFFFFFF;
    while (1) {
        found = _ZN8dActor_c15FindWithActorIDEjPS_(0x11d, found);
        if (!found) break;
        {
            Fix12i d = Vec3_Dist((char *)c + 0x5c, (char *)found + 0x5c);
            if (d < bestDist) {
                bestDist = d;
                best = found;
            }
        }
    }
    return (char *)best;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov062_02117bf4, 0x02117bf4, size 0xa4               */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02117bf4
extern "C" void func_ov062_02117bf4(char *c)
{
    func_0201267c(0xed, c + 0x74);
    *(int *)(c + 0x390) = 1;
    *(int *)(c + 0x38c) = 3;
    *(int *)(c + 0x98) = 0x14000;
    func_ov062_02117994(c, 7);
    {
        Vector3 v;
        int yy = *(int *)(c + 0x64);
        int zz = *(int *)(c + 0x60) + 0x3c000;
        v.x = *(int *)(c + 0x5c);
        v.y = zz;
        v.z = yy;
        _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x11d, *(unsigned int *)(c + 0x394), v, 0,
                                                        *(signed char *)(c + 0xcc), -1);
    }
    *(short *)(c + 0x3ca) = 0xa;
    func_0201267c(1, c + 0x74);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov062_02117c98, 0x02117c98, size 0x36c              */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02117c98
/* This member's AT() round-trips through long long; ordinal 19's does not.
 * They were two different macros with the same name in two legacy files, and
 * the difference moves bytes, so each is defined immediately before its own
 * member and undefined after it. */
#define AT(p, off) ((void*)(int)(((long long)(int)((char*)(p) + (off)))))
/* decl_common.h types this `void(char*)`; the shard defined it `void(void*)`
 * and aliased to u8* on line 1. Parameter retyped, body unchanged. */
extern "C" void func_ov062_02117c98(char *self)
{
    u8 *c = (u8 *)self;
    void *found;
    int r5;
    s32 flags;
    u32 id;

    id = *(u32 *)(c + 0x134);
    if (id == 0)
        return;
    found = _ZN8dActor_c10FindWithIDEj(id);
    if (found == 0)
        return;

    flags = *(s32 *)(c + 0x130);
    r5 = (int)(((long long)(int)0));

    if (flags & 0x10) {
        Vector3_16 v;
        v.x = (s16)-0x2000;
        v.y = (s16)r5;
        v.z = (s16)r5;
        _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(self, &v, found, 0x46000);
        return;
    }
    if (flags & 0x22400) {
        if (*(s32 *)(c + 0x390) == 0) {
            func_ov062_02117bf4(self);
            return;
        }
        *(s32 *)(c + 0x10c) = 5;
        func_ov002_020aea30(self, found, r5);
        return;
    }
    if (flags & 0x4000) {
        *(s32 *)(c + 0x10c) = 6;
        r5 = 1;
    } else if (flags & 0x447e0) {
        if (*(s32 *)(c + 0x390) == 0) {
            func_ov062_02117bf4(self);
            if (flags & 0x3c0)
                *(s16 *)(c + 0x94) = *(s16 *)((u8 *)found + 0x8e);
        } else {
            if (flags & 0x40040)
                *(s32 *)(c + 0x10c) = 2;
            else if (flags & 0x20400)
                *(s32 *)(c + 0x10c) = 5;
            else if (flags & 0x380)
                *(s32 *)(c + 0x10c) = 3;
            else if (flags & 0x4000)
                *(s32 *)(c + 0x10c) = 6;
            else {
                *(s32 *)(c + 0x10c) = 1;
                func_0201267c(0x113, (void *)(c + 0x74));
                *(s32 *)(c + 0x80) = 0x1000;
                *(s32 *)(c + 0x84) = 0x1000;
                *(s32 *)(c + 0x88) = 0x1000;
            }
            func_ov002_020aea30(self, found, 0);
            return;
        }
    } else {
        u8 *f = (u8 *)found;
        struct { Vec3 sv; Vec3 hv; } L;
        int shell;
        shell = (*(u16 *)(f + 0xc) == 0xbf) ? 1 : r5;
        if ((int)(((long long)shell)) == 0)
            goto tail;
        if (*(u8 *)(f + 0x6f9) != 0) {
            if (*(s32 *)(c + 0x390) == 0) {
                func_ov062_02117bf4(self);
                goto tail;
            }
            *(s32 *)(c + 0x10c) = 6;
            func_ov002_020aea30(self, found, 0);
            return;
        }
        {
            s32 *s = (s32 *)AT(f, 0x5c);
            L.sv.x = s[0];
            L.sv.y = s[1];
            L.sv.z = s[2];
        }
        if (_ZN6Player9IsOnShellEv(found)) {
            *(s32 *)(c + 0x10c) = 5;
            r5 = 1;
            goto tail;
        }
        if (_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(self, (void *)(c + 0x110), found)) {
            if (*(s32 *)(c + 0x390) == 0) {
                func_ov062_02117bf4(self);
            } else {
                *(s32 *)(c + 0x10c) = 1;
                func_ov002_020aea30(self, found, 0);
                *(s32 *)(c + 0x80) = 0x1000;
                *(s32 *)(c + 0x84) = 0x1000;
                *(s32 *)(c + 0x88) = 0x1000;
                func_0201267c(0x113, (void *)(c + 0x74));
            }
            _ZN6Player6BounceE5Fix12IiE(found, 0x28000);
            return;
        }
        if (*(u8 *)(f + 0x6fb) != 0)
            goto tail;
        if ((*(s32 *)(c + 0x130) & 0x400000) == 0)
            goto tail;
        if (*(s32 *)(c + 0x38c) == 0)
            goto tail;
        {
            s32 pw;
            L.hv.x = *(s32 *)(c + 0x5c);
            L.hv.y = *(s32 *)(c + 0x60);
            L.hv.z = *(s32 *)(c + 0x64);
            pw = *(s32 *)(c + 0x98);
            if (pw < 0xf000)
                pw = 0xf000;
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(found, &L.hv, 0, pw, 1, 0, 1);
            func_ov062_02117b48(self);
            if (*(s32 *)(c + 0x390) == 2) {
                _ZN8dActor_c8PoofDustEv(self);
                func_ov062_021179e4(self);
                _ZN8dActor_c24KillAndTrackInDeathTableEv(self);
            }
        }
    }

tail:
    func_ov002_020aea30(self, found, (int)(c + 0x144));
    if (r5)
        *(u16 *)(c + 0x8e) = *(s16 *)(c + 0x94) + 0x8000;
}
#undef AT

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov062_02118004, 0x02118004, size 0x54               */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02118004
/* Signature deliberately copied from decl_common.h: the ROM name carries
 * by-value class parameters (e.g. Fix12<int>), which mwccarm passes
 * differently at the call site, so declaring the true types breaks the byte
 * match. See notes/mwccarm-codegen.md 6az. */
extern "C" void func_ov062_02118004(void *c, int a1)
{
    int r = ((dBgCh_Actr *)((char *)c + 0x144))->IsOnGround();
    if (r == 0) return;
    _Z14ApproachLinearRiii((int *)((char *)c + 0x98), 0, a1);
    {
        int x = *(int *)((char *)c + 0x5c);
        int y = *(int *)((char *)c + 0x60);
        int z = *(int *)((char *)c + 0x64);
        _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(x, y, z);
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov062_02118058, 0x02118058, size 0x7c               */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02118058
/* The tail is `return func_ov062_02117994(c, 3);` -- a tail call whose r0 this
 * function forwards. decl_common.h types that member `void`, correctly (its
 * body returns nothing), so the forwarding is spelled through the cv_ret view
 * of the same symbol rather than by dropping the `return`. */
extern "C" int func_ov062_02118058(char *c)
{
    void *o = *(void **)(c + 0x3b4);
    int d = *(int *)(c + 0x3b8);
    if (o != 0) d = Vec3_Dist(c + 0x5c, (char *)o + 0x5c);
    if (d >= 0x12c000) return d;
    {
        int s = _ZN8dActor_c14GetSubtractionEss(c, *(short *)(c + 0x3c0), *(short *)(c + 0x94));
        if (s >= 0x3000) return s;
    }
    if (*(int *)(c + 0x390) == 1) *(int *)(c + 0x38c) = 1;
    else *(int *)(c + 0x38c) = 3;
    return cv_ret::func_ov062_02117994(c, 3);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov062_021180d4, 0x021180d4, size 0x48               */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_021180d4
/* The third argument is real in the sense that matters: the cartridge issues
 * `mov r2, #1` at 0x02118100, uses r2 for the store at 0x0211810c, and leaves
 * it live across the `bl 0x2117994` at 0x02118110. The legacy shard reproduced
 * that by declaring the callee with three parameters. Spelled through the
 * cv_arg3 view so decl_common.h's two-parameter declaration is not
 * overloaded. */
extern "C" void func_ov062_021180d4(void *c)
{
    _Z14ApproachLinearRiii((int *)((char *)c + 0x98), 0, 0x1000);
    {
        int done = _ZN9Animation8FinishedEv((char *)c + 0x350);
        if (!done) return;
    }
    *(int *)((char *)c + 0x38c) = 1;
    cv_arg3::func_ov062_02117994(c, 4, 1);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov062_0211811c, 0x0211811c, size 0x84               */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_0211811c
extern "C" void func_ov062_0211811c(char *c)
{
    char *b = c + 0x300;
    if (*(unsigned short *)(b + 0xc6) != 0) {
        unsigned short *p = (unsigned short *)(((int)c + 0x3c6));
        *p = (unsigned short)(*p - 1);
        return;
    }
    if (_ZNK9Animation12WillHitFrameEi(c + 0x350,
            (unsigned short)(_ZNK9Animation13GetFrameCountEv(c + 0x350) - 1)) == 0)
        return;
    {
        unsigned short *q = (unsigned short *)(((int)c + 0x3c4));
        *q = (unsigned short)(*q + 1);
    }
    func_ov062_02117994(c, 2);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov062_021181a0, 0x021181a0, size 0xb8               */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_021181a0
extern "C" void func_ov062_021181a0(char *c)
{
    if (*(int *)(c + 0x390) == 2) {
        _Z14ApproachLinearRiii((int *)(c + 0x98), 0x2000, 0x4cc);
    } else {
        _Z14ApproachLinearRiii((int *)(c + 0x98), 0x3000, 0x4cc);
    }
    if (_ZN9Animation8FinishedEv(c + 0x350) == 0) return;
    {
        unsigned short *p = (unsigned short *)(((int)c + 0x3c4));
        *p = (unsigned short)(*p + 1);
    }
    {
        char *b = c + 0x300;
        *(unsigned short *)(b + 0xc6) =
            (unsigned short)(((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) % 70 + 30);
    }
    func_ov062_02117994(c, 1);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov062_02118258, 0x02118258, size 0xdc               */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02118258
/* This member's AT() does NOT round-trip through long long -- see ordinal 13. */
#define AT(p, off) ((void*)(int)((char*)(p) + (off)))
/* decl_common.h types this `void(void*, int)`; `c` is the shard's alias. */
extern "C" void func_ov062_02118258(void *self, int lim)
{
    char *c = (char *)self;
    *(void **)(c + 0x3b4) = _ZN8dActor_c13ClosestPlayerEv(c);

    if (*(void **)(c + 0x3b4) == 0 || Vec3_Dist((Vector3 *)(c + 0x5c), (Vector3 *)(c + 0x39c)) > lim) {
        *(short *)(c + 0x3c0) = Vec3_HorzAngle((Vector3 *)(c + 0x5c), (Vector3 *)(c + 0x39c));
        *(int *)(c + 0x3b8) = 0x61a8000;
    } else {
        Vector3 v;
        int *src = (int *)AT(*(char **)(c + 0x3b4), 0x5c);
        v.x = src[0];
        v.y = src[1];
        v.z = src[2];
        if (Vec3_Dist((Vector3 *)(c + 0x39c), &v) > lim) {
            *(int *)(c + 0x3b8) = 0x4e20000;
            return;
        }
        *(int *)(c + 0x3b8) = Vec3_Dist((Vector3 *)(c + 0x5c), &v);
        *(short *)(c + 0x3c0) = Vec3_HorzAngle((Vector3 *)(c + 0x5c), &v);
    }
}
#undef AT

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov062_02118334, 0x02118334, size 0xac               */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02118334
/* The matrix is the flat `Mat12` spelling, not the `{Matrix3x3 r; Vector3 t;}`
 * one Koopa.h drags in through ModelAnim.h/ShadowModel.h. The whole-struct
 * assignment below is exactly the construct that splits into an ldm/stm plus a
 * CSE'd tail when the structured spelling wins. */
extern "C" void func_ov062_02118334(char *c)
{
    short *new_var;
    struct Vec3 v;
    Vec3_Asr(&v, (struct Vec3 *)(c + 0x5c), 3);
    if (1)
    {
        Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
        new_var = (short *)(c + 0x8e);
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *new_var);
        *((struct Mat12 *)(c + 0x31c)) = data_020a0e68;
        {
            int b = (int)(((*((int *)(c + 0xb0))) & 0x40000) != 0);
            if (b != 0)
            {
                return;
            }
        }
    }
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, (void *)(c + 0x364), (struct Mat12 *)(c + 0x31c), 0x50000, 0x50000, 0xf);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov062_021183e0, 0x021183e0, size 0x1a8              */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_021183e0
extern "C" void func_ov062_021183e0(char *c)
{
    int dist = 0x7fffffff;
    char *other;

    if (_ZN9Animation8FinishedEv(c + 0x350))
        func_ov062_02117994(c, 1);

    if (*(unsigned char *)(c + 0x3cc)) {
        *(unsigned char *)(c + 0x3cc) =
            (_Z14ApproachLinearRsss((short *)(c + 0x94), *(short *)(c + 0x3c2), 0x200) ^ 1) != 0;
    } else {
        if (*(int *)(c + 0x3b8) >= 0x61a8000)
            *(short *)(c + 0x3c2) = *(short *)(c + 0x3c0);
        other = func_ov062_02117b9c(c);
        if (other) {
            dist = Vec3_Dist(c + 0x5c, other + 0x5c);
            if (dist < 0xc8000)
                *(short *)(c + 0x3c2) = Vec3_HorzAngle(c + 0x5c, other + 0x5c);
        }
        *(unsigned char *)(c + 0x3cc) =
            _ZN12dEnemyBase_c24AngleAwayFromWallOrCliffER10dBgCh_ActrRs(c, c + 0x144, (short *)(c + 0x3c2));
        _Z14ApproachLinearRsss((short *)(c + 0x94), *(short *)(c + 0x3c2), 0x200);
    }

    if (dist < 0xc8000 && _ZN8dActor_c14GetSubtractionEss(c, *(short *)(c + 0x3c2), *(short *)(c + 0x94)) < 0xc00) {
        *(short *)(c + 0x94) = *(short *)(c + 0x3c2);
        *(int *)(c + 0x38c) = 2;
        *(int *)(c + 0x98) = 0x18000;
        *(int *)(c + 0xa8) = dist / 30;
        func_ov062_02117994(c, 6);
        *(short *)(c + 0x3c6) = 0x14;
        return;
    }

    if (*(int *)(c + 0x390) == 2)
        _Z14ApproachLinearRiii((int *)(c + 0x98), 0x2000, 0x4cc);
    else
        _Z14ApproachLinearRiii((int *)(c + 0x98), 0x3000, 0x4cc);

    func_ov062_02118058(c);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov062_02118588, 0x02118588, size 0x190              */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02118588
extern "C" void func_ov062_02118588(char *c)
{
    void *found = 0;
    int match = 0;
    if (*(int *)(c + 0x38c) == 2) {
        if (*(unsigned int *)(c + 0x134) != 0) {
            found = _ZN8dActor_c10FindWithIDEj(*(unsigned int *)(c + 0x134));
            if (found != 0) {
                /* NOT `int t = (x == 0x11d); if (t) ...`. That is the bool
                 * widening trap: as C the shard's spelling produced the ROM's
                 * `moveq r0,#1 / movne r0,r5 / cmp r0,#0 / movne r5,#1`, but in
                 * C++ the comparison is a bool, and re-testing it costs a
                 * `movne r0,#1 / moveq r0,#0 / cmp r0,#0` normalisation --
                 * measured, 0x19c against the ROM's 0x190. */
                int t;
                if (*(unsigned short *)((char *)found + 0xc) == 0x11d)
                    t = 1;
                else
                    t = match;
                if (t != 0)
                    match = 1;
            }
        }
    }
    if (match != 0) {
        int *hp = (int *)(c + 0x98);
        *(int *)(c + 0x390) = 0;
        *(int *)(c + 0x38c) = 4;
        *hp = *hp / 2;
        _ZN7fBase_c18MarkForDestructionEv(found);
        return;
    }
    if (*(int *)(c + 0x98) != 0) {
        func_ov062_02118004(c, 0x800);
        return;
    }

    {
        if (*(unsigned short *)(c + 0x3c6) != 0) {
            unsigned short *q = (unsigned short *)(c + 0x3c6);
            char *p = c + 0x300;
            *q = (unsigned short)(*q - 1);
            if (*(unsigned short *)(p + 0xc6) != 0)
                return;
            func_ov062_02117994(c, 8);
            return;
        }
    }

    if (_ZNK9Animation12WillHitFrameEi(c + 0x350, 0x1e) != 0)
        func_ov062_021175c0(c);
    if (_ZN9Animation8FinishedEv(c + 0x350) == 0)
        return;
    if (*(int *)(c + 0x394) != 0) {
        *(int *)(c + 0x38c) = 5;
        func_ov062_02117994(c, 0);
        return;
    }
    *(int *)(c + 0x38c) = 1;
    func_ov062_02117994(c, 3);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov062_02118718, 0x02118718, size 0x2e8              */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02118718
extern "C" void func_ov062_02118718(char *c)
{
    int dist = 0x7fffffff;
    char *other;

    if (*(unsigned char *)(c + 0x398) == 3)
        func_ov062_02117724(c, 2, 5, 8, 0xb);

    if (*(unsigned char *)(c + 0x3cc)) {
        *(unsigned char *)(c + 0x3cc) =
            (_Z14ApproachLinearRsss((short *)(c + 0x94), *(short *)(c + 0x3c2), 0x200) ^ 1) != 0;
    } else {
        if (*(int *)(c + 0x3b8) >= 0x61a8000)
            *(short *)(c + 0x3c2) = *(short *)(c + 0x3c0);

        other = func_ov062_02117b9c(c);
        if (other) {
            dist = Vec3_Dist(c + 0x5c, other + 0x5c);
            *(short *)(c + 0x3c2) = Vec3_HorzAngle(c + 0x5c, other + 0x5c);
        } else {
            *(unsigned char *)(c + 0x3cc) =
                _ZN12dEnemyBase_c24AngleAwayFromWallOrCliffER10dBgCh_ActrRs(c, c + 0x144, (short *)(c + 0x3c2));
            if (!*(unsigned char *)(c + 0x3cc)) {
                if (*(unsigned short *)(c + 0x3c8) != 0) {
                    *(unsigned short *)(((int)c + 0x3c8)) -= 1;
                } else {
                    if (((unsigned)RandomIntInternal(&data_0209e650) >> 16) & 0x8000)
                        *(short *)(((int)c + 0x3c2)) -= ((unsigned)RandomIntInternal(&data_0209e650) >> 16) & 0x1fff;
                    else
                        *(short *)(((int)c + 0x3c2)) += ((unsigned)RandomIntInternal(&data_0209e650) >> 16) & 0x1fff;
                    *(unsigned short *)(c + 0x3c8) = 0x14;
                }
            }
        }

        if (*(int *)(c + 0x3b8) > 0x320000 ||
            (other != 0 &&
             _ZN8dActor_c14GetSubtractionEss(c, *(short *)(c + 0x3c2),
                 (short)(*(short *)(c + 0x94) + 0x8000)) < 0x2000)) {
            _Z14ApproachLinearRsss((short *)(c + 0x94), *(short *)(c + 0x3c2), 0x600);
        } else {
            if (*(int *)(c + 0x394) != 0)
                _Z14ApproachLinearRsss((short *)(c + 0x94), *(short *)(c + 0x3c0), 0x600);
            else
                _Z14ApproachLinearRsss((short *)(c + 0x94), (short)(*(short *)(c + 0x3c0) + 0x8000), 0x600);
        }
    }

    if (_Z14ApproachLinearRiii((int *)(c + 0x98), 0x14000, 0x1000) == 0)
        return;
    if (dist >= 0xc8000)
        return;
    if (_ZN8dActor_c14GetSubtractionEss(c, *(short *)(c + 0x3c2), *(short *)(c + 0x94)) >= 0xc00)
        return;

    *(short *)(c + 0x94) = *(short *)(c + 0x3c2);
    *(int *)(c + 0x38c) = 2;
    *(int *)(((int)c + 0x98)) += *(int *)(c + 0x98) / 5;
    *(int *)(c + 0xa8) = dist / 30;
    func_ov062_02117994(c, 6);
    *(short *)(c + 0x3c6) = 0x14;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov062_02118a00, 0x02118a00, size 0x50               */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02118a00
extern "C" void func_ov062_02118a00(void *c)
{
    int gr = ((dBgCh_Actr *)((char *)c + 0x144))->IsOnGround();
    if (gr == 0) return;
    {
        int v = *(int *)((char *)c + 0x390);
        if (v == 1) {
            *(int *)((char *)c + 0x38c) = 5;
        } else {
            *(int *)((char *)c + 0x38c) = 2;
        }
    }
    func_ov062_02117994((char *)c, 0);
    func_ov062_021175c0(c);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov062_02118a50, 0x02118a50, size 0xfc               */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02118a50
extern "C" void func_ov062_02118a50(char *c)
{
    if (*(int *)(c + 0x98) != 0) {
        if (_ZNK10dBgCh_Actr8IsOnWallEv(c + 0x144) != 0) {
            void *sr = _ZNK10dBgCh_Actr13GetWallResultEv(c + 0x144);
            _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char *)sr + 4, (Vector3 *)(c + 0xe0));
            *(s16 *)(c + 0x94) = _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(
                c, *(int *)(c + 0xe0), *(int *)(c + 0xe8), *(s16 *)(c + 0x94));
        }
        func_ov062_02118004(c, 0x4cc);
        return;
    }

    {
        if (*(u16 *)(c + 0x3c6) != 0) {
            u16 *p = (u16 *)(c + 0x3c6);
            *p = (u16)(*p - 1);
            if (*(u16 *)((c + 0x300) + 0xc6) != 0) return;
            func_ov062_02117994(c, 8); return;
        }
    }

    if (_ZNK9Animation12WillHitFrameEi(c + 0x350, 0x1e) != 0)
        func_ov062_021175c0(c);
    if (_ZN9Animation8FinishedEv(c + 0x350) == 0)
        return;
    *(int *)(c + 0x38c) = 1;
    func_ov062_02117994(c, 2);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov062_02118b4c, 0x02118b4c, size 0x190              */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02118b4c
extern "C" void func_ov062_02118b4c(char *self)
{
    if (*(u8 *)(self + 0x398) == 3)
        func_ov062_02117724(self, 2, 5, 8, 0xb);

    if (*(int *)(self + 0x394) != 0) {
        if (*(u16 *)(self + 0x100) > 0x1e && func_ov062_02117b60(self) > 0x320000) {
            if (_Z14ApproachLinearRiii((int *)(self + 0x98), 0, 0x1000) == 0)
                return;
            *(int *)(self + 0x38c) = 1;
            func_ov062_02117994(self, 2);
            return;
        }
        _Z14ApproachLinearRsss((s16 *)(self + 0x94), *(s16 *)(self + 0x3c0), 0x400);
        _Z14ApproachLinearRiii((int *)(self + 0x98), 0x11000, 0x1000);
        return;
    }

    if (*(int *)(self + 0x3b8) >= 0x61a8000) {
        *(s16 *)(((int)self + 0x3c0)) += 0x8000;
        *(int *)(self + 0x3b8) = 0;
    }
    if (*(u16 *)(self + 0x100) > 0x1e && *(int *)(self + 0x3b8) > 0x320000) {
        if (_Z14ApproachLinearRiii((int *)(self + 0x98), 0, 0x1000) == 0)
            return;
        *(int *)(self + 0x38c) = 1;
        func_ov062_02117994(self, 2);
        return;
    }
    _Z14ApproachLinearRsss((s16 *)(self + 0x94), (s16)(*(s16 *)(self + 0x3c0) + 0x8000), 0x400);
    _Z14ApproachLinearRiii((int *)(self + 0x98), 0x11000, 0x1000);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- func_ov062_02118cdc, 0x02118cdc, size 0x10c              */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02118cdc
extern "C" void func_ov062_02118cdc(char *c)
{
    if (*(unsigned char *)(c + 0x3cc)) {
        *(unsigned char *)(c + 0x3cc) =
            (_Z14ApproachLinearRsss((short *)(c + 0x94), *(short *)(c + 0x3c2), 0x200) ^ 1) != 0;
    } else {
        if (*(int *)(c + 0x3b8) >= 0x61a8000) {
            *(short *)(c + 0x3c2) = *(short *)(c + 0x3c0);
        }
        *(unsigned char *)(c + 0x3cc) =
            _ZN12dEnemyBase_c24AngleAwayFromWallOrCliffER10dBgCh_ActrRs(c, c + 0x144, (short *)(c + 0x3c2));
        _Z14ApproachLinearRsss((short *)(c + 0x94), *(short *)(c + 0x3c2), 0x200);
    }
    if (*(unsigned char *)(c + 0x398) == 1) {
        func_ov062_02117724(c, 2, 8, 0x13, 0x19);
    }
    switch (*(unsigned short *)(c + 0x3c4)) {
    case 0: func_ov062_021181a0(c); break;
    case 1: func_ov062_0211811c(c); break;
    case 2: func_ov062_021180d4(c); break;
    }
    func_ov062_02118058(c);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- func_ov062_02118de8, 0x02118de8, size 0x11c              */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_02118de8
extern "C" void func_ov062_02118de8(char *c)
{
    *(int *)(c + 0x98) = 0;
    if (*(unsigned char *)(c + 0x398) == 2) {
        if (_ZN9Animation8FinishedEv(c + 0x350) == 0) return;
        func_ov062_02117994(c, 4);
        return;
    }
    if (_ZNK9Animation12WillHitFrameEi(c + 0x350,
            (unsigned short)(_ZNK9Animation13GetFrameCountEv(c + 0x350) - 1)) != 0) {
        unsigned short *hp = (unsigned short *)(((int)c + 0x3c4));
        *hp += 1;
    } else {
        func_ov062_02118058(c);
        return;
    }
    func_ov062_02117994(c, 0);
    if (*(int *)(c + 0x390) == 1)
        *(int *)(c + 0x38c) = 5;
    else
        *(int *)(c + 0x38c) = 2;
    if (((unsigned int)RandomIntInternal(&data_0209e650) >> 16) & 0x8000) {
        int r = ((unsigned int)RandomIntInternal(&data_0209e650) >> 16) & 0x1fff;
        *(short *)(c + 0x3c2) = *(short *)(c + 0x94) - r;
    } else {
        int r = ((unsigned int)RandomIntInternal(&data_0209e650) >> 16) & 0x1fff;
        *(short *)(c + 0x3c2) = *(short *)(c + 0x94) + r;
    }
    func_ov062_02118058(c);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- _ZN5Koopa16CleanupResourcesEv, 0x02118f04, size 0x7c     */
/* -------------------------------------------------------------------------- */
// @symbol _ZN5Koopa16CleanupResourcesEv
int Koopa::CleanupResources()
{
  int b = (int) ((*((unsigned short *) ((char *)&actorID))) == 0xcc);
  if (b == 0)
  {
    _ZN13SharedFilePtr7ReleaseEv(data_ov062_0211cee0[*((int *) ((char *)&mModelIndex))]);
  }
  _ZN13SharedFilePtr7ReleaseEv(data_ov062_0211ced8[*((int *) ((char *)&mModelIndex))]);
  {
    int i = 0;
    do
    {
      _ZN13SharedFilePtr7ReleaseEv((SharedFilePtr *) data_ov062_0211cee8[(long) i]);
      i++;
    }
    while (i < 9);
  }
  UnloadBlueCoinModel(((char *)this));
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- _ZN5Koopa16OnPendingDestroyEv, 0x02118f80, size 0x4      */
/* -------------------------------------------------------------------------- */
// @symbol _ZN5Koopa16OnPendingDestroyEv
/* fBase_c slot 12. Empty in the ROM: four bytes, `bx lr`. */
void Koopa::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- _ZN5Koopa6RenderEv, 0x02118f84, size 0x168               */
/* -------------------------------------------------------------------------- */
// @symbol _ZN5Koopa6RenderEv
int Koopa::Render()
{
  volatile struct Vec3 saved;
  int b = (mFlags & 0x40000) != 0;
  if (b) return 1;
  if (mKoopaVariant == 1) {
    _ZN5Model12ShowMaterialEii(((char*)this) + 0x300, 0, 1);
    _ZN5Model12HideMaterialEii(((char*)this) + 0x300, 0, 2);
  } else {
    _ZN5Model12HideMaterialEii(((char*)this) + 0x300, 0, 1);
    _ZN5Model12ShowMaterialEii(((char*)this) + 0x300, 0, 2);
  }
  saved.x = mScaleX;
  saved.y = mScaleY;
  saved.z = mScaleZ;
  if (mDeathState == 1 && mKoopaVariant == 2) {
    mScaleX = (int)(((long long)*(volatile int*)((char*)&mScaleX) * 0x800 + 0x800) >> 12);
    mScaleY = (int)(((long long)*(volatile int*)((char*)&mScaleY) * 0x800 + 0x800) >> 12);
    mScaleZ = (int)(((long long)*(volatile int*)((char*)&mScaleZ) * 0x800 + 0x800) >> 12);
  }
  ((struct Mdl*)((char*)&mModelAnim))->slot5((char*)&mScaleX);
  mScaleX = saved.x;
  mScaleY = saved.y;
  mScaleZ = saved.z;
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 32 -- _ZN5Koopa8BehaviorEv, 0x021190ec, size 0x334             */
/* -------------------------------------------------------------------------- */
// @symbol _ZN5Koopa8BehaviorEv
int Koopa::Behavior()
{
    int state;
    int kind;

    if (_ZN12dEnemyBase_c26UpdateKillByInvincibleCharER10dBgCh_ActrR9ModelAnimj(
            ((char *)this), ((char *)this) + 0x144, ((char *)this) + 0x300, 3) != 0)
        return 1;

    _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(((char *)this), ((char *)this) + 0x110);

    if (_ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(((char *)this), ((char *)this) + 0x144) != 0) {
        int *pb0 = (int *)((char *)&mFlags);
        *pb0 = *pb0 & ~0x10000000;
        if (_ZN12dEnemyBase_c27SpawnParticlesIfHitOtherObjER5dCc_c(((char *)this), ((char *)this) + 0x110) != 0) {
            _ZN8dActor_c8PoofDustEv(((char *)this));
            func_ov062_021179e4(((char *)this));
            _ZN8dActor_c24KillAndTrackInDeathTableEv(((char *)this));
        }
        if (mEatenByYoshi != 0)
            func_ov062_02117570(((char *)this));
        func_ov062_02118334(((char *)this));
        _ZN5dCc_c5ClearEv((char *)&mdCc_c);
        if (mEatenByYoshi != 0 && unk_104 == 0)
            _ZN5dCc_c6UpdateEv((char *)&mdCc_c);
        if (mKoopaVariant == 1)
            mState = 4;
        else
            mState = 1;
        if (_ZNK10dBgCh_Actr10IsOnGroundEv((char *)&mWithMeshClsn) != 0) {
            unk_3a8 = mPosX;
            unk_3ac = mPosY;
            unk_3b0 = mPosZ;
        }
        return 1;
    }

    if (mDeathState == 0) {
        if (_ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(((char *)this), 0x5dc000) != 0)
            return 1;

        {
            int *pb0 = (int *)((char *)&mFlags);
            if (mLandingDustTimer != 0)
                *(u8 *)((char *)&mLandingDustTimer) -= 1;
            *pb0 = *pb0 | 0x10000000;
        }
        func_ov062_02118258(((char *)this), 0x3e8000);

        if (mState != 0)
            mModelAnim.Advance();

        kind = mKoopaVariant;
        state = mState;
        switch (kind) {
        case 0:
        case 2:
            func_ov062_02117acc(((char *)this));
            break;
        case 1:
            func_ov062_02117a3c(((char *)this));
            break;
        }

        {
            int ang = mPrevAngleY;
            mAngleY = (s16)ang;
            {
                u16 *p100 = (u16 *)((char *)&mStateTimer);
                *p100 = (u16)(*p100 + 1);
            }
        }
        if (state != mState || kind != mKoopaVariant) {
            mStateTimer = 0;
            mWalkState = 0;
        }
        func_ov062_02117c98(((char *)this));
        _ZN8dActor_c9UpdatePosEP5dCc_c(((char *)this), ((char *)this) + 0x110);

        if (mDeathState == 0 && mState != 0) {
            if (_ZN12dEnemyBase_c15IsGoingOffCliffER10dBgCh_Actrisbbi(
                    ((char *)this), ((char *)this) + 0x144, 0x32000, 0x3800, 0, 1, 0x32000) != 0) {
                mPosX = unk_3a8;
                mPosY = unk_3ac;
                mPosZ = unk_3b0;
            } else {
                unk_3a8 = mPosX;
                unk_3ac = mPosY;
                unk_3b0 = mPosZ;
            }
        }

        _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(((char *)this), ((char *)this) + 0x144, 0);
        func_ov062_02117570(((char *)this));
        _ZN5dCc_c5ClearEv((char *)&mdCc_c);
        if (mDeathState == 0) {
            if (mInvincibleTimer == 0) {
                _ZN5dCc_c6UpdateEv((char *)&mdCc_c);
            } else {
                *(u16 *)((char *)&mInvincibleTimer) -= 1;
            }
        }
    } else {
        _ZN12dEnemyBase_c11UpdateDeathER10dBgCh_Actr(((char *)this), ((char *)this) + 0x144);
    }

    func_ov062_02118334(((char *)this));
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 33 -- _ZN5Koopa13InitResourcesEv, 0x02119420, size 0x1e8       */
/* -------------------------------------------------------------------------- */
// @symbol _ZN5Koopa13InitResourcesEv
int Koopa::InitResources()
{
    int i;
    int r, h;
    int kind;
    void* f;
    unsigned int isSpecial;

    kind = param1 & 1;
    mModelIndex = kind;

    for (i = 0; i < 9; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr((SharedFilePtr *) data_ov062_0211cee8[i]);

    f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov062_0211ced8[mModelIndex]);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0x300, f, 1, -1) == 0)
        return 0;

    if (_ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel) == 0)
        return 0;

    isSpecial = actorID == 0xcc;
    if (isSpecial)
    {
        mKoopaVariant = 2;
        mAnimSpeed = 0x2000;
        mScaleX = 0x599;
        mScaleY = 0x599;
        mScaleZ = 0x599;
        r = 0x1e000;
        h = 0x32000;
    }
    else
    {
        mKoopaVariant = 0;
        mAnimSpeed = 0x1000;
        mScaleX = 0xa66;
        mScaleY = 0xa66;
        mScaleZ = 0xa66;
        r = 0x3c000;
        h = 0x64000;
        _ZN5Model8LoadFileER13SharedFilePtr(data_ov062_0211cee0[mModelIndex]);
    }

    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char*)this) + 0x110, ((char*)this), r, h, 0x200000, 0xb6efe0);

    mStateTimer = 0;
    *(short*)(((char*)this) + 0x300 + 0xc4) = 0;
    mState = 1;
    func_ov062_02117994(((char*)this), 0);

    unk_106 = 0;
    *(short*)(((char*)this) + 0x300 + 0xca) = 0;
    unk_39c = mPosX;
    unk_3a0 = mPosY;
    unk_3a4 = mPosZ;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char*)this) + 0x144, ((char*)this), 0x32000, 0x32000, 0, 0);

    _ZN10dBgCh_Actr19StartDetectingWaterEv((char*)&mWithMeshClsn);

    unk_108 = 3;
    LoadBlueCoinModel(((char*)this));

    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 34 -- _ZN5Koopa16OnAimedAtWithEggEv, 0x02119608, size 0x20     */
/* -------------------------------------------------------------------------- */
// @symbol _ZN5Koopa16OnAimedAtWithEggEv
/* daNknk_c::OnAimedAtWithEgg -- vtable slot 29. */
s32 Koopa::OnAimedAtWithEgg() {
    void * c = (void *)this;
    unsigned short v = *(unsigned short*)((char*)c + 0xc);
    int r;
    if (v == 0xcb) r = 1; else r = 0;
    if (r != 0) r = 0x46000; else r = 0x25800;
    return r;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 35 -- _ZN5Koopa13OnTurnIntoEggER6Player, 0x02119628, size 0x80 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN5Koopa13OnTurnIntoEggER6Player
/* daNknk_c::OnTurnIntoEgg -- vtable slot 19, verified against ov062 relocs.txt:
 * _ZTV5Koopa (0x0211dab4) + 0x4c -> 0x02119628.
 *
 * `optimize_for_size on` came from the legacy shard. Bracketed with `off`
 * because `defer_codegen off` (top of file) makes the bracket bind; without
 * that pragma this pair would be file-global last-wins and would silently
 * recompile every member below it. */
#pragma optimize_for_size on
int Koopa::OnTurnIntoEgg(Player &player)
{
    if (unk_108 == 3) {
        if (OnYoshiTryEat() == 6 && !player.IsCollectingCap()) {
            player.RegisterEggCoinCount(0, 0, 1);
        } else {
            GivePlayerCoins(player, 1, 2);
        }
    }
    KillAndTrackInDeathTable();
}
#pragma optimize_for_size off

/* -------------------------------------------------------------------------- */
/* ROM ordinal 36 -- _ZN5Koopa13OnYoshiTryEatEv, 0x021196a8, size 0x14        */
/* -------------------------------------------------------------------------- */
// @symbol _ZN5Koopa13OnYoshiTryEatEv
s32 Koopa::OnYoshiTryEat() {
    char* c = (char*)this;
  if(*(int*)(c+0x394)==0) return 6;
  return 5;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 37 -- daNknk_c_classInit_NOKONOKO_S, 0x021196bc, size 0x50     */
/* -------------------------------------------------------------------------- */
// @symbol daNknk_c_classInit_NOKONOKO_S
/* Reconstructed source-style name: SM64DS proves daNknk_c through RTTI,
 * allocation size, vtable identity, and the NOKONOKO_S registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: KoopaSmall_Spawn.
 *
 * The vptr store is `&_ZTV5Koopa[2]`, not the bare symbol. In the legacy
 * one-function shard `_ZTV5Koopa` was an external whose ROM home IS the
 * address point; here this TU DEFINES the table, so mwcc's own symbol names
 * the storage start and the address point is eight bytes on. Only objisolate's
 * addend check sees the difference. */
extern "C" int *daNknk_c_classInit_NOKONOKO_S(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(976);
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)&_ZTV5Koopa[2];
        _ZN7dCcAc_cC1Ev((char *)p + 0x110);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x144);
        _ZN9ModelAnimC1Ev((char *)p + 0x300);
        _ZN11ShadowModelC1Ev((char *)p + 0x364);
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 38 -- daNknk_c_classInit_NOKONOKO, 0x0211970c, size 0x50       */
/* -------------------------------------------------------------------------- */
// @symbol daNknk_c_classInit_NOKONOKO
/* Historical alias: Koopa_Spawn. See ordinal 37 for the vptr addend. */
extern "C" int *daNknk_c_classInit_NOKONOKO(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(976);
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)&_ZTV5Koopa[2];
        _ZN7dCcAc_cC1Ev((char *)p + 0x110);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x144);
        _ZN9ModelAnimC1Ev((char *)p + 0x300);
        _ZN11ShadowModelC1Ev((char *)p + 0x364);
    }
    return p;
}
