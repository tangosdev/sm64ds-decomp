//cpp
/* Production translation unit for ov015/daObjBk_Dossunbar_c, hand-curated.
 * 25 function(s), .text 0x02111ba0..0x02112290.
 *
 * WHAT THE CARTRIDGE PROVES ABOUT THE NAME AND THE SHAPE:
 *   _ZTS  ov015 0x0211451c  "19daObjBk_Dossunbar_c"
 *   _ZTI  ov015 0x02114510  __si_class_type_info; +8 -> _ZTI10dBgActor_c
 *                           (ov002 0x021089ec), so the DIRECT base is
 *                           dBgActor_c and nothing else.
 *   _ZTV  ov015 0x0211458c  the ADDRESS POINT itself: V-8 is a zero
 *                           offset-to-top, V-4 is &_ZTI, V+0 is slot 0
 *                           (InitResources, 0x021120fc).
 *   size  0x338             both classInit factories' own literal (824).
 * The coined MovingBar alias that used to sit on this vtable is gone; the ROM's
 * own type string is where the class name now comes from.
 *
 * Eight of the 32 words differ from _ZTV10dBgActor_c's and all eight point
 * inside ov015 .text: slots 0, 3, 6, 9, 16, 17, 27 and 31. Every other slot
 * still holds dBgActor_c's word.
 *
 * TEXT ONLY. No delink entry anywhere in ov015 owns a .data or .bss section,
 * so this TU owns nothing but its 25 .text sections. The vtable, both RTTI
 * records, the fourteen pointer-to-member constants at 0x021144a0, the two
 * dispatch tables at 0x021149ec/0x02114a24 and the four SharedFilePtr objects
 * are all declared here and defined nowhere -- defining any of them would make
 * mwcc emit .data or .init that this entry is not allowed to own.
 *
 * A SEVEN-STATE MACHINE, DISPATCHED THROUGH POINTERS TO MEMBERS. Fourteen of
 * the twenty-five functions are its parts: a per-state entry routine that seeds
 * mStateTimer, a per-state body that spends it and installs the next state, and
 * the installer at 0x02111fb8 they all go through. The two dispatch tables are
 * filled by __sinit_ov015_02113048, which is a .init shard of its own and is
 * NOT part of this TU. Nothing in the cartridge names any of the fourteen, so
 * all of them keep the address-derived names symbols.txt gives them and keep C
 * linkage: each state body is the target of a pointer-to-member record in
 * unowned .data that dsd resolves BY NAME, and renaming one without the same
 * commit editing config/arm9/overlays/ov015/symbols.txt links that record as
 * zero with no byte gate noticing.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder;
 * tools/rombuild.py refuses the object outright when the emitted order and the
 * ROM's disagree.
 *
 * Consolidated from twenty-five legacy one-function sources, in ROM address
 * order (basenames only -- every one of them is deleted by this promotion):
 *   [0]  0x02111ba0  _ZN19daObjBk_Dossunbar_cD1Ev.cpp
 *   [1]  0x02111be4  _ZN19daObjBk_Dossunbar_cD0Ev.cpp
 *   [2]  0x02111c3c  _ZN19daObjBk_Dossunbar_c4KillEv.cpp
 *   [3]  0x02111cb8  _ZN19daObjBk_Dossunbar_c15OnHitByMegaCharER6Player.cpp
 *   [4]  0x02111ce0  func_ov015_02111ce0.cpp
 *   [5]  0x02111d28  func_ov015_02111d28.c
 *   [6]  0x02111d4c  func_ov015_02111d4c.c
 *   [7]  0x02111d8c  func_ov015_02111d8c.c
 *   [8]  0x02111d98  func_ov015_02111d98.cpp
 *   [9]  0x02111dd4  func_ov015_02111dd4.c
 *   [10] 0x02111df4  func_ov015_02111df4.c
 *   [11] 0x02111e60  func_ov015_02111e60.c
 *   [12] 0x02111e80  func_ov015_02111e80.c
 *   [13] 0x02111ee0  func_ov015_02111ee0.c
 *   [14] 0x02111eec  func_ov015_02111eec.c
 *   [15] 0x02111f4c  func_ov015_02111f4c.c
 *   [16] 0x02111f6c  func_ov015_02111f6c.c
 *   [17] 0x02111fac  func_ov015_02111fac.c
 *   [18] 0x02111fb8  func_ov015_02111fb8.c
 *   [19] 0x02112004  _ZN19daObjBk_Dossunbar_c16CleanupResourcesEv.cpp
 *   [20] 0x02112068  _ZN19daObjBk_Dossunbar_c6RenderEv.cpp
 *   [21] 0x02112090  _ZN19daObjBk_Dossunbar_c8BehaviorEv.cpp
 *   [22] 0x021120fc  _ZN19daObjBk_Dossunbar_c13InitResourcesEv.cpp
 *   [23] 0x02112230  d_a_obj_bk_dossunbar_bk_dossunbar_l.c
 *   [24] 0x02112260  d_a_obj_bk_dossunbar_bk_dossunbar_s.c
 *
 * BOTH FACTORIES BELONG HERE. daObjBk_Dossunbar_c_classInit_BK_DOSSUNBAR_L and
 * ..._BK_DOSSUNBAR_S sit immediately after InitResources in the ROM's own .text
 * order with no gap and no other class between them; the whole run is bounded
 * below by daObjBk_Botaosi_c_classInit and above by _ZN9TowerStepD1Ev. They
 * keep C linkage and are written first here, being the highest-address members.
 * tools/tu_map.py segments them into a separate two-function unit; that is the
 * known unhandled-_classInit segmentation defect -- the extender looks for a
 * neighbour spelled <class>_classInit while the tree spells factories
 * <ROM-RTTI-name>_classInit -- and it is the same split it made for the sibling
 * oracle daObjBkBillboard_c, whose factory was folded back in for the same
 * reason.
 */

#include "daObjBk_Dossunbar_c.h"
#include "Sound.h"
#include "Player.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

/* Shadow types, carried from the legacy files because each one reaches
 * something this tree has no real declaration for.
 *
 * `C` is the opaque class the fourteen pointer-to-member constants are bound
 * to. It must NOT be replaced by daObjBk_Dossunbar_c: a pointer to member
 * function of a polymorphic class does not have the same representation, and
 * the cartridge's records are the plain {ptr, adj} pair with adj always zero.
 * Its one named field is the state index at 0x330. */
struct C;
typedef void (C::*PMF)();
struct TabEnt { PMF pmf; };
struct C {
    char pad[0x330];
    int idx;
};

/* The installer walks the other table's records by hand, so it needs the same
   pair spelled as data rather than as a pointer to member. */
typedef void (*FnPtr)(void*);
struct VtEntry {
    int field0;
    int field1;
};

/* Render reaches ModelBase's own virtual through the Model subobject. */
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };

/* Externs: the union of the legacy files', kept at their legacy spelling and
 * gathered here above the first member so that no member is charged with
 * another member's ABI seam.
 *
 * dBgW_KcMbg::SetFile, dBgActor_c::IsClsnInRange and Particle::System::NewSimple
 * keep their mangled free spelling: all three carry Fix12<int> BY VALUE, so an
 * ordinary member call would trigger mwccarm's by-value-class parameter homing
 * and change how the caller passes them.
 *
 * data_ov015_02114534 is the two-record resource table indexed by mVariant*0xc;
 * data_ov015_02114538 and data_ov015_0211453c are its +4 and +8 columns and
 * come from decl_common.h. The two legacy files disagreed on how to spell
 * 02114534 -- CleanupResources declared it `char[]`, InitResources `int[]` --
 * and the char spelling wins here because it is what decl_common.h already
 * gives the neighbouring column; InitResources' own `(char *)` cast in front of
 * it is then a no-op and its arithmetic is unchanged.
 *
 * data_ov015_021149ec and data_ov015_02114a24 are the two .bss dispatch tables,
 * filled by __sinit_ov015_02113048 and never defined here. */
extern "C" {
extern char data_ov015_02114534[];
extern TabEnt data_ov015_021149ec[];
extern struct VtEntry data_ov015_02114a24[];

extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, const void *pos);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, Fix12i x, Fix12i y, Fix12i z);
extern void _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(void *, void *);
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *, int, int);
extern int _ZN5Model8LoadFileER13SharedFilePtr(void *);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *, int, int, int);
extern int _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *);
extern int _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *, int, void *, int, int, void *);
extern int _ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;

/* No shared header anywhere in the tree declares these three; every caller
   declares them locally, so this matches the house spelling rather than
   inventing an include. */
extern void Math_Function_0203b14c(int *p, int target, int scale, int max, int extra);
extern int func_01ffb0a4(void *);
extern void func_020393d4(int *p, int v);

/* The state installer, forward-declared because the emitted order is the
   reverse of the source order and its nine callers are written above it. */
void func_ov015_02111fb8(void *self, int idx);

/* The factories' own dependencies, restated here rather than pulled in through
   decl_ActorBase.h / decl_Platform.h as the legacy files did -- this TU
   declares in place, and pulling new decl_*.h headers in changes what the TU
   sees and can perturb members that already match. */
extern void *_ZN7fBase_cnwEj(unsigned size);
extern void _ZN10dBgActor_cC2Ev(void *self);
}

/* The vptr seam, at namespace scope: a namespace-scope variable is not mangled
   under the Itanium ABI, so no linkage block is needed to emit this name
   verbatim. symbols.txt's 0x0211458c IS the public address point, which is why
   the store below takes the symbol's own value. */
extern int _ZTV19daObjBk_Dossunbar_c[];

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- daObjBk_Dossunbar_c_classInit_BK_DOSSUNBAR_S, 0x02112260, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol daObjBk_Dossunbar_c_classInit_BK_DOSSUNBAR_S
/* Reconstructed source-style name: SM64DS proves daObjBk_Dossunbar_c through
   RTTI, allocation size, vtable identity, and the BK_DOSSUNBAR_S registry
   profile; later EAD lineage supplies classInit. Exact original spelling is not
   preserved. Historical alias: MovingBarSmall_Spawn.

   824 = 0x338 = the whole object, which is the literal the class's size assert
   is taken from. One vptr store only: the base here is dBgActor_c and this
   class derives from it directly. The S profile carries actor id 0x36, which
   InitResources reads back as mVariant 0. */
extern "C" int *daObjBk_Dossunbar_c_classInit_BK_DOSSUNBAR_S(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(824);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV19daObjBk_Dossunbar_c; }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- daObjBk_Dossunbar_c_classInit_BK_DOSSUNBAR_L, 0x02112230, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol daObjBk_Dossunbar_c_classInit_BK_DOSSUNBAR_L
/* The same factory for the other registry profile. Historical alias:
   MovingBarBig_Spawn. The L profile carries actor id 0x35, which InitResources
   reads back as mVariant 1. Two factories agreeing on 0x338 and on the same
   vptr is what proves these are two spawn profiles of one class and not two
   classes. */
extern "C" int *daObjBk_Dossunbar_c_classInit_BK_DOSSUNBAR_L(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(824);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV19daObjBk_Dossunbar_c; }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- _ZN19daObjBk_Dossunbar_c13InitResourcesEv, 0x021120fc, size 0x134 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjBk_Dossunbar_c13InitResourcesEv
/* Vtable slot 0. Actor id 0x35 selects the large bar's row of the resource
   table, anything else the small one; the model, the collision mesh and the
   CLPS block all come out of that one row. The three home-position stores are
   the last thing before state 5 is installed. */
int daObjBk_Dossunbar_c::InitResources()
{
  int b = (actorID == 0x35) ? 1 : 0;
  if(b) mVariant = 1; else mVariant = 0;
  int j0 = mVariant * 0xc;
  int m = _ZN5Model8LoadFileER13SharedFilePtr(*(void**)((char*)data_ov015_02114534 + j0));
  _ZN9ModelBase7SetFileEP8BMD_Fileii((char*)&mModel, m, 1, -1);
  _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char*)this));
  _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char*)this));
  int j = mVariant * 0xc;
  int k = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(*(void**)((char*)data_ov015_02114538 + j));
  _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block((char*)&mMeshCollider, k, (char*)&mClsnMat, 0x1000, mAngleY, *(void**)((char*)data_ov015_0211453c + j));
  func_020393d4((int*)&mMeshCollider, (int)&_ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
  int tmp[3];
  tmp[0] = 0x1000; tmp[1] = 0; tmp[2] = 0;
  func_01ffb0a4((char*)&mMeshCollider);
  func_01ffb07c((char*)&mMeshCollider, tmp);
  func_020396d0((int*)&mMeshCollider, 0xccd);
  mHomePosX = mPosX;
  mHomePosY = mPosY;
  mHomePosZ = mPosZ;
  func_ov015_02111fb8(((char*)this), 5);
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- _ZN19daObjBk_Dossunbar_c8BehaviorEv, 0x02112090, size 0x6c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjBk_Dossunbar_c8BehaviorEv
/* Vtable slot 6. One call through the current state's body every frame, then
   the base's own model and collision refresh. mState indexes the .bss table the
   module's static initializer filled with seven of the fourteen constants. */
int daObjBk_Dossunbar_c::Behavior()
{
    (((C *)this)->*(data_ov015_021149ec[mState].pmf))();
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((C *)this));
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((C *)this), 0, 0))
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((C *)this));
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- _ZN19daObjBk_Dossunbar_c6RenderEv, 0x02112068, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjBk_Dossunbar_c6RenderEv
/* Vtable slot 9. */
int daObjBk_Dossunbar_c::Render()
{
 Base *b = (Base *)&mModel; b->m(0); return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- _ZN19daObjBk_Dossunbar_c16CleanupResourcesEv, 0x02112004, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjBk_Dossunbar_c16CleanupResourcesEv
/* Vtable slot 3. Both of this variant's resource-table columns are released,
   so the small and the large bar let go of different files. */
int daObjBk_Dossunbar_c::CleanupResources()
{
    if (((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled())
        ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    ((SharedFilePtr *)(*(void **)(data_ov015_02114534 + mVariant * 0xc)))->Release();
    ((SharedFilePtr *)(*(void **)(data_ov015_02114538 + mVariant * 0xc)))->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov015_02111fb8, 0x02111fb8, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov015_02111fb8
/* THE STATE INSTALLER. Nine callers, all inside this TU and none outside it:
   the seven state bodies and InitResources. It runs the entry routine for the
   NEW state out of the 0x02114a24 table -- by hand, because it walks the
   {ptr, adj} pair itself rather than through a pointer-to-member type -- and
   only then stores the index into mState (0x330). It is reached only by BL,
   which carries no relocation, so a relocation-only scan reports it uncalled. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov015_02111fb8(void* self, int idx) {
    struct VtEntry* e = (struct VtEntry*)((char*)data_ov015_02114a24 + (idx << 3));
    int f1 = e->field1;
    void* obj = (void*)((char*)self + (f1 >> 1));
    FnPtr fn;
    if (f1 & 1) {
        fn = (FnPtr)*(int*)((char*)*(int**)obj + e->field0);
    } else {
        fn = (FnPtr)e->field0;
    }
    fn(obj);
    *(int*)((char*)self + 0x330) = idx;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov015_02111fac, 0x02111fac, size 0xc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov015_02111fac
/* State 0, entry. p[205] is mStateTimer, 0x334. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov015_02111fac(int *p)
{
    p[205] = 20;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov015_02111f6c, 0x02111f6c, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov015_02111f6c
/* State 0, body: spend the timer, then install state 1. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov015_02111f6c(char *c)
{
    *(int *)(((int)c + 0x334)) -= 1;
    if (*(int *)(c + 0x334) > 0)
        return;

    func_ov015_02111fb8(c, 1);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov015_02111f4c, 0x02111f4c, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov015_02111f4c
/* State 1, entry. The parameter was spelled `this` in the C original, which a
   C++ translation unit cannot accept; only the spelling changed. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov015_02111f4c(void *self) {
    *(int *)((char *)self + 0x334) = 10;
    _ZN5Sound9PlayBank3EjRK7Vector3(0xc3, (char *)self + 0x74);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov015_02111eec, 0x02111eec, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov015_02111eec
/* State 1, body: ease X towards home minus 0x168000, then install state 2. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov015_02111eec(char *c)
{
  Math_Function_0203b0fc((int *)(c + 0x5c), *(int *)(c + 0x320) - 0x168000, 0x800, 0x46000);
  {
    int *p = (int *)(((int)c + 0x334));
    *p = *p - 1;
  }
  if (*(int *)(c + 0x334) > 0)
    return;
  *(int *)(c + 0x5c) = *(int *)(c + 0x320) - 0x168000;
  func_ov015_02111fb8(c, 2);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov015_02111ee0, 0x02111ee0, size 0xc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov015_02111ee0
/* State 2, entry. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov015_02111ee0(int *p)
{
    p[205] = 5;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov015_02111e80, 0x02111e80, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov015_02111e80
/* State 2, body: the one place mVariant steers the machine -- state 3 for the
   small bar, state 4 for the large one. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov015_02111e80(int *c)
{
    /* The two spellings of +0x334 are load-bearing under 2004/b56 and must not be
       unified: the ROM keeps the decrement's address in a register (`add r2,r0,#0x334`)
       and then RE-READS the field as `ldr r1,[r0,#0x334]`. Written with one expression
       shape, b56 recognises the second read as the first lvalue and reuses r2. Every
       way of writing the decrement produces the ROM's form; only the re-read decides. */
    *(int *)((int)c + 0x334) -= 1;
    if (*(int *)((char *)c + 0x334) > 0)
        return;
    if (*(int *)((int)c + 0x32c) == 0)
        func_ov015_02111fb8(c, 3);
    else
        func_ov015_02111fb8(c, 4);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov015_02111e60, 0x02111e60, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov015_02111e60
/* State 3, entry. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov015_02111e60(char *c) {
    *(int *)(c + 0x334) = 0x18;
    _ZN5Sound9PlayBank3EjRK7Vector3(0xc3, c + 0x74);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov015_02111df4, 0x02111df4, size 0x6c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov015_02111df4
/* State 3, body: ease X back to home, then install state 5. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov015_02111df4(char *c)
{
    Math_Function_0203b14c((int *)(c + 0x5c), *(int *)(c + 0x320), 0x800, 0xb4000, 0x28000);
    *(int *)(((int)c + 0x334)) =
        *(int *)(((int)c + 0x334)) - 1;
    if (*(int *)(c + 0x334) > 0) {
        return;
    }
    *(int *)(c + 0x5c) = *(int *)(c + 0x320);
    func_ov015_02111fb8(c, 5);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov015_02111dd4, 0x02111dd4, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov015_02111dd4
/* State 4, entry: a speed rather than a timer. The parameter was spelled `this`
   in the C original; only the spelling changed. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov015_02111dd4(unsigned char *self) {
    *(unsigned int *)(self + 0xa4) = 0xf000;
    _ZN5Sound9PlayBank3EjRK7Vector3(0xc3, self + 0x74);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov015_02111d98, 0x02111d98, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov015_02111d98
/* State 4, body: coast under the base's own integrator until X reaches home,
   then clamp and install state 5. dActor_c::UpdatePosWithOnlySpeed keeps its
   mangled free spelling -- its second parameter is a dCc_c the callers pass as
   a null pointer and nothing here has a real declaration for. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov015_02111d98(char *c) {
    _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(c, 0);
    int r2 = *(int *)(c + 0x320);
    int r0 = *(int *)(c + 0x5c);
    if (r0 < r2) return;
    *(int *)(c + 0x5c) = r2;
    func_ov015_02111fb8(c, 5);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov015_02111d8c, 0x02111d8c, size 0xc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov015_02111d8c
/* State 5, entry -- the state InitResources installs, so this is where the
   cycle begins. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov015_02111d8c(int *p)
{
    p[205] = 10;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov015_02111d4c, 0x02111d4c, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov015_02111d4c
/* State 5, body: spend the timer, then install state 6. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov015_02111d4c(char *c)
{
    *(int *)(((int)c + 0x334)) -= 1;
    if (*(int *)(c + 0x334) > 0)
        return;

    func_ov015_02111fb8(c, 6);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov015_02111d28, 0x02111d28, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov015_02111d28
/* State 6, entry: a downward speed. The `recovered name` comment this file
   carried named daObjBk_Botaosi_c, which is the class one run lower; it is
   deleted rather than carried forward. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov015_02111d28(char *self) {
    *(int *)(self + 0xa4) = -0x14000;
    _ZN5Sound9PlayBank3EjRK7Vector3(0xc3, self + 0x74);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov015_02111ce0, 0x02111ce0, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov015_02111ce0
/* State 6, body: fall until X is 0x1ea000 below home, clamp, then close the
   cycle by installing state 0. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov015_02111ce0(char* c){
  _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(c, 0);
  int v=*(int*)(c+0x320)+(int)0xffe16000;
  if(*(int*)(c+0x5c)>v) return;
  *(int*)(c+0x5c)=v;
  func_ov015_02111fb8(c, 0);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN19daObjBk_Dossunbar_c15OnHitByMegaCharER6Player, 0x02111cb8, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjBk_Dossunbar_c15OnHitByMegaCharER6Player
/* Vtable slot 27. Player::IncMegaKillCount is a real method (include/Player.h);
   the trailing unqualified Kill() reaches this class's own slot-31 override
   through the vtable. */
void daObjBk_Dossunbar_c::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    Kill();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN19daObjBk_Dossunbar_c4KillEv, 0x02111c3c, size 0x7c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjBk_Dossunbar_c4KillEv
/* Vtable slot 31, this class's own override: the table's word at +0x7c
 * relocates to 0x02111c3c while _ZTV10dBgActor_c's relocates to 0x020ee55c,
 * dBgActor_c::Kill.
 *
 * Particle 0x10a, spawned 0xc8000 -- two hundred 20.12 units -- above the bar,
 * then the poof at the same point, bank-3 sound 0x41, then the bar destroys
 * itself. The offset is added to the built Vector3 rather than folded into the
 * load, which is the shape daObjFallBlock_c::Kill has and not the one
 * dBgActor_c::Kill has.
 *
 * The second Vector3 is copied MEMBERWISE on purpose: Vector3 declares a
 * destructor (types.h), so a whole-object assignment compiles to an ldm/stm
 * pair, four instructions where the ROM has six. */
void daObjBk_Dossunbar_c::Kill()
{
    Vector3 pos;
    Vector3 dustPos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0xc8000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x10a, pos.x, pos.y, pos.z);
    dustPos.x = pos.x;
    dustPos.y = pos.y;
    dustPos.z = pos.z;
    PoofDustAt(dustPos);
    Sound::PlayBank3(0x41, *(Vector3 *)&mCamSpacePosX);
    MarkForDestruction();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN19daObjBk_Dossunbar_cD0Ev, 0x02111be4, size 0x58        */
/* ROM ordinal 0 -- _ZN19daObjBk_Dossunbar_cD1Ev, 0x02111ba0, size 0x44        */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjBk_Dossunbar_cD1Ev
// @symbol _ZN19daObjBk_Dossunbar_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The destructor body is INLINE in
 * include/daObjBk_Dossunbar_c.h and declared ahead of the out-of-line virtuals.
 * Two measurements force that:
 *   - out of line, mwcc emits D0 ahead of D1 and the cartridge has D1 first,
 *     which rombuild refuses outright;
 *   - out of line it also emits the D2 base-object variant, which the ROM
 *     never carried.
 *
 * Both bodies are short because the chain is short: this class's vptr store,
 * then dBgActor_c's -- inlined, its destructor is defined in its class body --
 * then dBgActor_c's Model and dBgW_KcMbg, then dActor_c. This class adds no
 * member with a destructor of its own. D0's trailing deallocation is the
 * inherited inline operator delete, which is why nothing here names a heap.
 */
