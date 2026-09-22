//cpp
/* dMgJump3DMario_c -- one of the three player objects the two jump minigames
 * construct as `dMgJump3DMario_c mPlayers[3]`, ov006.
 *
 * Reconstructed translation unit: the whole linker run 0x020c762c..0x020c8a30,
 * 28 functions, 0x1404 bytes.  Assembled from the 28 one-function legacy
 * sources and reconciled by hand.
 *
 * THE RUN IS ONE CONTIGUOUS SEGMENT AND IS TAKEN WHOLE.  The 28 symbols.txt
 * sizes sum to exactly 0x1404 = 0x020c8a30 - 0x020c762c, every function starts
 * where the previous one ends, and every one of the 28 has both a src/ file and
 * a `complete` delinks.txt entry.  Unlike ov006/dScMgCurling2_c there is no
 * sourceless hole and so no choice of side: the text-only route licenses the
 * run whole, and the run holds the key function, D1 and C1.
 *
 * THE DESTRUCTOR IS NOT VIRTUAL, and that is what decides the key function.
 * The vtable has three slots (0x020c76d8, 0x020c76d0, 0x020c762c) and none of
 * them is 0x020c893c or a deleting stub; D1 is reached only through literal
 * pool constants handed to the array-destroy helper at arm9 0x0207328c.  So the
 * key function is the first non-inline virtual DECLARED in
 * include/dMgJump3DMario_c.h -- Unk_020c76d8 -- and this TU emits the class's
 * own _ZTV/_ZTI/_ZTS.  It emits the ABSTRACT BASE's table too: all three slots
 * of _ZTV22dMg3DHeyhoObjAdapter_c at 0x0213afd8 are zero, so
 * dMg3DHeyhoObjAdapter_c has no key function of its own, and D1 restores that
 * vptr at 0x020c89c0 from inside the licensed run.  Ten compiler-only rows in
 * all -- six data, four homeless text -- where the queue row estimated a floor
 * of five.  The base's own vtable is the row the floor cannot see.
 *
 * FUNCTION ORDER IS THE ROM'S OWN, LOWEST ADDRESS FIRST, and that is one
 * decision with `#pragma defer_codegen off` below.  With codegen deferred (the
 * default) mwccarm 2004/b56 emits one .text section per function in the REVERSE
 * of source order and expands the out-of-line destructor as D2, D0, D1;
 * generating at parse time emits source order and D1, D0, D2.  Both reach a
 * ROM-ascending licensed order here, but from opposite source layouts, and
 * `linkcheck [4b/8]` refuses the mixture.  Do not reorder.
 *
 * include/dMgJump3DMario_c.h NEEDED NO LAYOUT EDIT.  Its existing out-of-line
 * destructor declaration is what this TU wants, and the five shards that
 * already included it (the three virtuals, C1 and D1) already reproduced the
 * cartridge's bytes against the declared layout.  The header edit that DID
 * happen is the fifteen method declarations of the conversion pass below; it
 * adds no virtual and no field, and it was proven byte-neutral by compiling
 * src/minigames/d_s_mg_jump2.cpp, src/minigames/d_s_mg_trampoline2.cpp,
 * src/_ZN22dMg3DHeyhoObjAdapter_cC2Ev.cpp and all five of this class's shards
 * before and after and comparing the object sha256s -- all eight identical.
 *
 * decl_common.h is NOT included, following the ov006/dScMgPanel_c and
 * ov006/dScMgCurling2_c precedent.  It declares only two of these members
 * (func_ov006_020c76e0 and func_ov006_020c7860, both `void(char *)`, both
 * agreeing), but four of the DATA symbols this file reads collide with it:
 * data_ov006_02141a44 (`int` against this file's `void*`), data_ov006_02141a40
 * (`void*` against `char*`), data_ov006_02140428 (`int` against `int[]`) and
 * data_ov006_0214042c.  Including it would make each an error against code that
 * already matches.  No header under include/ pulls decl_common.h in, so
 * excluding it is enough.
 *
 * EVERY MEMBER KEEPS ITS OWN `extern` DECLARATIONS INSIDE ITS OWN BODY, and
 * that is deliberate, not untidiness.  Twenty-eight independent recoveries of
 * one symbol disagree constantly -- data_ov006_0214041c is spelled `int`,
 * `void*[]` and `void*` by three different members, data_ov006_0213b098 is
 * `void*[]` in one and `char*[]` in another -- and hoisting one canonical
 * spelling is a measured codegen hazard (see ov002/Player).  Block-scope data
 * declarations keep every spelling independent, and mwccarm leaves a
 * file-scope variable's name unmangled in C++, so they still name the ROM
 * symbol from inside a real method.
 *
 * ONE FILE-SCOPE `extern "C"` REGION carries what the METHODS call, and it has
 * to be file-scope: a class member function may not sit in a linkage
 * specification at all, and mwccarm 2004/b56 rejects a block-scope one outright
 * (`declarator expected`).  A declaration written at block scope inside a
 * method would get C++ linkage and reference a mangled name nothing defines --
 * a LINK failure that every byte gate here passes.  Only symbols reached from
 * at least one method are in that region; the eight surviving `func_ov006_*`
 * members are each wrapped in their own `extern "C" { }` and keep theirs at
 * block scope, where they inherit C linkage from the enclosing region.
 *
 * THREE SHADOW SPELLINGS ARE LOAD-BEARING AND WERE MEASURED.
 *   - `struct C` is incomplete when `typedef void (C::*PMF)()` is formed and
 *     complete afterwards, exactly as the shard for 0x020c7860 had it.  mwccarm
 *     picks its pointer-to-member representation from the completeness of the
 *     class at the point the pointer-to-member TYPE is declared.
 *   - The 8-byte records copied into +0x3c are spelled with an ARRAY member
 *     (`PmfRecord`, below), never two named ints.  The one shard that used
 *     `{ int a, b; }` -- func_ov006_020c81e0 -- was this file's single DIFF: C
 *     block-moved the record, C++ scalarised it and scheduled the load after
 *     the store, two words at +0x64/+0x68 with no size change.  Spelling it as
 *     an array restored 28/28.
 *   - func_ov006_020c7ba4 keeps its own `typedef struct { int x, y, z; } Vec3`
 *     for its local instead of the real Vector3.  types.h's Vector3 declares an
 *     empty destructor, so an object of it would odr-use `_ZN7Vector3D1Ev` and
 *     add an eleventh compiler-only row for nothing.
 *
 * STAGE 3 (humanizer) MEASURED EVERY CLEANUP, AND FIVE OF THEM COST BYTES.
 * Each survivor is commented at its own site; do not delete one on sight.  What
 * was tried, against `tubuild.py verify ov006/dMgJump3DMario_c`:
 *
 *   REMOVED, byte-neutral -- four `(int)` pointer launders (`(Pair *)((int)c +
 *   0x3c)` and kin, in Unk_020c762c / StateDamp / EnterHit / StateWindUp); the
 *   `int zero = 0` hoist in StateWindUp; the `int z = 0` hoist in
 *   func_ov006_020c7734; the `(&data_ov006_02140428)[0]` subscript in
 *   StateDamp; `t != false` -> `t != 0` in func_ov006_020c87d0; the
 *   declare-then-assign pair in EnterHit.  Eight `{ int w[2]; }` shadow structs
 *   under eight different names -- G, S, V2, W2, S60, S50, S48, G38 -- plus
 *   EnterDamp's lone `double` spelling of the same eight bytes, folded into one
 *   `PmfRecord`; all nine +0x3c installs now share it.
 *
 *   PUT BACK, load-bearing -- zeroing `Mtx m` through `int *` rather than
 *   m.a..m.d (11 words, func_ov006_020c7734); reusing `flag` as the scratch in
 *   StateHold (5 words); the `*(volatile int *)&v[2] = 0` stack demotion in
 *   StateMove (rewrites the whole 0x3c4-byte member); the same-on-both-arms
 *   select in EnterHit (4 words); the `int t` temporary in func_ov006_020c87d0
 *   (rewrites the whole 0x16c-byte function, 13 relocation destinations wrong).
 *
 *   WHAT IS MEASURED AND WHAT IS NOT.  Every word count and every
 *   whole-function verdict above was produced by removing that one spelling and
 *   re-running `tubuild.py verify`.  The sentences at the five sites that say
 *   WHY mwccarm behaves that way -- register spills, stack demotion, operand
 *   evaluation order -- are readings of the resulting diff, not separate
 *   measurements.  Trust the counts; treat the mechanisms as hypotheses.
 *
 * METHOD-CONVERSION PASS: 20 OF THE 28 MEMBERS ARE REAL dMgJump3DMario_c::
 * METHODS.  The five that already carried mangled names (three virtuals, D1,
 * C1) plus fifteen converted here.  It cost nothing: 28/28 MATCH before and
 * after, objisolate clean, emission order still ROM-ascending.
 *
 * THE EIGHT THAT STAYED FREE FUNCTIONS, and the measured reason.  Each is
 * called by a direct `bl` from a shard BELOW this run that is still a plain
 * `.c` file, and a C shard cannot name a C++ member -- renaming the definition
 * without editing the caller is an undefined symbol.  Twelve call sites, found
 * twice over (config/arm9/overlays/ov006/relocs.txt filtered on
 * `module:overlay(6)`, and a raw ARM `BL` decode of the overlay image), both
 * scans agreeing exactly:
 *
 *   func_ov006_020c76e0  4 sites  func_ov006_020c7388 / _020c7418 /
 *                                 _020c7490 / _020c7574
 *   func_ov006_020c7734  1 site   func_ov006_020c70d0
 *   func_ov006_020c7860  1 site   func_ov006_020c712c
 *   func_ov006_020c8084  1 site   func_ov006_020c7388
 *   func_ov006_020c81e0  1 site   func_ov006_020c7418
 *   func_ov006_020c862c  1 site   func_ov006_020c7490
 *   func_ov006_020c8658  2 sites  func_ov006_020c719c / _020c7574
 *   func_ov006_020c87d0  1 site   func_ov006_020c7574
 *
 * They convert the day those seven shards are promoted, not before.  None of
 * the eight calls a converted member, so the split needed no casts.
 *
 * THE RENAME WAS ONE EDIT WITH THIS FILE, and on this class it is not optional
 * bookkeeping.  Fifteen 8-byte pointer-to-member records sit in ov006's UNOWNED
 * .data at 0x0213b020..0x0213b098 -- {code pointer, 0}, fifteen separate
 * constants rather than an array, each loaded from its own literal site.  dsd
 * delinks them straight from the cartridge and resolves each code word by the
 * NAME symbols.txt gives at the target address, so the nine records whose
 * targets were converted needed their symbols.txt rows moved to the mangled
 * spelling in the same commit; fifteen rows moved in all.  Leave one behind and
 * its .data word links as 0x00000000 with a green compile.  There is no sinit
 * for this run -- no entry in ov006's 31-entry .ctor table at
 * 0x0213356c..0x021335ec points inside it -- so the records are baked into
 * initialised .data and there is no static-initialiser order to simulate.
 *
 * THE FIFTEEN NEW NAMES ARE COINED.  include/dMgJump3DMario_c.h carries the
 * derivation and says what the cartridge proves and what was chosen.  The three
 * virtual names stay address-derived; nothing in the ROM carries their 2004
 * spelling, and the fields below 0x4c are still unnamed for the same reason.
 *
 * Promoted members in this TU (ROM address order):
 *   [0]  0x020c762c  dMgJump3DMario_c::Unk_020c762c   (vtable slot 2)
 *   [1]  0x020c76d0  dMgJump3DMario_c::Unk_020c76d0   (vtable slot 1)
 *   [2]  0x020c76d8  dMgJump3DMario_c::Unk_020c76d8   (vtable slot 0, KEY FN)
 *   [3]  0x020c76e0  func_ov006_020c76e0
 *   [4]  0x020c7734  func_ov006_020c7734
 *   [5]  0x020c7860  func_ov006_020c7860
 *   [6]  0x020c78ec  dMgJump3DMario_c::StateDamp
 *   [7]  0x020c79a8  dMgJump3DMario_c::EnterDamp
 *   [8]  0x020c7a30  dMgJump3DMario_c::StateHold
 *   [9]  0x020c7ba4  dMgJump3DMario_c::EnterHold
 *   [10] 0x020c7c68  dMgJump3DMario_c::StateMove
 *   [11] 0x020c802c  dMgJump3DMario_c::EnterMove
 *   [12] 0x020c8048  dMgJump3DMario_c::StateFallOut
 *   [13] 0x020c8084  func_ov006_020c8084
 *   [14] 0x020c814c  dMgJump3DMario_c::StateRiseOut
 *   [15] 0x020c81e0  func_ov006_020c81e0
 *   [16] 0x020c8270  dMgJump3DMario_c::EnterHit
 *   [17] 0x020c833c  dMgJump3DMario_c::StateBounce
 *   [18] 0x020c85a0  dMgJump3DMario_c::EnterBounce
 *   [19] 0x020c85bc  dMgJump3DMario_c::StateWindUp
 *   [20] 0x020c862c  func_ov006_020c862c
 *   [21] 0x020c864c  dMgJump3DMario_c::StateIdle
 *   [22] 0x020c8658  func_ov006_020c8658
 *   [23] 0x020c8680  dMgJump3DMario_c::StateRespawn
 *   [24] 0x020c8768  dMgJump3DMario_c::EnterRespawn
 *   [25] 0x020c87d0  func_ov006_020c87d0
 *   [26] 0x020c893c  dMgJump3DMario_c::~dMgJump3DMario_c   (D1)
 *   [27] 0x020c8a04  dMgJump3DMario_c::dMgJump3DMario_c    (C1)
 *
 * The gameplay meaning of the fields is NOT settled and no comment here claims
 * one.  What is measured: +0x04, +0x14 and +0x20 are three-component vectors
 * (vtable slots 0 and 1 return the addresses of the last two), +0x10 is the
 * s16 the base constructor zeroes, +0x2e is an angle, +0x32 a countdown, +0x3c
 * the pointer-to-member this file's State/Enter pairs read and write, and
 * +0x4c a ModelAnim of 0x64 bytes.  Nothing in the run touches 0xb0..0xb8.
 */

#pragma defer_codegen off

#include "types.h"
#include "common.h"
#include "dMgJump3DMario_c.h"
#include "SharedFilePtr.h"

/* ---------------------------------------------------------------------------
 * Shadow types.  func_ov006_020c7860 dispatches through the object's +0x3c
 * pointer-to-member.  `struct C` is INCOMPLETE when PMF is formed and complete
 * afterwards, exactly as the shard had it -- mwccarm 2004/b56 picks the
 * pointer-to-member representation from the completeness of the class at the
 * point the pointer-to-member TYPE is declared, so this order is load-bearing.
 * ------------------------------------------------------------------------- */
struct C;
typedef void (C::*PMF)();
struct C { char pad[0x3c]; PMF m; };

/* The 8-byte records at 0x0213b020..0x0213b098 that the Enter* members install
 * at +0x3c.  What is measured is the SHAPE -- {code pointer, adjustment}, the
 * mwccarm pointer-to-member representation -- not any gameplay meaning, so the
 * type is named for the shape.
 *
 * THE ARRAY MEMBER IS LOAD-BEARING and this is the one type all nine installs
 * share.  Spelled `{ int a, b; }` instead, C++ scalarises the copy and
 * schedules the load after the store: two words at +0x64/+0x68 in
 * func_ov006_020c81e0, which was this file's single DIFF before the array
 * spelling restored 28/28.  `Pair` below is the SAME eight bytes read as two
 * named ints, which is what the two comparison sites want; the split is
 * deliberate, and the two must not be merged. */
struct PmfRecord { int w[2]; };
struct Pair { int a, b; };

/* ---------------------------------------------------------------------------
 * ONE file-scope `extern "C"` region.
 * ------------------------------------------------------------------------- */
extern "C" {

void func_ov006_020c8658(void *c);

void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *file, int a, int b, unsigned int d);
void _ZN5Sound12PlayBank2_2DEj(unsigned int id);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
int  _ZNK9Animation12WillHitFrameEi(void *anim, int frame);
void _Z14ApproachLinearRiii(int &v, int target, int step);
int  _Z15ApproachLinear2Rsss(short *v, short target, short step);
void _Z11UpdateAngleRssis(short *a, int b, int c, int d);
void Sound_PlayBank1Panned(int a, int b, int c);
void Vec3_Sub(void *out, void *a, void *b);
int  NormalizeVec3IfNonZero(void *v);
int  RandomIntInternal(int *seed);
void func_02012718(int a, int b);
void func_ov006_020e6e3c(int a, int b);
void func_ov006_020c8c78(int a, int b);

}  /* extern "C" */


/* [0] 0x020c762c */
// @symbol _ZN16dMgJump3DMario_c12Unk_020c762cEv
int dMgJump3DMario_c::Unk_020c762c()
{
    extern Pair data_ov006_0213b058;
    extern Pair data_ov006_0213b070;

    Pair *p;
    Pair *g;
    int r = 0;
    int m = 1;
    char *c = (char *)this;

    p = (Pair *)(c + 0x3c);
    g = &data_ov006_0213b058;
    if (p->a == g->a) {
        if (p->b == g->b)
            goto cl1;
        if (*(int *)(c + 0x3c) == 0) {
cl1:
            m = 0;
        }
    }
    if (m != 0) {
        m = 1;
        p = (Pair *)(c + 0x3c);
        g = &data_ov006_0213b070;
        if (p->a == g->a) {
            if (p->b == g->b)
                goto cl2;
            if (*(int *)(c + 0x3c) == 0) {
cl2:
                m = 0;
            }
        }
        if (m != 0)
            r = 1;
    }
    return r;
}


/* [1] 0x020c76d0 */
// @symbol _ZN16dMgJump3DMario_c12Unk_020c76d0Ev
void *dMgJump3DMario_c::Unk_020c76d0()
{
    return (char *)this + 0x20;
}


/* [2] 0x020c76d8 -- KEY FUNCTION */
// @symbol _ZN16dMgJump3DMario_c12Unk_020c76d8Ev
void *dMgJump3DMario_c::Unk_020c76d8()
{
    return (char *)this + 0x14;
}


/* [3] 0x020c76e0 */
extern "C" {
// @symbol func_ov006_020c76e0
void func_ov006_020c76e0(char *c)
{
    extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
    extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, short angY);
    extern struct Matrix4x3 data_020a0e68;

    Matrix4x3_FromTranslation(&data_020a0e68,
        *(int *)(c + 0x14), *(int *)(c + 0x18), *(int *)(c + 0x1c));
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short *)(c + 0x2e));
    *(struct Matrix4x3 *)(c + 0x68) = data_020a0e68;
}
}


/* [4] 0x020c7734 */
struct OamAttr;
struct Matrix2x2;

struct Mtx { int a, b, c, d; };

struct VBase {
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
    virtual void m4();
    virtual void m5(void *arg);
};

extern "C" {
// @symbol func_ov006_020c7734
void func_ov006_020c7734(char *c)
{
    extern void func_ov006_020bfec0(void *a, void *b, short *c);
    extern int func_02053200(int x);
    extern void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(
        bool en, OamAttr *attr, int a, int b, int c, int d, Matrix2x2 *m);
    extern unsigned char data_ov006_02140400;
    extern void *data_ov006_02141a44;
    extern unsigned short data_ov006_02140404;
    extern short data_02082214[];
    extern OamAttr *data_ov006_02134d1c;
    extern char data_ov006_0212ddd0;

    short v[2];
    int r1res;
    int r2res;
    int g;
    int t;
    Mtx m;

    if (*(unsigned char *)(c + 0x35) == 0)
        return;

    if (data_ov006_02140400 != 0) {
        func_ov006_020bfec0(data_ov006_02141a44, c + 0x14, v);

        g = data_ov006_02140404;
        t = data_02082214[(g >> 4) * 2];
        v[1] = v[1] - (((t << 2) + 0x30000) >> 12);
        r1res = func_02053200((t >> 2) + 0x1000);

        g = data_ov006_02140404;
        r2res = -func_02053200((data_02082214[(g >> 4) * 2 + 1] >> 2) + 0x1000);

        /* Zeroed THROUGH `int *`, not through m.a..m.d.  MEASURED: writing the
           named members instead costs 11 words.  Why, is a reading and not a
           measurement -- the extra words look like a spill of a struct mwccarm
           had kept in registers, for the Matrix2x2 argument below -- so trust
           the 11 and not the explanation. */
        int *mp = (int *)&m;
        mp[0] = 0; mp[1] = 0; mp[2] = 0; mp[3] = 0;
        m.d = r2res;
        m.a = r1res;
        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(
            false, data_ov006_02134d1c, v[0], v[1], -1, -1, (Matrix2x2 *)&m);
    }

    ((VBase *)(c + 0x4c))->m5(&data_ov006_0212ddd0);
}
}


/* [5] 0x020c7860 */
extern "C" {
// @symbol func_ov006_020c7860
void func_ov006_020c7860(char *c)
{
    extern void AddVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
    extern void func_ov006_020bfec0(void *a, char *b, short *d);
    extern void func_ov006_020c76e0(char *c);
    extern void _ZN9Animation7AdvanceEv(void *);
    extern int data_ov006_0213b010;
    extern int data_ov006_0213b018;
    extern void *data_ov006_02141a40;
    _Z14ApproachLinearRiii(*(int *)(c + 0x24), data_ov006_0213b010, data_ov006_0213b018);
    AddVec3((struct Vector3*)(c + 0x14), (struct Vector3*)(c + 0x20), (struct Vector3*)(c + 0x14));
    {
        /* Dispatch through the object's own +0x3c state handler. */
        C *o = (C *)c;
        (o->*o->m)();
    }
    func_ov006_020bfec0(*(void **)&data_ov006_02141a40, c + 0x14, (short *)(c + 0x36));
    func_ov006_020c76e0(c);
    _ZN9Animation7AdvanceEv(c + 0x9c);
}
}


/* [6] 0x020c78ec */
// @symbol _ZN16dMgJump3DMario_c9StateDampEv
void dMgJump3DMario_c::StateDamp()
{
    extern int data_ov006_02140428;
    char *c = (char *)this;
    *(short *)(c + 0x32) -= 1;
    if (*(short *)(c + 0x32) == 0) {
        _Z14ApproachLinearRiii(data_ov006_02140428, 0, 1);
        _ZN5Sound12PlayBank2_2DEj(0x130);
        func_ov006_020c8c78(*(short *)(c + 0x36), 0xc0);
        EnterRespawn();
        return;
    }
    *(int *)(c + 0x20) =
        (int)(((s64)*(int *)(c + 0x20) * 0xc00 + 0x800) >> 12);
    if (*(int *)(c + 0x14) < -0x6c000) {
        *(int *)(c + 0x14) = -0x6c000;
        return;
    }
    if (*(int *)(c + 0x14) > 0x6c000)
        *(int *)(c + 0x14) = 0x6c000;
}


/* [7] 0x020c79a8 */
// @symbol _ZN16dMgJump3DMario_c9EnterDampEv
void dMgJump3DMario_c::EnterDamp()
{
    extern int data_ov006_02140428[];
    extern int data_ov006_0214042c[];
    extern PmfRecord data_ov006_0213b030;
    char *c = (char *)this;
    if (data_ov006_02140428[0] > 1)
        _ZN5Sound12PlayBank2_2DEj(0x1ca);
    else
        _ZN5Sound12PlayBank2_2DEj(0x1c9);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void *)(c + 0x4c), (void *)data_ov006_0214042c[0], 0, 0x800, 0);
    *(short *)(c + 0x32) = 0x28;
    *(PmfRecord *)(c + 0x3c) = data_ov006_0213b030;
}


/* [8] 0x020c7a30 */
// @symbol _ZN16dMgJump3DMario_c9StateHoldEv
void dMgJump3DMario_c::StateHold()
{
    extern u8 data_020a0e40;
    extern u8 data_020a0de8[][4];
    extern u8 data_020a0de9[][4];
    extern u8 data_020a0dea[][4];
    extern u8 data_020a0deb[][4];
    extern int data_ov006_0213b008;
    char *c = (char *)this;
    u32 idx = data_020a0e40;
    int flag = 0;

    if (data_020a0de8[idx][0] != 0) {
        if (data_020a0de9[idx][0] != 0) {
            flag = 1;
        }
    }
    if (flag != 0) {
        /* Reusing `flag` as the scratch is load-bearing, not leftover: reading
           the table straight into `b` and leaving `flag` alone costs 5 words. */
        flag = data_020a0deb[idx][0];
        int b = flag;
        int x = *(s16 *)(c + 0x38) - 0x20;
        int dz = *(s16 *)(c + 0x36) - data_020a0dea[idx][0];
        if (dz < 0) {
            dz = -dz;
        }
        if (dz < 0x18) {
            int dx = x - b;
            if (dx < 0) {
                dx = -dx;
            }
            if (dx < 0x26) {
                _Z15ApproachLinear2Rsss((short *)(c + 0x32), 0, 8);
            }
        }
    }
    {
        int v = *(int *)(c + 0x14);
        if (v < -0x6c000) {
            v = -0x6c000;
        } else if (v > 0x6c000) {
            v = 0x6c000;
        }
        *(int *)(c + 0x14) = v;
    }
    if (*(u16 *)(c + 0x10) == 1) {
        *(u16 *)(c + 0x10) = 0;
        *(int *)(c + 0x24) = data_ov006_0213b008;
        EnterHit();
    } else if (_Z15ApproachLinear2Rsss((short *)(c + 0x32), 0, 1)) {
        *(u16 *)(c + 0x10) = 0;
        if (*(int *)(c + 0x24) > 0) {
            EnterBounce();
            StateBounce();
        } else {
            EnterMove();
            StateMove();
        }
    } else {
        if (*(s16 *)(c + 0x38) < 0xbc) {
            return;
        }
        EnterDamp();
    }
}


/* [9] 0x020c7ba4 */
// @symbol _ZN16dMgJump3DMario_c9EnterHoldEv
void dMgJump3DMario_c::EnterHold()
{
    char *c = (char *)this;
    extern int data_ov006_02140408[];
    extern PmfRecord data_ov006_0213b028;
    typedef struct { int x, y, z; } Vec3;
    Vec3 v;
    Vec3_Sub(&v, (Vec3 *)(c + 0x14), (Vec3 *)(c + 4));
    if (NormalizeVec3IfNonZero(&v) != 0) {
        *(int *)(c + 0x20) = v.x;
        *(int *)(c + 0x24) = v.y;
        *(int *)(c + 0x28) = v.z;
    } else {
        *(int *)(c + 0x20) = -*(int *)(c + 0x20);
        *(int *)(c + 0x24) = -*(int *)(c + 0x24);
    }
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x4c, (void *)data_ov006_02140408[0], 0x40000000, 0x800, 0);
    *(int *)(c + 0xa4) = 0;
    Sound_PlayBank1Panned(0, 6, *(int *)(c + 0x14));
    *(short *)(c + 0x32) = 0x20;
    *(PmfRecord *)(c + 0x3c) = data_ov006_0213b028;
}


/* [10] 0x020c7c68 */
// @symbol _ZN16dMgJump3DMario_c9StateMoveEv
void dMgJump3DMario_c::StateMove()
{
    char *c = (char *)this;
    extern int data_ov006_0213b008;
    extern int data_ov006_0213b00c;
    extern int data_ov006_0213b01c;
    extern void *data_ov006_0213b098[];
    extern u8 data_020a0e40;
    extern u8 data_020a0de8[];
    extern u8 data_020a0de9[];
    extern u8 data_020a0dea[];
    extern u8 data_020a0deb[];
    u16 st = *(u16 *)(c + 0x10);

    if (st == 1) {
        *(u16 *)(c + 0x10) = 0;
        *(int *)(c + 0x24) = data_ov006_0213b008;
        if (*(int *)(c + 0x44) != 0)
            *(int *)(c + 0x44) = 0;
        else
            *(int *)(c + 0x44) = 1;
        EnterHit();
        return;
    }
    if (st == 2) {
        *(u16 *)(c + 0x10) = 0;
        EnterHold();
        return;
    }

    {
        int flag = 0;
        u8 idx = data_020a0e40;
        if (data_020a0de8[idx * 4] != 0 && data_020a0de9[idx * 4] != 0)
            flag = 1;

        if (flag != 0) {
            s16 y = *(s16 *)(c + 0x38);
            int ax = (int)data_020a0dea[idx * 4];
            int az = (int)data_020a0deb[idx * 4];
            int dx = (int)*(s16 *)(c + 0x36) - ax;
            int ym = (int)y - 0x20;
            int t = dx < 0 ? -dx : dx;

            if (t < 0x18) {
                t = ym - az;
                if (t < 0) t = -t;
                if (t < 0x26 && (int)y < 0xbc) {
                    int v[3];
                    int *p = &data_ov006_0213b00c;
                    int t0 = (ax - 0x80) << 12;
                    int t1 = (-az) << 12;
                    v[0] = t0;
                    v[1] = t1;
                    /* The volatile round-trip is load-bearing.  MEASURED:
                       writing a plain `v[2] = 0;` rewrites the whole 0x3c4-byte
                       member, with 4 relocation destinations wrong.  The stack
                       demotion of v[] and the NewSimple() argument load order
                       below are a reading of that diff, not a separate
                       measurement. */
                    *(volatile int *)&v[2] = 0;
                    *(int *)(c + 0x24) = *p;
                    *(int *)(c + 0x20) = data_ov006_0213b01c * dx;
                    if (*(int *)(c + 0x44) != 0)
                        *(int *)(c + 0x44) = 0;
                    else
                        *(int *)(c + 0x44) = 1;
                    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x4c, *(void **)data_ov006_0213b098[*(int *)(c + 0x44)], 0x40000000, 0x800, 0);
                    *(int *)(c + 0xa4) = 0;
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xed, *(int *)(c + 0x14) << 3, *(int *)(c + 0x18) << 3, *(int *)(c + 0x1c) << 3);
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xee, v[0] << 3, v[1] << 3, v[2] << 3);
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xef, v[0] << 3, v[1] << 3, v[2] << 3);
                    Sound_PlayBank1Panned(0, *(int *)(c + 0x44), *(int *)(c + 0x14));
                    func_02012718(0x1c6, *(s16 *)(c + 0x36) << 12);
                    EnterBounce();
                }
            }
        }
    }

    if (*(s16 *)(c + 0x38) > 0xbc) {
        EnterDamp();
        return;
    }

    {
        int pos = *(int *)(c + 0x14);
        if (pos < -0x6c000 && *(int *)(c + 0x20) < 0) {
            *(int *)(c + 0x20) = -(int)(((s64)*(int *)(c + 0x20) * 0xd00 + 0x800) >> 12);
            if (*(int *)(c + 0x24) > 0) {
                *(int *)(c + 0x44) = 2;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x4c, *(void **)data_ov006_0213b098[*(int *)(c + 0x44)], 0x40000000, 0x800, 0);
                *(int *)(c + 0xa4) = 0;
                Sound_PlayBank1Panned(0, *(int *)(c + 0x44), *(int *)(c + 0x14));
            }
        } else if (pos > 0x6c000 && *(int *)(c + 0x20) > 0) {
            *(int *)(c + 0x20) = -(int)(((s64)*(int *)(c + 0x20) * 0xd00 + 0x800) >> 12);
            if (*(int *)(c + 0x24) > 0) {
                *(int *)(c + 0x44) = 2;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x4c, *(void **)data_ov006_0213b098[*(int *)(c + 0x44)], 0x40000000, 0x800, 0);
                *(int *)(c + 0xa4) = 0;
                Sound_PlayBank1Panned(0, *(int *)(c + 0x44), *(int *)(c + 0x14));
            }
        }
    }

    if (*(int *)(c + 0x20) > 0) {
        _Z11UpdateAngleRssis((s16 *)(c + 0x2e), 0x2800, 2, 0x1000);
        return;
    }
    _Z11UpdateAngleRssis((s16 *)(c + 0x2e), -0x2800, 2, 0x1000);
}


/* [11] 0x020c802c */
// @symbol _ZN16dMgJump3DMario_c9EnterMoveEv
void dMgJump3DMario_c::EnterMove()
{
    extern PmfRecord data_ov006_0213b020;
    char *c = (char *)this;

    *(PmfRecord *)(c + 0x3c) = data_ov006_0213b020;
}


/* [12] 0x020c8048 */
// @symbol _ZN16dMgJump3DMario_c12StateFallOutEv
void dMgJump3DMario_c::StateFallOut()
{
    void *c = (void *)this;

    if (*(int *)((char *)c + 0x18) >= -0x120000)
        return;
    *(int *)((char *)c + 0x24) = 0;
    func_ov006_020c8658(c);
}


/* [13] 0x020c8084 */
extern "C" {
// @symbol func_ov006_020c8084
void func_ov006_020c8084(char *c)
{
    extern int data_ov006_0213b088[2];
    extern void *data_ov006_0214042c;
    extern int data_ov006_0213b090[2];
    int *p = (int *)(c + 0x3c);
    int *g = data_ov006_0213b088;
    if (p[0] == g[0] && (p[1] == g[1] || *(int *)(c + 0x3c) == 0)) {
        *(int *)(c + 0x24) = 0;
        func_ov006_020c8658(c);
    } else {
        *(int *)(c + 0x20) = 0;
        *(int *)(c + 0x24) = 0x2000;
        _ZN5Sound12PlayBank2_2DEj(0x1c9);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void *)(c + 0x4c), data_ov006_0214042c, 0, 0x800, 0);
        *(int *)(c + 0xa4) = 0;
        *(PmfRecord *)(c + 0x3c) = *(PmfRecord *)data_ov006_0213b090;
    }
}
}


/* [14] 0x020c814c */
// @symbol _ZN16dMgJump3DMario_c12StateRiseOutEv
void dMgJump3DMario_c::StateRiseOut()
{
    extern int data_ov006_0214041c;
    extern int data_ov006_02140434;
    char *c = (char *)this;
    if (*(int *)(c + 0xac) == data_ov006_0214041c)
    {
        if (_ZNK9Animation12WillHitFrameEi(c + 0x9c, 0xc) ||
            _ZNK9Animation12WillHitFrameEi(c + 0x9c, 0x18))
            func_ov006_020e6e3c(0x1b5, *(int *)(c + 0x14));
    }
    if (*(int *)(c + 0x18) <= 0x180000)
        return;
    *(int *)(c + 0x24) = 0;
    data_ov006_02140434++;
    func_ov006_020c8658(c);
}


/* [15] 0x020c81e0 */
extern "C" {
// @symbol func_ov006_020c81e0
/* An ARRAY member, where the .c shard spelled it `{ int a, b; }`.  C block-moved
   the 8-byte record; C++ scalarises two named ints and schedules the load after
   the store, which is this file's one measured 2-word DIFF (+0x64/+0x68). */
void func_ov006_020c81e0(char *c)
{
    extern int data_ov006_0213b00c[];
    extern void *data_ov006_0214041c[];
    extern int data_ov006_0213b080[];
    *(int *)(c + 0x20) = 0;
    *(int *)(c + 0x24) = data_ov006_0213b00c[0];
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x4c, data_ov006_0214041c[0], 0x40000000, 0x800, 0);
    *(int *)(c + 0xa4) = 0;
    _ZN5Sound12PlayBank2_2DEj(0x10f);
    func_02012718(0x1b5, *(short *)(c + 0x36) << 0xc);
    *(PmfRecord *)(c + 0x3c) = *(PmfRecord *)data_ov006_0213b080;
}
}


/* [16] 0x020c8270 */
// @symbol _ZN16dMgJump3DMario_c8EnterHitEv
void dMgJump3DMario_c::EnterHit()
{
    char *c = (char *)this;
    extern void *data_ov006_0214041c;
    extern Pair data_ov006_0213b068;
    extern Pair data_ov006_0213b078;
    int t0, t1;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void *)(c + 0x4c), data_ov006_0214041c, 0x40000000, 0x800, 0);
    *(int *)(c + 0xa4) = 0;
    Pair *p = (Pair *)(c + 0x3c);
    Pair *g = &data_ov006_0213b068;
    if (p->a == g->a &&
        (p->b == g->b || *(int *)(c + 0x3c) == 0)) {
        func_02012718(0x110, *(short *)(c + 0x36) << 12);
    } else {
        Sound_PlayBank1Panned(0, 4, *(int *)(c + 0x14));
    }
    func_02012718(0x1b5, *(short *)(c + 0x36) << 12);
    t0 = data_ov006_0213b078.a;
    /* Both arms of this select are the same word on purpose.  MEASURED:
       collapsing it to a plain `t1 = data_ov006_0213b078.b;` costs 4 words.
       That it is what makes mwccarm read .a before .b and keep both live is a
       reading of the diff, not a second measurement. */
    t1 = t0 ? data_ov006_0213b078.b : data_ov006_0213b078.b;
    *(int *)(c + 0x3c) = t0;
    *(int *)(c + 0x40) = t1;
}


/* [17] 0x020c833c */
// @symbol _ZN16dMgJump3DMario_c11StateBounceEv
void dMgJump3DMario_c::StateBounce()
{
    char *c = (char *)this;
    extern int data_ov006_0214041c;
    extern char *data_ov006_0213b098[];
    extern int data_ov006_0213b008;
    if (*(int *)(c + 0x24) < 0) {
        EnterMove();
        StateMove();
        return;
    }

    if (*(int *)(c + 0xac) == data_ov006_0214041c) {
        if (_ZNK9Animation12WillHitFrameEi(c + 0x9c, 0xc) != 0 ||
            _ZNK9Animation12WillHitFrameEi(c + 0x9c, 0x18) != 0)
            func_ov006_020e6e3c(0x1b5, *(int *)(c + 0x14));
    }

    if (*(int *)(c + 0x14) < -0x6c000 && *(int *)(c + 0x20) < 0) {
        *(int *)(c + 0x20) = -(int)(((s64)*(int *)(c + 0x20) * 0xd00 + 0x800) >> 12);
        if (*(int *)(c + 0x24) > 0) {
            *(int *)(c + 0x44) = 2;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x4c,
                *(void **)data_ov006_0213b098[*(int *)(c + 0x44)], 0x40000000, 0x800, 0);
            *(int *)(c + 0xa4) = 0;
            Sound_PlayBank1Panned(0, *(int *)(c + 0x44), *(int *)(c + 0x14));
        }
    } else if (*(int *)(c + 0x14) > 0x6c000 && *(int *)(c + 0x20) > 0) {
        *(int *)(c + 0x20) = -(int)(((s64)*(int *)(c + 0x20) * 0xd00 + 0x800) >> 12);
        if (*(int *)(c + 0x24) > 0) {
            *(int *)(c + 0x44) = 2;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x4c,
                *(void **)data_ov006_0213b098[*(int *)(c + 0x44)], 0x40000000, 0x800, 0);
            *(int *)(c + 0xa4) = 0;
            Sound_PlayBank1Panned(0, *(int *)(c + 0x44), *(int *)(c + 0x14));
        }
    } else {
        u16 t = *(u16 *)(c + 0x10);
        if (t == 1) {
            *(u16 *)(c + 0x10) = 0;
            *(int *)(c + 0x24) = data_ov006_0213b008;
            EnterHit();
            return;
        }
        if (t == 2) {
            *(u16 *)(c + 0x10) = 0;
            EnterHold();
            return;
        }
    }

    if (*(int *)(c + 0x20) > 0)
        _Z11UpdateAngleRssis((short *)(c + 0x2e), 0x2800, 2, 0x1000);
    else
        _Z11UpdateAngleRssis((short *)(c + 0x2e), -0x2800, 2, 0x1000);
}


/* [18] 0x020c85a0 */
// @symbol _ZN16dMgJump3DMario_c11EnterBounceEv
void dMgJump3DMario_c::EnterBounce()
{
    extern PmfRecord data_ov006_0213b060;
    char *c = (char *)this;

    *(PmfRecord *)(c + 0x3c) = data_ov006_0213b060;
}


/* [19] 0x020c85bc */
// @symbol _ZN16dMgJump3DMario_c11StateWindUpEv
void dMgJump3DMario_c::StateWindUp()
{
    char *c = (char *)this;
    extern int data_ov006_0213b00c;
    if (_Z15ApproachLinear2Rsss((s16 *)(c + 0x32), 0, 1) == 0) {
        *(int *)(c + 0x18) = 0;
        return;
    }
    *(unsigned char *)(c + 0x35) = 1;
    *(int *)(c + 0x18) = 0;
    int *base = (int *)(c + 0x20);
    *base = *base << 1;
    *(int *)(c + 0x24) = data_ov006_0213b00c;
    *(short *)(c + 0x10) = 0;
    EnterHit();
}


/* [20] 0x020c862c */
extern "C" {
// @symbol func_ov006_020c862c
void func_ov006_020c862c(int *c, int v)
{
    extern PmfRecord data_ov006_0213b050;

    *(short *)((char *)c + 0x32) = (short)v;
    *(PmfRecord *)((char *)c + 0x3c) = data_ov006_0213b050;
}
}


/* [21] 0x020c864c */
// @symbol _ZN16dMgJump3DMario_c9StateIdleEv
void dMgJump3DMario_c::StateIdle()
{
    int *p = (int *)this;
    p[6] = 0;
}


/* [22] 0x020c8658 */
extern "C" {
// @symbol func_ov006_020c8658
void func_ov006_020c8658(void *c)
{
    extern PmfRecord data_ov006_0213b048;

    *(char *)((char *)c + 0x35) = 0;
    *(int *)((char *)c + 0x18) = 0;
    *(PmfRecord *)((char *)c + 0x3c) = data_ov006_0213b048;
}
}


/* [23] 0x020c8680 */
// @symbol _ZN16dMgJump3DMario_c12StateRespawnEv
void dMgJump3DMario_c::StateRespawn()
{
    char *c = (char *)this;
    extern int data_0209e650;
    extern int data_ov006_0213b01c;
    extern void *data_ov006_02140424;
    unsigned int r;
    int r5;

    *(short *)(c + 0x32) -= 1;
    if (*(short *)(c + 0x32) == 0) {
        *(int *)(c + 0x18) = 0x100000;
        r = ((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13;
        *(int *)(c + 0x14) = ((int)r - 0x800) * 0xc0;
        *(int *)(c + 0x24) = 0;
        r5 = data_ov006_0213b01c;
        r = ((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13;
        *(int *)(c + 0x20) = (int)(((s64)(((int)r - 0x800) << 1) * r5 + 0x800) >> 12);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x4c, data_ov006_02140424, 0x40000000, 0x800, 0);
        EnterMove();
        return;
    }
    *(int *)(c + 0x18) = 0x100000;
    *(int *)(c + 0x24) = 0;
}


/* [24] 0x020c8768 */
// @symbol _ZN16dMgJump3DMario_c12EnterRespawnEv
void dMgJump3DMario_c::EnterRespawn()
{
    char *c = (char *)this;
    extern int data_ov006_02140424[];
    extern PmfRecord data_ov006_0213b038;
    *(short *)(c + 0x32) = 0x3c;
    *(int *)(c + 0x18) = 0x100000;
    *(int *)(c + 0x24) = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x4c, (void *)data_ov006_02140424[0], 0x40000000, 0x800, 0);
    *(PmfRecord *)(c + 0x3c) = data_ov006_0213b038;
}


/* [25] 0x020c87d0 */
extern "C" {
// @symbol func_ov006_020c87d0
int func_ov006_020c87d0(char *c)
{
    extern SharedFilePtr data_ov006_02140450;
    extern SharedFilePtr data_ov006_02140460;
    extern SharedFilePtr data_ov006_02140468;
    extern SharedFilePtr data_ov006_02140458;
    extern SharedFilePtr data_ov006_02140438;
    extern SharedFilePtr data_ov006_02140440;
    extern SharedFilePtr data_ov006_02140448;

    extern void *data_ov006_02140430;
    extern void *data_ov006_0214040c;
    extern void *data_ov006_0214041c;
    extern void *data_ov006_02140424;
    extern void *data_ov006_02140408;
    extern void *data_ov006_0214042c;

    extern char *data_ov006_02141a40;
    extern void *data_0209f5c0;

    extern int func_020179b4(SharedFilePtr *f, void *model, int a);
    extern void *_ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr *f);
    extern void func_ov006_020bfec0(char *p, void *q, short *s);
    extern void func_02016a14(void *self, int a);
    extern void func_02016a04(void *self, int a);

    /* `t` is load-bearing, not a leftover: folding the comparison into the
       `if` below rewrites the whole 0x16c-byte function. */
    int t;

    if (func_020179b4(&data_ov006_02140450, c + 0x4c, 1) == 0)
        return 0;

    data_ov006_02140430 = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02140460);
    data_ov006_0214040c = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02140468);
    data_ov006_0214041c = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02140458);
    data_ov006_02140424 = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02140438);
    data_ov006_02140408 = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02140440);
    data_ov006_0214042c = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02140448);

    *(int *)(c + 0x44) = 0;
    if (data_ov006_02141a40 != 0)
        func_ov006_020bfec0(data_ov006_02141a40, c + 0x14, (short *)(c + 0x36));

    t = *(u16 *)((char *)data_0209f5c0 + 0xc) == 0x175;
    if (t != 0) {
        func_02016a14(c + 0x4c, 0x7fff);
        func_02016a04(c + 0x4c, 0x210);
    }
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        c + 0x4c, data_ov006_02140430, 0x40000000, 0x800, 0);

    func_ov006_020c8658(c);
    return 1;
}
}


/* [26] 0x020c893c */
// @symbol _ZN16dMgJump3DMario_cD1Ev
dMgJump3DMario_c::~dMgJump3DMario_c()
{
    extern SharedFilePtr data_ov006_02140450;
    extern SharedFilePtr data_ov006_02140460;
    extern SharedFilePtr data_ov006_02140468;
    extern SharedFilePtr data_ov006_02140458;
    extern SharedFilePtr data_ov006_02140438;
    extern SharedFilePtr data_ov006_02140440;
    extern SharedFilePtr data_ov006_02140448;
    extern int data_ov006_02140430;
    extern int data_ov006_0214040c;
    extern int data_ov006_02140424;
    extern int data_ov006_02140408;
    extern int data_ov006_0214042c;

    data_ov006_02140450.Release();
    data_ov006_02140460.Release();
    data_ov006_02140468.Release();
    data_ov006_02140458.Release();
    data_ov006_02140438.Release();
    data_ov006_02140440.Release();
    data_ov006_02140448.Release();
    data_ov006_02140430 = 0;
    data_ov006_0214040c = 0;
    data_ov006_02140424 = 0;
    data_ov006_02140408 = 0;
    data_ov006_0214042c = 0;
}


/* [27] 0x020c8a04 */
// @symbol _ZN16dMgJump3DMario_cC1Ev
dMgJump3DMario_c::dMgJump3DMario_c()
{
}
