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
 * Ten already had reconstructed mangled names in the tree.  The other 26 had
 * auto-generated `func_ov081_<address>` names, and this TU replaces those
 * with `daGmch_c::` methods; the header says what each name is and is not
 * allowed to claim.  Every one of the 26 still byte-matches, so nothing here is
 * a near-miss and nothing stayed a free function for want of a match.
 *
 * The one function that is deliberately NOT a member is daGmch_c_classInit, the
 * profile factory at the end.  Its name follows the project's free-factory
 * convention and later EAD lineage; the original spelling is not preserved.
 * Its observed role is to construct the object rather than run on one.
 *
 * A class member function may NOT sit in a linkage-specification region, so
 * once every member became a method, every external function declaration had to
 * move to the one file-scope `extern "C"` region below.  Reconciling the shards'
 * disagreeing spellings there was the whole cost of the conversion; that
 * region's own comment records what was measured.
 *
 * Every access to this object and to its dActor_c base goes through the fields
 * daGmch_c.h and dActor_c.h name; the member objects (mModelAnim, mModel,
 * mShadowModel, mdCcAc_c, mWithMeshClsn, mMatrix) are addressed by name, and
 * Animation::Advance/Finished, the two models' slot-5 Render and the Player
 * queries are called as the methods their headers declare.  Three shadow
 * types remain, each for a measured reason recorded at its definition: the
 * flat Vec3 EnterState7 hands to DetectRaycastClsn, the two pointer-to-member
 * windows the state invokers dispatch through, and the two-word BCA record
 * two entry states read.  Calls whose callee is a non-virtual member of
 * another class (`dCc_c::Clear`, `dActor_c::UpdatePos`, ...) are still spelt
 * as mangled `extern "C"` bridges: the callee's own header does not yet
 * declare them as methods.
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

/* EnterState7 (ordinal 12) -- a flat three-int vector, NOT types.h's
   Vector3: that one has a declared destructor and is not an aggregate. */
typedef struct Vec3_26e28 { int x, y, z; } Vec3_26e28;

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

/* EnterState4 and EnterState2 (ordinals 18, 22) -- the two-word BCA file-pointer records those members
   recovered as a struct rather than as an array.  Hoisted to file scope only
   because a block-scope tag cannot type an `extern` object once the member is a
   class method; the field expressions are untouched. */
struct Bca2 { int w[2]; };

/* ---------------------------------------------------------------------------
 * External function bridges used by class methods, with C linkage.
 * ---------------------------------------------------------------------------
 * A class member function may not sit in a linkage-specification region, so
 * once a member becomes `daGmch_c::Something` a declaration written in its body
 * gets C++ linkage and the reference mangles.  The 50 bridges used by these
 * methods are declared once here; the free factory keeps its seven declarations
 * at block scope.
 *
 * Of the original 54 declarations, exactly NINE had more than one type spelling
 * across the 36 shards: Vec3_Dist, dActor_c::DistToCPlayer, func_02038414,
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
    extern int data_0209e650;

    Fix12i distC;
    Fix12i dist;
    int v;

    distC = _ZN8dActor_c13DistToCPlayerEv(this);
    dist = Vec3_Dist(&mPosX, &mSpawnPosX);
    if (dist > 0x5dc000) {
        mNextState = 5;
    } else if (distC > 0x3e8000) {
        mNextState = 0;
    } else if (distC < 0x1f4000) {
        mNextState = 3;
    } else {
        if (RandomIntInternal(&data_0209e650) % 10 < 7)
            mNextState = 3;
        else
            mNextState = 1;

        if (mStateIndex == 1 && mNextState == 1) {
            ChooseNextState();
            return;
        }
    }

    v = mStateIndex;
    if (v == 1 && mNextState != 1) {
        SetState(2);
        return;
    }
    if (v == 3 && mNextState != 3) {
        SetState(4);
        return;
    }
    SetState(mNextState);
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 5 -- _ZN8daGmch_c16SpawnCoinsAndDieEv, 0x02126700, size 0x58 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c16SpawnCoinsAndDieEv
void daGmch_c::SpawnCoinsAndDie()
{
    Vector3 t;
    t.x = mPosX;
    t.y = mPosY;
    t.z = mPosZ;
    _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(this, t, 5, 0xf000, 0);
    _ZN8dActor_c8PoofDustEv(this);
    _ZN8dActor_c24KillAndTrackInDeathTableEv(this);
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 6 -- _ZN8daGmch_c18CheckPlayerContactEv, 0x02126758, size 0x1f8 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c18CheckPlayerContactEv
/* The actor found through mdCcAc_c.otherOwner is only acted on when its
   actorID is 0xbf, the player's, so it is typed as one.  Bounce and Hurt stay
   bridges: Player.h does not declare them yet. */
void daGmch_c::CheckPlayerContact()
{
    Player *player;
    int b;

    if (_ZN8dActor_c7FindEggER5dCc_c(this, &mdCcAc_c) != 0) {
        _ZN5Sound9PlayBank0EjRK7Vector3(9, &mCamSpacePosX);
        SpawnCoinsAndDie();
        return;
    }

    {
        unsigned int id = mdCcAc_c.otherOwner;
        if (id == 0)
            return;
        player = (Player *)_ZN8dActor_c10FindWithIDEj(id);
    }
    if (player == 0)
        return;

    b = (int)(player->actorID == 0xbf);
    if (b == 0)
        return;

    b = (int)((mFlags & 0x20000) != 0);
    if (b != 0) {
        SetState(6);
        return;
    }

    if ((mdCcAc_c.hitFlags & 0x66fe0)
        || player->IsOnShell() != 0
        || player->mIsMetal != 0) {
        _ZN5Sound9PlayBank0EjRK7Vector3(9, &mCamSpacePosX);
        SpawnCoinsAndDie();
        return;
    }

    if (mdCcAc_c.hitFlags & 0x10) {
        mPrevAngleY = Vec3_HorzAngle(&player->mPosX, &mPosX);
        mAngleY = (short)(mPrevAngleY + 0x8000);
        player->IncMegaKillCount();
        SetState(8);
        return;
    }

    if (_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(this, &mdCcAc_c, player) != 0) {
        if (mStateIndex == 0)
            return;
        _ZN6Player6BounceE5Fix12IiE(player, 0x28000);
        SpawnCoinsAndDie();
        return;
    }

    if (mStateIndex == 7)
        return;

    {
        int v[3];
        v[0] = mPosX;
        v[1] = mPosY;
        v[2] = mPosZ;
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(player, v, 2, 0xc000, 1, 0, 1);
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
    int n0[3];
    int n1[3];
    func_02038414(clsn);
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(clsn)) {
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char *)_ZNK10dBgCh_Actr14GetFloorResultEv(clsn) + 4, n0);
        if (n0[1] != 0) {
            long long a = (long long)n0[0] * (long long)unk_0a4;
            long long b = (long long)n0[2] * (long long)unk_0ac;
            int x = (int)((a + 0x800) >> 12);
            int y = (int)((b + 0x800) >> 12);
            mVertSpeed = -(_ZN4cstd4fdivEii(x + y, n0[1]) + 0x8000);
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
/* Both model matrices are edited through the shared scratch matrix
   data_020a0e68: copied out whole, transformed in place, copied back.  The
   slot-29 calls are this class's own OnAimedAtWithEgg, dispatched virtually
   exactly as the ROM does. */
void daGmch_c::UpdateDrawMatrices()
{
    extern Matrix4x3 data_020a0e68;
    extern Vector3 data_ov081_02128ef8;

    int b = (int)((mFlags & 0x40000) != 0);
    if (b)
        return;

    Matrix4x3_FromRotationY(&mModelAnim.mat4x3, mAngleY);
    mModelAnim.mat4x3.m[9] = mPosX >> 3;
    mModelAnim.mat4x3.m[10] = mPosY >> 3;
    mModelAnim.mat4x3.m[11] = mPosZ >> 3;

    _ZN9ModelBase12ApplyOpacityEj(&mModelAnim, mState, 0);

    if (mAngleX != 0) {
        data_020a0e68 = mModelAnim.mat4x3;
        int y1 = OnAimedAtWithEgg() >> 3;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y1, 0);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mAngleX);
        int y2 = (-OnAimedAtWithEgg()) >> 3;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y2, 0);
        mModelAnim.mat4x3 = data_020a0e68;
    } else {
        data_020a0e68 = mModelAnim.mat4x3;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, data_ov081_02128ef8.x, data_ov081_02128ef8.y, data_ov081_02128ef8.z);
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mSpinAngleY);
        mModel.mat4x3 = data_020a0e68;
    }

    _ZN9ModelBase12ApplyOpacityEj(&mModel, (0x20 - mState) & 0xff, 0);

    mMatrix.m[9] = mPosX >> 3;
    mMatrix.m[10] = mPosY >> 3;
    mMatrix.m[11] = mPosZ >> 3;

    int dh = (mStateIndex == 8) ? 0x258000 : 0x12c000;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        this, &mShadowModel, &mMatrix, 0x78000, dh, 0xf);
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 9 -- _ZN8daGmch_c12UpdateState8Ev, 0x02126c20, size 0x6c */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c12UpdateState8Ev
int daGmch_c::UpdateState8()
{
    mAngleX = mAngleX - 0x1000;
    mModelAnim.Advance();
    _ZN8dActor_c9UpdatePosEP5dCc_c(this, &mdCcAc_c);
    func_02038414(&mWithMeshClsn);
    if (_ZNK10dBgCh_Actr13JustHitGroundEv(&mWithMeshClsn) != 0 || DecIfAbove0_Byte(&mTimer) == 0) {
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
    extern int data_ov081_02128edc[];

    _ZN5Sound9PlayBank0EjRK7Vector3(9, (const void *)&mCamSpacePosX);
    mFlags &= ~1;
    mHorzSpeed = 0xa000;
    mVertSpeed = 0x28000;
    mTimer = 0x2d;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(void **)((char *)data_ov081_02128edc + 4), 0, 0x1000, 0);
    mModelAnim.speed = 0x4000;
    int r1 = OnAimedAtWithEgg();
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x43, mPosX, mPosY + r1, mPosZ);
    int r2 = OnAimedAtWithEgg();
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x44, mPosX, mPosY + r2, mPosZ);
    mStateIndex = 8;
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 11 -- _ZN8daGmch_c12UpdateState7Ev, 0x02126d64, size 0xc4 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c12UpdateState7Ev
/* The angle reset is on the IsOnGround path, not the JustHitGround one. */
int daGmch_c::UpdateState7()
{
    func_02038414(&mWithMeshClsn);
    mAngleX = mAngleX + 0x1000;
    if (_ZNK10dBgCh_Actr13JustHitGroundEv(&mWithMeshClsn) != 0) {
        mVertSpeed = mVertSpeed * -0x3c / 100;
    } else if (_ZNK10dBgCh_Actr10IsOnGroundEv(&mWithMeshClsn) != 0) {
        mVertSpeed = 0;
        _ZN10dBgCh_Actr15ClearLimMovFlagEv(&mWithMeshClsn);
        {
            short v94 = mPrevAngleY;
            mAngleX = 0;
            mAngleY = v94;
            mAngleZ = 0;
            ChooseNextState();
        }
    }
    _ZN8dActor_c9UpdatePosEP5dCc_c(this, &mdCcAc_c);
    CheckPlayerContact();
    _ZN5dCc_c5ClearEv(&mdCcAc_c);
    _ZN5dCc_c6UpdateEv(&mdCcAc_c);
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 12 -- _ZN8daGmch_c11EnterState7Ev, 0x02126e28, size 0x17c */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c11EnterState7Ev
/* mHolder is re-read before each use, as the ROM reloads it: the stores in
   between go to this object's own fields, but the shards were written with the
   reload and the bytes keep it. */
int daGmch_c::EnterState7()
{
    extern s16 data_02082214[];

    u32 *pf;
    Vec3_26e28 v;
    dActor_c *holder;
    s32 *src;
    int zero;
    s16 ang;
    int idx;
    s16 s;

    pf = &mFlags;
    *pf = *pf & ~0x80000;
    zero = 0;
    holder = mHolder;
    mHorzSpeed = holder->mHorzSpeed + 0x7000;
    mVertSpeed = zero;
    holder = mHolder;
    ang = holder->mAngleY;
    mAngleY = ang;
    mPrevAngleY = mAngleY;

    holder = mHolder;
    src = &holder->mPosX;
    mPosX = src[0];
    mPosY = src[1];
    mPosZ = src[2];

    idx = ((u16)mAngleY >> 4);
    s = *(s16 *)((char *)data_02082214 + (idx << 2));
    mPosX = mPosX + (int)(((s64)s * 0x50000 + 0x800) >> 12);
    mPosY = mPosY + 0x50000;
    idx = ((u16)mAngleY >> 4);
    s = *(s16 *)((char *)data_02082214 + ((idx * 2 + 1) << 1));
    mPosZ = mPosZ + (int)(((s64)s * 0x50000 + 0x800) >> 12);

    holder = mHolder;
    {
        int y = holder->mPosY;
        int z = holder->mPosZ;
        int y2 = y + 0x14000;
        int x = holder->mPosX;
        ((int *)&v)[0] = x;
        ((int *)&v)[1] = y2;
        ((int *)&v)[2] = z;
    }

    _ZN8dActor_c17DetectRaycastClsnER7Vector3S1_b(this, &v, (Vec3_26e28 *)&mPosX, 1);
    mHolder = (dActor_c *)zero;
    _ZN10dBgCh_Actr13SetLimMovFlagEv(&mWithMeshClsn);
    mStateIndex = 7;
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 13 -- _ZN8daGmch_c12UpdateState6Ev, 0x02126fa4, size 0xa0 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c12UpdateState6Ev
int daGmch_c::UpdateState6()
{
    int b;
    b = (mFlags & 0x40000) != 0;
    if (b) {
        s32 *sv = &mHolder->mPosX;
        mPosX = sv[0];
        mPosY = sv[1];
        mPosZ = sv[2];
    }
    b = (mFlags & 0x80000) != 0;
    if (b) {
        SetState(7);
    } else {
        b = (mFlags & 0x20000) != 0;
        if (!b) {
            b = (mFlags & 0x40000) != 0;
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
    mHorzSpeed = 0;
    _ZN5dCc_c5ClearEv(&mdCcAc_c);
    mStateIndex = 6;
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 15 -- _ZN8daGmch_c12UpdateState5Ev, 0x02127070, size 0xc4 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c12UpdateState5Ev
int daGmch_c::UpdateState5()
{
    Fix12i d = Vec3_Dist(&mPosX, &mSpawnPosX);
    if (d < mHorzSpeed) {
        mAngleY = Vec3_HorzAngle(&mPosX, &mSpawnPosX);
        mPrevAngleY = mAngleY;
        mHorzSpeed = d;
    } else {
        _Z14ApproachLinearRsss(&mAngleY, Vec3_HorzAngle(&mPosX, &mSpawnPosX), 0x2bc);
        mPrevAngleY = mAngleY;
    }
    mModelAnim.Advance();
    _ZN8dActor_c9UpdatePosEP5dCc_c(this, &mdCcAc_c);
    ApplySlopeToVertSpeed(&mWithMeshClsn);
    CheckPlayerContact();
    if (d < 0xa000) {
        ChooseNextState();
    }
    _ZN5dCc_c5ClearEv(&mdCcAc_c);
    _ZN5dCc_c6UpdateEv(&mdCcAc_c);
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 16 -- _ZN8daGmch_c11EnterState5Ev, 0x02127134, size 0x54 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c11EnterState5Ev
int daGmch_c::EnterState5()
{
    extern int data_ov081_02128edc[];

    mHorzSpeed = 0xa000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov081_02128edc[1], 0, 0x1000, 0);
    mModelAnim.speed = 0x1000;
    mStateIndex = 5;
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 17 -- _ZN8daGmch_c12UpdateState4Ev, 0x02127188, size 0x60 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c12UpdateState4Ev
int daGmch_c::UpdateState4()
{
    mModelAnim.Advance();
    if (mModelAnim.Finished()) {
        mFlags |= 1;
        SetState(mNextState);
    }
    CheckPlayerContact();
    _ZN5dCc_c5ClearEv(&mdCcAc_c);
    _ZN5dCc_c6UpdateEv(&mdCcAc_c);
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 18 -- _ZN8daGmch_c11EnterState4Ev, 0x021271e8, size 0x58 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c11EnterState4Ev
int daGmch_c::EnterState4()
{
    extern Bca2 data_ov081_02128ec4;

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov081_02128ec4.w[1], 0x40000000, 0x1000, 0);
    mModelAnim.speed = 0x1000;
    mHorzSpeed = 0;
    mStateIndex = 4;
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 19 -- _ZN8daGmch_c12UpdateState3Ev, 0x02127240, size 0xd4 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c12UpdateState3Ev
int daGmch_c::UpdateState3()
{
    switch (mPhase) {
    case 0:
        if ((((unsigned int)mModelAnim.currFrame) << 4) >> 16 == 7) {
            mHorzSpeed = 0x12000;
            mVertSpeed = 0x1e000;
            func_0201267c(0x77, &mCamSpacePosX);
            mPhase++;
        }
        break;
    case 1:
        if (_ZNK10dBgCh_Actr13JustHitGroundEv(&mWithMeshClsn)) {
            func_0201267c(0x71, &mCamSpacePosX);
            ChooseNextState();
        }
        break;
    }
    mModelAnim.Advance();
    _ZN8dActor_c9UpdatePosEP5dCc_c(this, &mdCcAc_c);
    ApplySlopeToVertSpeed(&mWithMeshClsn);
    CheckPlayerContact();
    _ZN5dCc_c5ClearEv(&mdCcAc_c);
    _ZN5dCc_c6UpdateEv(&mdCcAc_c);
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 20 -- _ZN8daGmch_c11EnterState3Ev, 0x02127314, size 0x84 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c11EnterState3Ev
int daGmch_c::EnterState3()
{
    extern int data_ov081_02128ecc[];
    extern int data_0209e650[];

    u32 *base = &mFlags;
    *base = *base & ~1;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov081_02128ecc[1], 0x40000000, 0x1000, 0);
    mModelAnim.currFrame = 0;
    mAngleY = (short)RandomIntInternal(data_0209e650);
    mPrevAngleY = mAngleY;
    mHorzSpeed = 0;
    mPhase = 0;
    mStateIndex = 3;
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 21 -- _ZN8daGmch_c12UpdateState2Ev, 0x02127398, size 0x50 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c12UpdateState2Ev
int daGmch_c::UpdateState2()
{
    if (mModelAnim.Finished()) {
        SetState(mNextState);
    }
    mModelAnim.Advance();
    CheckPlayerContact();
    _ZN5dCc_c5ClearEv(&mdCcAc_c);
    _ZN5dCc_c6UpdateEv(&mdCcAc_c);
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 22 -- _ZN8daGmch_c11EnterState2Ev, 0x021273e8, size 0x58 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c11EnterState2Ev
int daGmch_c::EnterState2()
{
    extern Bca2 data_ov081_02128ee4;

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov081_02128ee4.w[1], 0x40000000, 0x1000, 0);
    mModelAnim.speed = 0x1000;
    mHorzSpeed = 0;
    mStateIndex = 2;
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 23 -- _ZN8daGmch_c12UpdateState1Ev, 0x02127440, size 0x88 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c12UpdateState1Ev
int daGmch_c::UpdateState1()
{
    _Z14ApproachLinearRsss(&mAngleY, mTargetAngleY, 0x2bc);
    mPrevAngleY = mAngleY;
    _ZN8dActor_c9UpdatePosEP5dCc_c(this, &mdCcAc_c);
    ApplySlopeToVertSpeed(&mWithMeshClsn);
    CheckPlayerContact();
    mModelAnim.Advance();
    if (DecIfAbove0_Byte(&mTimer) == 0) {
        ChooseNextState();
    }
    _ZN5dCc_c5ClearEv(&mdCcAc_c);
    _ZN5dCc_c6UpdateEv(&mdCcAc_c);
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 24 -- _ZN8daGmch_c11EnterState1Ev, 0x021274c8, size 0x90 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c11EnterState1Ev
void daGmch_c::EnterState1()
{
    extern int data_ov081_02128edc[];
    extern int data_0209e650[];

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov081_02128edc[1], 0, 0x1000, 0);
    mModelAnim.speed = 0x1000;
    mHorzSpeed = 0xa000;
    mTargetAngleY = (short)RandomIntInternal(data_0209e650);
    mTimer = (unsigned char)((unsigned int)RandomIntInternal(data_0209e650) % 0x78);
    mStateIndex = 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 25 -- _ZN8daGmch_c12UpdateState0Ev, 0x02127558, size 0x158 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c12UpdateState0Ev
int daGmch_c::UpdateState0()
{
    u8 *pstate;
    u32 *pflg;

    switch (mPhase) {
    case 0:
        if (_Z15ApproachLinear2Riii((int *)&mState, 1, 2) != 0) {
            _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x4b000, 0x73000, 0x200000, 0);
            pflg = &mFlags;
            *pflg = *pflg & ~0x10000000;
            pstate = &mPhase;
            *pstate = *pstate + 1;
        }
        break;
    case 1:
        if (_ZN8dActor_c13DistToCPlayerEv(this) < 0x1f4000) {
            pflg = &mFlags;
            *pflg = *pflg | 0x10000000;
            func_0201267c(0x76, &mCamSpacePosX);
            pstate = &mPhase;
            *pstate = *pstate + 1;
        }
        break;
    case 2:
        if (_Z15ApproachLinear2Riii((int *)&mState, 0x1f, 2) != 0) {
            _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x4b000, 0x73000, 0x200000, 0x6eff0);
            SetState(3);
        }
        break;
    }

    /* Written as an explicit read-modify-write, not `+= 0xc00`: `ldrsh`/`strh`
       carry only an 8-bit offset, so 0x3ec cannot be encoded and the address
       MUST be split; the ROM splits it `add rN,this,#0x300` + `[rN,#0xec]`.
       Under 2004/b56 the compound-assignment form is the one spelling that
       instead materialises the whole 0x3ec and loads through `[rN]`. */
    {
        s16 v = mSpinAngleY;
        mSpinAngleY = (s16)(v + 0xc00);
    }
    CheckPlayerContact();
    _ZN5dCc_c5ClearEv(&mdCcAc_c);
    _ZN5dCc_c6UpdateEv(&mdCcAc_c);
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 26 -- _ZN8daGmch_c11EnterState0Ev, 0x021276b0, size 0x58 */
/* ------------------------------------------------------------------ */
// @symbol _ZN8daGmch_c11EnterState0Ev
int daGmch_c::EnterState0()
{
    extern void *data_ov081_02128ec4[];

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov081_02128ec4[1], 0, 0x1000, 0);
    mModelAnim.speed = 0x1000;
    mModelAnim.currFrame = 0x6000;
    mPhase = 0;
    mStateIndex = 0;
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
    extern char data_ov081_02128f40;

    mStatePmfPair = (void *)((int)&data_ov081_02128f40 + (a << 4));
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
   or below 0x1f, so the two overlap for 2..0x1f.  Both draws go through the
   models' own slot-5 Render, virtually, as the ROM dispatches them. */
int daGmch_c::Render()
{
    bool b = mFlags & 0x40000;
    if (b != 0)
        return 1;
    if (mState > 1) {
        mModelAnim.Render(0);
    }
    if (mState <= 0x1f) {
        mModel.Render(0);
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
    _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c((char *)this, (char *)&mdCcAc_c);
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
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModelAnim, m, 1, 1);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, (void *)data_ov002_0210d9b8[1], 1, 1) == 0)
        return 0;
    for (int i = 0; i < 4; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov081_021280d8[i]);
    if (_ZN11ShadowModel12InitCylinderEv(&mShadowModel) == 0)
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x4b000, 0x73000, 0x200000, 0x6eff0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x4b000, 0x4b000, 0, 0);
    _ZN10dBgCh_Actr19StartDetectingWaterEv(&mWithMeshClsn);
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
    mMatrix = IDENTITY_MATRIX4X3;
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
void daGmch_c::OnTurnIntoEgg(Player &player)
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
