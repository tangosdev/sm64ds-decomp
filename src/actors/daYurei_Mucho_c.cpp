//cpp
/* Production translation unit for ov065/daYurei_Mucho_c.
 *
 * The cartridge names this class daYurei_Mucho_c: overlay_0065.bin at file
 * offset 0x6c8c (address 0x0211cb6c, module base 0x02115ee0) holds
 * "15daYurei_Mucho_c\0", the length-prefixed _ZTS payload that
 * _ZTI15daYurei_Mucho_c at 0x0211cb60 points at. That record's third word is
 * ov002 0x021081c0 = _ZTI12dEnemyBase_c, which is where the base clause comes
 * from.
 *
 * mwccarm emits ordinary functions in reverse source order, so the twenty-one
 * definitions below run from the highest retail address (the factory at
 * 0x02116f48) back toward the compiler-owned destructor group at 0x02115ee0.
 *
 * The twelve func_ov065_* bodies keep their address-derived names: ov065's
 * symbols.txt spells them that way, and eight of them are reached ONLY through
 * the pointer-to-member records at 0x0211cb20..0x0211cb60, which dsd resolves
 * BY NAME out of that same file. Renaming one is a same-commit symbols.txt
 * edit and no byte gate would catch a miss, so they stay C-linkage free
 * functions here.
 *
 * Superseded one-function sources, by basename -- none of them exists any
 * more, and this file is the one delinks.txt entry that replaced all 23
 * (ROM address order):
 *   [0]  0x02115ee0  _ZN15daYurei_Mucho_cD1Ev.cpp
 *   [1]  0x02115f28  _ZN15daYurei_Mucho_cD0Ev.cpp
 *   [2]  0x02115f84  func_ov065_02115f84.c
 *   [3]  0x02115ff0  func_ov065_02115ff0.c
 *   [4]  0x021162c0  func_ov065_021162c0.c
 *   [5]  0x02116328  func_ov065_02116328.c
 *   [6]  0x02116364  func_ov065_02116364.cpp
 *   [7]  0x02116588  func_ov065_02116588.cpp
 *   [8]  0x021165d8  func_ov065_021165d8.cpp
 *   [9]  0x0211672c  func_ov065_0211672c.c
 *   [10] 0x02116744  func_ov065_02116744.c
 *   [11] 0x021168a8  func_ov065_021168a8.cpp
 *   [12] 0x0211691c  func_ov065_0211691c.cpp
 *   [13] 0x0211696c  func_ov065_0211696c.c
 *   [14] 0x02116ae8  _ZN15daYurei_Mucho_c16CleanupResourcesEv.cpp
 *   [15] 0x02116b30  _ZN15daYurei_Mucho_c16OnPendingDestroyEv.cpp
 *   [16] 0x02116b34  _ZN15daYurei_Mucho_c6RenderEv.cpp
 *   [17] 0x02116b84  _ZN15daYurei_Mucho_c8BehaviorEv.cpp
 *   [18] 0x02116e10  _ZN15daYurei_Mucho_c13InitResourcesEv.cpp
 *   [19] 0x02116f0c  _ZN15daYurei_Mucho_c16OnAimedAtWithEggEv.cpp
 *   [20] 0x02116f14  _ZN15daYurei_Mucho_c13OnTurnIntoEggER6Player.cpp
 *   [21] 0x02116f40  _ZN15daYurei_Mucho_c13OnYoshiTryEatEv.cpp
 *   [22] 0x02116f48  d_a_yurei_mucho.c
 *
 * [22] is the registry factory, folded in rather than left outside. Its body
 * relocates to exactly one vtable in this module, _ZTV15daYurei_Mucho_c (the
 * vptr store at 0x02116f68 off the literal at 0x02116f94), which is the
 * cartridge's own type evidence and the same test tools/tu_map.py already
 * applies to a `<Class>_Spawn` factory. tu_map splits it into its own unit only
 * because srcpath.class_of matches `_Spawn` and not `_classInit`, so the run's
 * label and the factory's ROM-RTTI label never met.
 */

#include "daYurei_Mucho_c.h"
#include "types.h"
#include "common.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "Player.h"

/* Two stand-ins with no project header to take them from. Both are load-bearing
   for codegen, not conveniences: the twelve-word matrix is copied as one object
   and the three-word group is copied as one object, and spelling either as
   three scalar assignments changes what mwccarm emits. */
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

/* Reconciled once, here, from the twenty-three legacy files' own views. Each
 * declared only what it needed, so the union below keeps the most complete
 * observation of every name -- a return type that is used over a `void`, a real
 * project type over a stand-in.
 *
 * `extern` inside the block on purpose: the bare block form would DEFINE the
 * data objects and collide at link time. Every ROM symbol is spelled by its
 * already-mangled name; an ordinary C++ declaration would mangle it a SECOND
 * time and emit a reference nothing defines. */
extern "C" {

/* arm9 */
extern void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
extern char *_ZN8dActor_c22ClosestNonVanishPlayerEv(void *self);
extern int _ZN8dActor_c13SmallPoofDustEv(void *self);
extern int _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void *self, const Vector3 *pos,
                                                          unsigned int n, int f, short s);
extern int _ZN8dActor_c24KillAndTrackInDeathTableEv(void *self);
extern int _ZN8dActor_c24BumpedUnderneathByPlayerER6Player(void *self, void *player);
extern int _ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(void *self, void *cyl, void *player);
extern void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int profile,
                                                             unsigned int param,
                                                             const void *pos, const void *rot,
                                                             int area, short id);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *self, void *shadow, void *mtx, int rad, int height, unsigned int flags);
extern int _ZN6Player9IsOnShellEv(void *p);
extern void _ZN6Player6BounceE5Fix12IiE(void *p, int f);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *p, const void *v, unsigned int a,
                                                    int b, unsigned int d, unsigned int e,
                                                    unsigned int f);
extern int _ZN9Animation8FinishedEv(void *anim);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca, int a, int fix,
                                                        unsigned int j);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(const void *m);
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
extern void MulMat4x3Mat4x3(void *dst, const void *a, const void *b);
extern int func_02012694(int id, void *v);
extern int data_020a0e68[];
extern int data_0209e650[];
extern short data_02082214[];

/* ov002 -- dEnemyBase_c members not declared in dEnemyBase_c.h yet */
extern void _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(
    void *self, const void *v, void *p, int a);
extern int _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(dEnemyBase_c *self, dBgCh_Actr *c);
extern void func_ov002_020aea30(void *self, void *p, int a, int b);

/* The four subobject entry points the folded factory calls that no header this
   TU includes declares: fBase_c::operator new, the dEnemyBase_c base-object
   constructor (C2 is never directly declarable in C++), and the two member
   constructors whose headers stop short of a default constructor. They are
   restated here rather than reached by adding include/decl_ActorBase.h,
   decl_Enemy.h, decl_dCcAc_c.h or decl_dBgCh_Actr.h: those headers also carry
   declarations this TU deliberately takes from elsewhere, and pulling one in to
   satisfy a single extern would silently move that choice.
   ModelAnim.h and ShadowModel.h do declare their constructors, but as C++
   constructors, which cannot be invoked on the raw allocation the retail
   factory holds; the mangled spelling names the same two symbols and is what
   the calls below resolve to. */
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN12dEnemyBase_cC2Ev(void *);
extern void _ZN7dCcAc_cC1Ev(void *);
extern void _ZN10dBgCh_ActrC1Ev(void *);
extern void _ZN9ModelAnimC1Ev(void *);
extern void _ZN11ShadowModelC1Ev(void *);

/* ov065 -- this class's own unowned .bss. The four SharedFilePtrs and the four
   State objects are constructed by the module's static initializer, which lives
   in ov065's .init section and is NOT part of this TU; defining any of them
   here would make mwccarm emit a second initializer that the text-only
   isolation would then have to strip. They stay extern. */
extern SharedFilePtr data_ov065_0211d600;
extern SharedFilePtr data_ov065_0211d608;
extern SharedFilePtr data_ov065_0211d610;
extern SharedFilePtr data_ov065_0211d618;
/* Only 0x0211d670 can be spelled with its real type here: include/decl_common.h,
   which this TU takes _ZTV15daYurei_Mucho_c from, already declares the other
   three -- 0x0211d650 and 0x0211d660 as char[] and 0x0211d680 as a bare int --
   and a second, differing declaration is a compile error. They are reached
   through a cast at each use instead. Only the address ever reaches the code,
   so the cast is byte-free. */
extern char data_ov065_0211d670[];

/* ov065 -- this TU's own free helpers, forward-declared because mwccarm emits
   .text in reverse source order and every caller is written above its callee. */
extern int func_ov065_02115f84(char *c);
extern void func_ov065_02115ff0(char *c);
extern int func_ov065_0211691c(void *c, daYurei_Mucho_c::State *s);
extern void func_ov065_0211696c(char *c);

}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- daYurei_Mucho_c_classInit, 0x02116f48, size 0x50          */
/* -------------------------------------------------------------------------- */
// @symbol daYurei_Mucho_c_classInit
/* The registry factory for profile 236: g_profile_YUREI_MUCHO at 0x0211cb80
 * points its +0 word here, and its own id word 0x005b00ec has low half 0xec =
 * 236, agreeing with ACTOR_SPAWN_TABLE[236] and the ACTOR_DEBUG_NAMES[236]
 * string "YUREI_MUCHO".
 *
 * SIZE IS THE ROM'S OWN LITERAL: `mov r0, #0x3e4` at 0x02116f4c, then
 * bl arm9 0x02043444 = fBase_c::operator new(u32). A field span is only a
 * lower bound; this is the allocation.
 *
 * The vptr store is written out rather than left to a constructor because the
 * retail body is exactly this shape: operator new, dEnemyBase_c::C2, one `str`
 * of the vtable address point, then the four subobject C1 calls in ascending
 * offset order. `_ZTV15daYurei_Mucho_c` is taken from include/decl_common.h,
 * which the tree already carried for this class; restating it locally would
 * raise the langmode extern_vtable metric for no byte benefit.
 *
 * THE `+ 2` IS LOAD-BEARING, and the fold is what changed it. The legacy shard
 * wrote the bare `(int)_ZTV15daYurei_Mucho_c`, and that was correct there: the
 * symbol was UNDEF and config/arm9/overlays/ov065/symbols.txt binds it to the
 * ADDRESS POINT at 0x0211cba4. Here the same spelling binds to this TU's own
 * definition instead -- the inline destructor is the key function, so mwcc
 * emits the vtable and addresses the storage OBJECT, two words of Itanium
 * preamble lower at 0x0211cb9c. `+ 2` is int-indexed, so eight bytes, which is
 * exactly the bias measured from the ROM: the literal at 0x02116f94 that
 * `ldr r1,[pc,#0x2c]` (0x02116f60) loads and `str r1,[r4]` (0x02116f68) writes
 * to this+0 is 0x0211cba4, and 0x0211cba4 - 0x0211cb9c = 8. It also makes this
 * store agree with the addend-8 vptr stores mwcc emits in D1/D0 by itself.
 * A relocated word is a wildcard to every byte gate, so tools/match.py reports
 * MATCH either way; tools/objisolate.py's addend check is what refuses the bare
 * form, and it did (`_ZTV15daYurei_Mucho_c: unexpected reloc type=2 addend=0`).
 *
 * Reconstructed source-style name: SM64DS proves daYurei_Mucho_c through RTTI,
 * allocation size, vtable identity and the YUREI_MUCHO registry profile; later
 * EAD lineage supplies `classInit`. Exact original spelling is not preserved.
 * Historical alias: Snufit_Spawn. */
extern "C" int *daYurei_Mucho_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(996);
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)(_ZTV15daYurei_Mucho_c + 2);
        _ZN7dCcAc_cC1Ev((char *)p + 0x110);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x144);
        _ZN9ModelAnimC1Ev((char *)p + 0x300);
        _ZN11ShadowModelC1Ev((char *)p + 0x364);
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- _ZN15daYurei_Mucho_c13OnYoshiTryEatEv, 0x02116f40, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c13OnYoshiTryEatEv
/* dActor_c vtable slot 18. */
s32 daYurei_Mucho_c::OnYoshiTryEat()
{
    return 4;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- _ZN15daYurei_Mucho_c13OnTurnIntoEggER6Player, 0x02116f14, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c13OnTurnIntoEggER6Player
/* dActor_c vtable slot 19, confirmed by address: _ZTV15daYurei_Mucho_c
 * (0x0211cba4) + 0x4c -> 0x02116f14.
 *
 * The `R6Player` reference spelling in the mangled name is a coined guess: a
 * reference and a pointer generate identical ARM for this body, so the bytes
 * cannot distinguish them. */
int daYurei_Mucho_c::OnTurnIntoEgg(Player &player)
{
    GivePlayerCoins(player, (unsigned char)(unk_10a + 1), 0);
    KillAndTrackInDeathTable();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- _ZN15daYurei_Mucho_c16OnAimedAtWithEggEv, 0x02116f0c, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c16OnAimedAtWithEggEv
/* dActor_c vtable slot 29. */
s32 daYurei_Mucho_c::OnAimedAtWithEgg()
{
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- _ZN15daYurei_Mucho_c13InitResourcesEv, 0x02116e10, size 0xfc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c13InitResourcesEv
/* dActor_c vtable slot 0.
 *
 * dCcAc_c::Init and dBgCh_Actr::Init are still reached through their mangled
 * names because neither is declared as a method yet. */
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
/* ROM ordinal 17 -- _ZN15daYurei_Mucho_c8BehaviorEv, 0x02116b84, size 0x28c   */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c8BehaviorEv
/* dActor_c vtable slot 6.
 *
 * dEnemyBase_c::UpdateYoshiEat is still reached by its mangled name -- unlike
 * UpdateDeath, UpdateWMClsn and UpdateKillByInvincibleChar, it is not declared
 * in dEnemyBase_c.h yet. */
int daYurei_Mucho_c::Behavior()
{
    if (_ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(this, &mWithMeshClsn) != 0) {
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
           `(int)this + 0x3d8` is not interchangeable with `&unk_3d8`. */
        pAngle = (int *)(((int)this + 0x3d8));
        *pAngle += 0x200;
        ang = unk_3d8;
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
        char *p = (char *)ClosestPlayer();
        if (p != 0 && *(unsigned char *)(p + 0x6fb) == 0) {
            mdCcAc_c.Update();
        }
    }
    mModelAnim.Advance();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- _ZN15daYurei_Mucho_c6RenderEv, 0x02116b34, size 0x50      */
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
/* ROM ordinal 15 -- _ZN15daYurei_Mucho_c16OnPendingDestroyEv, 0x02116b30, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c16OnPendingDestroyEv
/* fBase_c slot 12. Empty in the ROM: four bytes, `bx lr`. */
void daYurei_Mucho_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- _ZN15daYurei_Mucho_c16CleanupResourcesEv, 0x02116ae8, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c16CleanupResourcesEv
/* dActor_c vtable slot 3. Releases the four files InitResources claimed.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free. */
int daYurei_Mucho_c::CleanupResources()
{
    data_ov065_0211d610.Release();
    data_ov065_0211d618.Release();
    data_ov065_0211d600.Release();
    data_ov065_0211d608.Release();
    return 1;
}

/* ========================================================================== */
/* The twelve free helpers. A class member function cannot sit inside an       */
/* extern "C" region, so they are gathered here, after the last member.        */
/* ========================================================================== */
extern "C" {

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov065_0211696c, 0x0211696c, size 0x17c              */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_0211696c
/* Rebuilds the model matrix from the actor's position and Z/X/Y angles, then
   drops the shadow. Called three times from Behavior. */
void func_ov065_0211696c(char *c)
{
    int v[3];

    Vec3_Asr(v, c + 0x5c, 3);
    Matrix4x3_FromTranslation(data_020a0e68, v[0], v[1], v[2]);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(data_020a0e68,
        *(s16 *)(c + 0x8c), *(s16 *)(c + 0x8e), *(s16 *)(c + 0x90));

    *(Mtx43 *)(c + 0x31c) = *(Mtx43 *)data_020a0e68;
    *(int *)(c + 0x3c0) = 0;
    *(int *)(c + 0x3c4) = 0;
    *(int *)(c + 0x3c8) = 0;
    *(Mtx43 *)data_020a0e68 = *(Mtx43 *)(c + 0x31c);

    MulMat4x3Mat4x3(*(char **)(c + 0x314) + 0xc0, data_020a0e68, data_020a0e68);

    *(int *)(c + 0x3c0) = data_020a0e68[9];
    *(int *)(c + 0x3c4) = data_020a0e68[10];
    *(int *)(c + 0x3c8) = data_020a0e68[11];
    *(int *)(((int)c + 0x3c0)) <<= 3;
    *(int *)(((int)c + 0x3c4)) <<= 3;
    *(int *)(((int)c + 0x3c4)) -= 0xa000;
    *(int *)(((int)c + 0x3c8)) <<= 3;

    Matrix4x3_FromTranslation(data_020a0e68,
        *(int *)(c + 0x5c) >> 3,
        (*(int *)(c + 0x60) - 0x18000) >> 3,
        *(int *)(c + 0x64) >> 3);

    *(Mtx43 *)(c + 0x38c) = *(Mtx43 *)data_020a0e68;

    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x364, c + 0x38c, 0x64000, 0x258000, 0xf);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov065_0211691c, 0x0211691c, size 0x50               */
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
int func_ov065_0211691c(void *self, daYurei_Mucho_c::State *s)
{
    daYurei_Mucho_c *c = (daYurei_Mucho_c *)self;
    c->mCurrentState = s;
    daYurei_Mucho_c::State *q = c->mCurrentState;
    if (q->mEnter == 0) {
        return 1;
    }
    return (c->*(q->mEnter))();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov065_021168a8, 0x021168a8, size 0x74               */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_021168a8
/* State entry hook at 0x0211d670 +0x00: randomise the facing angle and the
   timer, then start the wait animation. The BCA file is read straight out of
   the SharedFilePtr's second word -- the ROM does `ldr r1,[r0,#4]` off the
   literal at 0x02116918, not a call, so the raw read is what reproduces. */
int func_ov065_021168a8(char *c)
{
    *(short *)(c + 0x3e0) = (short)((RandomIntInternal(data_0209e650) >> 8) << 0xc);
    *(short *)(c + 0x100) = (short)(((RandomIntInternal(data_0209e650) >> 8) & 0x1f) + 0x32);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300,
        ((void **)&data_ov065_0211d600)[1], 0, 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov065_02116744, 0x02116744, size 0x164              */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_02116744
/* State main hook at 0x0211d670 +0x08: drift, and switch to the attack state
   when a player comes close enough. */
int func_ov065_02116744(char *c)
{
    int in[3];
    int dp[3];
    char *pl;
    int *pos;

    in[0] = 0;
    in[1] = 0;
    in[2] = 0;

    if (Vec3_Dist(c + 0x5c, c + 0x3cc) > 0x1f4000 ||
        _ZNK10dBgCh_Actr8IsOnWallEv(c + 0x144) != 0) {
        *(s16 *)(c + 0x300 + 0xe0) =
            Vec3_HorzAngle(c + 0x5c, c + 0x3cc);
        if (*(unsigned short *)(c + 0x100) < 0x14)
            *(unsigned short *)(c + 0x100) = 0x14;
    }

    ApproachAngle((s16 *)(c + 0x94),
                  *(s16 *)(c + 0x300 + 0xe0),
                  0xa, 0x200, 0x100);
    ApproachAngle((s16 *)(c + 0x92), 0, 1, 0x500, 0x500);

    in[2] = 0xa000;
    Matrix4x3_FromRotationY(data_020a0e68, *(s16 *)(c + 0x8e));
    MulVec3Mat4x3(in, data_020a0e68, c + 0xa4);

    if (*(unsigned short *)(c + 0x100) == 0) {
        func_ov065_0211691c(c, (daYurei_Mucho_c::State *)data_ov065_0211d670);
        return 1;
    }

    pl = _ZN8dActor_c22ClosestNonVanishPlayerEv(c);
    if (pl != 0) {
        pos = (int *)(((int)pl + 0x5c));
        dp[0] = pos[0];
        dp[1] = pos[1];
        dp[2] = pos[2];

        if (Vec3_Dist(c + 0x5c, dp) < 0x3e8000) {
            *(unsigned short *)(c + 0x100) = 0x14;
            func_ov065_0211691c(c, (daYurei_Mucho_c::State *)&data_ov065_0211d680);
        }
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov065_0211672c, 0x0211672c, size 0x18                */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_0211672c
/* State entry hook at 0x0211d680 +0x00: stop the actor dead. */
int func_ov065_0211672c(char *p)
{
    *(int *)(p + 0xa4) = 0;
    *(int *)(p + 0xa8) = 0;
    *(int *)(p + 0xac) = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov065_021165d8, 0x021165d8, size 0x154               */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_021165d8
/* State main hook at 0x0211d680 +0x08: steer toward the nearest non-vanishing
   player.

   dActor_c::ClosestNonVanishPlayer takes `this` in r0; the ROM emits no `mov`
   before the `bl` at 0x021165e8 because r0 still holds the incoming object. */
int func_ov065_021165d8(char *c)
{
    short r4 = 0;
    char *p = _ZN8dActor_c22ClosestNonVanishPlayerEv(c);
    if (p != 0) {
        Vector3 tmp = *(Vector3 *)(p + 0x5c);
        Vector3 v;
        Vector3 a;
        a.x = tmp.x;
        a.y = tmp.y;
        a.z = tmp.z;
        *(short *)(c + 0x3e0) = Vec3_HorzAngle(c + 0x5c, &a);
        Vector3 b;
        b.x = tmp.x;
        b.y = tmp.y;
        b.z = tmp.z;
        r4 = Vec3_VertAngle(c + 0x5c, &b);
        if (Vec3_Dist(c + 0x5c, &tmp) >= 0x1f4000) {
            *(int *)(c + 0xa4) = 0;
            *(int *)(c + 0xa8) = 0;
            *(int *)(c + 0xac) = 0;
        } else {
            v.z = 0;
            v.z = -0x1000;
            v.x = 0;
            v.y = 0;
            Matrix4x3_FromRotationY(data_020a0e68, *(short *)(c + 0x8e));
            MulVec3Mat4x3(&v, data_020a0e68, c + 0xa4);
        }
    } else {
        *(short *)(c + 0x100) = r4;
    }
    ApproachAngle(c + 0x94, *(short *)(c + 0x3e0), 1, 0x500, 0x500);
    ApproachAngle(c + 0x92, r4, 1, 0x500, 0x500);
    if (*(unsigned short *)(c + 0x100) == 0)
        func_ov065_0211691c(c, (daYurei_Mucho_c::State *)data_ov065_0211d650);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov065_02116588, 0x02116588, size 0x50                */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_02116588
/* State entry hook at 0x0211d650 +0x00: start the attack animation. As at
   ordinal 11, the BCA file is the SharedFilePtr's second word, read directly
   (`ldr r1,[r0,#4]` at 0x021165a4 off the literal at 0x021165d4). */
short func_ov065_02116588(char *c)
{
    *(int *)(c + 0x3dc) = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300,
        ((void **)&data_ov065_0211d608)[1], 0x40000000, 0x1000, 0);
    *(short *)(c + 0x100) = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov065_02116364, 0x02116364, size 0x224               */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_02116364
/* State main hook at 0x0211d650 +0x08: aim, and spawn the projectile (profile
   0xe9) up to three times before returning to a wait state. */
int func_ov065_02116364(void *self)
{
    u8 *c = (u8 *)self;
    V3Quint L;
    u8 *pl = (u8 *)_ZN8dActor_c22ClosestNonVanishPlayerEv(self);
    if (pl != 0) {
        *(V3A *)(int)(&L.pp) = *(V3A *)(int)(pl + 0x5c);
        L.tgt = L.pp;
        *(s16 *)(c + 0x3e0) = Vec3_HorzAngle((void *)(c + 0x5c), &L.tgt);
        ApproachAngle((void *)(c + 0x94), *(s16 *)(c + 0x3e0), 1, 0x500, 0x500);

        if (((*(u32 *)(c + 0x358) << 4) >> 16) >= 0xf
            && *(u16 *)(c + 0x100) == 0
            && *(s32 *)(c + 0x3dc) < 3) {
            void *spawned = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                0xe9, 1, (void *)(c + 0x3c0), 0, *(signed char *)(c + 0xcc), -1);
            if (spawned != 0) {
                u8 *sp2 = (u8 *)spawned;
                func_02012694(0xfb, (void *)(c + 0x74));
                L.spv.x = 0;
                L.spv.y = 0;
                L.spv.z = 0x1e000;
                L.sout.x = 0;
                L.sout.y = 0;
                L.sout.z = 0;
                Matrix4x3_FromRotationY(&data_020a0e68, *(s16 *)(c + 0x8e));
                Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(s16 *)(c + 0x8c));
                MulVec3Mat4x3(&L.spv, &data_020a0e68, &L.sout);
                *(s32 *)(sp2 + 0xa4) = L.sout.x;
                *(s32 *)(sp2 + 0xa8) = L.sout.y;
                *(s32 *)(sp2 + 0xac) = L.sout.z;
                *(s32 *)(int)(c + 0x3dc) += 1;
                *(u16 *)(c + 0x100) = 4;
            }
        }
    }

    if (_ZN9Animation8FinishedEv((void *)(c + 0x350)) != 0) {
        if (pl != 0) {
            s32 *dsrc = (s32 *)(int)(pl + 0x5c);
            L.d.x = dsrc[0];
            L.d.y = dsrc[1];
            L.d.z = dsrc[2];
            if (Vec3_Dist((void *)(c + 0x5c), &L.d) > 0x3e8000) {
                *(u16 *)(c + 0x100) = 0;
                func_ov065_0211691c(self, (daYurei_Mucho_c::State *)data_ov065_0211d670);
            } else {
                *(u16 *)(c + 0x100) = 0x32;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void *)(c + 0x300),
                    ((void **)&data_ov065_0211d600)[1], 0, 0x1000, 0);
                func_ov065_0211691c(self, (daYurei_Mucho_c::State *)&data_ov065_0211d680);
            }
        } else {
            *(u16 *)(c + 0x100) = 0;
            func_ov065_0211691c(self, (daYurei_Mucho_c::State *)data_ov065_0211d670);
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov065_02116328, 0x02116328, size 0x3c                */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_02116328
/* State entry hook at 0x0211d660 +0x00: the bumped-from-below pop. */
int func_ov065_02116328(void *c)
{
    *(int *)((char *)c + 0xa4) = 0;
    *(int *)((char *)c + 0xa8) = 0;
    *(int *)((char *)c + 0xac) = 0;
    *(int *)((char *)c + 0xa8) = 0x32000;
    *(int *)((char *)c + 0x9c) = -0x5000;
    *(short *)((char *)c + 0x100) = 0xa;
    *(int *)((char *)c + 0xb0) = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov065_021162c0, 0x021162c0, size 0x68                */
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
int func_ov065_021162c0(char *t)
{
    ApproachAngle((short *)(t + 0x8c), -0x4000, 0xa, 0x200, 0x100);
    _Z14ApproachLinearRsss((short *)(t + 0x8c), -0x4000, 0x200);
    if (*(unsigned short *)(t + 0x100) == 0)
        func_ov065_02115f84(t);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov065_02115ff0, 0x02115ff0, size 0x2d0               */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_02115ff0
/* The collision response, called once from Behavior. */
void func_ov065_02115ff0(char *c)
{
    short v[3];
    int hv[3];
    char *p;
    int flags;
    unsigned int id;

    id = *(unsigned int *)(c + 0x134);
    if (id == 0) return;
    p = (char *)_ZN8dActor_c10FindWithIDEj(id);
    if (p == 0) return;
    flags = *(int *)(c + 0x130);

    if (flags & 0x40000) {
        *(int *)(c + 0x10c) = 4;
        func_ov002_020aea30(c, p, 0, 4);
        func_02012694(0x11e, c + 0x74);
        return;
    }
    if (flags & 0x2400) {
        *(int *)(c + 0x10c) = 2;
        func_ov002_020aea30(c, p, 0, 2);
        func_02012694(0x11e, c + 0x74);
        return;
    }
    if (flags & 0x4380) {
        *(int *)(c + 0x10c) = 3;
        func_ov002_020aea30(c, p, 0, 3);
        func_02012694(0x11e, c + 0x74);
        return;
    }
    if (_ZN8dActor_c24BumpedUnderneathByPlayerER6Player(c, p) == 1) {
        func_02012694(0x11e, c + 0x74);
        func_ov065_0211691c(c, (daYurei_Mucho_c::State *)data_ov065_0211d660);
        return;
    }
    if (flags & 0x10) {
        v[0] = -0x2000;
        v[1] = 0;
        v[2] = 0;
        _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(c, v, p, 0);
        func_02012694(0x11e, c + 0x74);
        return;
    }
    if (flags & 0x40) {
        *(int *)(c + 0x10c) = 2;
        func_ov002_020aea30(c, p, 0, 2);
        func_02012694(0x11e, c + 0x74);
        return;
    }
    if (flags & 0x20) {
        *(int *)(c + 0x10c) = 1;
        func_ov002_020aea30(c, p, 0, 1);
        func_02012694(0x11e, c + 0x74);
        return;
    }
    {
        int b = (int)(((long long)(*(unsigned short *)(p + 0xc) == 0xbf)));
        if (b == 0) return;
    }
    if (*(unsigned char *)(p + 0x6f9) == 1 || _ZN6Player9IsOnShellEv(p) == 1) {
        func_ov065_02115f84(c);
        return;
    }
    if (_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(c, c + 0x110, p)) {
        _ZN6Player6BounceE5Fix12IiE(p, 0x28000);
        *(int *)(c + 0x10c) = 1;
        func_ov002_020aea30(c, p, 0, 1);
        func_02012694(0x11e, c + 0x74);
        return;
    }
    hv[0] = *(int *)(c + 0x5c);
    hv[1] = *(int *)(c + 0x60);
    hv[2] = *(int *)(c + 0x64);
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(p, hv, 2, 0xc000, 1, 0, 1);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov065_02115f84, 0x02115f84, size 0x6c                */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_02115f84
/* The death helper: poof, drop the coins, unregister. */
int func_ov065_02115f84(char *c)
{
    Vector3 v;
    _ZN8dActor_c13SmallPoofDustEv(c);
    v.x = *(int *)(c + 0x5c);
    v.y = *(int *)(c + 0x60);
    v.z = *(int *)(c + 0x64);
    _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(c, &v, *(unsigned char *)(c + 0x10a) + 1, 0xa000, 0);
    _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
    return func_02012694(0x11e, c + 0x74);
}

}  /* extern "C" */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN15daYurei_Mucho_cD0Ev, 0x02115f28, size 0x5c           */
/* ROM ordinal 0 -- _ZN15daYurei_Mucho_cD1Ev, 0x02115ee0, size 0x48           */
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
