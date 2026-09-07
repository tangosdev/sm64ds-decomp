//cpp
/* Genuine production translation unit for ov030/daMky_c.
 *
 * The cartridge names this class daMky_c: ov030 0x02115b78 holds the bytes
 * '7daMky_c' NUL-terminated, the length-prefixed _ZTS payload that
 * _ZTI7daMky_c (0x02115b84) points at.  The tree used to spell it Ukiki; that
 * word appears nowhere in the ROM.  This TU, include/daMky_c.h and the nine
 * mangled member rows in config/arm9/overlays/ov030/symbols.txt were renamed
 * to the cartridge spelling in the commit before this one.
 *
 * THE SOURCE IS WRITTEN ROM-ASCENDING and the file opens with
 * `#pragma defer_codegen off`.  The two go together: with codegen deferred
 * (the default) mwccarm lays .text down in reverse source order and an
 * out-of-line destructor emits D2, D0, D1; with it off the file emits in
 * source order and the destructor emits D1, D0, D2.  The cartridge puts
 * ~daMky_c's D1 at 0x02111688 below D0 at 0x021116d0, so this is the form
 * that reproduces it.  D2 has no ROM home and is deadstripped -- it is the
 * homeless row in the manifest's compiler_only_output.
 *
 * ~daMky_c is DECLARED FIRST and defined out of line, so it is this class's
 * key function and this TU owns the emission of _ZTV7daMky_c, _ZTI7daMky_c
 * and _ZTS7daMky_c together with the inherited dActor_c / dBase_c / fBase_c
 * RTTI records.  All of those are configured in ov030 or arm9, so they
 * license as deadstrip-data / deadstrip-duplicate rather than blocking the
 * promotion.
 *
 * WHAT IS IN HERE.  44 members, counted from the file itself: 19 definitions
 * carrying 20 mangled daMky_c:: symbols -- the destructor definition carries
 * both D1 and D0 -- and 24 func_ov030_* free functions.  There are 26
 * file-scope extern "C" regions: one wrapping each of the 24 free functions,
 * plus the two shared declaration regions described below.
 *
 * Nine of the 20 methods were already C++-named when the shards were gathered:
 * the whole virtual interface, ~daMky_c and the seven overrides.  The other
 * eleven are EnterState0..EnterState10, converted from func_ov030_* free
 * functions by stage 3b; the block above the first of them says what the ROM
 * proves about each and what was coined.
 *
 * DECLARATIONS ARE NOT MERGED.  These members were recovered independently as
 * one-function files and they disagree about the spelling of shared ABI
 * symbols -- func_ov030_021141a8 alone was declared four different ways.  Each
 * of the 24 free functions therefore keeps its own declarations INSIDE ITS OWN
 * BODY, where an extern in an extern "C" region still gets C linkage, and
 * nothing is hoisted that such a body can hold.
 *
 * The two shared regions exist only because a class member function may not
 * sit inside a linkage specification at all, so a C++-named member cannot
 * carry its own C-linkage declarations.  The first sits at the top of the
 * file, above the eleven EnterState* members, and necessarily in view of every
 * free function below; the second sits below the last free function
 * (func_ov030_021141a8, ROM ordinal 37), in view only of the six C++-named
 * members after it.  Each has its own comment explaining what it holds and
 * what it cost.
 *
 * decl_common.h is NOT included, and that is a measurement rather than a
 * preference.  It declares four of the data objects these members reach --
 * data_ov030_02115cf0, _02114824, _02115d00 and data_02099368 -- with types
 * the byte-matched shards contradict (`void *[]` against `int []`), and a
 * block-scope redeclaration of a data symbol a project header already
 * declares is rejected outright by mwccarm 2004/b56.  Canonicalising the data
 * spelling instead is the codegen hazard measured on ov002/Player.  So the
 * header is excluded, the ov006/dScMgPanel_c way, and the one declaration it
 * was supplying that no member carried (data_ov030_02115ddc) is written out
 * where it is used.
 */
#pragma defer_codegen off

#include "common.h"
#include "types.h"
#include "daMky_c.h"
#include "decl_PathPtr.h"
#include "decl_SaveData.h"
#include "dBgCh_Gnd.h"
#include "dBgCh_Actr.h"
#include "private/mtx43.h"

/* ==========================================================================
 * The eleven state-entry members.
 *
 * Eleven of this class's members are named EnterState0..EnterState10 below.
 * The class part of each name is the cartridge's (_ZTS7daMky_c); the number is
 * read from the ROM; the word "EnterState" is COINED and describes the role
 * the matched bodies play.  What the ROM proves, separately from what was
 * chosen:
 *
 *   - Member-ness.  All eleven are among the 22 pointer-to-member records at
 *     0x02115ac8 (relocation stride 8, 0x02115ac8..0x02115b70, ending exactly
 *     where _ZTS7daMky_c begins).  A pointer-to-member record is only formed
 *     for a member function.
 *
 *   - The number.  Each writes one immediate to the state word at +0x3b4, and
 *     across all 44 functions in the run the eleven immediates are 0,1,2,...,10
 *     with no value written twice and none missing.  0x02115e0c holds exactly
 *     11 descriptors of 0x10 bytes.  Eleven handlers, eleven descriptors,
 *     eleven consecutive ids: the map is a bijection, not a guess.
 *
 *   - The role.  func_ov030_021141a8(self, idx) stores &data_ov030_02115e0c[idx]
 *     at +0x3a4 and immediately calls func_ov030_02114170, which invokes pp[0]
 *     -- the descriptor's FIRST pointer-to-member.  (func_ov030_02114134 is the
 *     same shape on pp[1].)  021141a8 itself never touches +0x3b4, so after
 *     entering state N the only thing that can have written N there is the
 *     pp[0] it just called.  The body writing N is therefore descriptor N's
 *     pp[0]: its entry handler.
 *
 *   - Corroboration.  Exactly 11 of the 22 records write a state id and exactly
 *     11 do not, which is the 11 x (entry, tick) split the descriptor size
 *     predicts.  The ARRAY ORDER does not pair them (2i, 2i+1) -- records 4 and
 *     5 are both entry handlers -- so which tick belongs to which entry is
 *     still unproven and no name here asserts it.
 *
 * The remaining 11 pointer-to-member records keep their func_ov030_* names:
 * their member-ness is equally proven, but nothing in the ROM names them or
 * numbers them, and a coined name with no evidence behind it is worth less
 * than the address.  That is the naming wall, and it is where this pass stops.
 * ========================================================================== */

/* ==========================================================================
 * File-scope declarations for the eleven state-entry members.
 *
 * The eleven daMky_c::EnterState* members below are C++-named, and mwccarm
 * 2004/b56 will not accept a linkage specification inside a function body, so
 * a block-scope `extern` written in one of them would get C++ linkage and the
 * already-mangled reference would mangle a second time.  Their declarations
 * therefore have to live at file scope, and above the first of them -- which
 * is near the top of the file, because the members are laid out ROM-ascending.
 *
 * That makes these visible to every wrapped func_ov030_* member below, several
 * of which recovered the same symbol with a different spelling at block scope
 * (Vec3_Dist as (void *, void *), _ZN5dCc_c5ClearEv returning void, SetAnim
 * with an int second parameter).  mwccarm accepts the disagreement: a
 * block-scope redeclaration inherits the C linkage of the visible file-scope
 * one, and with -gccext,on the differing spellings do not collide.  This is
 * measured, not assumed -- the TU still byte-matches 44/44 with this region in
 * place, and adding it alone (converting nothing) was run as its own step.
 *
 * One spelling is kept per symbol and the converted call sites are cast to it.
 * The disagreements were SetAnim's second parameter (void * in four members,
 * BCA_File * in one, plain int in one), Vec3_Dist's cv-qualifiers, and
 * dCc_c::Clear's return type (int in one member, void in another).  Every
 * spelling passes the same thing in the same register, so no cast costs a
 * byte.
 *
 * DATA objects are deliberately NOT hoisted here.  mwccarm accepts two
 * disagreeing function declarations for one symbol but rejects two disagreeing
 * data declarations outright -- measured: hoisting data_ov030_02115ce0,
 * _02115d08, _02115d18 and data_02082214 into this region produced
 * `identifier 'data_ov030_02115d18' redeclared; was declared as 'int[]', now
 * declared as 'void *[]'`, the same wall that keeps decl_common.h out of this
 * TU.  They do not need to be here: a global-scope variable is not mangled
 * under the Itanium C++ ABI, so an `extern int data_ov030_02115ce0[];` written
 * at block scope inside a C++-named member still resolves to the plain symbol
 * name.  Every member therefore keeps its own data declarations, in its own
 * spelling, exactly where it recovered them.
 * ========================================================================== */
extern "C" {

void  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *file, int idx, int speed, unsigned int flags);
char *_ZN8dActor_c13ClosestPlayerEv(void *self);
int   _ZN8SaveData16HasPlayerLostCapEv(void);
short Vec3_HorzAngle(const void *a, const void *b);
int   Vec3_Dist(const void *a, const void *b);
void  _ZN10dBgCh_Actr13SetLimMovFlagEv(void *self);
int   _ZN10dBgCh_Actr15ClearGroundFlagEv(void *self);
void  _ZN8dActor_c13SpawnSoundObjEj(void *self, unsigned int id);
int   _ZN5dCc_c5ClearEv(void *self);
void  _ZN7PathPtr6FromIDEj(void *self, unsigned int id);
int   _ZN8dActor_c17DetectRaycastClsnER7Vector3S1_b(void *self, void *a, void *out, int flag);
void  func_0201267c(int a, void *b);

}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN7daMky_cD1Ev + _ZN7daMky_cD0Ev, 0x02111688 / 0x021116d0, size 0x48 / 0x5c */
/* -------------------------------------------------------------------------- */
/* ~daMky_c is the first virtual DECLARED in include/daMky_c.h and it is
 * defined here, out of line, so it is this class's key function: this TU owns
 * _ZTV7daMky_c, _ZTI7daMky_c and _ZTS7daMky_c.
 *
 * Under `#pragma defer_codegen off` an out-of-line destructor emits D1, then
 * D0, then D2, in that order and at this position.  The cartridge puts D1 at
 * 0x02111688 and D0 at 0x021116d0, the two lowest addresses in the run, which
 * is exactly what this produces.  D2 has no ROM home anywhere and is
 * deadstripped.
 *
 * The body is empty on purpose.  The compiler writes the whole of both
 * variants from the class definition: the vptr store, then ModelAnim,
 * ShadowModel, dCcAc_c and dBgCh_Actr destroyed in reverse declaration order
 * (PathPtr is trivial and skipped), then ~dActor_c; D0 additionally inlines
 * all of that and tail-calls Memory::Deallocate. */
// @symbol _ZN7daMky_cD1Ev
// @symbol _ZN7daMky_cD0Ev
daMky_c::~daMky_c()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN7daMky_c13OnYoshiTryEatEv, 0x0211172c, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMky_c13OnYoshiTryEatEv
/* Vtable slot 18.  Two instructions: mov r0,#7; bx lr. */
s32 daMky_c::OnYoshiTryEat()
{
    return 7;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov030_02111734, 0x02111734, size 0x15c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov030_02111734
extern "C" {
void func_ov030_02111734(char* c)
{
    /* recovered: shared common types */
    extern unsigned char DecIfAbove0_Byte(unsigned char* p);
    extern void _ZN9dBgCh_LinC1Ev(void* self);
    extern void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(void* self, void* a, void* b, void* actor);
    extern int _ZN9dBgCh_Lin10DetectClsnEv(void* self);
    extern void Vec3_Asr(void* d, void* s, int sh);
    extern int _ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(void* m, void* v, void* w, int fix, void* out);
    extern void _ZN9dBgCh_LinD1Ev(void* self);
    extern void func_ov030_02112094(void* c);

    extern char data_0209f43c;
    extern char data_0209b3ec;
    struct Vector3 a, b, out, asr;
    char rc[0x7c];

    if (DecIfAbove0_Byte((unsigned char*)(c + 0x3cb)))
        return;

    _ZN9dBgCh_LinC1Ev(rc);

    a.x = *(int*)(c + 0x5c);
    a.y = *(int*)(c + 0x60);
    a.z = *(int*)(c + 0x64);
    a.y = a.y + 0x32000;
    b.x = *(int*)(c + 0x5c);
    b.y = *(int*)(c + 0x60);
    b.z = *(int*)(c + 0x64);
    b.y = b.y - 0x96000;
    _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(rc, &a, &b, c);

    if (*(int*)(c + 0x384) - *(int*)(c + 0x60) <= 0x96000) {
        if (!_ZN9dBgCh_Lin10DetectClsnEv(rc))
            goto done;
    }

    Vec3_Asr(&asr, c + 0x380, 3);

    if (_ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(&data_0209f43c, &data_0209b3ec, &asr, 0x1f400, &out) <= 0xc350000)
        goto done;

    *(int*)(c + 0x5c) = *(int*)(c + 0x380);
    *(int*)(c + 0x60) = *(int*)(c + 0x384);
    *(int*)(c + 0x64) = *(int*)(c + 0x388);
    *(int*)(c + 0x38c) = *(int*)(c + 0x380);
    *(int*)(c + 0x390) = *(int*)(c + 0x384);
    *(int*)(c + 0x394) = *(int*)(c + 0x388);
    *(int*)(c + 0x68) = *(int*)(c + 0x380);
    *(int*)(c + 0x6c) = *(int*)(c + 0x384);
    *(int*)(c + 0x70) = *(int*)(c + 0x388);
    func_ov030_02112094(c);
    *(unsigned char*)(c + 0x3cb) = 0x96;

done:
    _ZN9dBgCh_LinD1Ev(rc);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov030_02111890, 0x02111890, size 0x78 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov030_02111890
extern "C" {
void func_ov030_02111890(char *c)
{
    extern int func_0201267c(int a, void *b);
    extern int data_ov030_02115d18[];
    int b = (int)(*(int *)(c + 0x134) == data_ov030_02115d18[1]);
    if (b == 0)
        return;
    int v = (short)((unsigned int)*(int *)(c + 0x12c) << 4 >> 16);
    if (v == 0xa || v == 0xc)
        func_0201267c(0xea, c + 0x74);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov030_02111908, 0x02111908, size 0xf8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov030_02111908
extern "C" {
void func_ov030_02111908(char* c)
{
    extern int data_ov030_02115cf0[];
    extern int data_ov030_02115cd0[];
    extern int data_ov030_02115cf8[];
    extern void func_0201267c(int a, char* b);

    enum Bool { FALSE, TRUE };
    int r3 = (short)(((unsigned)(*(int*)(c + 0x12c) << 4)) >> 16);
    int v = *(int*)(c + 0x134);
    enum Bool b;

    b = (enum Bool)(v == data_ov030_02115cf0[1]);
    if (b) {
        if (r3 != 7) {
            if (r3 != 0x28) return;
        }
        func_0201267c(0xeb, c + 0x74);
        return;
    }
    b = (enum Bool)(v == data_ov030_02115cd0[1]);
    if (b) {
        if (r3 != 1) return;
        func_0201267c(0xf1, c + 0x74);
        func_0201267c(0xe8, c + 0x74);
        return;
    }
    b = (enum Bool)(v == data_ov030_02115cf8[1]);
    if (b) {
        if (r3 != 8) return;
        func_0201267c(0xe9, c + 0x74);
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov030_02111a00, 0x02111a00, size 0x120 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov030_02111a00
extern "C" {
int func_ov030_02111a00(char* c)
{
    extern void func_ov030_02111908(void* c);
    extern int _ZNK9Animation12WillHitFrameEi(void* a, int f);
    extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* m, void* f, int a, int b, unsigned int e);
    extern int data_ov030_02115cf0[];
    extern int data_ov030_02115cd0[];
    extern int data_ov030_02115cf8[];
    extern int data_ov030_02115cd8[];
    extern void** data_ov030_02115bc8[];
    func_ov030_02111908(c);
    if (_ZNK9Animation12WillHitFrameEi(c + 0x124, 0) == 0) {
        int v = *(int*)(c + 0x134);
        int b;
        b = (int)(v == data_ov030_02115cf0[1]); if (b != 0) goto fail;
        b = (int)(v == data_ov030_02115cd0[1]); if (b != 0) goto fail;
        b = (int)(v == data_ov030_02115cf8[1]); if (b != 0) goto fail;
        b = (int)(v == data_ov030_02115cd8[1]); if (b != 0) goto fail;
    }
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        c + 0xd4, data_ov030_02115bc8[*(unsigned char*)(c + 0x3ca)][1], 0, 0x1000, 0);
    {
        unsigned char* p = (unsigned char*)(((int)c + 0x3ca));
        *(int*)(c + 0x130) = 0x1000;
        (*p)++;
    }
    if (*(unsigned char*)(c + 0x3ca) >= 0xb)
        *(unsigned char*)(c + 0x3ca) = 0;
    return 1;
fail:
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov030_02111b20, 0x02111b20, size 0xa4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov030_02111b20
extern "C" {
int func_ov030_02111b20(char* c) {
    /* recovered: shared common types, declarations from a shared header */
    /* recovered: shared common types */
    typedef short s16;

    extern void _ZNK7PathPtr7GetNodeER7Vector3j(void* self, struct Vector3* v, unsigned int i);
    extern int Vec3_HorzDist(const struct Vector3* a, const struct Vector3* b);
    extern s16 Vec3_HorzAngle(const struct Vector3* a, const struct Vector3* b);
    extern void _Z11UpdateAngleRssis(short* p, short a, int b, short c);
  struct Vector3 v;
  int d;
  s16 ang;
  int *p;
  int n;
  _ZNK7PathPtr7GetNodeER7Vector3j(c+0x398, &v, *(unsigned int*)(c+0x3a0));
  d = Vec3_HorzDist((struct Vector3*)(c+0x5c), &v);
  ang = Vec3_HorzAngle((struct Vector3*)(c+0x5c), &v);
  _Z11UpdateAngleRssis((short*)(c+0x8e), ang, 2, 0x400);
  *(s16*)(c+0x94) = *(s16*)(c+0x8e);
  if (d < *(int*)(c+0x98)) {
    n = _ZNK7PathPtr8NumNodesEv(c+0x398);
    p = (int*)(c + 0x3a0);
    n = n - 1;
    *p = *p + 1;
    if (*(int*)(c+0x3a0) >= n) return 1;
  }
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov030_02111bc4, 0x02111bc4, size 0x20c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov030_02111bc4
extern "C" {
int func_ov030_02111bc4(void *thiz)
{
    extern int func_ov030_021141a8(void *a, int b);
    extern void *_ZN8dActor_c13ClosestPlayerEv(void *a);
    extern void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
    extern int _ZN6Player7TryGrabER8dActor_c(void *p, void *a);
    unsigned char *c = (unsigned char *)thiz;
    unsigned char *r4;
    int b;

    b = (int)((*(int *)(c + 0xb0) & 0x20000) != 0);
    if (b != 0 && *(int *)(c + 0x3b4) != 2) {
        *(int *)(c + 0x3a8) = *(int *)(c + 0xd0);
        b = (int)(*(unsigned short *)(c + 0xc) == 0x10b);
        if (b != 0) {
            func_ov030_021141a8(c, 5);
        } else {
            b = (int)(*(unsigned short *)(c + 0xc) == 0x10c);
            if (b != 0)
                func_ov030_021141a8(c, 6);
        }
        return 1;
    }

    if (*(unsigned int *)(c + 0x184) == 0)
        return 0;

    if ((*(int *)(c + 0x180) & 0x40000) && *(int *)(c + 0x3b4) != 2) {
        *(void **)(c + 0x3a8) = _ZN8dActor_c13ClosestPlayerEv(c);
        *(int *)(c + 0x3b8) = *(int *)(c + 0x3b4);
        func_ov030_021141a8(c, 2);
        return 1;
    }

    r4 = (unsigned char *)_ZN8dActor_c10FindWithIDEj(*(unsigned int *)(c + 0x184));
    if (r4 == 0 || (b = (int)(*(unsigned short *)(r4 + 0xc) == 0xbf)) == 0)
        return 0;

    if (*(unsigned char *)(r4 + 0x706) != 0)
        return 0;
    if (*(unsigned char *)(r4 + 0x6f9) != 0)
        return 0;
    if (*(unsigned char *)(r4 + 0x6fb) != 0)
        return 0;
    if (*(unsigned char *)(r4 + 0x6ff) != 0)
        return 0;

    if (*(int *)(c + 0x180) & 0x1000) {
        if (_ZN6Player7TryGrabER8dActor_c(r4, c)) {
            *(void **)(c + 0x3a8) = r4;
            b = (int)(*(unsigned short *)(c + 0xc) == 0x10b);
            if (b != 0) {
                func_ov030_021141a8(c, 3);
            } else {
                b = (int)(*(unsigned short *)(c + 0xc) == 0x10c);
                if (b != 0)
                    func_ov030_021141a8(c, 4);
            }
        }
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov030_02111dd0, 0x02111dd0, size 0xd4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov030_02111dd0
extern "C" {
int func_ov030_02111dd0(char* c)
{
    extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void* thiz);
    extern int func_02038ea4(void* thiz);
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x194) != 0) {
        dBgCh_Gnd rg;
        Vector3 v;
        int y, z, x, s;
        y = *(int*)(c + 0x60);
        z = *(int*)(c + 0x64);
        x = *(int*)(c + 0x5c);
        s = y + 0x1e000;
        v.x = x;
        v.y = s;
        v.z = z;
        rg.SetObjAndPos(v, (dActor_c*)c);
        if (func_02038ea4(&rg) == 0 || *(int*)(c + 0x60) - rg.clsnY > 0x2000) {
            *(int*)(c + 0x5c) = *(int*)(c + 0x38c);
            *(int*)(c + 0x60) = *(int*)(c + 0x390);
            *(int*)(c + 0x64) = *(int*)(c + 0x394);
            return 1;
        }
        *(int*)(c + 0x38c) = *(int*)(c + 0x5c);
        *(int*)(c + 0x390) = *(int*)(c + 0x60);
        *(int*)(c + 0x394) = *(int*)(c + 0x64);
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov030_02111ea4, 0x02111ea4, size 0xc8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov030_02111ea4
extern "C" {
int func_ov030_02111ea4(char* thiz)
{
    /* recovered: shared common types */

    extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void*);

    #define ABS(x) ((x) < 0 ? -(x) : (x))
    char* c = thiz;
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x194) != 0) {
        dBgCh_Gnd rg;
        Vector3 pos;
        {
            int y = *(int*)(c + 0x60);
            int z = *(int*)(c + 0x64);
            int x = *(int*)(c + 0x5c);
            int y2 = y + 0x1e000;
            pos.x = x;
            pos.y = y2;
            pos.z = z;
        }
        rg.SetObjAndPos(pos, (dActor_c*)c);
        if (rg.DetectClsn() == 0 ||
            ABS(rg.clsnY - *(int*)(c + 0x60)) > 0x1000) {
            *(int*)(c + 0x98) = 0;
            *(int*)(c + 0x5c) = *(int*)(c + 0x68);
            *(int*)(c + 0x60) = *(int*)(c + 0x6c);
            *(int*)(c + 0x64) = *(int*)(c + 0x70);
            return 1;
        }
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov030_02111f6c, 0x02111f6c, size 0x128 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov030_02111f6c
extern "C" {
void func_ov030_02111f6c(char* c, dBgCh_Actr* w){
    typedef struct { int x, y, z; } Vector3;
    typedef struct dBgCh_Actr dBgCh_Actr;
    typedef struct SurfaceInfo SurfaceInfo;
    void func_020383f0(void* p);
    void dBgCh_Actr_UpdateContinuous_Veneer(void* p);
    int _ZNK10dBgCh_Actr10IsOnGroundEv(dBgCh_Actr* w);
    void* _ZNK10dBgCh_Actr14GetFloorResultEv(dBgCh_Actr* w);
    void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(SurfaceInfo* s, Vector3* v);
    int _ZN4cstd4fdivEii(int a, int b);
    int _ZNK10dBgCh_Actr8IsOnWallEv(dBgCh_Actr* w);
    void* _ZNK10dBgCh_Actr13GetWallResultEv(dBgCh_Actr* w);
  int b = (int)((*(int*)(c+0xb0) & 0x4000) != 0);
  if (b != 0) return;
  int bb = (int)(*(unsigned short*)(c+0xc) == 0x10b);
  if (bb != 0 && *(int*)(c+0x3b4) != 9) func_020383f0(c+0x194);
  else dBgCh_Actr_UpdateContinuous_Veneer(c+0x194);
  if (_ZNK10dBgCh_Actr10IsOnGroundEv(w) != 0) {
    Vector3 n;
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3((SurfaceInfo*)((char*)_ZNK10dBgCh_Actr14GetFloorResultEv(w) + 4), &n);
    if (n.y != 0) {
      int s = (int)(((long long)n.x * *(int*)(c+0xa4) + 0x800) >> 0xc)
            + (int)(((long long)n.z * *(int*)(c+0xac) + 0x800) >> 0xc);
      *(int*)(c+0xa8) = -(_ZN4cstd4fdivEii(s, n.y) + 0x8000);
    }
  }
  if (_ZNK10dBgCh_Actr8IsOnWallEv(w) != 0) {
    Vector3 wn;
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3((SurfaceInfo*)((char*)_ZNK10dBgCh_Actr13GetWallResultEv(w) + 4), &wn);
  }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov030_02112094, 0x02112094, size 0x21c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov030_02112094
extern "C" {
void func_ov030_02112094(void* self)
{
    /* from include/decl_common.h:255, which this TU does not include. */
    extern char data_ov030_02115ddc[];
    /* recovered: shared common types, declarations from a shared header */
    /* recovered: shared common types */
    typedef struct M4x3 { int w[12]; } M4x3;



    struct Bundle { Vector3_16 rot; short _p; Vector3 trans; int _tail[2]; };

    extern int _ZN6Player14IsFrontSlidingEv(void*);
    extern int _ZN6Player17LostGrabbedObjectEv(void*);
    extern void* _ZN8dActor_c11UpdateCarryER6PlayerRK7Vector3(void*, void*, void*);
    extern void Matrix4x3_FromRotationY(void* m, int angle);
    extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        void* self, void* shadow, void* mtx, int rad, int height, unsigned int flags);
    extern void* _ZN8dActor_c10FindWithIDEj(unsigned int id);
    extern void MulMat4x3Mat4x3(void* a, void* b, void* out);
    extern void Matrix4x3_ApplyInPlaceToTranslation(void* m, int x, int y, int z);
    extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
    extern M4x3 data_020a0e68;
    void func_ov030_02112094(void* self);
    char* c = (char*)self;
    int idx;
    void* res;
    void* obj;
    unsigned int id;
    Bundle bnd;

    int a = (int)((*(int*)(c + 0xb0) & 0x100) != 0);
    if (a && *(void**)(c + 0x3a8)
        && *(int*)(*(char**)(c + 0x3a8) + 0xc8)) {
        idx = 0;
        if (_ZN6Player14IsFrontSlidingEv(*(void**)(c + 0x3a8))
            || _ZN6Player17LostGrabbedObjectEv(*(void**)(c + 0x3a8))) {
            idx = 1;
        }
        if (*(int*)(*(char**)(c + 0x3a8) + 8) == 2) {
            idx = (idx + 2) & 0xff;
        }
        res = _ZN8dActor_c11UpdateCarryER6PlayerRK7Vector3(c, *(void**)(c + 0x3a8),
            data_ov030_02115ddc + idx * 0xc);
        *(M4x3*)(c + 0xf0) = *(M4x3*)res;
    } else {
        Matrix4x3_FromRotationY(c + 0xf0, *(short*)(c + 0x8e));
        *(int*)(c + 0x114) = *(int*)(c + 0x5c) >> 3;
        *(int*)(c + 0x118) = *(int*)(c + 0x60) >> 3;
        *(int*)(c + 0x11c) = *(int*)(c + 0x64) >> 3;
    }

    int b = (int)((*(int*)(c + 0xb0) & 0x40000) != 0);
    if (!b) {
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, c + 0x138, c + 0xf0, 0x5a000, 0x190000, 0xf);
    }

    id = *(unsigned int*)(c + 0x3ac);
    if (id == 0)
        return;

    obj = _ZN8dActor_c10FindWithIDEj(id);

    bnd.trans.x = 0xa00;
    bnd.trans.y = 0;
    bnd.trans.z = -0x2f00;
    bnd.rot.x = -0x3f00;
    bnd.rot.y = 0;
    bnd.rot.z = -0x4000;

    data_020a0e68 = *(M4x3*)(c + 0xf0);
    MulMat4x3Mat4x3(*(char**)(c + 0xe8) + 0xf0, &data_020a0e68, &data_020a0e68);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68,
        *(volatile int*)&bnd.trans.x, *(volatile int*)&bnd.trans.y, *(volatile int*)&bnd.trans.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        *(volatile short*)&bnd.rot.x, *(volatile short*)&bnd.rot.y, *(volatile short*)&bnd.rot.z);
    *(M4x3*)(c + 0x350) = data_020a0e68;

    *(int*)((char*)obj + 0xc8) = (int)(c + 0x350);
    *(int*)((char*)obj + 0x5c) = *(int*)(c + 0x5c);
    *(int*)((char*)obj + 0x60) = *(int*)(c + 0x60);
    *(int*)((char*)obj + 0x64) = *(int*)(c + 0x64);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov030_021122b0, 0x021122b0, size 0xf4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov030_021122b0
extern "C" {
int func_ov030_021122b0(dActor_c *self)
{
    extern void _Z14ApproachLinearRsss(short *dst, short target, short rate);
    extern unsigned char DecIfAbove0_Byte(unsigned char *p);
    extern void func_ov030_021141a8(char *c, int x);
    extern void func_ov030_02111dd0(char *c);
    extern void func_ov030_02111f6c(char *c, void *w);
    extern void func_ov030_02111bc4(char *c);
    extern void func_ov030_02111ea4(char *c);
    extern void func_ov030_02111890(char *c);
    char *s = (char*)self;
    short ang = self->HorzAngleToCPlayer() + 0x8000;
    _Z14ApproachLinearRsss((short*)(s + 0x8e), ang, 0xa28);
    *(short*)(s + 0x94) = *(short*)(s + 0x8e);
    if (DecIfAbove0_Byte((unsigned char*)(s + 0x3c6)) == 0)
        func_ov030_021141a8(s, 0);
    ((Animation*)(s + 0x124))->Advance();
    int b = (int)(*(unsigned short*)(s + 0xc) == 0x10b);
    if (b) {
        self->UpdatePos((dCc_c*)(s + 0x160));
        func_ov030_02111dd0(s);
        func_ov030_02111f6c(s, s + 0x194);
        func_ov030_02111bc4(s);
    } else {
        self->UpdatePos((dCc_c*)(s + 0x160));
        func_ov030_02111f6c(s, s + 0x194);
        func_ov030_02111bc4(s);
        func_ov030_02111ea4(s);
    }
    ((dCc_c*)(s + 0x160))->Clear();
    ((dCc_c*)(s + 0x160))->Update();
    func_ov030_02111890(s);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov030_021123a4, 0x021123a4, size 0x5c */
/* -------------------------------------------------------------------------- */
/* state 10 entry handler.  Member-ness is read from the ROM: 021123a4 is pointer-to-member record 6 of the 22 at 0x02115ac8. */
/* The number 10 is read from the ROM too: this body writes the immediate 10 to the state word at +0x3b4, and it is the only one of the 44 that writes 10.  "EnterState" is coined; see the block above. */
// @symbol _ZN7daMky_c12EnterState10Ev
int daMky_c::EnterState10(){
    char *c = (char *)this;
    struct S { int w[2]; };
    extern struct S data_ov030_02115d18;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xd4, (void*)data_ov030_02115d18.w[1], 0, 0x1000, 0);
  *(int*)(c+0x130) = 0x1000;
  *(int*)(c+0x98) = 0x13000;
  *(unsigned char*)(c+0x3c6) = 0x1e;
  *(int*)(c+0x3b4) = 0xa;
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov030_02112400, 0x02112400, size 0x160 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov030_02112400
extern "C" {
int func_ov030_02112400(char* c)
{
    typedef void* (*Vfn)();

    struct Vector3;

    extern void func_ov030_02111a00(void* c);
    extern void func_ov030_02111f6c(char* c, dBgCh_Actr* w);
    extern void func_ov030_02111bc4(void* c);
    extern int Vec3_Dist(const Vector3* a, const Vector3* b);
    extern void func_ov030_021141a8(void* c, int x);
    extern void* data_02099368[];


    typedef struct { int a, b; } P2;
    struct ClsnResultTmp {
        Vfn* vtb;
        P2 v;
        int a2, a3, a4;
        u16 h0, h1;
        int t0, t1, t2;
    };
    extern char* func_0203567c(dBgCh_Actr* w);
    extern int func_02037f44(ClsnResultTmp* r);
    extern unsigned _ZNK5dBgPi9GetClsnIDEv(const ClsnResultTmp* r);
    extern void _ZN5dBgPiD1Ev(ClsnResultTmp* r);
    func_ov030_02111a00(c);
    ((Animation*)(c + 0x124))->Advance();
    ((dActor_c*)c)->UpdatePos((dCc_c*)(c + 0x160));
    func_ov030_02111f6c(c, (dBgCh_Actr*)(c + 0x194));
    func_ov030_02111bc4(c);
    ((dCc_c*)(c + 0x160))->Clear();
    ((dCc_c*)(c + 0x160))->Update();

    int b = (int)(*(u16*)(c + 0xc) == 0x10c);
    if (b != 0) {
        if (Vec3_Dist((Vector3*)(c + 0x380), (Vector3*)(c + 0x5c)) < 0x514000) {
            if (*(int*)(c + 0x60) > *(int*)(c + 0x384) - 0x12c000) {
                func_ov030_021141a8(c, 1);
            }
        }
    } else {
        if (((dBgCh_Actr*)(c + 0x194))->IsOnGround()) {
            char* r = func_0203567c((dBgCh_Actr*)(c + 0x194));
            ClsnResultTmp res;
            int* d = (int*)&res.v;
            *(double*)d = *(double*)(r + 4);
            d[2] = *(int*)(r + 0xc);
            d[3] = *(int*)(r + 0x10);
            d[4] = *(int*)(r + 0x14);
            res.vtb = (Vfn*)data_02099368;
            res.h0 = *(u16*)(r + 0x18);
            res.h1 = *(u16*)(r + 0x1a);
            res.t0 = *(int*)(r + 0x1c);
            res.t1 = *(int*)(r + 0x20);
            res.t2 = *(int*)(r + 0x24);
            if (_ZNK5dBgPi9GetClsnIDEv(&res) == -1 || func_02037f44(&res) == 0) {
                func_ov030_021141a8(c, 0);
            }
            _ZN5dBgPiD1Ev(&res);
        }
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov030_02112560, 0x02112560, size 0x18 */
/* -------------------------------------------------------------------------- */
/* state 9 entry handler.  Member-ness is read from the ROM: 02112560 is pointer-to-member record 8 of the 22 at 0x02115ac8. */
/* The number 9 is read from the ROM too: this body writes the immediate 9 to the state word at +0x3b4, and it is the only one of the 44 that writes 9.  "EnterState" is coined; see the block above. */
// @symbol _ZN7daMky_c11EnterState9Ev
int daMky_c::EnterState9()
{
    char *p = (char *)this;
    *(int *)(p + 0x98) = 0;
    *(int *)(p + 0x3b4) = 9;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov030_02112578, 0x02112578, size 0x49c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov030_02112578
extern "C" {
int func_ov030_02112578(void *arg0)
{
    void *_ZN8dActor_c15FindWithActorIDEjPS_(unsigned int id, void *p);
    void *_ZN8dActor_c13ClosestPlayerEv(void *self);
    int func_ov030_02111b20(void *self);
    void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thiz, void *bca, int a, int fx, unsigned int f);
    void func_ov030_02111890(void *c);
    s16 Vec3_HorzAngle(const void *v0, const void *v1);
    int _Z14ApproachLinearRsss(s16 *v, s16 target, s16 step);
    s32 Vec3_Dist(const void *a, const void *b);
    int _ZN6Player9StartTalkER7fBase_cb(void *player, void *actor, int b);
    void func_ov030_02111908(void *c);
    int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void *player, void *actor, unsigned int id, const void *pos, unsigned int a, unsigned int b);
    void _ZN9Animation8SetFlagsEi(void *thiz, int flags);
    void func_0201267c(int a, void *b);
    int _ZN6Player12GetTalkStateEv(void);
    void _ZN6Player18HasFinishedTalkingEv(void *player);
    int _ZNK10dBgCh_Actr13JustHitGroundEv(const void *thiz);
    int _ZN9Animation8FinishedEv(void *thiz);
    void _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int c, int fx, int e);
    u8 DecIfAbove0_Byte(u8 *p);
    void _ZN7fBase_c18MarkForDestructionEv(void *thiz);
    void _ZN9Animation7AdvanceEv(void *thiz);
    void _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *clsn);
    void func_ov030_02111f6c(void *c, void *w);
    void _ZN5dCc_c5ClearEv(void *thiz);

    extern void *data_ov030_02115cd0[];
    extern void *data_ov030_02115cf8[];
    extern void *data_ov030_02115d08[];
    extern void *data_ov030_02115d10[];
    extern void *data_ov030_02115d18[];
    u8 *c = (u8 *)arg0;
    void *r4 = _ZN8dActor_c15FindWithActorIDEjPS_(0x67, 0);
    void *r6 = _ZN8dActor_c13ClosestPlayerEv(arg0);
    s32 v[3];
    *(s32 *)((u8 *)v + 0) = 0x981;
    *(s32 *)((u8 *)v + 4) = 0x77a;
    *(s32 *)((u8 *)v + 8) = 0x501;

    switch (c[0x3c7]) {
    case 0:
        if (func_ov030_02111b20(arg0) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, data_ov030_02115cd0[1], 0, 0x1000, 0);
            *(s32 *)(c + 0x130) = 0x1000;
            *(s32 *)(c + 0x98) = 0;
            { u8 *p = (u8 *)((unsigned int)c + 0x3c7); *p = *p + 1; }
        }
        func_ov030_02111890(arg0);
        break;
    case 1:
        _Z14ApproachLinearRsss((s16 *)(c + 0x8e), Vec3_HorzAngle(c + 0x5c, (u8 *)r6 + 0x5c), 0x300);
        if (Vec3_Dist(c + 0x5c, (u8 *)r6 + 0x5c) < 0x96000) {
            if (_ZN6Player9StartTalkER7fBase_cb(r6, arg0, 1) != 0) {
                { u8 *p = (u8 *)((unsigned int)c + 0x3c7); *p = *p + 1; }
            }
        }
        func_ov030_02111908(arg0);
        break;
    case 2: {
        s32 sp[3];
        sp[0] = *(s32 *)(c + 0x5c);
        sp[1] = *(s32 *)(c + 0x60);
        sp[2] = *(s32 *)(c + 0x64);
        sp[1] += 0x50000;
        if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(r6, arg0, 0xbd, sp, 1, 0) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, data_ov030_02115cf8[1], 0, 0x1000, 0);
            _ZN9Animation8SetFlagsEi(c + 0x124, 0);
            func_0201267c(0xd1, c + 0x74);
            { u8 *p = (u8 *)((unsigned int)c + 0x3c7); *p = *p + 1; }
        }
        break;
    }
    case 3:
        if (_ZN6Player12GetTalkStateEv() == 2) {
            _ZN6Player18HasFinishedTalkingEv(r6);
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, data_ov030_02115d18[1], 0, 0x1000, 0);
            { u8 *p = (u8 *)((unsigned int)c + 0x3c7); *p = *p + 1; }
        }
        break;
    case 4:
        if (_Z14ApproachLinearRsss((s16 *)(c + 0x8e), (s16)0xffffe04e, 0x400) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, data_ov030_02115d08[1], 0x40000000, 0x1000, 0);
            *(s16 *)(c + 0x94) = *(s16 *)(c + 0x8e);
            *(s32 *)(c + 0x98) = 0xf000;
            *(s32 *)(c + 0xa8) = 0x2f000;
            func_0201267c(0xf1, c + 0x74);
            { u8 *p = (u8 *)((unsigned int)c + 0x3c7); *p = *p + 1; }
        }
        break;
    case 5:
        if (_ZNK10dBgCh_Actr13JustHitGroundEv(c + 0x194) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, data_ov030_02115d10[1], 0x40000000, 0x1000, 0);
            *(s32 *)(c + 0x98) = 0;
            { u8 *p = (u8 *)((unsigned int)c + 0x3c7); *p = *p + 1; }
        }
        break;
    case 6:
        if (_ZN9Animation8FinishedEv(c + 0x124) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, data_ov030_02115cd0[1], 0, 0x1000, 0);
            *(s32 *)(c + 0x130) = 0x1000;
            *(s32 *)((u8 *)r4 + 0x98) = 0x400;
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x20, 0x14, 0x7f, 0x15666, 0);
            c[0x3c6] = 0x78;
            { u8 *p = (u8 *)((unsigned int)c + 0x3c7); *p = *p + 1; }
        }
        /* fallthrough */
    case 7: {
        s32 *pp = (s32 *)((unsigned int)c + 0x3bc);
        *pp = *pp + 0x400;
        if (*(s32 *)(c + 0x3bc) > 0x17ffd) {
            { u8 *p = (u8 *)((unsigned int)c + 0x3c7); *p = *p + 1; }
        }
    }
        /* fallthrough */
    case 8:
        if (DecIfAbove0_Byte((u8 *)((unsigned int)c + 0x3c6)) == 0) {
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x20, 0x7f, 0, 0x15666, 0);
            { u8 *p = (u8 *)((unsigned int)c + 0x3c7); *p = *p + 1; }
        }
        break;
    case 9:
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, data_ov030_02115d08[1], 0x40000000, 0x1000, 0);
        *(s32 *)((u8 *)r4 + 0x9c) = -0x2000;
        *(s32 *)((u8 *)r4 + 0xa0) = -0x3c000;
        { u8 *p = (u8 *)((unsigned int)c + 0x3c7); *p = *p + 1; }
        break;
    case 10:
        if (_ZN8dActor_c15FindWithActorIDEjPS_(0x67, 0) == 0) {
            _ZN7fBase_c18MarkForDestructionEv(arg0);
        }
        break;
    default:
        break;
    }

    _ZN9Animation7AdvanceEv(c + 0x124);
    _ZN8dActor_c9UpdatePosEP5dCc_c(arg0, c + 0x160);
    func_ov030_02111f6c(arg0, c + 0x194);
    _ZN5dCc_c5ClearEv(c + 0x160);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov030_02112a14, 0x02112a14, size 0x70 */
/* -------------------------------------------------------------------------- */
/* state 8 entry handler.  Member-ness is read from the ROM: 02112a14 is pointer-to-member record 0 of the 22 at 0x02115ac8. */
/* The number 8 is read from the ROM too: this body writes the immediate 8 to the state word at +0x3b4, and it is the only one of the 44 that writes 8.  "EnterState" is coined; see the block above. */
/* This shard used to be labelled daObjHmMaruta_c::AfterClsn and read through a
   daObjHmMaruta_c shadow struct.  The cartridge refutes that on four counts:
   0x02112a14 falls inside this TU's own .text run (0x02111688..0x021145e0)
   while d_a_obj_hm_maruta.c's run is 0x0211164c..0x02111688; it is record 0 of
   daMky_c's 22-entry pointer-to-member array; it writes 8 to daMky_c's state
   word; and daObjHmMaruta_c's factory allocates 836 (0x344) bytes, so the
   0x3c7 this body stores to is 131 bytes past the end of that object, where
   daMky_c is 972 (0x3cc).  The two classes share dActor_c's low offsets, which
   is why the wrong header still compiled.  Offsets now go through `c` like the
   ten sibling EnterState members. */
// @symbol _ZN7daMky_c11EnterState8Ev
int daMky_c::EnterState8() {
    char *c = (char *)this;
    struct G { void *a; void *b; };
    extern struct G data_ov030_02115d18;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xd4, data_ov030_02115d18.b, 0, 0x1000, 0);
    *(int *)(c + 0x130) = 0x1000;
    _ZN7PathPtr6FromIDEj(c+0x398, *(int*)(c+8) & 0xff);
    *(int *)(c + 0x3a0) = 1;
    *(char *)(c + 0x3c7) = 0;
    *(int *)(c + 0x98) = 0x6000;
    *(int *)(c + 0x3b4) = 8;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov030_02112a84, 0x02112a84, size 0x190 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov030_02112a84
extern "C" {
int func_ov030_02112a84(char *a)
{
    typedef struct P2 { int w[2]; } P2;  /* array member: C++ would scalarise a two-int struct copy that C block-moved */

    typedef struct dBgPi {
        void *vtb;
        int s0, s1, s2, s3, s4;
        u16 f, g;
        int h, i, j;
    } dBgPi;

    extern void dBgCh_Actr_UpdateContinuous_Veneer(void *p);
    extern int _ZNK10dBgCh_Actr13JustHitGroundEv(void *p);
    extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *p);
    extern void *func_0203567c(void *p);
    extern u32 _ZNK5dBgPi9GetClsnIDEv(void *r);
    extern void func_ov030_021141a8(void *a, int m);
    extern void _ZN5dBgPiD1Ev(void *r);
    extern int Vec3_Dist(void *a, void *b);
    extern void _ZN9Animation7AdvanceEv(void *p);
    extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *a, void *c);
    extern void func_ov030_02111bc4(void *a);
    extern void _ZN5dCc_c5ClearEv(void *p);
    extern void *data_02099368[];
    dBgPi res;

    dBgCh_Actr_UpdateContinuous_Veneer(a + 0x194);
    if (_ZNK10dBgCh_Actr13JustHitGroundEv(a + 0x194) || _ZNK10dBgCh_Actr10IsOnGroundEv(a + 0x194)) {
        int b;
        u16 id;

        b = 0;
        *(int *)(a + 0xa8) = 0;
        id = *(u16 *)(a + 0xc);
        if (id == 0x10b)
            b = 1;
        if (b) {
            char *r = (char *)func_0203567c(a + 0x194);
            int *d = &res.s0;
            *(P2 *)d = *(P2 *)(r + 4);
            d[2] = *(int *)(r + 0xc);
            d[3] = *(int *)(r + 0x10);
            d[4] = *(int *)(r + 0x14);
            res.vtb = data_02099368;
            res.f = *(u16 *)(r + 0x18);
            res.g = *(u16 *)(r + 0x1a);
            res.h = *(int *)(r + 0x1c);
            res.i = *(int *)(r + 0x20);
            res.j = *(int *)(r + 0x24);
            if (_ZNK5dBgPi9GetClsnIDEv(&res) != 0xffffffff)
                func_ov030_021141a8(a, 9);
            else
                func_ov030_021141a8(a, *(int *)(a + 0x3b8));
            _ZN5dBgPiD1Ev(&res);
        } else {
            int t = (int)(id == 0x10c);
            if (t != 0) {
                if (Vec3_Dist(a + 0x380, a + 0x5c) < 0x514000
                    && *(int *)(a + 0x60) > *(int *)(a + 0x384) - 0x12c000) {
                    func_ov030_021141a8(a, *(int *)(a + 0x3b8));
                } else {
                    func_ov030_021141a8(a, 9);
                }
            }
        }
    }

    _ZN9Animation7AdvanceEv(a + 0x124);
    _ZN8dActor_c9UpdatePosEP5dCc_c(a, a + 0x160);
    func_ov030_02111bc4(a);
    _ZN5dCc_c5ClearEv(a + 0x160);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov030_02112c14, 0x02112c14, size 0x18c */
/* -------------------------------------------------------------------------- */
/* state 7 entry handler.  Member-ness is read from the ROM: 02112c14 is pointer-to-member record 4 of the 22 at 0x02115ac8. */
/* The number 7 is read from the ROM too: this body writes the immediate 7 to the state word at +0x3b4, and it is the only one of the 44 that writes 7.  "EnterState" is coined; see the block above. */
// @symbol _ZN7daMky_c11EnterState7Ev
int daMky_c::EnterState7()
{
    void *arg0 = (void *)this;
    typedef unsigned char u8;
    typedef unsigned short u16;
    typedef short s16;
    typedef long long s64;
    struct Vector3
    {
      int x;
      int y;
      int z;
    };
    extern short data_02082214[];
    extern void *data_ov030_02115d08[];
  u8 *new_var;
  u8 *c = (u8 *) arg0;
  int *pos;
  int *py;
  int *pz;
  struct Vector3 v;
  u8 *other;
  u16 ang;
  s16 s;
  int mul = 0x4b000;
  int rnd = 0x800;
  *((int *) ((int) (((s64) ((int) (c + 0xb0)))))) &= ~0x80000;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, data_ov030_02115d08[1], 0x40000000, 0x1000, 0);
  *((int *) (c + 0x98)) = 0xa000;
  *((int *) (c + 0xa8)) = 0;
  other = *((u8 **) (c + 0x3a8));
  pos = (int *) ((int) (((s64) ((int) (c + 0x5c)))));
  s = *((s16 *) (other + 0x8e));
  *((s16 *) (c + 0x8e)) = s;
  s = *((s16 *) (c + 0x8e));
  py = (int *) ((int) (((s64) ((int) (c + 0x60)))));
  pz = (int *) ((int) (((s64) ((int) (c + 0x64)))));
  *((s16 *) (c + 0x94)) = s;
  other = *((u8 **) (c + 0x3a8));
  {
    int *op = (int *) ((int) (((s64) ((int) (other + 0x5c)))));
    int t0 = op[0];
    *((int *) (c + 0x5c)) = t0;
    int t1 = op[1];
    *((int *) (c + 0x60)) = t1;
    int t2 = op[2];
    *((int *) (c + 0x64)) = t2;
  }
  ang = *((u16 *) (c + 0x8e));
  s = data_02082214[(ang >> 4) * 2];
  *pos = (*pos) + ((int) ((((((s64) s) * mul) + rnd) >> 1) >> 11));
  *py = (*py) + 0x50000;
  ang = *((u16 *) (c + 0x8e));
  s = data_02082214[((ang >> 4) * 2) + 1];
  *pz = (*pz) + ((int) (((((s64) s) * mul) + rnd) >> 12));
  other = *((u8 **) (c + 0x3a8));
  {
    int oy = *((int *) (other + 0x60));
    int oz = *((int *) (new_var = other + 0x64));
    mul = oy + 0x50000;
    v.x = *((int *) (other + 0x5c));
    v.y = mul;
    v.z = oz;
  }
  _ZN8dActor_c17DetectRaycastClsnER7Vector3S1_b(c, &v, (struct Vector3 *) pos, 1);
  *((int *) (c + 0xd0)) = 0;
  *((int *) (c + 0x3a8)) = 0;
  *((int *) (c + 0x3b4)) = 7;
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov030_02112da0, 0x02112da0, size 0x258 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov030_02112da0
extern "C" {
int func_ov030_02112da0(char *a) {
    extern void func_ov030_021141a8(void *a, int m);
    extern int Vec3_Dist(void *a, void *b);
    extern int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void *a, void *self, unsigned int, void *, unsigned int, unsigned int);
    extern void func_0201267c(int, void *);
    extern int _ZN6Player12GetTalkStateEv(void *p);
    extern void _ZN6Player9DropActorEv(void *p);
    extern u8 DecIfAbove0_Byte(u8 *p);
    extern u8 data_0209d684;
    int b = (int)((*(u32 *)(a + 0xb0) & 0x40000) != 0);
    if (b != 0) {
        int p = (int)(*(char **)(a + 0x3a8) + 0x5c);
        *(int *)(a + 0x5c) = *(int *)p;
        *(int *)(a + 0x60) = *(int *)(p + 4);
        *(int *)(a + 0x64) = *(int *)(p + 8);
    }

    {
        u32 flags = *(u32 *)(a + 0xb0);
        b = (int)((flags & 0x80000) != 0);
        if (b != 0) {
            *(int *)(a + 0x3b8) = 1;
            func_ov030_021141a8(a, 7);
            return 1;
        }

        switch (*(u8 *)(a + 0x3c7)) {
        case 0: {
            int b2 = (int)((flags & 0x40000) != 0);
            if (b2 != 0) {
                char *s = *(char **)(a + 0x3a8);
                int off = 0x3c7;
                int *p = (int *)(s + 0x5c);
                int x = *p;
                u8 *st = (u8 *)((int)a + off);
                *(int *)(a + 0x5c) = x;
                *(int *)(a + 0x60) = p[1];
                *(int *)(a + 0x64) = p[2];
                (*st)++;
            } else {
                int b3 = (int)((flags & 0x20000) != 0);
                if (b3 != 0) break;
                if (b2 != 0) break;
                *(int *)(a + 0xd0) = 0;
                func_ov030_021141a8(a, *(int *)(a + 0x3b8));
            }
            break;
        }
        case 1:
            if (Vec3_Dist(a + 0x380, a + 0x5c) < 0x514000 &&
                *(int *)(a + 0x60) > *(int *)(a + 0x384) - 0x12c000) {
                if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(*(char **)(a + 0x3a8), a, 0xc1, 0, 0, 0) != 0) {
                    func_0201267c(0xd1, a + 0x74);
                    (*(u8 *)((int)a + 0x3c7))++;
                }
            }
            {
                char *s = *(char **)(a + 0x3a8);
                u8 val = 0x3c;
                int *p = (int *)(s + 0x5c);
                *(int *)(a + 0x5c) = *p;
                *(int *)(a + 0x60) = p[1];
                *(int *)(a + 0x64) = p[2];
                *(u8 *)(a + 0x3c6) = val;
            }
            break;
        case 2:
            if (_ZN6Player12GetTalkStateEv(*(char **)(a + 0x3a8)) == -1) {
                u8 g = data_0209d684;
                if (g == 1) {
                    _ZN6Player9DropActorEv(*(char **)(a + 0x3a8));
                    *(int *)(a + 0x3b8) = 8;
                    func_ov030_021141a8(a, 7);
                } else if (g == 2) {
                    (*(u8 *)((int)a + 0x3c7))++;
                }
            }
            break;
        case 3:
            if (DecIfAbove0_Byte((u8 *)((int)a + 0x3c6)) == 0) {
                *(u8 *)(a + 0x3c7) = 1;
            }
            break;
        }
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov030_02112ff8, 0x02112ff8, size 0x9c */
/* -------------------------------------------------------------------------- */
/* state 6 entry handler.  Member-ness is read from the ROM: 02112ff8 is pointer-to-member record 3 of the 22 at 0x02115ac8. */
/* The number 6 is read from the ROM too: this body writes the immediate 6 to the state word at +0x3b4, and it is the only one of the 44 that writes 6.  "EnterState" is coined; see the block above. */
// @symbol _ZN7daMky_c11EnterState6Ev
int daMky_c::EnterState6()
{
    char *c = (char *)this;
    *(int *)(c + 0xb0) &= ~0x80000;
    if (Vec3_Dist(c + 0x380, c + 0x5c) < 0x514000 &&
        *(int *)(c + 0x60) > *(int *)(c + 0x384) - 0x12c000) {
        *(unsigned char *)(c + 0x3c7) = 0;
        _ZN8dActor_c13SpawnSoundObjEj(c, 1);
    } else {
        *(unsigned char *)(c + 0x3c7) = 3;
    }
    *(int *)(c + 0x98) = 0;
    *(unsigned char *)(c + 0x3c6) = 0x3c;
    _ZN5dCc_c5ClearEv(c + 0x160);
    *(int *)(c + 0x3b8) = *(int *)(c + 0x3b4);
    *(int *)(c + 0x3b4) = 6;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov030_02113094, 0x02113094, size 0x240 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov030_02113094
extern "C" {
int func_ov030_02113094(char* self)
{
    /* recovered: shared common types */
    struct dActor_c;



    extern struct dActor_c* _ZN8dActor_c10FindWithIDEj(u32 id);
    extern void func_ov030_021141a8(char* self, int a);
    extern int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(char* p, char* self, u32 msg, const struct Vector3* pos, u32 a, u32 b);
    extern void func_0201267c(u32 id, char* p);
    extern int _ZN6Player12GetTalkStateEv(char* p);
    extern void _ZN6Player9DropActorEv(char* p);
    extern struct dActor_c* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 id, u32 param, const struct Vector3* pos, const struct Vector3_16* rot, int a, int b);
    extern void _ZN7fBase_c18MarkForDestructionEv(char* self);
    {
        int b = (int)((*(u32*)(self + 0xb0) & 0x40000) != 0);
        if (b != 0) {
            int p = (int)((((int)*(char**)(self + 0x3a8)) + 0x5c));
            *(int*)(self + 0x5c) = *(int*)p;
            *(int*)(self + 0x60) = *(int*)(p + 4);
            *(int*)(self + 0x64) = *(int*)(p + 8);
        }
    }

    switch (*(u8*)(self + 0x3c7)) {
    case 0: {
        int b2 = (int)((*(u32*)(self + 0xb0) & 0x40000) != 0);
        if (b2 != 0) {
            if (*(u8*)(self + 0x3c8) != 0) {
                struct dActor_c* a = _ZN8dActor_c10FindWithIDEj(*(u32*)(self + 0x3ac));
                *(char**)((char*)a + 0xd0) = *(char**)(self + 0x3a8);
                *(u32*)(((int)a + 0xb0)) |= 0x40000;
            }
            (*(u8*)(((int)self + 0x3c7)))++;
        } else {
            int b3 = (int)((*(u32*)(self + 0xb0) & 0x20000) != 0);
            if (b3 != 0) break;
            if (b2 != 0) break;
            *(int*)(self + 0xd0) = 0;
            func_ov030_021141a8(self, *(int*)(self + 0x3b8));
        }
        break;
    }
    case 1: {
        int msg = (*(u8*)(self + 0x3c8) != 0) ? 0xc2 : 0xc3;
        if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(*(char**)(self + 0x3a8), self, (s16)msg, 0, 0, 0) != 0) {
            func_0201267c(0xd1, self + 0x74);
            (*(u8*)(((int)self + 0x3c7)))++;
        }
        {
            int b4 = (int)((*(u32*)(self + 0xb0) & 0x80000) != 0);
            if (b4 != 0) {
                func_ov030_021141a8(self, 7);
            }
        }
        break;
    }
    case 2:
        if (_ZN6Player12GetTalkStateEv(*(char**)(self + 0x3a8)) == -1) {
            _ZN6Player9DropActorEv(*(char**)(self + 0x3a8));
            (*(u8*)(((int)self + 0x3c7)))++;
        }
        break;
    case 3: {
        int b5 = (int)((*(u32*)(self + 0xb0) & 0x80000) != 0);
        if (b5 != 0) {
            if (*(u8*)(self + 0x3c8) != 0) {
                _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x10d, (*(u32*)(self + 0x3b0) << 8) | 5, (struct Vector3*)(*(char**)(self + 0x3a8) + 0x5c), 0, *(s8*)(self + 0xcc), -1);
                _ZN7fBase_c18MarkForDestructionEv((char*)_ZN8dActor_c10FindWithIDEj(*(u32*)(self + 0x3ac)));
                {
                    u32 z = 0;
                    *(u32*)(self + 0x3ac) = z;
                    *(u8*)(self + 0x3c8) = (u8)z;
                    *(int*)(self + 0x3b8) = 0xa;
                }
            }
            func_ov030_021141a8(self, 7);
        }
        break;
    }
    }

    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov030_021132d4, 0x021132d4, size 0x50 */
/* -------------------------------------------------------------------------- */
/* state 5 entry handler.  Member-ness is read from the ROM: 021132d4 is pointer-to-member record 18 of the 22 at 0x02115ac8. */
/* The number 5 is read from the ROM too: this body writes the immediate 5 to the state word at +0x3b4, and it is the only one of the 44 that writes 5.  "EnterState" is coined; see the block above. */
// @symbol _ZN7daMky_c11EnterState5Ev
int daMky_c::EnterState5() {
    char* c = (char*)this;
    int* p = (int*)((int)c + 0xb0);
    int tmp = *p;
    *p = tmp & ~0x80000;
    *(unsigned char*)(c + 0x3c7) = 0;
    void* arg1 = (void*)(c + 0x160);
    *(int*)(c + 0x98) = 0;
    _ZN5dCc_c5ClearEv(arg1);
    _ZN10dBgCh_Actr15ClearGroundFlagEv((void*)(c + 0x194));
    *(int*)(c + 0x3b8) = *(int*)(c + 0x3b4);
    *(int*)(c + 0x3b4) = 5;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov030_02113324, 0x02113324, size 0x2e8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov030_02113324
extern "C" {
int func_ov030_02113324(void* thiz)
{
    extern int _ZN8dActor_c17DetectRaycastClsnER7Vector3S1_b(void* self, Vector3* a, Vector3* out, int doStore);
    extern int Vec3_Dist(const Vector3* a, const Vector3* b);
    extern void func_ov030_021141a8(char* c, int v);
    extern int _ZN6Player9StartTalkER7fBase_cb(void* self, void* actor, int b);
    extern short Vec3_HorzAngle(const Vector3* v0, const Vector3* v1);
    extern void Matrix4x3_FromTranslation(Mtx43* m, int x, int y, int z);
    extern void Matrix4x3_ApplyInPlaceToRotationY(Mtx43* m, short angY);
    extern void Matrix4x3_ApplyInPlaceToTranslation(Mtx43* m, int x, int y, int z);
    extern int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void* self, void* actor, unsigned int msgId, const Vector3* pos, unsigned int d, unsigned int e);
    extern int func_0201267c(int a, void* b);
    extern int _ZN6Player12GetTalkStateEv(void* self);
    extern void _ZN6Player9DropActorEv(void* self);
    extern void _ZN9Animation7AdvanceEv(void* self);
    extern void _ZN5dCc_c5ClearEv(void* self);
    extern unsigned char DecIfAbove0_Byte(unsigned char* p);

    extern void* data_0209f318;
    extern Mtx43 data_020a0e68;
    extern unsigned char data_0209d684;
    char *c = (char*)thiz;

    *(short*)(c + 0x8e) = *(short*)((char*)(*(void**)(c + 0x3a8)) + 0x8e);
    *(short*)(c + 0x94) = *(short*)(c + 0x8e);

    {
        unsigned int flags = *(unsigned int*)(c + 0xb0);
        int f1 = (flags & 0x100) != 0;
        if (!f1)
            goto do_raycast;
        {
            int f2 = (flags & 0x2000) != 0;
            if (f2)
                goto do_raycast;
        }
    }
    goto skip_raycast;
do_raycast:
    {
        char *other = (char*)(*(void**)(c + 0x3a8));
        Vector3 v;
        int oy = *(int*)(other + 0x60);
        int oz = *(int*)(other + 0x64);
        int vy = oy + 0x32000;
        v.x = *(int*)(other + 0x5c);
        v.y = vy;
        v.z = oz;
        _ZN8dActor_c17DetectRaycastClsnER7Vector3S1_b(c, &v, (Vector3*)(c + 0x5c), 1);

        if (Vec3_Dist((Vector3*)(c + 0x380), (Vector3*)(c + 0x5c)) < 0x514000 &&
            *(int*)(c + 0x60) > *(int*)(c + 0x384) - 0x12c000) {
            func_ov030_021141a8(c, 1);
        } else {
            func_ov030_021141a8(c, 9);
        }
        *(void**)(c + 0x3a8) = 0;
        return 1;
    }
skip_raycast:
    switch (*(unsigned char*)(c + 0x3c7)) {
    case 0:
        if (Vec3_Dist((Vector3*)(c + 0x380), (Vector3*)(c + 0x5c)) < 0x514000 &&
            *(int*)(c + 0x60) > *(int*)(c + 0x384) - 0x12c000) {
            if (_ZN6Player9StartTalkER7fBase_cb(*(void**)(c + 0x3a8), c, 1) != 0) {
                Vector3 camPos;
                {
                    Vector3 *src = (Vector3*)((char*)data_0209f318 + 0x8c);
                    camPos.x = src->x;
                    camPos.y = src->y;
                    camPos.z = src->z;
                }
                short ang = Vec3_HorzAngle(&camPos, (Vector3*)((char*)(*(void**)(c + 0x3a8)) + 0x5c));
                {
                    Vector3 *op = (Vector3*)((char*)(*(void**)(c + 0x3a8)) + 0x5c);
                    Matrix4x3_FromTranslation(&data_020a0e68, op->x, op->y, op->z);
                }
                Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, ang);
                Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, 0, -0x64000);

                Vector3 msgPos;
                msgPos.x = data_020a0e68.a[9];
                msgPos.y = data_020a0e68.a[10];
                msgPos.z = data_020a0e68.a[11];
                msgPos.y = *(int*)(c + 0x60) + 0x64000;

                if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(*(void**)(c + 0x3a8), c, 0xc0, &msgPos, 0, 2) != 0) {
                    func_0201267c(0xd1, c + 0x74);
                    {
                        unsigned char *p = (unsigned char*)(c + 0x3c7);
                        (*p)++;
                    }
                }
            }
        }
        *(unsigned char*)(c + 0x3c6) = 0x3c;
        break;
    case 1:
        if (_ZN6Player12GetTalkStateEv(*(void**)(c + 0x3a8)) == -1) {
            unsigned char g = data_0209d684;
            if (g == 1) {
                _ZN6Player9DropActorEv(*(void**)(c + 0x3a8));
                func_ov030_021141a8(c, 8);
            } else if (g == 2) {
                unsigned char *p = (unsigned char*)(c + 0x3c7);
                (*p)++;
            }
        }
        break;
    case 2:
        if (DecIfAbove0_Byte((unsigned char*)(c + 0x3c6)) == 0)
            *(unsigned char*)(c + 0x3c7) = 0;
        break;
    }

    _ZN9Animation7AdvanceEv(c + 0x124);
    _ZN5dCc_c5ClearEv(c + 0x160);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov030_0211360c, 0x0211360c, size 0xa4 */
/* -------------------------------------------------------------------------- */
/* state 4 entry handler.  Member-ness is read from the ROM: 0211360c is pointer-to-member record 9 of the 22 at 0x02115ac8. */
/* The number 4 is read from the ROM too: this body writes the immediate 4 to the state word at +0x3b4, and it is the only one of the 44 that writes 4.  "EnterState" is coined; see the block above. */
// @symbol _ZN7daMky_c11EnterState4Ev
int daMky_c::EnterState4(){
    char* c = (char*)this;
    extern int data_ov030_02115ce0[];
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xd4, (void *)data_ov030_02115ce0[1], 0, 0x1000, 0);
    *(int*)(c+0x130) = 0x1000;
    if (Vec3_Dist(c+0x380, c+0x5c) < 0x514000
        && *(int*)(c+0x60) > *(int*)(c+0x384) - 0x12c000) {
        *(unsigned char*)(c+0x3c7) = 0;
        _ZN8dActor_c13SpawnSoundObjEj(c, 1);
    } else {
        *(unsigned char*)(c+0x3c7) = 2;
    }
    *(unsigned char*)(c+0x3c6) = 0x3c;
    *(int*)(c+0x3b4) = 4;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- func_ov030_021136b0, 0x021136b0, size 0x3d0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov030_021136b0
extern "C" {
int func_ov030_021136b0(char *c)
{
    typedef short s16;
    typedef signed char s8;
    typedef unsigned char u8;
    typedef unsigned int u32;
    typedef int s32;


    extern void _ZN8SaveData13PlayerLoseCapEv(void);
    /* ROM symbol is Vector3_16 (not Vector3s) â€” wrong mangling left this BLIND and broke mwldarm. */
    extern void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
        u32 actorID, u32 param1, const Vector3 *pos,
        const void *rot, int areaID, int deathTableID);
    extern void func_02012790(int arg);
    extern s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
    extern void Matrix4x3_FromTranslation(Mtx43 *m, int x, int y, int z);
    extern void Matrix4x3_ApplyInPlaceToRotationY(Mtx43 *m, s16 angY);
    extern void Matrix4x3_ApplyInPlaceToTranslation(Mtx43 *m, int x, int y, int z);
    extern int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(
        void *self, void *actor, unsigned int msgId, const Vector3 *pos,
        unsigned int d, unsigned int e);
    extern void func_0201267c(int a, void *b);
    extern int _ZN6Player12GetTalkStateEv(void *self);
    extern void _ZN6Player9DropActorEv(void *self);
    extern void _ZN6Player18SetNewHatCharacterEjjb(void *self, unsigned int a, unsigned int b, int c);
    extern void *_ZN8dActor_c10FindWithIDEj(u32 id);
    extern void func_ov030_021141a8(char *c, int v);
    extern void _ZN9Animation7AdvanceEv(void *self);
    extern void _ZN5dCc_c5ClearEv(void *self);

    extern void *data_0209f318;
    extern Mtx43 data_020a0e68;
    int msg;
    s16 a = *(s16 *)(*(char **)(c + 0x3a8) + 0x8e);
    *(s16 *)(c + 0x8e) = a;
    *(s16 *)(c + 0x94) = *(s16 *)(c + 0x8e);

    switch (*(u8 *)(c + 0x3c7)) {
    case 0:
        if (*(u8 *)(c + 0x3c8) != 0) {
            {
                char *p = *(char **)(c + 0x3a8);
                int t = (*(u8 *)(p + 0x6d9) == *(int *)(p + 8));
                t = (t != 0);
                *(u8 *)(c + 0x3c9) = t;
            }
            if (*(u8 *)(c + 0x3c9) == 0) {
                char *p = *(char **)(c + 0x3a8);
                *(u8 *)(p + 0x71a) = 1;
            } else {
                _ZN8SaveData13PlayerLoseCapEv();
            }
            {
                char *p = *(char **)(c + 0x3a8);
                void *spawned;
                *(u32 *)(c + 0x3b0) = *(u32 *)(p + 8);
                msg = *(s8 *)(c + 0xcc);
                spawned = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                    0x10d,
                    (*(u32 *)(c + 0x3b0) << 8) | 2,
                    (Vector3 *)(c + 0x5c),
                    0,
                    msg,
                    -1);
                *(u32 *)(c + 0x3ac) = ((u32 *)spawned)[1];
            }
        } else {
            if (*(u8 *)(c + 0x3c9) != 0)
                func_02012790(0xa);
        }
        {
            u8 *st = (u8 *)(c + 0x3c7);
            (*st)++;
        }
        /* fall through */
    case 1: {
        s16 ang;
        Vector3 camPos;
        Vector3 msgPos;
        {
            u8 fl = *(u8 *)(c + 0x3c8);
            msg = fl ? 0xbe : 0xbf;
            void *camBase = data_0209f318;
            Vector3 *src = (Vector3 *)((char *)camBase + 0x8c);
            camPos.x = src->x;
            camPos.y = src->y;
            camPos.z = src->z;
            ang = Vec3_HorzAngle(&camPos, (Vector3 *)(*(char **)(c + 0x3a8) + 0x5c));
        }
        {
            int *op = (int *)(*(char **)(c + 0x3a8) + 0x5c);
            Matrix4x3_FromTranslation(&data_020a0e68, op[0], op[1], op[2]);
        }
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, ang);
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, 0, -0x64000);
        {
            int msgArg = ((int)msg << 16) >> 16;
            int my = data_020a0e68.a[10];
            int mx = data_020a0e68.a[9];
            int mz = data_020a0e68.a[11];
            msgPos.x = mx;
            msgPos.y = my;
            msgPos.z = mz;
            msgPos.y = *(int *)(c + 0x60) + 0x64000;
            if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(
                    *(void **)(c + 0x3a8), c, msgArg, &msgPos, 0, 2) != 0) {
                func_0201267c(0xd1, c + 0x74);
                {
                    u8 *st = (u8 *)(c + 0x3c7);
                    (*st)++;
                }
            }
        }
        break;
    }
    case 2:
        if (_ZN6Player12GetTalkStateEv(*(void **)(c + 0x3a8)) == -1) {
            if (*(u8 *)(c + 0x3c8) != 0) {
                {
                    u32 *fl = (u32 *)(c + 0xb0);
                    *fl &= ~0x200u;
                }
                _ZN6Player9DropActorEv(*(void **)(c + 0x3a8));
                {
                    u32 *fl = (u32 *)(c + 0xb0);
                    *fl |= 0x200u;
                }
            } else {
                _ZN6Player9DropActorEv(*(void **)(c + 0x3a8));
            }
            {
                u8 *st = (u8 *)(c + 0x3c7);
                (*st)++;
            }
        }
        break;
    case 3: {
        int f1 = (int)((*(u32 *)(c + 0xb0) & 0x100) != 0);
        if (f1 == 0) {
            if (*(u8 *)(c + 0x3c8) != 0) {
                if (*(u8 *)(c + 0x3c9) == 0) {
                    char *p = *(char **)(c + 0x3a8);
                    _ZN6Player18SetNewHatCharacterEjjb(p, *(u8 *)(p + 0x6d9), 0, 0);
                }
                *(int *)(c + 0x3b8) = 1;
                func_ov030_021141a8(c, 2);
            } else {
                void *act = _ZN8dActor_c10FindWithIDEj(*(u32 *)(c + 0x3ac));
                int z = 0;
                *(int *)((char *)act + 0xc8) = z;
                {
                    char *p = *(char **)(c + 0x3a8);
                    int *src = (int *)(p + 0x5c);
                    *(int *)((char *)act + 0x5c) = src[0];
                    *(int *)((char *)act + 0x60) = src[1];
                    *(int *)((char *)act + 0x64) = src[2];
                }
                *(u32 *)(c + 0x3ac) = (u32)z;
                func_ov030_021141a8(c, 0xa);
            }
            *(void **)(c + 0x3a8) = 0;
        }
        break;
    }
    case 4: {
        char *p = *(char **)(c + 0x3a8);
        if (*(u8 *)(p + 0x6f9) == 0 &&
            *(u8 *)(p + 0x6fb) == 0 &&
            *(u8 *)(p + 0x6ff) == 0) {
            *(u8 *)(c + 0x3c7) = 0;
            {
                u8 *f = (u8 *)(c + 0x3c8);
                *f ^= 1;
            }
            break;
        }
        /* fall through */
    }
    case 5: {
        u32 flags = *(u32 *)(c + 0xb0);
        int f1 = (int)((flags & 0x100) != 0);
        if (f1 != 0) {
            int f2 = (int)((flags & 0x2000) != 0);
            if (f2 == 0)
                break;
        }
        func_ov030_021141a8(c, 0xa);
        *(void **)(c + 0x3a8) = 0;
        break;
    }
    default:
        break;
    }

    _ZN9Animation7AdvanceEv(c + 0x124);
    _ZN5dCc_c5ClearEv(c + 0x160);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- func_ov030_02113a80, 0x02113a80, size 0xb8 */
/* -------------------------------------------------------------------------- */
/* state 3 entry handler.  Member-ness is read from the ROM: 02113a80 is pointer-to-member record 16 of the 22 at 0x02115ac8. */
/* The number 3 is read from the ROM too: this body writes the immediate 3 to the state word at +0x3b4, and it is the only one of the 44 that writes 3.  "EnterState" is coined; see the block above. */
// @symbol _ZN7daMky_c11EnterState3Ev
int daMky_c::EnterState3()
{
    char* c = (char*)this;
    struct BCA_File;
    struct ModelAnim {
        void SetAnim(BCA_File*, int, int, unsigned int);
    };
    /* Signature deliberately copied from the local declaration above: the
       ROM name carries by-value class parameters (e.g. Fix12<int>), which
       mwccarm passes differently at the call site, so declaring the true
       types breaks the byte match. See notes/mwccarm-codegen.md 6az. */

    extern char data_ov030_02115ce0[];
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, *(void **)(data_ov030_02115ce0 + 4), 0, 0x1000, 0);
    *(int*)(c + 0x130) = 0x1000;
    if (*(unsigned char*)(c + 0x3c8) == 0 && _ZN8SaveData16HasPlayerLostCapEv()) {
        *(unsigned char*)(c + 0x3c7) = 5;
    } else {
        char* p = *(char**)(c + 0x3a8);
        if (*(unsigned char*)(p + 0x6f9) != 0 ||
            *(unsigned char*)(p + 0x6fb) != 0 ||
            *(unsigned char*)(p + 0x6ff) != 0) {
            *(unsigned char*)(c + 0x3c7) = 4;
        } else {
            *(unsigned char*)(c + 0x3c7) = 0;
            unsigned char* f = (unsigned char*)((unsigned long long)((int)(c) + 0x3c8));
            *f ^= 1;
        }
    }
    *(int*)(c + 0x3b4) = 3;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- func_ov030_02113b38, 0x02113b38, size 0xb0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov030_02113b38
extern "C" {
int func_ov030_02113b38(char* c){
    typedef int Fix12i;
    extern void _ZN9Animation7AdvanceEv(char* a);
    extern void _ZN8dActor_c9UpdatePosEP5dCc_c(char* c, char* cl);
    extern void func_ov030_02111f6c(char* c, char* w);
    extern int _ZNK10dBgCh_Actr13JustHitGroundEv(char* w);
    extern void _ZN10dBgCh_Actr15ClearLimMovFlagEv(char* w);
    extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char* ma, void* f, int b, Fix12i c, unsigned int d);
    extern void func_ov030_021141a8(char* c, int v);
    extern void _ZN5dCc_c5ClearEv(char* cl);
    extern void func_ov030_02111bc4(char* c);
    extern int data_ov030_02115d18[];
  _ZN9Animation7AdvanceEv(c+0x124);
  _ZN8dActor_c9UpdatePosEP5dCc_c(c, c+0x160);
  func_ov030_02111f6c(c, c+0x194);
  if (_ZNK10dBgCh_Actr13JustHitGroundEv(c+0x194) != 0) {
    _ZN10dBgCh_Actr15ClearLimMovFlagEv(c+0x194);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xd4, (void*)data_ov030_02115d18[1], 0, 0x1000, 0);
    *(int*)(c+0x130) = 0x1000;
    *(int*)(c+0x38c) = *(int*)(c+0x5c);
    *(int*)(c+0x390) = *(int*)(c+0x60);
    *(int*)(c+0x394) = *(int*)(c+0x64);
    func_ov030_021141a8(c, *(int*)(c+0x3b8));
  }
  _ZN5dCc_c5ClearEv(c+0x160);
  func_ov030_02111bc4(c);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- func_ov030_02113be8, 0x02113be8, size 0x138 */
/* -------------------------------------------------------------------------- */
/* state 2 entry handler.  Member-ness is read from the ROM: 02113be8 is pointer-to-member record 7 of the 22 at 0x02115ac8. */
/* The number 2 is read from the ROM too: this body writes the immediate 2 to the state word at +0x3b4, and it is the only one of the 44 that writes 2.  "EnterState" is coined; see the block above. */
// @symbol _ZN7daMky_c11EnterState2Ev
int daMky_c::EnterState2() {
    void *c = (void *)this;
    typedef int Fix12i;
    typedef short s16;

    extern void *data_ov030_02115d08;
    void *p;
    int b;

    p = *(void **)((char *)c + 0x3a8);
    if (p == 0)
        p = _ZN8dActor_c13ClosestPlayerEv(c);

    b = (int)(*(unsigned short *)((char *)c + 0xc) == 0x10b);
    if (b != 0) {
        *(s16 *)((char *)c + 0x8e) = Vec3_HorzAngle((char *)c + 0x5c, (char *)c + 0x380);
    } else if (Vec3_Dist((char *)c + 0x380, (char *)c + 0x5c) < 0x514000 &&
               *(int *)((char *)c + 0x60) > *(int *)((char *)c + 0x384) - 0x12c000) {
        *(s16 *)((char *)c + 0x8e) = Vec3_HorzAngle((char *)c + 0x5c, (char *)p + 0x5c);
    } else {
        *(s16 *)((char *)c + 0x8e) = Vec3_HorzAngle((char *)c + 0x5c, (char *)c + 0x380);
    }

    *(s16 *)((char *)c + 0x94) = *(s16 *)((char *)c + 0x8e);
    *(int *)((char *)c + 0x98) = 0xd000;
    b = (int)(*(unsigned short *)((char *)c + 0xc) == 0x10b);
    *(int *)((char *)c + 0xa8) = b ? 0x23000 : 0x1e000;

    _ZN10dBgCh_Actr13SetLimMovFlagEv((char *)c + 0x194);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)c + 0xd4,
        ((void **)&data_ov030_02115d08)[1], 0x40000000, 0x1000, 0);
    func_0201267c(0xd1, (char *)c + 0x74);
    func_0201267c(0xf1, (char *)c + 0x74);
    *(int *)((char *)c + 0x3b4) = 2;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- func_ov030_02113d20, 0x02113d20, size 0x2b8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov030_02113d20
extern "C" {
int func_ov030_02113d20(void *c) {
    typedef int Fix12i;
    typedef short s16;

    extern void *_ZN8dActor_c13ClosestPlayerEv(void *c);
    extern Fix12i Vec3_Dist(const void *a, const void *b);
    extern s16 Vec3_HorzAngle(const void *v0, const void *v1);
    extern void _Z14ApproachLinearRsss(s16 *dst, s16 target, s16 step);
    extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *anim, void *file, int n, Fix12i speed, unsigned int flags);
    extern void func_ov030_02111a00(void *c);
    extern void _ZN9Animation7AdvanceEv(void *c);
    extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *c, void *clsn);
    extern int func_ov030_02111dd0(void *c);
    extern int _ZNK10dBgCh_Actr8IsOnWallEv(void *c);
    extern void func_ov030_02111f6c(void *c, void *w);
    extern void func_ov030_02111bc4(void *c);
    extern int func_ov030_02111ea4(void *c);
    extern int _ZN6Player7IsInAirEv(void *p);
    extern void func_ov030_021141a8(void *c, int mode);
    extern void _ZN5dCc_c5ClearEv(void *c);
    extern void _ZN5dCc_c6UpdateEv(void *c);
    extern void func_ov030_02111890(void *c);
    extern void *data_ov030_02115d18;
    void *p;
    Fix12i dist;
    s16 angle;
    int onWall;
    int b;

    p = *(void **)((char *)c + 0x3a8);
    if (p == 0)
        p = _ZN8dActor_c13ClosestPlayerEv(c);

    dist = Vec3_Dist((char *)c + 0x5c, (char *)p + 0x5c);

    b = (int)(*(unsigned short *)((char *)c + 0xc) == 0x10c);
    if ((b && Vec3_Dist((char *)c + 0x380, (char *)p + 0x5c) > 0x514000) ||
        *(int *)((char *)p + 0x60) < *(int *)((char *)c + 0x384) - 0x12c000) {
        *(unsigned char *)((char *)c + 0x3c7) = 2;
    }

    switch (*(unsigned char *)((char *)c + 0x3c7)) {
    case 0:
        angle = Vec3_HorzAngle((char *)c + 0x5c, (char *)p + 0x5c) + 0x8000;
        *(int *)((char *)c + 0x98) = 0x13000;
        if (dist >= 0x1f4000)
            *(unsigned char *)((char *)c + 0x3c7) = 2;
        break;
    case 1:
        angle = Vec3_HorzAngle((char *)c + 0x5c, (char *)p + 0x5c);
        *(int *)((char *)c + 0x98) = 0xc000;
        if (dist < 0x190000)
            *(unsigned char *)((char *)c + 0x3c7) = 0;
        else if (dist < 0x1f4000)
            *(unsigned char *)((char *)c + 0x3c7) = 2;
        break;
    case 2:
        angle = Vec3_HorzAngle((char *)c + 0x5c, (char *)p + 0x5c);
        *(int *)((char *)c + 0x98) = 0;
        if (dist < 0x190000)
            *(unsigned char *)((char *)c + 0x3c7) = 0;
        else if (dist >= 0x258000)
            *(unsigned char *)((char *)c + 0x3c7) = 1;
        break;
    }

    _Z14ApproachLinearRsss((s16 *)((char *)c + 0x8e), angle, 0xa28);
    *(s16 *)((char *)c + 0x94) = *(s16 *)((char *)c + 0x8e);
    if (*(int *)((char *)c + 0x98) != 0)
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)c + 0xd4,
            ((void **)&data_ov030_02115d18)[1], 0, 0x1000, 0);
    else
        func_ov030_02111a00(c);

    _ZN9Animation7AdvanceEv((char *)c + 0x124);

    b = (int)(*(unsigned short *)((char *)c + 0xc) == 0x10b);
    if (b) {
        _ZN8dActor_c9UpdatePosEP5dCc_c(c, (char *)c + 0x160);
        onWall = (func_ov030_02111dd0(c) | _ZNK10dBgCh_Actr8IsOnWallEv((char *)c + 0x194)) != 0;
        func_ov030_02111f6c(c, (char *)c + 0x194);
        func_ov030_02111bc4(c);
    } else {
        _ZN8dActor_c9UpdatePosEP5dCc_c(c, (char *)c + 0x160);
        func_ov030_02111f6c(c, (char *)c + 0x194);
        func_ov030_02111bc4(c);
        onWall = (func_ov030_02111ea4(c) | _ZNK10dBgCh_Actr8IsOnWallEv((char *)c + 0x194)) != 0;
    }

    if (*(int *)((char *)c + 0x3b4) == 1 && onWall && dist < 0x12c000) {
        if (*(int *)((char *)p + 0x98) > 0x9000 || _ZN6Player7IsInAirEv(p) != 0) {
            *(int *)((char *)c + 0x3b8) = 1;
            func_ov030_021141a8(c, 2);
        }
    }

    _ZN5dCc_c5ClearEv((char *)c + 0x160);
    if (*(int *)((char *)c + 0x3b4) == 1)
        _ZN5dCc_c6UpdateEv((char *)c + 0x160);
    func_ov030_02111890(c);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 32 -- func_ov030_02113fd8, 0x02113fd8, size 0x18 */
/* -------------------------------------------------------------------------- */
/* state 1 entry handler.  Member-ness is read from the ROM: 02113fd8 is pointer-to-member record 5 of the 22 at 0x02115ac8. */
/* The number 1 is read from the ROM too: this body writes the immediate 1 to the state word at +0x3b4, and it is the only one of the 44 that writes 1.  "EnterState" is coined; see the block above. */
// @symbol _ZN7daMky_c11EnterState1Ev
int daMky_c::EnterState1(){
    char* c = (char*)this;
  *(char*)(c+0x3c7)=0;
  *(int*)(c+0x3b4)=1;
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 33 -- func_ov030_02113ff0, 0x02113ff0, size 0x134 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov030_02113ff0
extern "C" {
int func_ov030_02113ff0(char* c){
    typedef int Fix12i;
    typedef short s16;
    extern int _ZN8dActor_c13DistToCPlayerEv(char* c);
    extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char* ma, void* f, int b, Fix12i c, unsigned int d);
    extern void func_ov030_02111a00(char* c);
    extern s16 _ZN8dActor_c18HorzAngleToCPlayerEv(char* c);
    extern void _Z14ApproachLinearRsss(s16* p, s16 target, s16 step);
    extern void _ZN9Animation7AdvanceEv(char* a);
    extern void _ZN8dActor_c9UpdatePosEP5dCc_c(char* c, char* cl);
    extern void func_ov030_02111dd0(char* c);
    extern void func_ov030_02111f6c(char* c, char* w);
    extern void func_ov030_02111bc4(char* c);
    extern void func_ov030_02111ea4(char* c);
    extern void _ZN5dCc_c5ClearEv(char* cl);
    extern void _ZN5dCc_c6UpdateEv(char* cl);
    extern void func_ov030_02111890(char* c);
    extern int data_ov030_02115d18[];
  int dist = _ZN8dActor_c13DistToCPlayerEv(c);
  int b;
  if (dist > 0x15e000) {
    *(int*)(c+0x98) = 0xc000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xd4, (void*)data_ov030_02115d18[1], 0, 0x1000, 0);
  } else if (dist <= 0xfa000) {
    *(int*)(c+0x98) = 0;
    func_ov030_02111a00(c);
  }
  _Z14ApproachLinearRsss((s16*)(c+0x8e), _ZN8dActor_c18HorzAngleToCPlayerEv(c), 0x514);
  *(s16*)(c+0x94) = *(s16*)(c+0x8e);
  *(int*)(c+0x130) = 0x1000;
  _ZN9Animation7AdvanceEv(c+0x124);
  b = (*(unsigned short*)(c+0xc) == 0x10b);
  if (b) {
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, c+0x160);
    func_ov030_02111dd0(c);
    func_ov030_02111f6c(c, c+0x194);
    func_ov030_02111bc4(c);
  } else {
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, c+0x160);
    func_ov030_02111f6c(c, c+0x194);
    func_ov030_02111bc4(c);
    func_ov030_02111ea4(c);
  }
  _ZN5dCc_c5ClearEv(c+0x160);
  _ZN5dCc_c6UpdateEv(c+0x160);
  func_ov030_02111890(c);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 34 -- func_ov030_02114124, 0x02114124, size 0x10 */
/* -------------------------------------------------------------------------- */
/* state 0 entry handler.  Member-ness is read from the ROM: 02114124 is pointer-to-member record 20 of the 22 at 0x02115ac8. */
/* The number 0 is read from the ROM too: this body writes the immediate 0 to the state word at +0x3b4, and it is the only one of the 44 that writes 0.  "EnterState" is coined; see the block above. */
// @symbol _ZN7daMky_c11EnterState0Ev
int daMky_c::EnterState0()
{
    mState = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 35 -- func_ov030_02114134, 0x02114134, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov030_02114134
struct C4134;
typedef void (C4134::*PMF4134)();
struct C4134 { char pad[0x3a4]; PMF4134 *pp; };
extern "C" {
void func_ov030_02114134(C4134 *c)
{
    PMF4134 *p = c->pp + 1;
    (c->**p)();
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 36 -- func_ov030_02114170, 0x02114170, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov030_02114170
struct C4170;
typedef void (C4170::*PMF4170)();
struct C4170 { char pad[0x3a4]; PMF4170 *pp; };
extern "C" {
void func_ov030_02114170(C4170 *c)
{
    PMF4170 *p = c->pp;
    (c->**p)();
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 37 -- func_ov030_021141a8, 0x021141a8, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov030_021141a8
extern "C" {
void func_ov030_021141a8(char *self, int idx)
{
    typedef struct { int a, b, c, d; } Item16;
    extern Item16 data_ov030_02115e0c[];

    *(Item16 **)(self + 0x3a4) = &data_ov030_02115e0c[idx];
    func_ov030_02114170((C4170 *)self);
}
}

/* ==========================================================================
 * The TU's second file-scope extern "C" region.
 *
 * func_ov030_021141a8, directly above, is ROM ordinal 37 and the last of this
 * TU's 24 func_ov030_* free functions; every one of them carries its own
 * declarations at block scope, where an extern in an extern "C" region still
 * gets C linkage.  Everything below this line is a daMky_c:: member, and
 * mwccarm 2004/b56 will not accept a linkage specification inside a function
 * body -- a block-scope `extern` written in a C++-named member gets C++
 * linkage and the reference mangles a second time.  So the six members below
 * share these declarations, and the region is placed here so that no free
 * function is affected by them.
 *
 * The eleven EnterState* members are C++-named too, but they are laid out
 * ROM-ascending among the free functions and so cannot be reached from here.
 * Their declarations live in the region at the TOP of the file instead, which
 * for that reason IS in view of every free function; the comment there records
 * what that cost and what it did not.
 *
 * Where two of the six recovered one symbol differently, one spelling is kept
 * and the call sites are cast; the disagreements were ClosestPlayer and Spawn
 * (pointer type only) and the four SharedFilePtr data objects (spelt `char` in
 * InitResources, `SharedFilePtr` in CleanupResources).  Members of this TU are
 * NOT declared here at all -- func_ov030_02111734, _02112094, _02114134 and
 * _021141a8 are all defined above and bind to their definitions.
 * ========================================================================== */
extern "C" {

struct SharedFilePtr_ { u32 data[4]; };

void  _ZN13SharedFilePtr7ReleaseEv(SharedFilePtr_ *self);
void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
int   _ZN5Model6RenderEPK7Vector3(void *self, void *ofs);
int   _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *file, int a, int b);
void *_ZN9Animation8LoadFileER13SharedFilePtr(void *fp);
int   _ZN11ShadowModel12InitCylinderEv(void *self);
void  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *file, int idx, int speed, u32 flags);
void  _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *actor, int a, int b, u32 c, u32 d);
void  _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, int a, int b, void *v0, void *v1);
int   _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(void *self, int d);
char *_ZN8dActor_c13ClosestPlayerEv(void *self);
char *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 id, u32 param, const void *pos, const void *rot, int areaID, int deathTableID);
int   _ZN8SaveData16HasPlayerLostCapEv(void);

extern SharedFilePtr_  data_ov002_0210da40;
extern SharedFilePtr_  data_ov002_0210d9a0;
extern SharedFilePtr_  data_ov002_0210d9c0;
extern SharedFilePtr_  data_ov030_02115d00;
extern SharedFilePtr_ *data_ov030_02114824[10];
extern void           *data_ov030_02115cf0[];

}

/* Slot 3 of whatever object sits at +0xd4, reached through a raw cast in
   Behavior; the model's own header is not what that call goes through. */
struct VObj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
};

/* -------------------------------------------------------------------------- */
/* ROM ordinal 38 -- _ZN7daMky_c16CleanupResourcesEv, 0x021141c4, size 0x68 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMky_c16CleanupResourcesEv
/* Vtable slot 3.  Releases the four shared files the class holds plus its
 * ten-entry table; it never touches `this`, which is why the legacy C form
 * could declare itself nullary and still reproduce. */
s32 daMky_c::CleanupResources()
{
    int i;
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210da40);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210d9a0);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210d9c0);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov030_02115d00);
    for (i = 0; i < 10; i++)
        _ZN13SharedFilePtr7ReleaseEv(data_ov030_02114824[i]);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 39 -- _ZN7daMky_c16OnPendingDestroyEv, 0x0211422c, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMky_c16OnPendingDestroyEv
/* Vtable slot 12.  The ROM body is one `bx lr`: the override exists only to
 * occupy the slot. */
void daMky_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 40 -- _ZN7daMky_c6RenderEv, 0x02114230, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMky_c6RenderEv
/* Vtable slot 9. */
s32 daMky_c::Render()
{
    int b = (mFlags & 0x40000) != 0;
    if (b) return 1;
    _ZN5Model6RenderEPK7Vector3(((char *)this) + 0xd4, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 41 -- _ZN7daMky_c8BehaviorEv, 0x02114278, size 0x100 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMky_c8BehaviorEv
/* Vtable slot 6. */
s32 daMky_c::Behavior()
{
    char *c = (char *)((dActor_c *)this);
    if (_ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(((dActor_c *)this), 0x5dc000) != 0 &&
        *(int *)(c + 0x3b4) != 8) {
        int b = (*(unsigned short *)(c + 0xc) == 0x10b);
        if (b != 0 && *(unsigned char *)(c + 0x3c8) == 0 && _ZN8SaveData16HasPlayerLostCapEv() != 0) {
            char *pl = _ZN8dActor_c13ClosestPlayerEv(((dActor_c *)this));
            unsigned cp = *(unsigned *)(pl + 8);
            if (cp < 3) {
                char *spawned;
                *(int *)(c + 0x3b0) = cp;
                spawned = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                    0x10d,
                    ((unsigned)*(int *)(c + 0x3b0) << 8) | 2,
                    (const void *)(c + 0x5c),
                    (const void *)0,
                    (int)*(signed char *)(c + 0xcc),
                    -1);
                *(int *)(c + 0x3ac) = *(int *)(spawned + 4);
                *(unsigned char *)(c + 0x3c8) = 1;
                func_ov030_021141a8(c, 1);
            }
        }
        func_ov030_02111734(c);
    } else {
        func_ov030_02114134((C4134 *)c);
        ((VObj *)(c + 0xd4))->v3();
        func_ov030_02112094(c);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 42 -- _ZN7daMky_c13InitResourcesEv, 0x02114378, size 0x25c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMky_c13InitResourcesEv
/* Vtable slot 0. */
s32 daMky_c::InitResources()
{
    int i;
    int b;
    u16 h;
    int *py;

    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da40);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9a0);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9c0);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0xd4, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov030_02115d00), 1, 1);
    for (i = 0; i < 10; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov030_02114824[i]);
    if (_ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel) == 0)
        return 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(((char *)this) + 0xd4, data_ov030_02115cf0[1], 0, 0x1000, 0);
    mModelAnim.speed = 0x1000;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char *)this) + 0x160, ((char *)this), 0x28000, 0x64000, 0x800004, 0x49000);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char *)this) + 0x194, ((char *)this), 0x32000, 0x32000, 0, 0);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    py = (int *)(((int)((char *)this) + 0x384));
    unk_380 = mPosX;
    unk_384 = mPosY;
    unk_388 = mPosZ;
    *py += 0x64000;
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    unk_3a8 = 0;
    mCapUniqueID = 0;
    unk_3cb = 0x96;
    h = actorID;
    b = 0;
    if (h == 0x10b)
        b = 1;
    if (b != 0) {
        if (_ZN8SaveData16HasPlayerLostCapEv() != 0) {
            char *player = _ZN8dActor_c13ClosestPlayerEv(((char *)this));
            if (*(u32 *)(player + 8) >= 3)
                goto ov030_no_spawn;
            {
                char *spawned;
                mCapPlayerNo = *(u32 *)(player + 8);
                spawned = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x10d, (mCapPlayerNo << 8) | 2, ((char *)this) + 0x5c, 0, mAreaId, -1);
                mCapUniqueID = *(int *)(spawned + 4);
                mHasSpawnedCap = 1;
                func_ov030_021141a8(((char *)this), 1);
                goto ov030_cap_done;
            }
ov030_no_spawn:
            func_ov030_021141a8(((char *)this), 0);
ov030_cap_done:
            ;
        } else {
            func_ov030_021141a8(((char *)this), 0);
        }
    } else {
        int t = (int)(h == 0x10c);
        if (t != 0)
            func_ov030_021141a8(((char *)this), 1);
    }
    func_ov030_02112094(((char *)this));
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 43 -- _ZN7daMky_c13OnTurnIntoEggER6Player, 0x021145d4, size 0xc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMky_c13OnTurnIntoEggER6Player
/* Vtable slot 19.  The cartridge's twelve bytes at this address are a linker
 * long-branch veneer to _ZN7fBase_c18MarkForDestructionEv in arm9 -- r0 still
 * holds `this` at the jump and the Player& is dropped -- so the source form
 * that reproduces it is a one-line forwarding member with no return. */
void daMky_c::OnTurnIntoEgg(Player &player)
{
    MarkForDestruction();
}

