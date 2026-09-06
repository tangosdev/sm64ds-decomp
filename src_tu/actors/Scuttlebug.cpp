//cpp
/* Genuine production translation unit for ov071/Scuttlebug.
 *
 * SM64DS RTTI names the implementation daSpd_c; the project spells the class
 * Scuttlebug and symbols.txt carries both names on the vtable at 0x02122c2c.
 * daSpd_c_classInit is a reconstructed source-style name (historical alias
 * Scuttlebug_Spawn); the SPIDER registry profile, the 0x3ac allocation size and
 * the vtable identity are proven, the exact original spelling is not.
 *
 * mwccarm emits ordinary function sections in REVERSE source order, so this
 * file is written ROM-descending: the factory at 0x02120618 first and
 * OnYoshiTryEat at 0x0211f0a4 last.  The destructor is defined inline in
 * include/Scuttlebug.h and OnYoshiTryEat is the key function; together they
 * emit retail D1 (0x0211f000) then D0 (0x0211f048) below every other section,
 * plus the class vtable and RTTI, with no D2 and no forcing scaffold.
 *
 * common.h is included FIRST on purpose.  Scuttlebug.h reaches math/Matrix.h
 * through ModelAnim.h, and that header spells Matrix4x3 as `Matrix3x3 r;
 * Vector3 t;` where common.h spells it flat.  The legacy InitResources shard
 * saw the flat one (decl_common.h -> common.h ahead of the actor header) and
 * the whole-matrix assignment into +0x350 only reproduces with that spelling.
 */

#include "common.h"
#include "Scuttlebug.h"
#include "dBgCh_Gnd.h"
#include "Player.h"

/* ------------------------------------------------------------------------
 * Local value shapes carried from the legacy one-function files.
 * ------------------------------------------------------------------------ */

/* Three plain fixed-point words.  Vector3 has a user-declared destructor, so
 * spelling these as Vector3 would drag its vague-linkage D1 into functions
 * that never had it. */
typedef struct { int x, y, z; } Vec3;

/* Flat 0x30-byte matrix block, as func_ov071_0211f524 and the scratch matrix
 * at arm9:0x020a0e68 are used: whole-block assignment, never by field. */
typedef struct Mtx43 { int w[12]; } Mtx43;

/* One row of the state table at ov071:0x02122fa8. */
typedef struct { int a, b, c, d; } Item16;

/* The dispatch object as the two state-machine trampolines see it: a
 * pointer-to-member-function table at +0x380.  PMF is deliberately formed
 * while ScuttlebugState is INCOMPLETE -- that is what selects the general
 * (offset + index) pointer-to-member representation the ROM uses. */
struct ScuttlebugState;
typedef void (ScuttlebugState::*PMF)();
struct ScuttlebugState { char pad[0x380]; PMF *pp; };

/* Slot 29 (vtable+0x74) reached through a raw cast, in the two functions that
 * ask the actor for its own height rather than going through the header. */
struct VSlot29 {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual int  m29();
};

/* ------------------------------------------------------------------------
 * ABI imports.  One declaration per symbol, hand-reconciled: the generated
 * preamble carried these names with contradictory spellings from thirty-odd
 * source files.  The by-value Fix12<int> and Vector3 parameters are spelt as
 * plain words / pointers on purpose -- the true types are passed differently
 * and break the register convention.
 * ------------------------------------------------------------------------ */
extern "C" {

short AngleDiff(short a, short b);
void  DecIfAbove0_Short(void *p);
int   Vec3_Dist(const Vector3 *a, const Vector3 *b);
short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
void  _Z14ApproachLinearRsss(short &dst, short target, short step);
int   _ZN4cstd4fdivEii(int a, int b);

void  Matrix4x3_FromRotationY(void *m, int angle);
void  Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
void  Matrix4x3_ApplyInPlaceToRotationX(void *m, s16 angX);

void  func_0201267c(int id, void *pos);
void  func_02012694(int id, void *pos);
int   func_02037e38(unsigned int *p);

void  dBgCh_Actr_UpdateDiscreteNoLava_veneer(void *p);
int   _ZNK10dBgCh_Actr10IsOnGroundEv(void *self);
int   _ZNK10dBgCh_Actr8IsOnWallEv(void *self);
int   _ZNK10dBgCh_Actr13JustHitGroundEv(void *self);
int   _ZNK10dBgCh_Actr14GetResultFlag1Ev(void *self);
int   _ZNK10dBgCh_Actr12TouchesWaterEv(void *self);
void *_ZNK10dBgCh_Actr14GetFloorResultEv(void *self);
void *_ZNK10dBgCh_Actr13GetWallResultEv(void *self);
void  _ZN10dBgCh_Actr13SetLimMovFlagEv(void *self);
void  _ZN10dBgCh_Actr15ClearLimMovFlagEv(void *self);
void  _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *self, Vector3 *out);

void  _ZN9Animation7AdvanceEv(void *self);
void  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca, int a,
                                                  int fix, unsigned int j);
void  _ZN5dCc_c5ClearEv(void *self);
void  _ZN5dCc_c6UpdateEv(void *self);

void  _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *clsn);
void *_ZN8dActor_c22ClosestNonVanishPlayerEv(void *self);
void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
void *_ZN8dActor_c7FindEggER5dCc_c(void *self, void *clsn);
int   _ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(void *self, void *clsn,
                                                     void *player);
void  _ZN8dActor_c8PoofDustEv(void *self);
void  _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void *self, const void *pos,
                                                     unsigned int count,
                                                     int value, short s);
int   _ZN8dActor_c17DetectRaycastClsnER7Vector3S1_b(void *self, Vector3 *from,
                                                    Vector3 *out, int doStore);
void  _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
          void *self, void *shadow, void *mtx, int radius, int height, u32 flags);
void  _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(char *self, void *clsn);
void  _ZN8dActor_c15GivePlayerCoinsER6Playerhj(void *self, void *player,
                                               unsigned char count,
                                               unsigned int flags);
int   _ZN8dActor_c13DistToCPlayerEv(void *self);

void  _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int id, const void *pos);
void  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x,
                                                     int y, int z);

int   _ZN6Player15IsCollectingCapEv(void *player);
void  _ZN6Player20RegisterEggCoinCountEjbb(void *player, unsigned int count,
                                           int b1, int b2);
int   _ZN6Player9IsOnShellEv(void *player);
void  _ZN6Player16IncMegaKillCountEv(void *player);
void  _ZN6Player6BounceE5Fix12IiE(void *player, int speed);
int   _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *player, void *pos,
                                              unsigned int a, int damage,
                                              unsigned int b, unsigned int c,
                                              unsigned int d);

void  _ZN7fBase_c18MarkForDestructionEv(void *self);
void *_ZN7fBase_cnwEj(u32 size);
void  _ZN8dActor_cC2Ev(void *actor);
void  _ZN9ModelAnimC1Ev(void *p);
void  _ZN11ShadowModelC1Ev(void *p);
void  _ZN7dCcAc_cC1Ev(void *p);
void  _ZN10dBgCh_ActrC1Ev(void *p);

void  _ZN13SharedFilePtr7ReleaseEv(void *p);
void  _ZN9Animation8LoadFileER13SharedFilePtr(void *p);
void *_ZN5Model8LoadFileER13SharedFilePtr(void *p);

/* Declared by final name rather than as members: both take Fix12<int> where
 * these call sites pass int literals, and dBgCh_Actr::Init's last two
 * parameters are Vector3_16* (the S5_ back-references the pointer type). */
void  _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, dActor_c *actor,
                                                int radius, int height,
                                                unsigned int flags,
                                                unsigned int vulnFlags);
void  _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
          void *self, dActor_c *actor, int radius, int height,
          Vector3_16 *a, Vector3_16 *b);

extern int       data_ov071_02122f80[];   /* SharedFilePtr, model  */
extern int       data_ov071_02122f88[];   /* SharedFilePtr, anim   */
extern Item16    data_ov071_02122fa8[];   /* state table           */
extern Matrix4x3 IDENTITY_MATRIX4X3;
extern Mtx43     data_020a0e68;           /* scratch matrix        */
extern s16       data_02082214[];         /* sin/cos table         */

/* Intra-TU forward declarations.  mwccarm lays .text down in reverse source
 * order, so this file is written ROM-descending and nearly every intra-TU call
 * is a forward reference. */
void Scuttlebug_SetState(char *self, int idx);
void func_ov071_021202b4(ScuttlebugState *c);
void func_ov071_02120278(ScuttlebugState *c);
void func_ov071_0211f0b4(char *c);
void func_ov071_0211f148(char *a, char *w);
void func_ov071_0211f29c(void *thiz);
void func_ov071_0211f498(int *t);
void func_ov071_0211f524(char *c);

}

/* ------------------------------------------------------------------------
 * ROM ordinal 36 -- daSpd_c_classInit, 0x02120618, size 0x50.
 * Natural `new` selects the wrong allocator, so the measured actor
 * construction seam is retained verbatim.
 * ------------------------------------------------------------------------ */

// @symbol daSpd_c_classInit
extern "C" int *daSpd_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(940);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)&_ZTV10Scuttlebug[2];
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
        _ZN11ShadowModelC1Ev((char *)p + 0x138);
        _ZN7dCcAc_cC1Ev((char *)p + 0x160);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x194);
    }
    return p;
}

/* ROM ordinal 35 -- OnTurnIntoEgg, 0x02120580, size 0x98.
 * Pays the player mCoinCount coins (a cap-collection coin if Yoshi is wearing
 * the cap, an egg coin otherwise).  A spawned-child Scuttlebug (+0x08 != 0)
 * resets to its egg state; the original marks itself for destruction. */

// @symbol _ZN10Scuttlebug13OnTurnIntoEggER6Player
int Scuttlebug::OnTurnIntoEgg(Player &player)
{
    volatile int force_stack;
    char *a = (char *)this;
    void *p = &player;
    int *bp;
    int t;
    if (_ZN6Player15IsCollectingCapEv(p))
        _ZN8dActor_c15GivePlayerCoinsER6Playerhj(a, p, *(unsigned char *)(a + 0x3aa), 0);
    else
        _ZN6Player20RegisterEggCoinCountEjbb(p, *(unsigned char *)(a + 0x3aa), 0, 0);
    if (*(int *)(a + 8) != 0) {
        a[0x3aa] = 0;
        bp = (int *)((int)(a) + 0xb0);
        t = *bp;
        t &= ~0x40000;
        *bp = t;
        Scuttlebug_SetState(a, 0);
    } else {
        _ZN7fBase_c18MarkForDestructionEv(a);
    }
}

/* ROM ordinal 34 -- InitResources, 0x021203f8, size 0x188. */

// @symbol _ZN10Scuttlebug13InitResourcesEv
int Scuttlebug::InitResources()
{
    char *s = (char *)((dActor_c *)this);
    void *mf = _ZN5Model8LoadFileER13SharedFilePtr(data_ov071_02122f80);
    ((ModelBase *)(s + 0xd4))->SetFile((BMD_File *)mf, 1, -1);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov071_02122f88);
    if (((ShadowModel *)(s + 0x138))->InitCylinder() == 0)
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        s + 0x160, ((dActor_c *)this), 0x46000, 0x64000, 0x200000, 0x6eff0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        s + 0x194, ((dActor_c *)this), 0x50000, 0x50000, (Vector3_16 *)0, (Vector3_16 *)0);
    ((dBgCh_Actr *)(s + 0x194))->StartDetectingWater();
    *(int *)(s + 0x384) = *(int *)(s + 0x5c);
    *(int *)(s + 0x388) = *(int *)(s + 0x60);
    *(int *)(s + 0x38c) = *(int *)(s + 0x64);
    *(short *)(s + 0x3a6) = *(short *)(s + 0x8e);
    *(int *)(s + 0x390) = *(int *)(s + 0x5c);
    *(int *)(s + 0x394) = *(int *)(s + 0x60);
    *(int *)(s + 0x398) = *(int *)(s + 0x64);
    if (*(int *)(s + 8) != 0)
        Scuttlebug_SetState(s, 0);
    else
        Scuttlebug_SetState(s, 2);
    *(char *)(s + 0x3aa) = 3;
    *(int *)(s + 0x9c) = -0x2000;
    *(int *)(s + 0xa0) = -0x3c000;
    *(int *)(s + 0x80) = 0x1000;
    *(int *)(s + 0x84) = 0x1000;
    *(int *)(s + 0x88) = 0x1000;
    *(int *)(s + 0xd0) = 0;
    *(short *)(s + 0x3a8) = 0x3c;
    *(Matrix4x3 *)(s + 0x350) = IDENTITY_MATRIX4X3;
    func_ov071_0211f524(s);
    return 1;
}

/* ROM ordinal 33 -- Behavior, 0x02120398, size 0x60. */

// @symbol _ZN10Scuttlebug8BehaviorEv
int Scuttlebug::Behavior()
{
    DecIfAbove0_Short((char *)&mTimer);
    func_ov071_02120278((ScuttlebugState *)((char *)this));
    _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(((char *)this), ((char *)this) + 0x160);
    if (_ZNK10dBgCh_Actr14GetResultFlag1Ev((char *)&mWithMeshClsn) &&
        _ZNK10dBgCh_Actr12TouchesWaterEv((char *)&mWithMeshClsn)) {
        func_ov071_0211f498((int *)((char *)this));
    }
    func_ov071_0211f524(((char *)this));
    return 1;
}

/* ROM ordinal 32 -- Render, 0x0212033c, size 0x5c. */

struct RenderSub {
    virtual int method0();
    virtual int method1();
    virtual int method2();
    virtual int method3();
    virtual int method4();
    virtual int method5(int);
};
struct RenderObj {
    char pad1[0xb0];
    int flags;
    char pad2[0x20];
    RenderSub sub;
    char pad3[0x2c3];
    int state39c;
};

// @symbol _ZN10Scuttlebug6RenderEv
int Scuttlebug::Render()
{
    int flag = (((RenderObj *)this)->flags & 0x40000) ? 1 : 0;
    if (flag) goto ret;
    if (!((RenderObj *)this)->state39c) goto ret;
    goto call;
ret:
    return 1;
call:
    ((RenderObj *)this)->sub.method5(0);
    return 1;
}

/* ROM ordinal 31 -- OnPendingDestroy, 0x02120338, size 0x4.  The ROM body is
 * empty: the override exists only to occupy vtable slot 12. */

// @symbol _ZN10Scuttlebug16OnPendingDestroyEv
void Scuttlebug::OnPendingDestroy()
{
}

/* ROM ordinal 30 -- CleanupResources, 0x02120308, size 0x30.  Releases the two
 * shared files the class holds; it never touches `this`. */

// @symbol _ZN10Scuttlebug16CleanupResourcesEv
int Scuttlebug::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov071_02122f80);
    _ZN13SharedFilePtr7ReleaseEv(data_ov071_02122f88);
    return 1;
}

/* ROM ordinal 29 -- Scuttlebug_SetState, 0x021202ec, size 0x1c.  Points the
 * state slot at one row of the table and tail-calls the entry handler. */

// @symbol Scuttlebug_SetState
extern "C" void Scuttlebug_SetState(char *self, int idx)
{
    *(Item16 **)(self + 0x380) = &data_ov071_02122fa8[idx];
    func_ov071_021202b4((ScuttlebugState *)self);
}

/* ROM ordinal 28 -- 0x021202b4, size 0x38: call the state's entry handler. */

// @symbol func_ov071_021202b4
extern "C" void func_ov071_021202b4(ScuttlebugState *c)
{
    PMF *p = c->pp;
    (c->**p)();
}

/* ROM ordinal 27 -- 0x02120278, size 0x3c: call the state's per-frame
 * handler, one slot further into the row. */

// @symbol func_ov071_02120278
extern "C" void func_ov071_02120278(ScuttlebugState *c)
{
    PMF *p = c->pp + 1;
    (c->**p)();
}

/* ROM ordinal 26 -- 0x02120200, size 0x78. */

// @symbol func_ov071_02120200
extern "C" int func_ov071_02120200(char *c)
{
    int *p = (int *)(((int)c + 0xb0));
    int z;
    short ang;

    *p = *p & ~0x10000001;
    *(int *)(c + 0x5c) = *(int *)(c + 0x384);
    z = 0;
    *(int *)(c + 0x60) = *(int *)(c + 0x388);
    *(int *)(c + 0x64) = *(int *)(c + 0x38c);
    *(short *)(c + 0x94) = *(short *)(c + 0x3a6);
    ang = *(short *)(c + 0x3a6);
    *(short *)(c + 0x8c) = z;
    *(short *)(c + 0x8e) = ang;
    *(short *)(c + 0x90) = z;
    *(int *)(c + 0x98) = z;
    *(short *)(c + 0x3a8) = 0x1e;
    _ZN5dCc_c5ClearEv(c + 0x160);
    *(int *)(c + 0x39c) = 0;
    return 1;
}

/* ROM ordinal 25 -- 0x021201b4, size 0x4c. */

// @symbol func_ov071_021201b4
extern "C" int func_ov071_021201b4(void *c)
{
    if (*(unsigned short *)((char *)c + 0x3a8)) return 1;
    if (_ZN8dActor_c13DistToCPlayerEv(c) < 0x5dc000) Scuttlebug_SetState((char *)c, 1);
    return 1;
}

/* ROM ordinal 24 -- 0x02120130, size 0x84. */

// @symbol func_ov071_02120130
extern "C" int func_ov071_02120130(char *c)
{
    *(int *)(c + 0x9c) = -0x4000;
    *(int *)(c + 0xa0) = -0x3e000;
    *(int *)(c + 0x98) = 0x16000;
    *(int *)(c + 0xa8) = 0x4d000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void *)data_ov071_02122f88[1], 0, 0x1000, 0);
    *(int *)(c + 0x130) = 0x1000;
    _ZN10dBgCh_Actr13SetLimMovFlagEv(c + 0x194);
    func_0201267c(0xf1, c + 0x74);
    *(int *)(c + 0x39c) = 1;
    return 1;
}

/* ROM ordinal 23 -- 0x02120028, size 0x108. */

// @symbol func_ov071_02120028
extern "C" int func_ov071_02120028(char *c)
{
    _ZN9Animation7AdvanceEv(c + 0x124);
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, c + 0x160);
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(c + 0x194);
    if (_ZNK10dBgCh_Actr13JustHitGroundEv(c + 0x194)) {
        Vec3 v;
        int x, y, z;
        x = *(volatile int *)(c + 0x5c);
        *(volatile int *)&v.x = x;
        y = *(volatile int *)(c + 0x60);
        *(volatile int *)&v.y = y;
        z = *(volatile int *)(c + 0x64);
        y += 0x28000;
        *(volatile int *)&v.z = z;
        *(volatile int *)&v.y = y;
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb2, x, y, z);
        *(int *)(c + 0xa8) = *(int *)(c + 0xa8) * -0x28 / 100;
    } else if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x194)) {
        *(int *)(c + 0xa8) = 0;
        _ZN10dBgCh_Actr15ClearLimMovFlagEv(c + 0x194);
        *(int *)(c + 0x390) = *(int *)(c + 0x5c);
        *(int *)(c + 0x394) = *(int *)(c + 0x60);
        *(int *)(c + 0x398) = *(int *)(c + 0x64);
        *(int *)(c + 0xb0) |= 0x10000001;
        Scuttlebug_SetState(c, 2);
    }
    func_ov071_0211f29c(c);
    _ZN5dCc_c5ClearEv(c + 0x160);
    _ZN5dCc_c6UpdateEv(c + 0x160);
    return 1;
}

/* ROM ordinal 22 -- 0x0211ff84, size 0xa4. */

// @symbol func_ov071_0211ff84
extern "C" int func_ov071_0211ff84(char *c)
{
    if (Vec3_Dist((Vector3 *)(c + 0x5c), (Vector3 *)(c + 0x390)) > 0x5dc000) {
        Scuttlebug_SetState(c, 5);
        return 1;
    }
    *(int *)(c + 0x9c) = -0x2000;
    *(int *)(c + 0xa0) = -0x3c000;
    *(int *)(c + 0x98) = 0x4000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, *(void **)((char *)data_ov071_02122f88 + 4), 0, 0x1000, 0);
    *(int *)(c + 0x130) = 0x1000;
    *(int *)(c + 0x39c) = 2;
    return 1;
}

/* ROM ordinal 21 -- 0x0211fee4, size 0xa0. */

// @symbol func_ov071_0211fee4
extern "C" int func_ov071_0211fee4(char *c)
{
    *(short *)(c + 0x8e) = (short)(*(short *)(c + 0x8e) + 0x2bc);
    *(short *)(c + 0x94) = *(short *)(c + 0x8e);
    ((Animation *)(c + 0x124))->Advance();
    unsigned short f = (unsigned short)(*(int *)(c + 0x12c) >> 12);
    if (f == 0 || f == 8 || f == 0x17 || f == 0x1f) {
        func_0201267c(0xf0, (void *)(c + 0x74));
    }
    func_ov071_0211f0b4(c);
    ((dActor_c *)c)->UpdatePos((dCc_c *)(c + 0x160));
    func_ov071_0211f148(c, (char *)(c + 0x194));
    func_ov071_0211f29c(c);
    ((dCc_c *)(c + 0x160))->Clear();
    ((dCc_c *)(c + 0x160))->Update();
    return 1;
}

/* ROM ordinal 20 -- 0x0211fe38, size 0xac. */

// @symbol func_ov071_0211fe38
extern "C" int func_ov071_0211fe38(char *c)
{
    int *p3a0 = (int *)(c + 0x3a0);
    *(int *)(c + 0x9c) = -0x2000;
    *(int *)(c + 0xa0) = -0x3c000;
    *(int *)(c + 0x98) = 0xf000;
    *(int *)(c + 0xa8) = 0x12000;
    *(short *)(c + 0x8e) = *(short *)(c + 0x3a4);
    *(short *)(c + 0x94) = *(short *)(c + 0x8e);
    *p3a0 += 0x12c000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void *)data_ov071_02122f88[1], 0, 0x1000, 0);
    *(int *)(c + 0x130) = 0x2c00;
    *(int *)(c + 0x12c) = 0;
    func_0201267c(0xf1, c + 0x74);
    *(int *)(c + 0x39c) = 3;
    return 1;
}

/* ROM ordinal 19 -- 0x0211fd58, size 0xe0. */

// @symbol func_ov071_0211fd58
extern "C" int func_ov071_0211fd58(char *c)
{
    _ZN9Animation7AdvanceEv(c + 0x124);
    int *p = (int *)(((int)c + 0x3a0));
    *p -= 0xf000;
    if (*(int *)(c + 0x3a0) <= 0) {
        char *b = c + 0x300;
        *(short *)(b + 0xa8) = 0x3c;
        Scuttlebug_SetState(c, 2);
    }
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, c + 0x160);
    func_ov071_0211f148(c, c + 0x194);
    func_ov071_0211f29c(c);
    _ZN5dCc_c5ClearEv(c + 0x160);
    _ZN5dCc_c6UpdateEv(c + 0x160);
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x194) != 0) {
        unsigned int t = (unsigned int)(*(int *)(c + 0x12c) << 4) >> 0x10;
        if ((t <= 2) || (t >= 8 && t <= 0xa) || (t >= 0x18 && t <= 0x1a) || (t >= 0x20 && t <= 0x22)) {
            func_0201267c(0xf0, c + 0x74);
        }
    }
    return 1;
}

/* ROM ordinal 18 -- 0x0211fcd4, size 0x84. */

// @symbol func_ov071_0211fcd4
extern "C" int func_ov071_0211fcd4(char *c)
{
    *(int *)(c + 0x9c) = -0x2000;
    *(int *)(c + 0xa0) = -0x3c000;
    *(int *)(c + 0x98) = -0x4000;
    *(int *)(c + 0xa8) = 0x12000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void *)data_ov071_02122f88[1], 0, 0x1000, 0);
    *(int *)(c + 0x130) = 0x2c00;
    func_0201267c(0xf1, c + 0x74);
    *(int *)(c + 0x39c) = 4;
    return 1;
}

/* ROM ordinal 17 -- 0x0211fc60, size 0x74. */

// @symbol func_ov071_0211fc60
extern "C" int func_ov071_0211fc60(char *c)
{
    _ZN9Animation7AdvanceEv((char *)c + 0x124);
    if (_ZNK10dBgCh_Actr10IsOnGroundEv((char *)c + 0x194)) {
        *(short *)(c + 0x3a8) = 0x3c;
        Scuttlebug_SetState(c, 2);
    }
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, (char *)c + 0x160);
    func_ov071_0211f148(c, (char *)c + 0x194);
    func_ov071_0211f29c(c);
    _ZN5dCc_c5ClearEv((char *)c + 0x160);
    _ZN5dCc_c6UpdateEv((char *)c + 0x160);
    return 1;
}

/* ROM ordinal 16 -- 0x0211fbf4, size 0x6c. */

// @symbol func_ov071_0211fbf4
extern "C" int func_ov071_0211fbf4(char *c)
{
    *(int *)((char *)c + 0x9c) = -0x2000;
    *(int *)((char *)c + 0xa0) = -0x3c000;
    *(int *)((char *)c + 0x98) = 0x4000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)c + 0xd4, (void *)data_ov071_02122f88[1], 0, 0x1000, 0);
    *(int *)((char *)c + 0x130) = 0x1000;
    *(int *)((char *)c + 0x39c) = 5;
    return 1;
}

/* ROM ordinal 15 -- 0x0211fb24, size 0xd0. */

// @symbol func_ov071_0211fb24
extern "C" int func_ov071_0211fb24(char *c)
{
    short ang = Vec3_HorzAngle((Vector3 *)(c + 0x5c), (Vector3 *)(c + 0x390));
    _Z14ApproachLinearRsss(*(short *)(c + 0x8e), ang, 0x2bc);
    *(short *)(c + 0x94) = *(short *)(c + 0x8e);
    _ZN9Animation7AdvanceEv(c + 0x124);
    if (Vec3_Dist((Vector3 *)(c + 0x5c), (Vector3 *)(c + 0x390)) < 0x12c000)
        Scuttlebug_SetState(c, 2);
    func_ov071_0211f0b4(c);
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, c + 0x160);
    func_ov071_0211f148(c, c + 0x194);
    func_ov071_0211f29c(c);
    _ZN5dCc_c5ClearEv(c + 0x160);
    _ZN5dCc_c6UpdateEv(c + 0x160);
    unsigned short v = (unsigned short)(*(int *)(c + 0x12c) >> 0xc);
    if (v == 0 || v == 8 || v == 0x17 || v == 0x1f)
        func_0201267c(0xf0, c + 0x74);
    return 1;
}

/* ROM ordinal 14 -- 0x0211fb0c, size 0x18. */

// @symbol func_ov071_0211fb0c
extern "C" int func_ov071_0211fb0c(char *p)
{
    *(int *)(p + 0x98) = 0;
    *(int *)(p + 0x39c) = 6;
    return 1;
}

/* ROM ordinal 13 -- 0x0211fa54, size 0xb8. */

// @symbol func_ov071_0211fa54
extern "C" int func_ov071_0211fa54(void *thiz)
{
    char *c = (char *)thiz;
    int b0 = (int)((*(int *)(c + 0xb0) & 0x40000) != 0);
    if (b0 != 0) {
        int *src = (int *)(((int)(*(char **)(c + 0xd0)) + 0x5c));
        *(int *)(c + 0x5c) = src[0];
        *(int *)(c + 0x60) = src[1];
        *(int *)(c + 0x64) = src[2];
    }
    {
        int v = *(int *)(c + 0xb0);
        int b1 = (int)((v & 0x80000) != 0);
        if (b1 != 0) {
            Scuttlebug_SetState(c, 7);
            goto done;
        }
        {
            int b2 = (int)((v & 0x20000) != 0);
            if (b2 != 0) goto done;
        }
        {
            int b3 = (int)((v & 0x40000) != 0);
            if (b3 != 0) goto done;
        }
        *(int *)(c + 0xd0) = 0;
        Scuttlebug_SetState(c, 2);
    }
done:
    _ZN5dCc_c5ClearEv(c + 0x160);
    return 1;
}

/* ROM ordinal 12 -- 0x0211f8d0, size 0x184. */

// @symbol func_ov071_0211f8d0
extern "C" int func_ov071_0211f8d0(char *self)
{
    Vector3 *pos;
    Vector3 v;
    int zero;
    char *parent;
    int *pb0;
    int *py;
    int *pz;
    int s0, s1;
    u16 hang;
    int saved_x;
    int mul;
    int rnd;
    int one;
    Vector3 *srcv;
    int adj;
    int y, z, x, y2;

    zero = 0;
    pb0 = (int *)(((int)self + 0xb0));
    *pb0 = (*pb0) & 0xfff7fffe;

    parent = *(char **)(self + 0xd0);
    pos = (Vector3 *)(((int)self + 0x5c));
    mul = 0x5a000;
    *(int *)(self + 0x98) = *(int *)(parent + 0x98) + 0x7000;
    *(int *)(self + 0xa8) = zero;

    parent = *(char **)(self + 0xd0);
    rnd = 0x800;
    *(s16 *)(self + 0x8e) = *(s16 *)(parent + 0x8e);
    py = (int *)(((int)self + 0x60));
    pz = (int *)(((int)self + 0x64));
    *(s16 *)(self + 0x94) = *(s16 *)(self + 0x8e);

    parent = *(char **)(self + 0xd0);
    one = 1;
    srcv = (Vector3 *)(((int)parent + 0x5c));
    *(int *)(self + 0x5c) = srcv->x;
    *(int *)(self + 0x60) = srcv->y;
    *(int *)(self + 0x64) = srcv->z;

    saved_x = pos->x;
    hang = *(u16 *)(self + 0x8e);
    s0 = data_02082214[(hang >> 4) * 2];
    adj = (int)(((s64)s0 * mul + rnd) >> 12);
    pos->x = saved_x + adj;

    *py = *py + 0x50000;

    hang = *(u16 *)(self + 0x8e);
    s1 = data_02082214[(hang >> 4) * 2 + 1];
    adj = (int)(((s64)s1 * mul + rnd) >> 12);
    *pz = *pz + adj;

    parent = *(char **)(self + 0xd0);
    y = *(int *)(parent + 0x60);
    z = *(int *)(parent + 0x64);
    y2 = y + 0x50000;
    x = *(int *)(parent + 0x5c);
    v.x = x;
    v.y = y2;
    v.z = z;

    _ZN8dActor_c17DetectRaycastClsnER7Vector3S1_b(self, &v, pos, one);

    *(int *)(self + 0xd0) = zero;
    _ZN10dBgCh_Actr13SetLimMovFlagEv(self + 0x194);

    *(int *)(self + 0x39c) = 7;
    return 1;
}

/* ROM ordinal 11 -- 0x0211f7d4, size 0xfc. */

// @symbol func_ov071_0211f7d4
extern "C" int func_ov071_0211f7d4(dActor_c *self)
{
    char *s = (char *)self;
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(s + 0x194);
    *(short *)(s + 0x8c) = *(short *)(s + 0x8c) + 0x1000;
    if (((dBgCh_Actr *)(s + 0x194))->JustHitGround()) {
        if (func_02037e38((unsigned int *)((char *)_ZNK10dBgCh_Actr14GetFloorResultEv((dBgCh_Actr *)(s + 0x194)) + 4)) == 4) {
            func_ov071_0211f498((int *)s);
        } else {
            *(int *)(s + 0xa8) = (*(int *)(s + 0xa8) * -0x3c) / 0x64;
        }
    } else if (((dBgCh_Actr *)(s + 0x194))->IsOnGround()) {
        dBgCh_Actr *wm = (dBgCh_Actr *)(s + 0x194);
        *(int *)(s + 0xa8) = 0;
        wm->ClearLimMovFlag();
        *(int *)(s + 0xb0) |= 1;
        short z = 0;
        short ang = *(short *)(s + 0x94);
        *(short *)(s + 0x8c) = z;
        *(short *)(s + 0x8e) = ang;
        *(short *)(s + 0x90) = z;
        Scuttlebug_SetState(s, 2);
    }
    self->UpdatePos((dCc_c *)(s + 0x160));
    func_ov071_0211f29c(s);
    _ZN5dCc_c5ClearEv((dCc_c *)(s + 0x160));
    _ZN5dCc_c6UpdateEv((dCc_c *)(s + 0x160));
    return 1;
}

/* ROM ordinal 10 -- 0x0211f6f8, size 0xdc. */

// @symbol func_ov071_0211f6f8
extern "C" int func_ov071_0211f6f8(char *c)
{
    _ZN5Sound9PlayBank0EjRK7Vector3(9, (const void *)(c + 0x74));
    *(int *)(((int)c + 0xb0)) &= ~1;
    *(int *)(c + 0x98) = 0xa000;
    *(int *)(c + 0xa8) = 0x28000;
    *(short *)(c + 0x3a8) = 0x2d;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, *(void **)((char *)data_ov071_02122f88 + 4), 0, 0x1000, 0);
    *(int *)(c + 0x130) = 0x4000;
    VSlot29 *b = (VSlot29 *)c;
    int r1 = b->m29();
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x43, *(int *)(c + 0x5c), *(int *)(c + 0x60) + r1, *(int *)(c + 0x64));
    int r2 = b->m29();
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x44, *(int *)(c + 0x5c), *(int *)(c + 0x60) + r2, *(int *)(c + 0x64));
    *(int *)(c + 0x39c) = 8;
    return 1;
}

/* ROM ordinal 9 -- 0x0211f694, size 0x64. */

// @symbol func_ov071_0211f694
extern "C" int func_ov071_0211f694(char *t)
{
    *(short *)(t + 0x8c) = *(short *)(t + 0x8c) - 0x1000;
    _ZN9Animation7AdvanceEv(t + 0x124);
    _ZN8dActor_c9UpdatePosEP5dCc_c(t, t + 0x160);
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(t + 0x194);
    if (_ZNK10dBgCh_Actr13JustHitGroundEv(t + 0x194) != 0 || *(unsigned short *)(t + 0x3a8) == 0)
        func_ov071_0211f498((int *)t);
    return 1;
}

/* ROM ordinal 8 -- 0x0211f524, size 0x170. */

// @symbol func_ov071_0211f524
extern "C" void func_ov071_0211f524(char *c)
{
    int b = (int)((*(u32 *)(c + 0xb0) & 0x40000) != 0);
    if (b) {
        if (*(int *)(c + 0x39c) == 0)
            return;
    }

    Matrix4x3_FromRotationY(c + 0xf0, *(s16 *)(c + 0x8e));
    *(int *)(c + 0x114) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x118) = *(int *)(c + 0x60) >> 3;
    *(int *)(c + 0x11c) = *(int *)(c + 0x64) >> 3;

    if (*(s16 *)(c + 0x8c) != 0) {
        data_020a0e68 = *(Mtx43 *)(c + 0xf0);
        int y1 = ((VSlot29 *)c)->m29() >> 3;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y1, 0);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(s16 *)(c + 0x8c));
        int y2 = (-((VSlot29 *)c)->m29()) >> 3;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y2, 0);
        *(Mtx43 *)(c + 0xf0) = data_020a0e68;
    }

    *(int *)(c + 0x374) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x378) = *(int *)(c + 0x60) >> 3;
    *(int *)(c + 0x37c) = *(int *)(c + 0x64) >> 3;

    int dh = (*(int *)(c + 0x39c) == 8) ? 0x190000 : 0xc8000;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x138, c + 0x350, 0xa0000, dh, 0xf);
}

/* ROM ordinal 7 -- 0x0211f498, size 0x8c. */

// @symbol func_ov071_0211f498
extern "C" void func_ov071_0211f498(int *t)
{
    Vec3 v;
    v.x = t[0x17];
    v.y = t[0x18];
    v.z = t[0x19];
    _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(t, &v, *(unsigned char *)((char *)t + 0x3aa), 0xf000, 0);
    _ZN8dActor_c8PoofDustEv(t);
    func_02012694(0xc4, (char *)t + 0x74);
    if (t[2]) {
        *(unsigned char *)((char *)t + 0x3aa) = 0;
        Scuttlebug_SetState((char *)t, 0);
        return;
    }
    _ZN7fBase_c18MarkForDestructionEv(t);
}

/* ROM ordinal 6 -- 0x0211f29c, size 0x1fc. */

// @symbol func_ov071_0211f29c
extern "C" void func_ov071_0211f29c(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    unsigned char *r4;
    int b;

    if (_ZN8dActor_c7FindEggER5dCc_c(c, c + 0x160) != 0) {
        _ZN5Sound9PlayBank0EjRK7Vector3(9, c + 0x74);
        func_ov071_0211f498((int *)c);
        return;
    }

    {
        unsigned int id = *(unsigned int *)(c + 0x184);
        if (id == 0)
            return;
        r4 = (unsigned char *)_ZN8dActor_c10FindWithIDEj(id);
    }
    if (r4 == 0)
        return;

    b = (int)(*(unsigned short *)(r4 + 0xc) == 0xbf);
    if (b == 0)
        return;

    b = (int)((*(int *)(c + 0xb0) & 0x20000) != 0);
    if (b != 0) {
        Scuttlebug_SetState((char *)c, 6);
        return;
    }

    if ((*(int *)(c + 0x180) & 0x66fe0)
        || _ZN6Player9IsOnShellEv(r4) != 0
        || *(unsigned char *)(r4 + 0x6f9) != 0) {
        _ZN5Sound9PlayBank0EjRK7Vector3(9, c + 0x74);
        func_ov071_0211f498((int *)c);
        return;
    }

    if (*(int *)(c + 0x180) & 0x10) {
        *(short *)(c + 0x94) = Vec3_HorzAngle((Vector3 *)(r4 + 0x5c), (Vector3 *)(c + 0x5c));
        *(short *)(c + 0x8e) = (short)(*(short *)(c + 0x94) + 0x8000);
        _ZN6Player16IncMegaKillCountEv(r4);
        Scuttlebug_SetState((char *)c, 8);
        return;
    }

    if (_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(c, c + 0x160, r4) != 0) {
        _ZN6Player6BounceE5Fix12IiE(r4, 0x28000);
        func_ov071_0211f498((int *)c);
        return;
    }

    if (*(int *)(c + 0x39c) == 7)
        return;

    {
        int v[3];
        v[0] = *(int *)(c + 0x5c);
        v[1] = *(int *)(c + 0x60);
        v[2] = *(int *)(c + 0x64);
        if (_ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r4, v, 1, 0xc000, 1, 0, 1) != 0)
            Scuttlebug_SetState((char *)c, 4);
    }
}

/* ROM ordinal 5 -- 0x0211f148, size 0x154. */

// @symbol func_ov071_0211f148
extern "C" void func_ov071_0211f148(char *a, char *w)
{
    Vector3 pos;
    Vector3 normal;
    Vector3 wallnormal;

    dBgCh_Actr_UpdateDiscreteNoLava_veneer(w);
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(w)) {
        dBgCh_Gnd rc;
        {
            int p60 = *(int *)(a + 0x60);
            int pz = *(int *)(a + 0x64);
            int py = p60 + 0x1e000;
            pos.x = *(int *)(a + 0x5c);
            pos.y = py;
            pos.z = pz;
        }
        rc.SetObjAndPos(pos, (dActor_c *)a);
        if (!rc.DetectClsn() || rc.clsnY < *(int *)(a + 0x60) - 0x32000) {
            *(int *)(a + 0x98) = 0;
            *(int *)(a + 0x5c) = *(int *)(a + 0x68);
            *(int *)(a + 0x60) = *(int *)(a + 0x6c);
            *(int *)(a + 0x64) = *(int *)(a + 0x70);
        } else {
            void *fr = _ZNK10dBgCh_Actr14GetFloorResultEv(w);
            _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char *)fr + 4, &normal);
            if (normal.y != 0) {
                *(int *)(a + 0xa8) = -(_ZN4cstd4fdivEii(
                    (int)(((long long)normal.x * *(int *)(a + 0xa4) + 0x800) >> 12)
                  + (int)(((long long)normal.z * *(int *)(a + 0xac) + 0x800) >> 12),
                    normal.y) + 0x8000);
            }
        }
    }
    if (_ZNK10dBgCh_Actr8IsOnWallEv(w)) {
        void *wr = _ZNK10dBgCh_Actr13GetWallResultEv(w);
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char *)wr + 4, &wallnormal);
    }
}

/* ROM ordinal 4 -- 0x0211f0b4, size 0x94. */

// @symbol func_ov071_0211f0b4
extern "C" void func_ov071_0211f0b4(char *c)
{
    dActor_c *p;
    Fix12i d;
    short ang;
    if (*(unsigned short *)(c + 0x3a8) != 0) return;
    p = (dActor_c *)_ZN8dActor_c22ClosestNonVanishPlayerEv(c);
    if (p == 0) return;
    d = Vec3_Dist((const Vector3 *)(c + 0x5c), (const Vector3 *)((char *)p + 0x5c));
    if (d > 0x5dc000) return;
    ang = Vec3_HorzAngle((const Vector3 *)(c + 0x5c), (const Vector3 *)((char *)p + 0x5c));
    if (AngleDiff(ang, *(short *)(c + 0x8e)) > 0x12c) return;
    *(Fix12i *)(c + 0x3a0) = d;
    *(short *)(c + 0x3a4) = ang;
    Scuttlebug_SetState(c, 3);
}

/* ROM ordinal 3 -- OnAimedAtWithEgg, 0x0211f0ac, size 0x8.  Vtable slot 29;
 * the ROM body ignores `this` and returns a constant. */

// @symbol _ZN10Scuttlebug16OnAimedAtWithEggEv
int Scuttlebug::OnAimedAtWithEgg()
{
    return 204800;
}

/* ROM ordinal 2 -- OnYoshiTryEat, 0x0211f0a4, size 0x8.  Vtable slot 18, the
 * first non-inline virtual this class declares, and therefore the KEY
 * FUNCTION: it anchors _ZTV10Scuttlebug and the class RTTI in this TU. */

// @symbol _ZN10Scuttlebug13OnYoshiTryEatEv
int Scuttlebug::OnYoshiTryEat()
{
    return 6;
}

/* ROM ordinals 0 and 1 -- D1 at 0x0211f000 and D0 at 0x0211f048.  The inline
 * destructor in include/Scuttlebug.h emits both, in that order, below every
 * section above, with no D2 and no forcing scaffold. */
