//cpp
/* daGmch_c -- the GAMAGUCHI actor (a hopping clasp-purse that pays out coins),
 * ov081.
 *
 * Reconstructed translation unit: the contiguous linker run
 * 0x02126504..0x02127b34, 37 functions, assembled from 36 one-function legacy
 * sources plus the GAMAGUCHI factory, which the delinked tree carried in its
 * own file (that file is superseded by this one; the manifest's
 * `legacy_source` column records where each of the 37 came from).
 *
 * THE CLASS NAME IS THE CARTRIDGE'S OWN RTTI, not a coined one.
 * extracted/overlays/overlay_0081.bin at file offset 0x5488 -> address
 * 0x02128bc8 holds the ten bytes 38 64 61 47 6d 63 68 5f 63 00, i.e. the
 * Itanium length-prefixed form "8daGmch_c", and 0x02128bd4 is the
 * __si_class_type_info record that points at it.  The tree previously spelt
 * this class `Moneybag`; that name appears nowhere in the cartridge and this
 * TU's promotion retires it.
 *
 * ---------------------------------------------------------------------------
 * BOUNDARIES
 * ---------------------------------------------------------------------------
 * LEFT, hard: the function immediately below is daSnowball_c_classInit at
 * 0x021264b4 size 0x50, ending exactly at 0x02126504.  The run therefore starts
 * on a function boundary and takes nothing from daSnowball_c.
 *
 * RIGHT, hard: the last function here is daGmch_c_classInit at 0x02127adc
 * (0x58 bytes), ending exactly at 0x02127b34, where _ZN8IceBlockD1Ev begins.
 * Nothing of daObjIceBlock_c is taken.
 *
 * SELF-CONTAINED: of the 91,808 relocation rows in the 106 module relocs.txt
 * files, 436 land in this address range but only 64 carry module:overlay(81) --
 * ov077..ov082 all load at base 0x02123740, so the other 372 are phantoms from
 * the overlays sharing the window.  Those 64 are 35 from .text inside the run,
 * 18 pointer-to-member records, the 10 own vtable slots and the profile's
 * factory word.  Zero come from .text outside the run, so no sibling class in
 * ov081 calls into it.
 *
 * ---------------------------------------------------------------------------
 * `#pragma defer_codegen off` AND THE SOURCE ORDER ARE ONE DECISION
 * ---------------------------------------------------------------------------
 * The cartridge puts D1 (0x02126504) BELOW D0 (0x02126554) and carries no
 * daGmch_c D2 at all.  Out-of-line under `#pragma defer_codegen off` mwccarm
 * 2004/b56 emits D1, D0, D2 in that order, so the two ROM symbols land in the
 * ROM's own order and the D2 is a homeless extra the manifest licenses as a
 * plain deadstrip.  The same pragma makes mwccarm lay .text down in SOURCE
 * order rather than reversed, which is why the members below run lowest ROM
 * address first.  Flipping either one alone makes `linkcheck [4b/8]`'s
 * ROM-ascending emission-order audit refuse the result.  Do not reorder them.
 *
 * ---------------------------------------------------------------------------
 * ALL 36 MEMBERS ARE CLASS METHODS
 * ---------------------------------------------------------------------------
 * The cartridge names only ten of them.  The other 26 carry auto-generated
 * `func_ov081_<address>` names in the delinked tree, and this TU replaces those
 * with `daGmch_c::` methods; the header says what each name is and is not
 * allowed to claim.  Every one of the 26 still byte-matches, so nothing here is
 * a near-miss and nothing stayed a free function for want of a match.
 *
 * The one function that is deliberately NOT a member is daGmch_c_classInit, the
 * profile factory at the end: the cartridge's own symbol for it is a free
 * symbol, not a member of this class, and it constructs the object rather than
 * running on one.
 *
 * A class member function may NOT sit in a linkage-specification region, so
 * once every member became a method, every external function declaration had to
 * move to the one file-scope `extern "C"` region below.  Reconciling the shards'
 * disagreeing spellings there was the whole cost of the conversion; that
 * region's own comment records what was measured.
 *
 * Shadow TYPE tags are uniquified with the member's ROM address as a suffix.
 * That is safe only for types used for VIRTUAL dispatch, where the call is by
 * vtable slot; a shadow class whose non-virtual member name is what produces
 * the mangled callee (`dCc_c::Clear`) cannot be renamed, so those calls are
 * spelt as mangled `extern "C"` symbols instead -- the same spelling four other
 * members of this very TU already use for the same callees.
 *
 * decl_common.h is deliberately NOT included.  Before this promotion it declared
 * 7 of this TU's own members under their auto-generated names, and 3 of the 7
 * contradicted the byte-matched definitions -- 0x02126950 as `void*` against
 * `int*`, 0x02127708 as `int(char*)` against `void(C*)`, 0x0212777c as `void*`
 * against `char*` -- which would have been three `illegal function overloading`
 * errors pointed at the definitions rather than at the header.  Those 7 rows
 * named nothing else in the tree, so this promotion removes them; the two
 * `extern int _ZTV8daGmch_c[];` rows it also carries are left alone.
 */

#pragma defer_codegen off

#include "types.h"
/* BEFORE daGmch_c.h, and load-bearing: common.h and math/Matrix.h both define
   Matrix4x3 under one guard and whichever is seen FIRST stands.  Ordinal 34
   assigns IDENTITY_MATRIX4X3 whole into mMatrix; under common.h flat s32[12]
   spelling mwccarm block-moves it, under math/Matrix.h Matrix3x3+Vector3
   spelling it splits into a 9-word ldm/stm plus a CSE tail -- measured here as
   0x1d4 against 0x1f0, +7 words.  The shards reached the flat spelling through
   decl_common.h -> common.h; this include is what replaces that route. */
#include "common.h"
#include "daGmch_c.h"
#include "dBgCh_Gnd.h"
#include "Player.h"

/* ---------------------------------------------------------------------------
 * Shadow types, one set per member that recovered one.  The tag suffix is the
 * member's ROM address.
 * ------------------------------------------------------------------------- */

/* UpdateDrawMatrices (ordinal 8) -- the flat 12-word matrix spelling, kept
   because the whole-object assignments to data_020a0e68 block-move under this
   spelling and scalarize under math/Matrix.h's structured one. */
typedef struct Mtx43_26a20 { int w[12]; } Mtx43_26a20;

/* UpdateDrawMatrices (ordinal 8) -- dActor_c seen only as a vtable, to reach
   slot 29 (offset 0x74) virtually.  Renaming the tag is safe: the call is by
   slot, not by mangled member name. */
struct VObj_26a20 {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual int  m29();
};

/* EnterState8 (ordinal 10) -- the same slot-29 window. */
struct VObj_26c8c {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual int  m29();
};

/* EnterState7 (ordinal 12) -- a flat three-int vector, NOT types.h's
   Vector3: that one has a declared destructor and is not an aggregate. */
typedef struct Vec3_26e28 { int x, y, z; } Vec3_26e28;

/* UpdateState5 (ordinal 15) -- the whole object seen as named fields. */
struct Obj_27070 {
    char pad0[0x5c];
    Vector3 pos;              /* 0x5c */
    char pad1[0x26];
    s16 f8e;                  /* 0x8e */
    char pad2[4];
    s16 f94;                  /* 0x94 */
    char pad3[2];
    Fix12i f98;               /* 0x98 */
    char pad4[0x124 - 0x9c];
    char anim[0x1b0 - 0x124]; /* 0x124 */
    char clsn[0x1e4 - 0x1b0]; /* 0x1b0 */
    char field1e4[0x3d0 - 0x1e4];
    Vector3 target;           /* 0x3d0 */
};

/* CallStateUpdate and CallStateEnter (ordinals 27, 28), the two
   pointer-to-member invokers.  Two separate tags
   even though the windows are identical: mwccarm's pointer-to-member
   representation depends on whether the class was complete when the PMF type
   was formed, and merging two members' views of one object changes it. */
struct C_27708;
typedef void (C_27708::*PMF_27708)();
struct C_27708 { char pad[0x3dc]; PMF_27708 *pp; };

struct C_27744;
typedef void (C_27744::*PMF_27744)();
struct C_27744 { char pad[0x3dc]; PMF_27744 *pp; };

/* Render (ordinal 32) -- ModelBase seen only as a vtable, to reach
   the draw entry at slot 5 virtually. */
struct Sub_277e0 {
    virtual int v0(); virtual int v1(); virtual int v2();
    virtual int v3(); virtual int v4(); virtual int m(void *);
};

/* EnterState4 and EnterState2 (ordinals 18, 22) -- the two-word BCA file-pointer records those members
   recovered as a struct rather than as an array.  Hoisted to file scope only
   because a block-scope tag cannot type an `extern` object once the member is a
   class method; the field expressions are untouched. */
struct Bca2 { int w[2]; };

/* ---------------------------------------------------------------------------
 * EVERY external function this TU calls, declared once, with C linkage.
 * ---------------------------------------------------------------------------
 * A class member function may not sit in a linkage-specification region, so
 * once a member becomes `daGmch_c::Something` a declaration written in its body
 * gets C++ linkage and the reference mangles.  All 36 members below are class
 * methods, so all 54 external function declarations have to live here.
 *
 * Of the 54, exactly NINE were declared with more than one type spelling across
 * the 36 shards: Vec3_Dist, dActor_c::DistToCPlayer, func_02038414,
 * ModelAnim::SetAnim, Vec3_HorzAngle, DecIfAbove0_Byte, Sound::PlayBank0,
 * ApproachLinear and RandomIntInternal.  Collapsing each onto ONE spelling was
 * measured, not assumed, and eight of the nine cost nothing:
 *
 *   TWO were never different types at all.  Fix12i is `typedef s32`, so
 *   Vec3_Dist and dActor_c::DistToCPlayer spelt `Fix12i` and spelt `int` are
 *   the same declaration written twice.
 *
 *   FOUR disagreed only about something no call site can see: a return value
 *   that every caller discards or compares against zero (func_02038414,
 *   ModelAnim::SetAnim, DecIfAbove0_Byte) or which pointer type spells the same
 *   address (Vec3_HorzAngle, ModelAnim::SetAnim).
 *
 *   TWO needed a call site adapted rather than a declaration chosen.
 *   Sound::PlayBank0 keeps ordinal 6's `const void *`, so EnterState8 passes
 *   `(const void *)(c + 0x74)` instead of an lvalue; ApproachLinear keeps
 *   ordinal 23's `short *`, so UpdateState5 passes `&self->f8e`.  Same address,
 *   same register, and both members still match.
 *
 * The NINTH is genuinely load-bearing: RandomIntInternal must return
 * `unsigned int`.  Measured -- declaring it `int` leaves 36/37 matching and
 * breaks ChooseNextState alone, which is the only caller that takes `% 10` of
 * the result and so the only one that pays for a signed remainder.
 *
 * Data is different -- mwccarm leaves a file-scope variable's name unmangled in
 * C++ -- so every `data_*` declaration stays at block scope in the member that
 * recovered it, and the members that disagree about a data object's TYPE
 * (data_ov081_02128ec4 is a two-word struct to ordinal 18 and a pointer array
 * to ordinal 26) keep both views.
 * ------------------------------------------------------------------------- */
extern "C" {
extern int    _ZN8dActor_c13DistToCPlayerEv(void *self);
extern Fix12i Vec3_Dist(const void *a, const void *b);
extern s16    Vec3_HorzAngle(const void *a, const void *b);
extern unsigned int RandomIntInternal(int *seed);
extern void   _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void *self, Vector3 const &pos, unsigned int n, int fix, short s);
extern void   _ZN8dActor_c8PoofDustEv(void *self);
extern void   _ZN8dActor_c24KillAndTrackInDeathTableEv(void *self);
extern void  *_ZN8dActor_c7FindEggER5dCc_c(void *self, void *clsn);
extern void   _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int id, const void *pos);
extern void  *_ZN8dActor_c10FindWithIDEj(unsigned int id);
extern int    _ZN6Player9IsOnShellEv(void *p);
extern void   _ZN6Player16IncMegaKillCountEv(void *p);
extern int    _ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(void *self, void *clsn, void *player);
extern void   _ZN6Player6BounceE5Fix12IiE(void *p, int fix);
extern void   _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *p, void *pos, unsigned int a, int fix, unsigned int b, unsigned int cc, unsigned int d);
extern int    func_02038414(void *clsn);
extern int    _ZNK10dBgCh_Actr10IsOnGroundEv(void *clsn);
extern void  *_ZNK10dBgCh_Actr14GetFloorResultEv(void *clsn);
extern void   _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *s, int *out);
extern int    _ZN4cstd4fdivEii(int a, int b);
extern int    _ZNK10dBgCh_Actr8IsOnWallEv(void *clsn);
extern void  *_ZNK10dBgCh_Actr13GetWallResultEv(void *clsn);
extern void   Matrix4x3_FromRotationY(void *m, int angle);
extern void   _ZN9ModelBase12ApplyOpacityEj(void *self, u32 op, int z);
extern void   Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
extern void   Matrix4x3_ApplyInPlaceToRotationX(void *m, s16 angX);
extern void   Matrix4x3_ApplyInPlaceToRotationY(void *m, s16 angY);
extern void   _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *self, void *shadow, void *mtx, int rad, int height, u32 flags);
extern void   _ZN9Animation7AdvanceEv(void *a);
extern int    _ZN9Animation8FinishedEv(void *a);
extern void   _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *clsn);
extern int    _ZNK10dBgCh_Actr13JustHitGroundEv(void *clsn);
extern int    DecIfAbove0_Byte(void *p);
extern void   _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca, int a, int fix, unsigned int j);
extern void   _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int n, int a, int b, int c);
extern void   _ZN10dBgCh_Actr15ClearLimMovFlagEv(void *clsn);
extern void   _ZN10dBgCh_Actr13SetLimMovFlagEv(void *clsn);
extern void   _ZN5dCc_c5ClearEv(void *clsn);
extern void   _ZN5dCc_c6UpdateEv(void *clsn);
extern int    _ZN8dActor_c17DetectRaycastClsnER7Vector3S1_b(void *self, Vec3_26e28 *a, Vec3_26e28 *b, int cc);
extern void   _Z14ApproachLinearRsss(short *p, short target, short step);
extern int    _Z15ApproachLinear2Riii(int *p, int target, int step);
extern void   func_0201267c(int id, void *pos);
extern void   _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *actor, int a, int b, unsigned int c, unsigned int d);
extern void   _ZN13SharedFilePtr7ReleaseEv(void *self);
extern void   _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(char *self, char *clsn);
extern int    _ZNK10dBgCh_Actr14GetResultFlag1Ev(char *clsn);
extern int    _ZNK10dBgCh_Actr12TouchesWaterEv(char *clsn);
extern void  *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern int    _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *file, int a, int b);
extern void  *_ZN9Animation8LoadFileER13SharedFilePtr(void *fp);
extern int    _ZN11ShadowModel12InitCylinderEv(void *self);
extern void   _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, int a, int b, void *v, int c);
extern void   _ZN10dBgCh_Actr19StartDetectingWaterEv(void *self);
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 0 -- _ZN8daGmch_cD1Ev, 0x02126504, size 0x50           */
/* ROM ordinal 1 -- _ZN8daGmch_cD0Ev, 0x02126554, size 0x64           */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_cD1Ev
// @symbol _ZN8daGmch_cD0Ev
/* One definition, two emitted variants, and this is the key function: it is the
   first virtual daGmch_c.h declares out of line, so this TU is where the class's
   _ZTV/_ZTI/_ZTS are emitted.  The compiler writes both bodies -- the vptr
   store, the five members destroyed in reverse construction order, then
   ~dActor_c -- and D0 additionally reaches Memory::Deallocate through
   fBase_c's operator delete.  Nothing belongs in the braces. */
daGmch_c::~daGmch_c()
{
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 2 -- _ZN8daGmch_c13OnYoshiTryEatEv, 0x021265b8, size 0x8 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c13OnYoshiTryEatEv
/* Vtable slot 18.  The ROM body ignores `this` and returns a constant. */
int daGmch_c::OnYoshiTryEat()
{
    return 6;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 3 -- _ZN8daGmch_c16OnAimedAtWithEggEv, 0x021265c0, size 0x8 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c16OnAimedAtWithEggEv
/* Vtable slot 29.  The ROM body ignores `this` and returns a constant. */
int daGmch_c::OnAimedAtWithEgg()
{
    return 235520;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 4 -- _ZN8daGmch_c15ChooseNextStateEv, 0x021265c8, size 0x138 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c15ChooseNextStateEv
void daGmch_c::ChooseNextState()
{
    void *c = this;
    extern int data_0209e650;

    Fix12i distC;
    Fix12i dist;
    int v;

    distC = _ZN8dActor_c13DistToCPlayerEv(c);
    dist = Vec3_Dist((char *)c + 0x5c, (char *)c + 0x3d0);
    if (dist > 0x5dc000) {
        *(int *)((char *)c + 0x3e4) = 5;
    } else if (distC > 0x3e8000) {
        *(int *)((char *)c + 0x3e4) = 0;
    } else if (distC < 0x1f4000) {
        *(int *)((char *)c + 0x3e4) = 3;
    } else {
        if (RandomIntInternal(&data_0209e650) % 10 < 7)
            *(int *)((char *)c + 0x3e4) = 3;
        else
            *(int *)((char *)c + 0x3e4) = 1;

        if (*(int *)((char *)c + 0x3e0) == 1 && *(int *)((char *)c + 0x3e4) == 1) {
            ChooseNextState();
            return;
        }
    }

    v = *(int *)((char *)c + 0x3e0);
    if (v == 1 && *(int *)((char *)c + 0x3e4) != 1) {
        SetState(2);
        return;
    }
    if (v == 3 && *(int *)((char *)c + 0x3e4) != 3) {
        SetState(4);
        return;
    }
    SetState(*(int *)((char *)c + 0x3e4));
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 5 -- _ZN8daGmch_c16SpawnCoinsAndDieEv, 0x02126700, size 0x58 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c16SpawnCoinsAndDieEv
void daGmch_c::SpawnCoinsAndDie()
{
    char *c = (char *)this;

    Vector3 t;
    t.x = *(int *)(c + 0x5c);
    t.y = *(int *)(c + 0x60);
    t.z = *(int *)(c + 0x64);
    _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(c, t, 5, 0xf000, 0);
    _ZN8dActor_c8PoofDustEv(c);
    _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 6 -- _ZN8daGmch_c18CheckPlayerContactEv, 0x02126758, size 0x1f8 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c18CheckPlayerContactEv
void daGmch_c::CheckPlayerContact()
{
    void *thiz = this;

    unsigned char *c = (unsigned char *)thiz;
    unsigned char *r4;
    int b;

    if (_ZN8dActor_c7FindEggER5dCc_c(c, c + 0x1b0) != 0) {
        _ZN5Sound9PlayBank0EjRK7Vector3(9, c + 0x74);
        SpawnCoinsAndDie();
        return;
    }

    {
        unsigned int id = *(unsigned int *)(c + 0x1d4);
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
        SetState(6);
        return;
    }

    if ((*(int *)(c + 0x1d0) & 0x66fe0)
        || _ZN6Player9IsOnShellEv(r4) != 0
        || *(unsigned char *)(r4 + 0x6f9) != 0) {
        _ZN5Sound9PlayBank0EjRK7Vector3(9, c + 0x74);
        SpawnCoinsAndDie();
        return;
    }

    if (*(int *)(c + 0x1d0) & 0x10) {
        *(short *)(c + 0x94) = Vec3_HorzAngle(r4 + 0x5c, c + 0x5c);
        *(short *)(c + 0x8e) = (short)(*(short *)(c + 0x94) + 0x8000);
        _ZN6Player16IncMegaKillCountEv(r4);
        SetState(8);
        return;
    }

    if (_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(c, c + 0x1b0, r4) != 0) {
        if (*(int *)(c + 0x3e0) == 0)
            return;
        _ZN6Player6BounceE5Fix12IiE(r4, 0x28000);
        SpawnCoinsAndDie();
        return;
    }

    if (*(int *)(c + 0x3e0) == 7)
        return;

    {
        int v[3];
        v[0] = *(int *)(c + 0x5c);
        v[1] = *(int *)(c + 0x60);
        v[2] = *(int *)(c + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r4, v, 2, 0xc000, 1, 0, 1);
    }
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 7 -- _ZN8daGmch_c21ApplySlopeToVertSpeedEPv, 0x02126950, size 0xd0 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c21ApplySlopeToVertSpeedEPv
/* Declared `int` and it falls off the end: the ROM leaves r0 holding whatever
   the last call left there and every caller discards the result. */
int daGmch_c::ApplySlopeToVertSpeed(void *clsn)
{
    int *self = (int *)this;

    int n0[3];
    int n1[3];
    func_02038414(clsn);
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(clsn)) {
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char *)_ZNK10dBgCh_Actr14GetFloorResultEv(clsn) + 4, n0);
        if (n0[1] != 0) {
            long long a = (long long)n0[0] * (long long)self[0xa4 / 4];
            long long b = (long long)n0[2] * (long long)self[0xac / 4];
            int x = (int)((a + 0x800) >> 12);
            int y = (int)((b + 0x800) >> 12);
            self[0xa8 / 4] = -(_ZN4cstd4fdivEii(x + y, n0[1]) + 0x8000);
        }
    }
    if (_ZNK10dBgCh_Actr8IsOnWallEv(clsn)) {
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char *)_ZNK10dBgCh_Actr13GetWallResultEv(clsn) + 4, n1);
    }
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 8 -- _ZN8daGmch_c18UpdateDrawMatricesEv, 0x02126a20, size 0x200 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c18UpdateDrawMatricesEv
void daGmch_c::UpdateDrawMatrices()
{
    char *c = (char *)this;
    extern Mtx43_26a20 data_020a0e68;
    extern Vector3 data_ov081_02128ef8;

    int b = (int)((*(u32 *)(c + 0xb0) & 0x40000) != 0);
    if (b)
        return;

    Matrix4x3_FromRotationY(c + 0xf0, *(s16 *)(c + 0x8e));
    *(int *)(c + 0x114) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x118) = *(int *)(c + 0x60) >> 3;
    *(int *)(c + 0x11c) = *(int *)(c + 0x64) >> 3;

    _ZN9ModelBase12ApplyOpacityEj(c + 0xd4, *(u8 *)(c + 0x3f0), 0);

    if (*(s16 *)(c + 0x8c) != 0) {
        data_020a0e68 = *(Mtx43_26a20 *)(c + 0xf0);
        int y1 = ((VObj_26a20 *)c)->m29() >> 3;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y1, 0);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(s16 *)(c + 0x8c));
        int y2 = (-((VObj_26a20 *)c)->m29()) >> 3;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y2, 0);
        *(Mtx43_26a20 *)(c + 0xf0) = data_020a0e68;
    } else {
        data_020a0e68 = *(Mtx43_26a20 *)(c + 0xf0);
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, data_ov081_02128ef8.x, data_ov081_02128ef8.y, data_ov081_02128ef8.z);
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(s16 *)(c + 0x3ec));
        *(Mtx43_26a20 *)(c + 0x154) = data_020a0e68;
    }

    _ZN9ModelBase12ApplyOpacityEj(c + 0x138, (0x20 - *(u8 *)(c + 0x3f0)) & 0xff, 0);

    *(int *)(c + 0x3c4) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x3c8) = *(int *)(c + 0x60) >> 3;
    *(int *)(c + 0x3cc) = *(int *)(c + 0x64) >> 3;

    int dh = (*(int *)(c + 0x3e0) == 8) ? 0x258000 : 0x12c000;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x188, c + 0x3a0, 0x78000, dh, 0xf);
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 9 -- _ZN8daGmch_c12UpdateState8Ev, 0x02126c20, size 0x6c */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c12UpdateState8Ev
int daGmch_c::UpdateState8()
{
    char *c = (char *)this;

    *(short *)(c + 0x8c) = *(short *)(c + 0x8c) - 0x1000;
    _ZN9Animation7AdvanceEv(c + 0x124);
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, c + 0x1b0);
    func_02038414(c + 0x1e4);
    if (_ZNK10dBgCh_Actr13JustHitGroundEv(c + 0x1e4) != 0 || DecIfAbove0_Byte(c + 0x3f2) == 0) {
        SpawnCoinsAndDie();
    }
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 10 -- _ZN8daGmch_c11EnterState8Ev, 0x02126c8c, size 0xd8 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c11EnterState8Ev
int daGmch_c::EnterState8()
{
    char *c = (char *)this;
    extern int data_ov081_02128edc[];

    _ZN5Sound9PlayBank0EjRK7Vector3(9, (const void *)(c + 0x74));
    *(int *)(((int)c + 0xb0)) &= ~1;
    *(int *)(c + 0x98) = 0xa000;
    *(int *)(c + 0xa8) = 0x28000;
    *(unsigned char *)(c + 0x3f2) = 0x2d;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, *(void **)((char *)data_ov081_02128edc + 4), 0, 0x1000, 0);
    *(int *)(c + 0x130) = 0x4000;
    VObj_26c8c *b = (VObj_26c8c *)c;
    int r1 = b->m29();
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x43, *(int *)(c + 0x5c), *(int *)(c + 0x60) + r1, *(int *)(c + 0x64));
    int r2 = b->m29();
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x44, *(int *)(c + 0x5c), *(int *)(c + 0x60) + r2, *(int *)(c + 0x64));
    *(int *)(c + 0x3e0) = 8;
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 11 -- _ZN8daGmch_c12UpdateState7Ev, 0x02126d64, size 0xc4 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c12UpdateState7Ev
/* The angle reset is on the IsOnGround path, not the JustHitGround one. */
int daGmch_c::UpdateState7()
{
    char *s = (char *)this;

    func_02038414(s + 0x1e4);
    *(short *)(s + 0x8c) = *(short *)(s + 0x8c) + 0x1000;
    if (_ZNK10dBgCh_Actr13JustHitGroundEv(s + 0x1e4) != 0) {
        *(int *)(s + 0xa8) = *(int *)(s + 0xa8) * -0x3c / 100;
    } else if (_ZNK10dBgCh_Actr10IsOnGroundEv(s + 0x1e4) != 0) {
        *(int *)(s + 0xa8) = 0;
        _ZN10dBgCh_Actr15ClearLimMovFlagEv(s + 0x1e4);
        {
            short v94 = *(short *)(s + 0x94);
            *(short *)(s + 0x8c) = 0;
            *(short *)(s + 0x8e) = v94;
            *(short *)(s + 0x90) = 0;
            ChooseNextState();
        }
    }
    _ZN8dActor_c9UpdatePosEP5dCc_c(s, s + 0x1b0);
    CheckPlayerContact();
    _ZN5dCc_c5ClearEv(s + 0x1b0);
    _ZN5dCc_c6UpdateEv(s + 0x1b0);
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 12 -- _ZN8daGmch_c11EnterState7Ev, 0x02126e28, size 0x17c */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c11EnterState7Ev
int daGmch_c::EnterState7()
{
    char *c = (char *)this;
    extern s16 data_02082214[];

    int *pf;
    Vec3_26e28 v;
    char *player;
    char *src;
    int zero;
    s16 ang;
    int idx;
    s16 s;

    pf = (int *)((int)c + 0xb0);
    *pf = *pf & ~0x80000;
    zero = 0;
    player = *(char **)(c + 0xd0);
    *(int *)(c + 0x98) = *(int *)(player + 0x98) + 0x7000;
    *(int *)(c + 0xa8) = zero;
    player = *(char **)(c + 0xd0);
    ang = *(s16 *)(player + 0x8e);
    *(s16 *)(c + 0x8e) = ang;
    *(s16 *)(c + 0x94) = *(s16 *)(c + 0x8e);

    player = *(char **)(c + 0xd0);
    src = (char *)(player + 0x5c);
    *(int *)(c + 0x5c) = *(int *)src;
    *(int *)(c + 0x60) = *(int *)(src + 4);
    *(int *)(c + 0x64) = *(int *)(src + 8);

    idx = (*(u16 *)(c + 0x8e) >> 4);
    s = *(s16 *)((char *)data_02082214 + (idx << 2));
    *(int *)((int)c + 0x5c) =
        *(int *)((int)c + 0x5c)
        + (int)(((s64)s * 0x50000 + 0x800) >> 12);
    *(int *)((int)c + 0x60) =
        *(int *)((int)c + 0x60) + 0x50000;
    idx = (*(u16 *)(c + 0x8e) >> 4);
    s = *(s16 *)((char *)data_02082214 + ((idx * 2 + 1) << 1));
    *(int *)((int)c + 0x64) =
        *(int *)((int)c + 0x64)
        + (int)(((s64)s * 0x50000 + 0x800) >> 12);

    player = *(char **)(c + 0xd0);
    {
        int y = *(int *)(player + 0x60);
        int z = *(int *)(player + 0x64);
        int y2 = y + 0x14000;
        int x = *(int *)(player + 0x5c);
        ((int *)&v)[0] = x;
        ((int *)&v)[1] = y2;
        ((int *)&v)[2] = z;
    }

    _ZN8dActor_c17DetectRaycastClsnER7Vector3S1_b(c, &v, (Vec3_26e28 *)(c + 0x5c), 1);
    *(int *)(c + 0xd0) = zero;
    _ZN10dBgCh_Actr13SetLimMovFlagEv(c + 0x1e4);
    *(int *)(c + 0x3e0) = 7;
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 13 -- _ZN8daGmch_c12UpdateState6Ev, 0x02126fa4, size 0xa0 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c12UpdateState6Ev
int daGmch_c::UpdateState6()
{
    char *self = (char *)this;

    int b;
    b = (*(int *)(self + 0xb0) & 0x40000) != 0;
    if (b) {
        int *sv = (int *)(*(char **)(self + 0xd0) + 0x5c);
        *(int *)(self + 0x5c) = sv[0];
        *(int *)(self + 0x60) = sv[1];
        *(int *)(self + 0x64) = sv[2];
    }
    b = (*(int *)(self + 0xb0) & 0x80000) != 0;
    if (b) {
        SetState(7);
    } else {
        b = (*(int *)(self + 0xb0) & 0x20000) != 0;
        if (!b) {
            b = (*(int *)(self + 0xb0) & 0x40000) != 0;
            if (!b) {
                ChooseNextState();
            }
        }
    }
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 14 -- _ZN8daGmch_c11EnterState6Ev, 0x02127044, size 0x2c */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c11EnterState6Ev
int daGmch_c::EnterState6()
{
    char *c = (char *)this;

    *(int *)(c + 0x98) = 0;
    _ZN5dCc_c5ClearEv((char *)c + 0x1b0);
    *(int *)(c + 0x3e0) = 6;
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 15 -- _ZN8daGmch_c12UpdateState5Ev, 0x02127070, size 0xc4 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c12UpdateState5Ev
int daGmch_c::UpdateState5()
{
    Obj_27070 *self = (Obj_27070 *)this;

    Fix12i d = Vec3_Dist(&self->pos, &self->target);
    if (d < self->f98) {
        self->f8e = Vec3_HorzAngle(&self->pos, &self->target);
        self->f94 = self->f8e;
        self->f98 = d;
    } else {
        _Z14ApproachLinearRsss(&self->f8e, Vec3_HorzAngle(&self->pos, &self->target), 0x2bc);
        self->f94 = self->f8e;
    }
    _ZN9Animation7AdvanceEv(self->anim);
    _ZN8dActor_c9UpdatePosEP5dCc_c(self, self->clsn);
    ApplySlopeToVertSpeed(self->field1e4);
    CheckPlayerContact();
    if (d < 0xa000) {
        ChooseNextState();
    }
    _ZN5dCc_c5ClearEv(self->clsn);
    _ZN5dCc_c6UpdateEv(self->clsn);
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 16 -- _ZN8daGmch_c11EnterState5Ev, 0x02127134, size 0x54 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c11EnterState5Ev
int daGmch_c::EnterState5()
{
    char *c = (char *)this;
    extern int data_ov081_02128edc[];

    *(int *)(c + 0x98) = 0xa000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void *)data_ov081_02128edc[1], 0, 0x1000, 0);
    *(int *)(c + 0x130) = 0x1000;
    *(int *)(c + 0x3e0) = 5;
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 17 -- _ZN8daGmch_c12UpdateState4Ev, 0x02127188, size 0x60 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c12UpdateState4Ev
int daGmch_c::UpdateState4()
{
    char *c = (char *)this;

    _ZN9Animation7AdvanceEv(c + 0x124);
    if (_ZN9Animation8FinishedEv(c + 0x124)) {
        *(unsigned int *)(c + 0xb0) |= 1;
        SetState(*(int *)(c + 0x3e4));
    }
    CheckPlayerContact();
    _ZN5dCc_c5ClearEv(c + 0x1b0);
    _ZN5dCc_c6UpdateEv(c + 0x1b0);
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 18 -- _ZN8daGmch_c11EnterState4Ev, 0x021271e8, size 0x58 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c11EnterState4Ev
int daGmch_c::EnterState4()
{
    char *c = (char *)this;
    extern Bca2 data_ov081_02128ec4;

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void *)data_ov081_02128ec4.w[1], 0x40000000, 0x1000, 0);
    *(int *)(c + 0x130) = 0x1000;
    *(int *)(c + 0x98) = 0;
    *(int *)(c + 0x3e0) = 4;
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 19 -- _ZN8daGmch_c12UpdateState3Ev, 0x02127240, size 0xd4 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c12UpdateState3Ev
int daGmch_c::UpdateState3()
{
    char *c = (char *)this;

    switch (*(unsigned char *)(c + 0x3f1)) {
    case 0:
        if ((((unsigned int)*(int *)(c + 0x12c)) << 4) >> 16 == 7) {
            *(int *)(c + 0x98) = 0x12000;
            *(int *)(c + 0xa8) = 0x1e000;
            func_0201267c(0x77, c + 0x74);
            (*(unsigned char *)(((int)c + 0x3f1)))++;
        }
        break;
    case 1:
        if (_ZNK10dBgCh_Actr13JustHitGroundEv(c + 0x1e4)) {
            func_0201267c(0x71, c + 0x74);
            ChooseNextState();
        }
        break;
    }
    _ZN9Animation7AdvanceEv(c + 0x124);
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, c + 0x1b0);
    ApplySlopeToVertSpeed(c + 0x1e4);
    CheckPlayerContact();
    _ZN5dCc_c5ClearEv(c + 0x1b0);
    _ZN5dCc_c6UpdateEv(c + 0x1b0);
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 20 -- _ZN8daGmch_c11EnterState3Ev, 0x02127314, size 0x84 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c11EnterState3Ev
int daGmch_c::EnterState3()
{
    char *c = (char *)this;
    extern int data_ov081_02128ecc[];
    extern int data_0209e650[];

    int *base = (int *)((int)c + 0xb0);
    *base = *base & ~1;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void *)data_ov081_02128ecc[1], 0x40000000, 0x1000, 0);
    *(int *)(c + 0x12c) = 0;
    *(short *)(c + 0x8e) = (short)RandomIntInternal(data_0209e650);
    *(short *)(c + 0x94) = *(short *)(c + 0x8e);
    *(int *)(c + 0x98) = 0;
    *(unsigned char *)(c + 0x3f1) = 0;
    *(int *)(c + 0x3e0) = 3;
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 21 -- _ZN8daGmch_c12UpdateState2Ev, 0x02127398, size 0x50 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c12UpdateState2Ev
int daGmch_c::UpdateState2()
{
    char *c = (char *)this;

    if (_ZN9Animation8FinishedEv(c + 0x124)) {
        SetState(*(int *)(c + 0x3e4));
    }
    _ZN9Animation7AdvanceEv(c + 0x124);
    CheckPlayerContact();
    _ZN5dCc_c5ClearEv(c + 0x1b0);
    _ZN5dCc_c6UpdateEv(c + 0x1b0);
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 22 -- _ZN8daGmch_c11EnterState2Ev, 0x021273e8, size 0x58 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c11EnterState2Ev
int daGmch_c::EnterState2()
{
    char *c = (char *)this;
    extern Bca2 data_ov081_02128ee4;

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void *)data_ov081_02128ee4.w[1], 0x40000000, 0x1000, 0);
    *(int *)(c + 0x130) = 0x1000;
    *(int *)(c + 0x98) = 0;
    *(int *)(c + 0x3e0) = 2;
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 23 -- _ZN8daGmch_c12UpdateState1Ev, 0x02127440, size 0x88 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c12UpdateState1Ev
int daGmch_c::UpdateState1()
{
    char *c = (char *)this;

    _Z14ApproachLinearRsss((short *)(c + 0x8e), *(short *)(c + 0x300 + 0xee), 0x2bc);
    *(short *)(c + 0x94) = *(short *)(c + 0x8e);
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, c + 0x1b0);
    ApplySlopeToVertSpeed(c + 0x1e4);
    CheckPlayerContact();
    _ZN9Animation7AdvanceEv(c + 0x124);
    if (DecIfAbove0_Byte((unsigned char *)(c + 0x3f2)) == 0) {
        ChooseNextState();
    }
    _ZN5dCc_c5ClearEv(c + 0x1b0);
    _ZN5dCc_c6UpdateEv(c + 0x1b0);
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 24 -- _ZN8daGmch_c11EnterState1Ev, 0x021274c8, size 0x90 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c11EnterState1Ev
void daGmch_c::EnterState1()
{
    char *r4 = (char *)this;
    extern int data_ov081_02128edc[];
    extern int data_0209e650[];

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(r4 + 0xd4, (void *)data_ov081_02128edc[1], 0, 0x1000, 0);
    *(int *)(r4 + 0x130) = 0x1000;
    *(int *)(r4 + 0x98) = 0xa000;
    *(short *)(r4 + 0x3ee) = (short)RandomIntInternal(data_0209e650);
    *(unsigned char *)(r4 + 0x3f2) = (unsigned char)((unsigned int)RandomIntInternal(data_0209e650) % 0x78);
    *(int *)(r4 + 0x3e0) = 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 25 -- _ZN8daGmch_c12UpdateState0Ev, 0x02127558, size 0x158 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c12UpdateState0Ev
int daGmch_c::UpdateState0()
{
    char *c = (char *)this;

    u8 *pstate;
    int *pflg;

    switch (*(u8 *)(c + 0x3f1)) {
    case 0:
        if (_Z15ApproachLinear2Riii((int *)(c + 0x3f0), 1, 2) != 0) {
            _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(c + 0x1b0, c, 0x4b000, 0x73000, 0x200000, 0);
            pflg = (int *)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFFLL);
            *pflg = *pflg & ~0x10000000;
            pstate = (u8 *)(((int)c + 0x3f1) & 0xFFFFFFFFFFFFFFFFLL);
            *pstate = *pstate + 1;
        }
        break;
    case 1:
        if (_ZN8dActor_c13DistToCPlayerEv(c) < 0x1f4000) {
            pflg = (int *)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFFLL);
            *pflg = *pflg | 0x10000000;
            func_0201267c(0x76, c + 0x74);
            pstate = (u8 *)(((int)c + 0x3f1) & 0xFFFFFFFFFFFFFFFFLL);
            *pstate = *pstate + 1;
        }
        break;
    case 2:
        if (_Z15ApproachLinear2Riii((int *)(c + 0x3f0), 0x1f, 2) != 0) {
            _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(c + 0x1b0, c, 0x4b000, 0x73000, 0x200000, 0x6eff0);
            SetState(3);
        }
        break;
    }

    /* Written as an explicit read-modify-write, not `+= 0xc00`, and against the
       flat +0x3ec rather than a pre-split base.  `ldrsh`/`strh` carry only an
       8-bit offset, so 0x3ec cannot be encoded and the address MUST be split;
       the ROM splits it `add rN,c,#0x300` + `[rN,#0xec]`.  Under 2004/b56 the
       compound-assignment form is the one spelling that instead materialises
       the whole 0x3ec and loads through `[rN]`.  Letting b56 do its own
       splitting reproduces the ROM; pre-splitting it in the source does not. */
    {
        s16 v = *(s16 *)(c + 0x3ec);
        *(s16 *)(c + 0x3ec) = (s16)(v + 0xc00);
    }
    CheckPlayerContact();
    _ZN5dCc_c5ClearEv(c + 0x1b0);
    _ZN5dCc_c6UpdateEv(c + 0x1b0);
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 26 -- _ZN8daGmch_c11EnterState0Ev, 0x021276b0, size 0x58 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c11EnterState0Ev
int daGmch_c::EnterState0()
{
    char *c = (char *)this;
    extern void *data_ov081_02128ec4[];

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, data_ov081_02128ec4[1], 0, 0x1000, 0);
    *(int *)(c + 0x130) = 0x1000;
    *(int *)(c + 0x12c) = 0x6000;
    *(char *)(c + 0x3f1) = 0;
    *(int *)(c + 0x3e0) = 0;
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 27 -- _ZN8daGmch_c15CallStateUpdateEv, 0x02127708, size 0x3c */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c15CallStateUpdateEv
/* Invokes PMF[1] -- the "update" half -- of the current state's 16-byte pair,
   whose address this+0x3dc holds. */
void daGmch_c::CallStateUpdate()
{
    C_27708 *c = (C_27708 *)this;
    PMF_27708 *p = c->pp + 1;
    (c->**p)();
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 28 -- _ZN8daGmch_c14CallStateEnterEv, 0x02127744, size 0x38 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c14CallStateEnterEv
/* Invokes PMF[0] -- the "enter" half -- of the current state's pair.  It is
   never BL'd: ordinal 29 takes its address from a literal pool and tail-calls
   it through `bx ip`. */
void daGmch_c::CallStateEnter()
{
    C_27744 *c = (C_27744 *)this;
    PMF_27744 *p = c->pp;
    (c->**p)();
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 29 -- _ZN8daGmch_c8SetStateEi, 0x0212777c, size 0x1c */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c8SetStateEi
/* The state setter: stores a pointer to state `a`'s 16-byte pair in the .bss
   mirror at 0x02128f40 and tail-calls the enter half. */
void daGmch_c::SetState(int a)
{
    char *c = (char *)this;
    extern char data_ov081_02128f40;

    *(int *)(c + 0x3dc) = (int)&data_ov081_02128f40 + (a << 4);
    CallStateEnter();
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 30 -- _ZN8daGmch_c16CleanupResourcesEv, 0x02127798, size 0x44 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c16CleanupResourcesEv
/* Vtable slot 3.  Releases one shared file, then a four-entry table; it never
   touches `this`. */
int daGmch_c::CleanupResources()
{
    extern void *data_ov081_02128ed4;
    extern void *data_ov081_021280d8[];

    _ZN13SharedFilePtr7ReleaseEv(&data_ov081_02128ed4);
    for (int i = 0; i < 4; i++) {
        _ZN13SharedFilePtr7ReleaseEv(data_ov081_021280d8[i]);
    }
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 31 -- _ZN8daGmch_c16OnPendingDestroyEv, 0x021277dc, size 0x4 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c16OnPendingDestroyEv
/* Vtable slot 12.  The ROM body is one `bx lr`: the override exists only to
   occupy the slot. */
void daGmch_c::OnPendingDestroy()
{
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 32 -- _ZN8daGmch_c6RenderEv, 0x021277e0, size 0x74 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c6RenderEv
/* Vtable slot 9.  Draws the ModelAnim only above mState 1 and the Model only at
   or below 0x1f, so the two overlap for 2..0x1f. */
int daGmch_c::Render()
{
    bool b = mFlags & 0x40000;
    if (b != 0)
        return 1;
    if (mState > 1) {
        ((Sub_277e0 *)((char *)&mModelAnim))->m(0);
    }
    if (mState <= 0x1f) {
        ((Sub_277e0 *)((char *)&mModel))->m(0);
    }
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 33 -- _ZN8daGmch_c8BehaviorEv, 0x02127854, size 0x54 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c8BehaviorEv
/* Vtable slot 6.  Runs the current state's update half, then the shared
   per-frame work. */
int daGmch_c::Behavior()
{
    CallStateUpdate();
    _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(((char *)this), ((char *)this) + 0x1b0);
    if (_ZNK10dBgCh_Actr14GetResultFlag1Ev((char *)&mWithMeshClsn) != 0) {
        if (_ZNK10dBgCh_Actr12TouchesWaterEv((char *)&mWithMeshClsn) != 0) {
            SpawnCoinsAndDie();
        }
    }
    UpdateDrawMatrices();
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 34 -- _ZN8daGmch_c13InitResourcesEv, 0x021278a8, size 0x1d4 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c13InitResourcesEv
/* Vtable slot 0. */
int daGmch_c::InitResources()
{
    extern void *data_ov081_02128ed4;
    extern void *data_ov081_021280d8[];
    extern int data_ov002_0210d9b8[];
    extern Matrix4x3 IDENTITY_MATRIX4X3;

    Vector3 pos;
    void *m = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov081_02128ed4);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0xd4, m, 1, 1);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0x138, (void *)data_ov002_0210d9b8[1], 1, 1) == 0)
        return 0;
    for (int i = 0; i < 4; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov081_021280d8[i]);
    if (_ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel) == 0)
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char *)this) + 0x1b0, ((char *)this), 0x4b000, 0x73000, 0x200000, 0x6eff0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char *)this) + 0x1e4, ((char *)this), 0x4b000, 0x4b000, 0, 0);
    _ZN10dBgCh_Actr19StartDetectingWaterEv((char *)&mWithMeshClsn);
    SetState(0);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    {
        int p60;
        pos.x = mPosX;
        p60 = mPosY;
        pos.y = p60;
        pos.z = mPosZ;
        pos.y = p60 + 0x14000;
    }
    dBgCh_Gnd ground;
    ground.SetObjAndPos(pos, 0);
    if (ground.DetectClsn())
        mPosY = ground.clsnY;
    else
        mPosY = pos.y;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    mState = 1;
    *(Matrix4x3 *)((char *)&mMatrix) = IDENTITY_MATRIX4X3;
    UpdateDrawMatrices();
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 35 -- _ZN8daGmch_c13OnTurnIntoEggER6Player, 0x02127a7c, size 0x60 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c13OnTurnIntoEggER6Player
/* Vtable slot 19.  Gives the player 5 coins -- as cap-collection coins if Yoshi
   is wearing the cap, otherwise as egg coins -- then kills this actor and
   tracks it in the death table. */
int daGmch_c::OnTurnIntoEgg(Player &player)
{
    if (player.IsCollectingCap())
        GivePlayerCoins(player, 5, 0);
    else
        player.RegisterEggCoinCount(5, 0, 0);
    KillAndTrackInDeathTable();
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 36 -- daGmch_c_classInit, 0x02127adc, size 0x58 */
/* ------------------------------------------------------------------ */
// @symbol daGmch_c_classInit
extern "C" {
/* The GAMAGUCHI factory: allocates 0x3f4, runs dActor_c's constructor, installs
 * this class's vtable and constructs the five member objects.
 *
 * Reconstructed source-style name: SM64DS proves daGmch_c through RTTI,
 * allocation size, vtable identity and the GAMAGUCHI registry profile; later
 * EAD lineage supplies classInit.  Exact original spelling is not preserved.
 * Historical alias: Moneybag_Spawn. */
int *daGmch_c_classInit(void)
{
    extern void *_ZN7fBase_cnwEj(unsigned int size);
    extern void _ZN8dActor_cC2Ev(void *p);
    extern void _ZN9ModelAnimC1Ev(void *p);
    extern void _ZN5ModelC1Ev(void *p);
    extern void _ZN11ShadowModelC1Ev(void *p);
    extern void _ZN7dCcAc_cC1Ev(void *p);
    extern void _ZN10dBgCh_ActrC1Ev(void *p);
    /* _ZTV8daGmch_c is declared in daGmch_c.h -- see the note there. */

    int *p = (int *)_ZN7fBase_cnwEj(1012);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)&_ZTV8daGmch_c[2];
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
        _ZN5ModelC1Ev((char *)p + 0x138);
        _ZN11ShadowModelC1Ev((char *)p + 0x188);
        _ZN7dCcAc_cC1Ev((char *)p + 0x1b0);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x1e4);
    }
    return p;
}
}
