//cpp
/* daPukupuku_c -- the Cheep Cheep.
 *
 * A swimming enemy with a home point. InitResources records where it was
 * placed and picks a starting state; Behavior runs the frame -- getting eaten
 * by Yoshi, moving, dispatching the current state through a pointer-to-member
 * at +0x370, and dealing contact damage. The four state handlers count a timer
 * down and steer back toward home.
 *
 * Function order is the REVERSE of the ROM's (highest address first): mwccarm
 * 2004/b56 emits one .text section per function in reverse source order. Do
 * not reorder. D0/D1 order inside the destructor group is the compiler's to
 * pick (verify reports PARTIAL [(0, 1)], pilot report sec 3).
 *
 * The factory is `return (int *)new daPukupuku_c` -- a real instantiation,
 * which is what makes mwccarm emit the D1/D0 pair in ROM order and home the
 * vtable here. The synthesized constructor reproduces the ROM's init sequence
 * byte-exact.
 *
 * Reconstruction notes. The class name comes from the cartridge (RTTI at
 * 0x021342f0, PUKUPUKU registry at 0x02134300); `daPukupuku_c_classInit`,
 * `g_profile_PUKUPUKU` and the historical alias CheepCheep_Spawn are
 * source-style names retail does not store, and the profile stays outside
 * this TU. Behavior and the state handlers still reach fields as char* plus
 * an offset (0x100 mStateTimer, 0x107 mEatenByYoshi, 0x110 mdCcAcPos_c, 0x150
 * mWithMeshClsn, 0x370 the state PMF); InitResources already proves
 * 0x110/0x150/0x374 are real members, but plain member form DIFFs here on
 * address rematerialization, so the conversion is unfinished rather than
 * refused. The func_ov090_* helpers keep their address-derived spellings and
 * no replacement is coined -- the old daManta_c_Kill claim for 0x2133200 is
 * refuted in the manifest, its only referrer being this TU's own state table
 * at 0x21342b8. The data_ov090_* symbols are kept as this TU observes them:
 * 0x213455c an AnimFilePtr (InitResources reads .file), 0x2134564 a
 * SharedFilePtr, 0x21342d8 a Vector3, 0x2134594 a scalar int via
 * decl_common.h.
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
/* common.h FIRST, deliberately. Matrix4x3 has two spellings that share the
 * MATRIX4X3_DEFINED guard -- common.h's flat `s32 m[12]` and math/Matrix.h's
 * nested `Matrix3x3 r; Vector3 t` -- and whichever a TU sees first stands.
 * daPukupuku_c.h reaches math/Matrix.h through ModelAnim.h, so leaving it first
 * selected the nested form, which splits func_ov090_02133338's 48-byte matrix
 * copy into a 9-word and a 3-word move (0x90 bytes against the ROM's 0x74).
 * The flat spelling restores the ROM's three 4-register ldm/stm blocks. Both
 * are 0x30 bytes, and nothing in this TU names .r or .t. */
#include "common.h"
#include "daPukupuku_c.h"
#include "types.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "Animation.h"
/* For daPukupuku_c_classInit (ROM ordinal 14) -- the class factory that sits
 * immediately above InitResources in this run and installs this class's
 * vtable. The legacy shard that carried daPukupuku_c_classInit took these
 * from the same decl_*.h headers; promotion absorbed it into this file. */
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAcPos_c.h"
#include "decl_dBgCh_Actr.h"

/* Local shadow declarations carried from the legacy files verbatim.
 * NOT reconciled against real project headers -- check include/*.h for
 * each of these before compiling; a real header should usually win. */
/* shadow typedef 'Fix12i' */
typedef int Fix12i;

/* RECONCILED: the generator carried a 'struct dCcAcPos_c { int d; };' stand-in
 * from a legacy file, but daPukupuku_c.h already pulls in the real dCcAcPos_c.h,
 * so the stand-in redefined the class. The real header wins. */

/* shadow struct 'Player' */
struct Player;

/* shadow struct 'Obj' */
struct Obj {
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
    virtual void m4();
    virtual void Target(int);
};

/* shadow struct 'dBgCh_Actr' */
struct dBgCh_Actr;

/* shadow struct 'dCc_c' */
struct dCc_c;

/* shadow struct 'dEnemyBase_c' */
struct dEnemyBase_c;

/* shadow typedef 'void' */
typedef void (dEnemyBase_c::*PMF)();

/* shadow struct 'Holder' */
struct Holder { char pad[8]; PMF fn; };

/* RECONCILED: func_ov090_021332e8 (ROM ordinal 7 below) is this TU's state
 * setter. Its own legacy file declared its parameters with a local `struct C`
 * and `typedef int (C::*PMF)();` -- both names collide with the Holder/PMF pair
 * above, which Behavior needs. The state setter's pair is renamed here; only
 * the names change. The forward-declare-then-typedef-then-define ordering is
 * kept exactly as the legacy file had it, because that is what fixes the
 * pointer-to-member representation mwccarm uses for the call at ordinal 7. */
struct PukuStateC;
typedef int (PukuStateC::*PukuStatePMF)();

/* shadow struct 'SharedFilePtr' */
struct SharedFilePtr;

/* shadow struct 'BMD_File' */
struct BMD_File;

/* shadow struct 'BCA_File' */
struct BCA_File;

/* shadow struct 'dActor_c' */
struct dActor_c;

/* shadow struct 'Vector3_16' */
struct Vector3_16;

/* shadow struct 'AnimFilePtr' */
struct AnimFilePtr { int a; struct BCA_File *file; };

extern "C" {
extern "C" void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj( Player*, const Vector3&, unsigned int, Fix12i, unsigned int, unsigned int, unsigned int);
extern Vector3 data_ov090_021342d8;
/* RECONCILED: the three callers of func_ov090_021332e8 each declared it with a
 * different parameter spelling: (char *, int *), (void *, void *), and a
 * void-returning form of the second.
 * It has C linkage, so it cannot be overloaded -- the TU needs exactly
 * one declaration. This is the DEFINITION's own signature (ordinal 7 below);
 * the call sites cast, which changes no argument value. */
extern int func_ov090_021332e8(PukuStateC *, PukuStatePMF *);
/* RECONCILED: data_ov090_02134594 is already declared by decl_common.h (as a
 * scalar `int`). The generator re-declared it here as `int[]`, which is the
 * conflict mwccarm reported. decl_common.h's spelling is kept and the call
 * sites take its address explicitly. */
extern int RandomIntInternal(void*);
extern int data_0209e650[];
extern s32 Vec3_Dist(void* a, void* b);
extern s16 Vec3_HorzAngle(void* a, void* b);
extern int ApproachAngle(s16* angle, int target, int invFactor, int maxDelta, int minDelta);
extern void Vec3_Asr(void *dst, void *src, int n);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, short rx, short ry, short rz);
extern struct Matrix4x3 data_020a0e68;
extern SharedFilePtr data_ov090_02134564;
/* RECONCILED: CleanupResources saw data_ov090_0213455c as a SharedFilePtr (it
 * calls Release() on it); InitResources saw it as an AnimFilePtr (it reads
 * .file). AnimFilePtr is the more complete observation -- it carries a field
 * layout -- so it is the declared type, and CleanupResources casts to reach
 * Release(). Both spellings name the same address, so no call changes. */
extern AnimFilePtr data_ov090_0213455c;
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
void *thisp, struct dActor_c *, struct Vector3 const &, int, int, unsigned int, unsigned int);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
void *thisp, struct dActor_c *, int, int, struct Vector3_16 *, int);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
void *thisp, struct BCA_File *, int, int, unsigned int);
/* TUBUILD CONFLICT -- alternate declaration of func_ov090_021332e8, from the legacy file for func_ov090_02133200, NOT applied: extern int func_ov090_021332e8(void* c, void* p); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov090_021332e8, from the legacy file for _ZN12daPukupuku_c13InitResourcesEv, NOT applied: extern "C" void func_ov090_021332e8(void *c, void *p); */
/* TUBUILD CONFLICT -- alternate declaration of data_ov090_02134564, from the legacy file for _ZN12daPukupuku_c13InitResourcesEv, NOT applied: extern struct SharedFilePtr data_ov090_02134564; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov090_0213455c, from the legacy file for _ZN12daPukupuku_c13InitResourcesEv, NOT applied: extern struct AnimFilePtr data_ov090_0213455c; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov090_021342d8, from the legacy file for _ZN12daPukupuku_c13InitResourcesEv, NOT applied: extern struct Vector3 data_ov090_021342d8; */
}

/* ROM ordinal 14 -- daPukupuku_c_classInit, 0x02133634, size 0x48 */
// @symbol daPukupuku_c_classInit
/* resolved: VT0 = _ZTV12daPukupuku_c */
/* Reconstructed source-style name: SM64DS proves daPukupuku_c through RTTI,
 * allocation size, vtable identity, and the PUKUPUKU registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: CheepCheep_Spawn. */
extern "C" {
int *daPukupuku_c_classInit(void)
{
    return (int *)new daPukupuku_c;
}
}

/* ROM ordinal 13 -- _ZN12daPukupuku_c13InitResourcesEv, 0x02133530, size 0x104 */
// @symbol _ZN12daPukupuku_c13InitResourcesEv
int daPukupuku_c::InitResources()
{
    struct BMD_File *bmd;
    struct Vector3 v;

    bmd = (struct BMD_File *)Model::LoadFile(data_ov090_02134564);
    mModelAnim.SetFile(bmd, 1, -1);

    Animation::LoadFile(*(struct SharedFilePtr *)&data_ov090_0213455c);

    v = data_ov090_021342d8;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        (void *)((char *)&(*(dCcAcPos_c *)&mdCcAcPos_c)), (struct dActor_c *)((char *)this), v, 0x32000, 0x3c000, 0x200004, 0x8000);

    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mAngleY = mPrevAngleY;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        (void *)((char *)&(*(dBgCh_Actr *)&mWithMeshClsn)), (struct dActor_c *)((char *)this), 0x1e000, 0x1e000, 0, 0);

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        (void *)((char *)&(*(u8 *)&mModelAnim)), data_ov090_0213455c.file, 0, 0x1000, 0);

    func_ov090_021332e8((PukuStateC *)((char *)this), (PukuStatePMF *)&data_ov090_02134594);
    return 1;
}

/* ROM ordinal 12 -- _ZN12daPukupuku_c8BehaviorEv, 0x02133430, size 0x100 */
// @symbol _ZN12daPukupuku_c8BehaviorEv
int daPukupuku_c::Behavior()
{
    char *c = (char *)((dEnemyBase_c *)this);
    if (UpdateYoshiEat(*(dBgCh_Actr *)(c + 0x150)) != 0) {
        ((dCc_c *)(c + 0x110))->Clear();
        if (*(unsigned char *)(c + 0x107) != 0) {
            if (*(unsigned short *)(c + 0x104) == 0) {
                ((dCc_c *)(c + 0x110))->Update();
            }
        }
        func_ov090_02133338(c);
        return 1;
    }

    DecIfAbove0_Short((unsigned short *)(c + 0x100));
    UpdatePos((dCc_c *)(c + 0x110));
    {
        Holder *q = *(Holder **)(c + 0x370);
        if (q->fn != 0) (((dEnemyBase_c *)this)->*(q->fn))();
    }
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);
    *(int *)(c + 0x368) = 0x1000;
    ((Animation *)(c + 0x35c))->Advance();
    func_ov090_02133338(c);
    func_ov090_021330c8(c);
    ((dCc_c *)(c + 0x110))->Clear();
    {
        char *p = (char *)ClosestPlayer();
        if (p != 0 && *(unsigned char *)(p + 0x6fb) == 0) {
            ((dCc_c *)(c + 0x110))->Update();
        }
    }
    return 1;
}

/* ROM ordinal 11 -- _ZN12daPukupuku_c6RenderEv, 0x021333e0, size 0x50 */
// @symbol _ZN12daPukupuku_c6RenderEv
int daPukupuku_c::Render()
{
    int b = ((mFlags & 0x40000) != 0);
    if (b) return 1;
    Obj *o = (Obj*)((char *)&(*(u8 *)&mModelAnim));
    o->Target(0);
    return 1;
}

/* ROM ordinal 10 -- _ZN12daPukupuku_c16OnPendingDestroyEv, 0x021333dc, size 0x4 */
// @symbol _ZN12daPukupuku_c16OnPendingDestroyEv
void daPukupuku_c::OnPendingDestroy()
{
}

/* ROM ordinal 9 -- _ZN12daPukupuku_c16CleanupResourcesEv, 0x021333ac, size 0x30 */
// @symbol _ZN12daPukupuku_c16CleanupResourcesEv
int daPukupuku_c::CleanupResources()
{
    data_ov090_02134564.Release();
    ((SharedFilePtr *)&data_ov090_0213455c)->Release();
    return 1;
}

/* ROM ordinal 8 -- func_ov090_02133338, 0x02133338, size 0x74 */
// @symbol func_ov090_02133338
extern "C" {
void func_ov090_02133338(char *c) {
    int v[3];
    Vec3_Asr(v, c+0x5c, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v[0], v[1], v[2]);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        *(short*)(c+0x8c), *(short*)(c+0x8e), *(short*)(c+0x90));
    *(struct Matrix4x3*)(c+0x328) = data_020a0e68;
}
}

/* ROM ordinal 7 -- func_ov090_021332e8, 0x021332e8, size 0x50 */
// @symbol func_ov090_021332e8
/* PukuStateC / PukuStatePMF are forward-declared in the preamble (renamed there
 * from this file's original 'C' / 'PMF' to clear a collision with Behavior's
 * Holder::PMF); this is the same definition the legacy file carried. */
struct PukuStateC { char pad[0x370]; PukuStatePMF *pp; };
extern "C" int func_ov090_021332e8(PukuStateC *c, PukuStatePMF *p) { c->pp = p; PukuStatePMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(); }

/* ROM ordinal 6 -- func_ov090_02133290, 0x02133290, size 0x58 */
// @symbol func_ov090_02133290
extern "C" {
int func_ov090_02133290(char* c){
  unsigned int r = RandomIntInternal(data_0209e650);
  *(short*)(c+0x384) = ((r>>8)&0xf)<<0xc;
  r = RandomIntInternal(data_0209e650);
  *(short*)(c+0x100) = ((r>>8)&0x3f)+0x32;
  *(int*)(c+0x98) = 0x5000;
  return 1;
}
}

/* ROM ordinal 5 -- func_ov090_02133200, 0x02133200, size 0x90 */
// @symbol func_ov090_02133200
/* The legacy shard that carried func_ov090_02133200 -- absorbed into this
 * file by promotion -- carried
 * `// recovered name: daManta_c_Kill` and `daManta_c::Kill - recovered from
 * vtable slot identity`. That attribution is REFUTED and is not carried here.
 * _ZTV9daManta_c spans 0x0213423c..0x021342b8 and its slots hold only
 * daManta_c's own functions (0x2132e38, 0x2132c1c, 0x2132c94, 0x2132c68,
 * 0x2132c64, 0x213269c, 0x21326dc); 0x2133200 is in no vtable at all. Its one
 * and only referrer in the whole ROM is 0x21342b8, the first of the four
 * 8-byte pointer-to-member entries in daPukupuku_c's state table -- which
 * begins exactly where daManta_c's vtable ends, the likely source of the
 * mis-assignment. This is a daPukupuku_c state handler, reached through
 * func_ov090_021332e8. No replacement name is coined: the ROM names nothing
 * here. */
extern "C" {
int func_ov090_02133200(char* c)
{
    if (Vec3_Dist(c + 0x5c, c + 0x374) > 0x3e8000) {
        *(u16*)(c + 0x100) = 0x32;
        *(s16*)(c + 0x384) = Vec3_HorzAngle(c + 0x5c, c + 0x374);
    }
    ApproachAngle((s16*)(c + 0x94), *(s16*)(c + 0x384), 1, 0x100, 0x200);
    if (*(u16*)(c + 0x100) == 0)
        func_ov090_021332e8((PukuStateC *)c, (PukuStatePMF *)data_ov090_02134584);
    return 1;
}
}

/* ROM ordinal 4 -- func_ov090_021331c4, 0x021331c4, size 0x3c */
// @symbol func_ov090_021331c4
extern "C" {
int func_ov090_021331c4(char* c){
  unsigned int r = RandomIntInternal(data_0209e650);
  *(short*)(c+0x100) = ((r>>8)&0x3f)+0x32;
  *(int*)(c+0x98) = 0;
  return 1;
}
}

/* ROM ordinal 3 -- func_ov090_02133190, 0x02133190, size 0x34 */
// @symbol func_ov090_02133190
extern "C" {
int func_ov090_02133190(char *c) {
    unsigned short h = *(unsigned short*)(c + 0x100);
    if (h == 0) {
        func_ov090_021332e8((PukuStateC *)c, (PukuStatePMF *)&data_ov090_02134594);
    }
    return 1;
}
}

/* ROM ordinal 2 -- func_ov090_021330c8, 0x021330c8, size 0xc8 */
// @symbol func_ov090_021330c8
extern "C" void func_ov090_021330c8(char* thiz)
{
    char* c = thiz;
    Vector3 v;
    v.x = data_ov090_021342d8.x;
    v.y = data_ov090_021342d8.y;
    v.z = data_ov090_021342d8.z;
    ((dCcAcPos_c*)(c + 0x110))->SetPosRelativeToActor(v);
    {
        unsigned int id = *(unsigned int*)(c + 0x134);
        if (id == 0) return;
        {
            Player* a = (Player*)dActor_c::FindWithID(id);
            int b = (int)(*(unsigned short*)((char*)a + 0xc) == 0xbf);
            if (b == 0) return;
            if (*(unsigned char*)((char*)a + 0x6fb) != 0) return;
            {
                Vector3 hv;
                hv.x = *(int*)(c + 0x5c);
                hv.y = *(int*)(c + 0x60);
                hv.z = *(int*)(c + 0x64);
                _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, hv, 1, 0xc000, 1, 0, 1);
            }
        }
    }
}

/* ROM ordinal 1 -- _ZN12daPukupuku_cD0Ev, 0x02133074, size 0x54 */
// @symbol _ZN12daPukupuku_cD0Ev
/* D0 is the DELETING destructor: destroy through this class (dEnemyBase_c
 * chain) then return the object to its heap via an inline operator delete.
 * Both variants are emitted from the single inline destructor in
 * daPukupuku_c.h (class-form skill): D1 then D0 in ROM order, no leaf D2. */

/* ROM ordinal 0 -- _ZN12daPukupuku_cD1Ev, 0x02133034, size 0x40 */
// @symbol _ZN12daPukupuku_cD1Ev
/* D1 is emitted from the inline destructor in daPukupuku_c.h alongside D0
 * (class-form skill); this marker at D1's ROM ordinal keeps the
 * accounting naming it. This class adds no member with a destructor
 * of its own. */
