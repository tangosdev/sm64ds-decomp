//cpp
/* Lethal Lava Land: the rolling log (daObjFlMaruta_c, spelled RollingLogLll in
 * this tree) and the volcano cannon's flame (daObj_volcanoCannon_c), in the one
 * translation unit the linker says they came from.
 *
 * WHY THE TWO CLASSES ARE ONE FILE. daObjFlMaruta_c's vtable (ov022 0x021143a4)
 * claims two slots whose code sits in the MIDDLE of daObj_volcanoCannon_c's
 * address run: slot 32 -> 0x021126ac and slot 34 -> 0x02112710, both between
 * daObj_volcanoCannon_c's D0 at 0x02112610 and its ChangeState at 0x02112790.
 * A translation unit occupies a contiguous address run, so the two classes'
 * runs overlap and neither can be carved out alone. The merged run is
 * 0x02112498..0x02112950, which is the end of this overlay's .text.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S. mwccarm 2004/b56 emits one .text
 * section per function in the reverse of source order, so the highest-address
 * ROM function is written first here. Do not reorder. A destructor's D0/D1/D2
 * group is the documented exception: the compiler picks that group's order.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [ 0] 0x02112498  src/_ZN13RollingLogLllD1Ev.cpp
 *   [ 1] 0x021124e8  src/_ZN13RollingLogLllD0Ev.cpp
 *   [ 2] 0x0211254c  src/_ZN13RollingLogLll16CleanupResourcesEv.cpp
 *   [ 3] 0x02112560  src/_ZN13RollingLogLll8BehaviorEv.cpp
 *   [ 4] 0x02112590  src/_ZN13RollingLogLll13InitResourcesEv.cpp
 *   [ 5] 0x021125a4  src/d_a_obj_fl_maruta.c
 *   [ 6] 0x021125e0  src/_ZN21daObj_volcanoCannon_cD1Ev.cpp
 *   [ 7] 0x02112610  src/_ZN21daObj_volcanoCannon_cD0Ev.cpp
 *   [ 8] 0x02112654  src/func_ov022_02112654.cpp
 *   [ 9] 0x021126ac  src/func_ov022_021126ac.c
 *   [10] 0x02112710  src/func_ov022_02112710.c
 *   [11] 0x02112790  src/_ZN21daObj_volcanoCannon_c11ChangeStateEPNS_5StateE.cpp
 *   [12] 0x021127e0  src/_ZN21daObj_volcanoCannon_c16CleanupResourcesEv.cpp
 *   [13] 0x02112800  src/_ZN21daObj_volcanoCannon_c8BehaviorEv.cpp
 *   [14] 0x021128b8  src/_ZN21daObj_volcanoCannon_c13InitResourcesEv.cpp
 *   [15] 0x02112918  src/d_a_obj_volcano_cannon.c
 *
 * Reconciled while merging: the legacy shards declared
 * _ZN7fBase_c18MarkForDestructionEv with two different return types (int in one,
 * void in the other) and func_ov080_0212714c as unsigned char against
 * decl_common.h's int. Both results are discarded or masked at every call site
 * here, so one spelling each is kept.
 *
 * Also changed by the merge, and load-bearing: both factories now take the vptr
 * as `&_ZTV<C>[2]` rather than `_ZTV<C>`. A one-function file referenced the
 * vtable as an undefined symbol, and the ROM's symbols.txt convention makes
 * _ZTV<C> the slot array itself, so a bare reference was already the vptr. This
 * TU defines both vtables -- it is the key-function TU for both classes -- and
 * mwccarm's own _ZTV<C> addresses the START of the vtable object, two words
 * earlier. `[2]` steps over the offset-to-top and typeinfo words. The .text
 * bytes are identical either way because the word is relocated, so only
 * tools/objisolate.py's relocation check can see the difference; it reads the
 * bare form as an addend of 0 and refuses it.
 */

#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_dCcAc_c.h"
#include "decl_common.h"

#include "RollingLogLll.h"
#include "daObj_volcanoCannon_c.h"

struct Vector3_16f;

extern "C" {

/* daObj_volcanoCannon_c's collaborators. The dCcAc_c::Init signature is
   deliberately the local spelling: the ROM name carries by-value class
   parameters (Fix12<int>), which mwccarm passes differently at the call site,
   so declaring the true types breaks the byte match. */
unsigned short DecIfAbove0_Short(unsigned short *p);
void _ZN8dActor_c9UpdatePosEP5dCc_c(void *a, dCc_c *c);
void _ZN5dCc_c5ClearEv(void *c);
void _ZN5dCc_c6UpdateEv(void *c);
unsigned int _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
    unsigned int a, unsigned int b, Fix12i c, Fix12i d, Fix12i e, const Vector3_16f *f);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    void *, dActor_c *a, int b, int c, unsigned int d, unsigned int e);

/* daObjFlMaruta_c's collaborators. */
void func_020393a4(void *p, int v);
extern int data_ov022_02112c98[];
extern int _ZTV13RollingLogLll[];
extern int _ZTV13daObjMaruta_c[];

/* Shared by the two flame helpers below. */
int _ZN7fBase_c18MarkForDestructionEv(void *c);
int RandomIntInternal(int *seed);
extern int data_0209e650;
void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
void _ZN6Player4BurnEv(void *p);
void func_ov022_02112654(char *c);

}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- daObj_volcanoCannon_c_classInit, 0x02112918, size 0x38     */
/* -------------------------------------------------------------------------- */
// @symbol daObj_volcanoCannon_c_classInit
/* The factory stays hand-spelled because a natural `new daObj_volcanoCannon_c()`
 * makes mwccarm call unresolved `_Znwm`; the ROM calls the class allocator
 * `_ZN7fBase_cnwEj`. This form preserves that destination exactly.
 *
 * Reconstructed source-style name: SM64DS proves daObj_volcanoCannon_c through
 * RTTI, allocation size, vtable identity, and the OBJ_VOLCANO_CANNON registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: VolcanoFire_Spawn. */
extern "C" int *daObj_volcanoCannon_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(0x11c);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)&_ZTV21daObj_volcanoCannon_c[2];
        _ZN7dCcAc_cC1Ev((char *)p + 0xd4);
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- _ZN21daObj_volcanoCannon_c13InitResourcesEv, 0x021128b8    */
/* -------------------------------------------------------------------------- */
// @symbol _ZN21daObj_volcanoCannon_c13InitResourcesEv
int daObj_volcanoCannon_c::InitResources()
{
    mTerminalVelocity = -0xc8000;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mCylinderClsn, this, 0x1e000, 0x1e000, 0x200002, 0);
    ChangeState((State *)&data_ov022_02114690);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- _ZN21daObj_volcanoCannon_c8BehaviorEv, 0x02112800          */
/* -------------------------------------------------------------------------- */
// @symbol _ZN21daObj_volcanoCannon_c8BehaviorEv
int daObj_volcanoCannon_c::Behavior()
{
    DecIfAbove0_Short(&mKillTimer);
    if (mState->behavior != 0)
        (this->*mState->behavior)();
    _ZN8dActor_c9UpdatePosEP5dCc_c(this, &mCylinderClsn);
    _ZN5dCc_c5ClearEv(&mCylinderClsn);
    _ZN5dCc_c6UpdateEv(&mCylinderClsn);
    {
        int b = (int)((mFlags & 8) != 0);
        if (b == 0) {
            mParticleID = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
                mParticleID, 0x129, mPosX, mPosY, mPosZ, 0);
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- _ZN21daObj_volcanoCannon_c16CleanupResourcesEv, 0x021127e0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN21daObj_volcanoCannon_c16CleanupResourcesEv
/* Vtable slot 3. The only one of these leaves that owns no files: it hands its
 * slot back to the spawner that made it, by decrementing the live-flame count
 * that spawner keeps at +0x324. mSpawner is null for a flame that was never
 * registered, hence the guard. The spawner's own class is not described by any
 * header yet, so that offset stays raw. */
int daObj_volcanoCannon_c::CleanupResources()
{
    if (mSpawner)
        *(u16 *)((char *)mSpawner + 0x324) -= 1;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- _ZN21daObj_volcanoCannon_c11ChangeStateEPNS_5StateE        */
/* -------------------------------------------------------------------------- */
// @symbol _ZN21daObj_volcanoCannon_c11ChangeStateEPNS_5StateE
/* The state record contains two pointer-to-member callbacks; the transition
 * stores the record and runs its init. */
int daObj_volcanoCannon_c::ChangeState(State *state)
{
    mState = state;
    if (mState->init == 0)
        return 1;
    return (this->*mState->init)();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov022_02112710, 0x02112710, size 0x80                 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov022_02112710
/* daObjFlMaruta_c vtable slot 34 (ov022 0x021143a4, 34 words in). Rolls the log
 * back to a randomised start: a 0x3c timer, a random facing, and randomised
 * speed and range in 12-bit fixed point. */
extern "C" int func_ov022_02112710(char *c)
{
    unsigned int r = (unsigned int)RandomIntInternal(&data_0209e650) >> 8;
    int *p98;
    int *pa8;
    *(short *)(c + 0x110) = 0x3c;
    *(short *)(c + 0x94) = (short)((r & 0xf) << 0xc);
    *(int *)(c + 0x98) = (r & 0x1f) + 0x14;
    p98 = (int *)((int)c + 0x98);
    *p98 = *p98 << 0xc;
    *(int *)(c + 0x9c) = -0x4000;
    *(int *)(c + 0xa8) = (r & 0x3f) + 0x28;
    pa8 = (int *)((int)c + 0xa8);
    *pa8 = *pa8 << 0xc;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov022_021126ac, 0x021126ac, size 0x64                  */
/* -------------------------------------------------------------------------- */
// @symbol func_ov022_021126ac
/* daObjFlMaruta_c vtable slot 32 (ov022 0x021143a4, 32 words in). Historical
 * alias: daObjFlMaruta_c_AfterClsn. */
extern "C" int func_ov022_021126ac(char *c)
{
    int a = *(int *)(c + 0x60);
    int b = *(int *)(c + 0x118);
    if (a < b) {
        _ZN7fBase_c18MarkForDestructionEv(c);
        return 1;
    }
    unsigned short h = *(unsigned short *)(c + 0x100 + 0x10);
    if (h == 0) {
        _ZN7fBase_c18MarkForDestructionEv(c);
        return 1;
    }
    func_ov022_02112654(c);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov022_02112654, 0x02112654, size 0x58                  */
/* -------------------------------------------------------------------------- */
// @symbol func_ov022_02112654
/* Not in either vtable: a helper of the slot-32 method above. Burns the player
 * if the actor this one is tracking by ID is Mario (actor type 0xbf), then
 * retires itself. */
extern "C" void func_ov022_02112654(char *c)
{
    if (*(unsigned int *)(c + 0xf8) == 0) return;
    void *a = _ZN8dActor_c10FindWithIDEj(*(unsigned int *)(c + 0xf8));
    if (a == 0) return;
    unsigned int b = *(unsigned short *)((char *)a + 0xc) == 0xbf;
    if (b == 0) return;
    _ZN6Player4BurnEv(a);
    _ZN7fBase_c18MarkForDestructionEv(c);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN21daObj_volcanoCannon_cD0Ev, 0x02112610, size 0x44       */
/* -------------------------------------------------------------------------- */
// @symbol _ZN21daObj_volcanoCannon_cD0Ev
/* NO BODY HERE, DELIBERATELY. daObj_volcanoCannon_c's destructor is defined
 * inline in the class body (include/daObj_volcanoCannon_c.h), so this TU has no
 * out-of-line destructor source to carry. mwccarm emits D1 and D0 anyway,
 * because this TU defines the class's key function -- InitResources, the first
 * non-inline virtual the class declares. That emits _ZTV21daObj_volcanoCannon_c,
 * whose slots 16 and 17 reference the two destructor variants.
 *
 * While these two functions lived in one-function files, each needed a forcing
 * helper of its own -- a `delete` expression for D0, an explicit destructor call
 * for D1 -- because neither file emitted the vtable. Folding the TU retires both
 * helpers, which compiled to two global functions the ROM does not contain.
 *
 * D0 is the deleting variant; its deallocation is dActor_c's inline operator
 * delete, which is why no heap call appears in it. */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN21daObj_volcanoCannon_cD1Ev, 0x021125e0, size 0x30       */
/* -------------------------------------------------------------------------- */
// @symbol _ZN21daObj_volcanoCannon_cD1Ev
/* NO BODY HERE either, for the reason given just above: D1 is the complete-object
 * variant of the same inline class-body destructor, emitted from the same vtable
 * reference. Both markers are parked at their ROM ordinals so the address order
 * of this file stays readable; the next definition below carries its own marker,
 * so neither of these lends its name to it. */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- daObjFlMaruta_c_classInit, 0x021125a4, size 0x3c            */
/* -------------------------------------------------------------------------- */
// @symbol daObjFlMaruta_c_classInit
/* vtable identified: VT0 = _ZTV13daObjMaruta_c, then this class's own.
 *
 * Reconstructed source-style name: SM64DS proves daObjFlMaruta_c through RTTI,
 * allocation size, most-derived vtable identity, and the FL_MARUTA registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. The project's RollingLogLll implementation aliases remain
 * unchanged. Historical alias: RollingLogLll_Spawn. */
extern "C" int *daObjFlMaruta_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(836);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV13daObjMaruta_c;
        p[0] = (int)&_ZTV13RollingLogLll[2];
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN13RollingLogLll13InitResourcesEv, 0x02112590, size 0x14  */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13RollingLogLll13InitResourcesEv
int RollingLogLll::InitResources()
{
    return func_ov080_021274ac(((void *)this), data_ov022_02112c9c);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN13RollingLogLll8BehaviorEv, 0x02112560, size 0x30        */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13RollingLogLll8BehaviorEv
int RollingLogLll::Behavior()
{
    func_020393a4(((char *)this) + 0x124, 0x500000);
    return func_ov080_0212714c(((char *)this), data_ov022_02112c98) & 0xff;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN13RollingLogLll16CleanupResourcesEv, 0x0211254c          */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13RollingLogLll16CleanupResourcesEv
int RollingLogLll::CleanupResources()
{
    return func_ov080_021270dc(((void *)this), data_ov022_02112c9c);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinals 1 and 0 -- the destructor pair, lowest addresses in the TU      */
/*   0x021124e8  _ZN13RollingLogLllD0Ev, size 0x64                              */
/*   0x02112498  _ZN13RollingLogLllD1Ev, size 0x50                              */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13RollingLogLllD0Ev
// @symbol _ZN13RollingLogLllD1Ev
/* One out-of-line definition; mwccarm emits the D1 and D0 variants from it and
 * picks their relative order itself. THREE vtable stores, and the middle one is
 * the finding: `RollingLogLll : daObjMaruta_c : dBgActor_c` emits its own vptr,
 * then daObjMaruta_c's -- inlined, because that destructor is defined in its
 * class body -- then dBgActor_c's, then dBgActor_c's dBgW_KcMbg and Model, then
 * dActor_c. Nothing in the chain adds a member with a destructor, so the body is
 * empty. D0's deallocation is an inline operator delete, which is why nothing
 * below mentions a heap. */
RollingLogLll::~RollingLogLll()
{
}
