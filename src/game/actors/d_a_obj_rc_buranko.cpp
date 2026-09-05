//cpp
/* Production translation unit for ov036/daObjRcBuranko_c, hand-curated.
 * 8 function(s), .text 0x021111a0..0x02111444.
 *
 * The Rainbow Cruise swinging platform (profile RC_BURANKO): a mesh collider
 * hung like a pendulum, tipped 0x2000 to one side at spawn and swinging back
 * and forth about its Z axis forever.
 *
 * WHAT THE CARTRIDGE PROVES ABOUT THE NAME AND THE SHAPE:
 *   _ZTS  ov036 0x02113a60  "16daObjRcBuranko_c"
 *   _ZTI  ov036 0x02113a54  __si_class_type_info; +8 -> _ZTI10dBgActor_c
 *                           (ov002 0x021089ec), so the DIRECT base is
 *                           dBgActor_c and nothing else.
 *   _ZTV  ov036 0x02113a98  the ADDRESS POINT itself: V-8 is a zero
 *                           offset-to-top, V-4 is &_ZTI, V+0 is slot 0
 *                           (InitResources, 0x0211137c). The vptr store is
 *                           addend-0 against this symbol.
 *   size  0x320             daObjRcBuranko_c_classInit's own literal (800),
 *                           the same 0x320 dBgActor_c already rounds up to:
 *                           this class's one field lives in that tail padding.
 * The coined SwingingPlatform alias that used to sit on this vtable is gone;
 * the ROM's own type string is where the class name now comes from.
 *
 * Only slots 0, 3, 6, 9, 16 and 17 of the cartridge's 32-word table point
 * inside ov036; every other slot still holds dBgActor_c's arm9 word, including
 * slot 31 (Kill, 0x020ee55c), so nothing else is overridden.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder. The
 * destructor is not written here at all; it is inline in the header and so
 * emits last, which is where the cartridge has it.
 *
 * Consolidated from these legacy one-function sources (ROM address order):
 *   [0] 0x021111a0  src/_ZN16daObjRcBuranko_cD1Ev.cpp
 *   [1] 0x021111e4  src/_ZN16daObjRcBuranko_cD0Ev.cpp
 *   [2] 0x0211123c  src/func_ov036_0211123c.c
 *   [3] 0x02111284  src/_ZN16daObjRcBuranko_c16CleanupResourcesEv.cpp
 *   [4] 0x021112c8  src/_ZN16daObjRcBuranko_c6RenderEv.cpp
 *   [5] 0x021112f0  src/_ZN16daObjRcBuranko_c8BehaviorEv.cpp
 *   [6] 0x0211137c  src/_ZN16daObjRcBuranko_c13InitResourcesEv.cpp
 *   [7] 0x02111414  src/daObjRcBuranko_c_classInit.c
 *
 * THE EIGHTH IS THE FACTORY. daObjRcBuranko_c_classInit (historical alias
 * SwingingPlatform_Spawn) is the RC_BURANKO registry profile's spawn function
 * and sits immediately after InitResources in the ROM's own .text order, so it
 * is part of this TU. It was outside it only because the promotion predated
 * the profile-reconstruction campaign. It keeps C linkage and is written first
 * here, being the highest-address member.
 */

#include "daObjRcBuranko_c.h"
#include "SharedFilePtr.h"

/* Externs: the union of the legacy files', kept at their legacy spelling.
 *
 * The two SharedFilePtrs are real bss objects in ov036 and carry no coined
 * alias, so the address-true names stand.
 *
 * func_020393d4 and Matrix4x3_FromRotationXYZExt have no shared header
 * anywhere in the tree -- every caller declares them locally, so this matches
 * the house spelling rather than inventing an include. dBgW_KcMbg::SetFile and
 * dBgActor_c::IsClsnInRange keep their mangled free spelling: both carry
 * Fix12<int> by value, so an ordinary member call would trigger mwccarm's
 * by-value-class parameter homing and change the ROM ABI.
 *
 * func_ov036_0211123c is this class's own private helper -- see its definition
 * at the bottom of this file. It is a global symbol the cartridge calls with
 * `this` from two of the methods below, so it keeps C linkage and its existing
 * name; renaming it would move a symbol other config still names. */
struct KCL_File;
struct CLPS_Block;

extern "C" {
extern SharedFilePtr data_ov036_02114020;   /* the collision KCL  */
extern SharedFilePtr data_ov036_02114028;   /* the platform's BMD */
extern CLPS_Block    data_ov036_02112b68;

void func_ov036_0211123c(char *self);
void func_020393d4(void *bgw, void *fn);
void Matrix4x3_FromRotationXYZExt(void *mtx, int x, int y, int z);
extern int _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_[];

int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, const void *mtx, int scale, s16 angleY, void *clps);

/* The factory's own dependencies, restated here rather than reached through a
   decl_*.h -- this TU declares in place, and pulling in a shared header would
   change what the rest of the TU sees. */
extern void *_ZN7fBase_cnwEj(unsigned size);
extern void _ZN10dBgActor_cC2Ev(void *self);
extern int _ZTV16daObjRcBuranko_c[];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- daObjRcBuranko_c_classInit, 0x02111414, size 0x30         */
/* -------------------------------------------------------------------------- */
// @symbol daObjRcBuranko_c_classInit
/* Reconstructed source-style name: SM64DS proves daObjRcBuranko_c through
   RTTI, allocation size, vtable identity, and the RC_BURANKO registry profile;
   later EAD lineage supplies classInit. Exact original spelling is not
   preserved. Historical alias: SwingingPlatform_Spawn.

   800 = 0x320, the literal the header comment above already reads as this
   class's size: dBgActor_c's own, with this class's one s16 in its tail
   padding. */
extern "C" int *daObjRcBuranko_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(800);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV16daObjRcBuranko_c; }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN16daObjRcBuranko_c13InitResourcesEv, 0x0211137c, size 0x98 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjRcBuranko_c13InitResourcesEv
int daObjRcBuranko_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov036_02114028), 1, -1);

    /* The pendulum starts a quarter of the way over, not hanging straight
       down, so it is already moving on the frame the player first sees it. */
    mAngleZ = 0x2000;
    func_ov036_0211123c((char *)this);
    UpdateClsnPosAndRot();

    void *kcl = dBgW_Kc::LoadFile(data_ov036_02114020);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x1000, mAngleY, &data_ov036_02112b68);

    /* A rider is carried by the collider's whole transform, not by a position
       and a pair of angles: the platform tilts underfoot as it swings. */
    func_020393d4(&mMeshCollider,
        (void *)_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);

    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN16daObjRcBuranko_c8BehaviorEv, 0x021112f0, size 0x8c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjRcBuranko_c8BehaviorEv
int daObjRcBuranko_c::Behavior()
{
    /* The whole pendulum is these four lines: a constant 4-unit acceleration
       always pointed back at zero, integrated into the angle. There is no
       damping term, so the swing never decays and never needs re-kicking. */
    if (mAngleZ < 0)
        mSwingVelZ += 4;
    else
        mSwingVelZ -= 4;

    mAngleZ += mSwingVelZ;

    func_ov036_0211123c((char *)this);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();

    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN16daObjRcBuranko_c6RenderEv, 0x021112c8, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjRcBuranko_c6RenderEv
int daObjRcBuranko_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN16daObjRcBuranko_c16CleanupResourcesEv, 0x02111284, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjRcBuranko_c16CleanupResourcesEv
int daObjRcBuranko_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov036_02114028.Release();
    data_ov036_02114020.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov036_0211123c, 0x0211123c, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov036_0211123c
/* This class's private "put the model where the actor is" helper, and the
 * reason the swing is visible at all: it rebuilds mModel's own Matrix4x3
 * (mModel + 0x1c, object offset 0xf0) from the actor's three angles, then
 * writes the actor position >> 3 into that matrix's translation row (0x114).
 *
 * KEPT AT RAW OFFSETS ON PURPOSE. The cartridge gives it C linkage and a
 * global symbol -- it is called with `this` from InitResources and Behavior
 * above, and other config still names it -- so it is not yet a member, and the
 * offsets are what the delinked body actually spells. The comments say what
 * each one is; promoting it to a real method is a separate change with its own
 * codegen risk. */
extern "C" {
void func_ov036_0211123c(char *self)
{
    /* mModel.mat4x3 <- rotation from (mAngleX, mAngleY, mAngleZ) */
    Matrix4x3_FromRotationXYZExt(self + 0xf0,
        *(short *)(self + 0x8c), *(short *)(self + 0x8e), *(short *)(self + 0x90));

    /* ... and its translation row <- (mPosX, mPosY, mPosZ) >> 3 */
    *(int *)(self + 0x114) = *(int *)(self + 0x5c) >> 3;
    *(int *)(self + 0x118) = *(int *)(self + 0x60) >> 3;
    *(int *)(self + 0x11c) = *(int *)(self + 0x64) >> 3;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN16daObjRcBuranko_cD0Ev, 0x021111e4, size 0x58        */
/* ROM ordinal 0 -- _ZN16daObjRcBuranko_cD1Ev, 0x021111a0, size 0x44        */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjRcBuranko_cD1Ev
// @symbol _ZN16daObjRcBuranko_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The destructor body is INLINE in
 * include/daObjRcBuranko_c.h and declared FIRST. Two measurements force that:
 *   - out of line, mwcc emits D0 ahead of D1 and the cartridge has D1 first,
 *     which rombuild refuses outright;
 *   - out of line it also emits the D2 base-object variant, which the ROM
 *     never carried.
 * Declaring it first is what makes this TU the vtable's home, so _ZTV and the
 * RTTI pair land here rather than in whichever other TU happens to name them.
 *
 * Both bodies are short because the chain is short: this class's vptr store,
 * then dBgActor_c's -- inlined, its destructor is defined in its class body --
 * then dBgActor_c's Model and dBgW_KcMbg, then dActor_c. This class adds only
 * an s16, which has no destructor of its own. D0's trailing deallocation is
 * the inherited inline operator delete, which is why nothing here names a
 * heap.
 */
