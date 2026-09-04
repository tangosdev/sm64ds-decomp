//cpp
/* ov052/daObjEmmLog_c -- the rolling log of Tiny-Huge Island, one file.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here and the lowest LAST. Do
 * not reorder.
 *
 * THE DESTRUCTOR IS A REAL C++ MEMBER, `virtual ~daObjEmmLog_c() {}` in
 * include/daObjEmmLog_c.h, defined INLINE in the class body on purpose: out of
 * line, mwccarm emits D0 before D1 -- the reverse of the cartridge's
 * 0x021111a0 D1 / 0x021111e4 D0 -- plus a homeless D2, and the whole-range
 * link refuses the TU with `licensed .text functions are not emitted in ROM
 * address order`. In the class body it emits D1 then D0, the ROM's own order,
 * and no D2 at all. Same lever, same reason, as daObjRc_Dorifu_c.
 *
 * BOTH VARIANTS ARE COMPILER-GENERATED, so nothing below defines D1 or D0 --
 * the ordinal markers just record where each one's bytes come from. The source
 * body is genuinely empty: this class adds no member with a destructor
 * (mSpinAngle/mBasePosY/mBobAmplitude are plain integers), and the two vptr
 * stores plus three destructor calls in the ROM are all base work, dBgActor_c's
 * own destructor being inline per include/dBgActor_c.h.
 *
 * THE VPTR SPELLING IS `&_ZTV13daObjEmmLog_c[2]`, and the bare symbol is
 * refused with `unexpected reloc type=2 addend=0`. That is tools/objisolate.py:472
 * saying this object took the EXTERNALISE path, which converts mwccarm's
 * object-start `_ZTV` into the ROM's address point by subtracting
 * VTABLE_PREAMBLE (8) and so requires addend >= 8. Other classes take the
 * intact-multi-symbol path and demand the bare symbol instead (#2184's
 * daObjWc_Mizu_c), so neither spelling is "the" convention -- the two refusal
 * messages are the tool telling you which path your object took. Either way
 * the emitted relocation is addend 0, and ov052:0x02112520 already IS the
 * address point: overlay_0052.bin at load base 0x021111a0 holds 0 at 0x02112518
 * (offset-to-top), 0x021124e0 at 0x0211251c (&_ZTI), and 0x02111348 at
 * 0x02112520 (slot 0, InitResources).
 *
 * Absorbed from these legacy one-function sources (ROM address order):
 *   [0] 0x021111a0  src/_ZN13daObjEmmLog_cD1Ev.cpp
 *   [1] 0x021111e4  src/_ZN13daObjEmmLog_cD0Ev.cpp
 *   [2] 0x0211123c  src/_ZN13daObjEmmLog_c16CleanupResourcesEv.cpp
 *   [3] 0x02111284  src/_ZN13daObjEmmLog_c6RenderEv.cpp
 *   [4] 0x021112ac  src/_ZN13daObjEmmLog_c8BehaviorEv.cpp
 *   [5] 0x02111348  src/_ZN13daObjEmmLog_c13InitResourcesEv.cpp
 *   [6] 0x02111410  src/daObjEmmLog_c_Spawn.c
 */

#include "daObjEmmLog_c.h"
#include "SharedFilePtr.h"   /* CleanupResources releases the two shared files by name */

/* The class layout and every method it inherits come from
 * include/daObjEmmLog_c.h and its base chain. What no header supplies is
 * declared here: the three-pointer resource block ov052 keeps at 0x021124d4,
 * whose shape no header names, and then four free symbols -- the arm9 sine
 * table data_02082214, the unnamed arm9 collision helper func_020393a4, the
 * two allocator/base-constructor entry points, and dBgActor_c::IsClsnInRange,
 * which include/dBgActor_c.h discusses in a comment but does not declare. */

struct daObjEmmLog_c_Resources {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};

extern "C" {
void func_020393a4(int *clsn, int v);
/* The name claims Fix12<int> by value; the bytes take a plain word, the same
disagreement include/dBgActor_c.h records for both IsClsnInRange symbols. */
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int range, int b);
extern short data_02082214[];
extern daObjEmmLog_c_Resources data_ov052_021124d4;
extern "C" void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block( dBgW_KcMbg *, KCL_File *, const Matrix4x3 *, Fix12i, s16, CLPS_Block *);
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern int _ZTV13daObjEmmLog_c[];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- daObjEmmLog_c_Spawn, 0x02111410, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol daObjEmmLog_c_Spawn
extern "C" {  /* .c-derived member: C linkage for the whole block */
int *daObjEmmLog_c_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(808);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)&_ZTV13daObjEmmLog_c[2]; }
    return p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN13daObjEmmLog_c13InitResourcesEv, 0x02111348, size 0xc8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjEmmLog_c13InitResourcesEv
/* 2004/b56 homes the real Fix12<int>-by-value member form through a literal
 * pool instead of passing the immediate found in the ROM. Keep this one ABI
 * seam until the shared fixed-point compiler wall is solved. */
int daObjEmmLog_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(*data_ov052_021124d4.model), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider,
        (KCL_File *)dBgW_Kc::LoadFile(*data_ov052_021124d4.collision),
        &mClsnMat, 0x1000, mAngleY, data_ov052_021124d4.clps);

    mBasePosY = mPosY;
    u8 amplitude = param1 & 0xff;
    if (amplitude == 0xff || param1 == 0)
        mBobAmplitude = 0x64000;
    else
        mBobAmplitude = amplitude * 0xa000;

    mSpinAngle = mAngleX;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN13daObjEmmLog_c8BehaviorEv, 0x021112ac, size 0x9c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjEmmLog_c8BehaviorEv
/* daObjEmmLog_c::Behavior -- vtable slot 6. The rolling log of Tiny-Huge
 * Island: it spins at a fixed rate and rides up and down on a sine of its own
 * spin angle, mBobAmplitude being the amplitude and mBasePosY the centre height. */
int daObjEmmLog_c::Behavior()
{
    func_020393a4((int *)&mMeshCollider, 0x600000);

    int idx = (u16)mSpinAngle >> 4;
    int s = *(short *)((char *)data_02082214 + (idx << 2));
    int m = (int)(((long long)mBobAmplitude * s + 0x800) >> 12);
    mPosY = mBasePosY + m;

    mSpinAngle += 0x200;

    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0x600000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN13daObjEmmLog_c6RenderEv, 0x02111284, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjEmmLog_c6RenderEv
/* daObjEmmLog_c::Render -- vtable slot 9 (overlay_0052.bin at 0x02112544 holds
 * 0x02111284; slot 4 is an arm9 method this class does not override). Hands the
 * whole job to the inherited
 * Model at +0xd4, through its own vtable: the ROM loads the model's vptr and
 * calls slot 5, which include/Model.h records as Render(const Vector3 *), with
 * a null scale. */
int daObjEmmLog_c::Render() { mModel.Render(0); return 1; }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN13daObjEmmLog_c16CleanupResourcesEv, 0x0211123c, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjEmmLog_c16CleanupResourcesEv
/* daObjEmmLog_c::CleanupResources -- vtable slot 3. Takes the inherited mesh
 * collider out of the collision world if it is still in it, then releases the
 * two shared files the log holds. */
int daObjEmmLog_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    data_ov052_021124d4.model->Release();
    data_ov052_021124d4.collision->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN13daObjEmmLog_cD0Ev, 0x021111e4, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjEmmLog_cD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases, then
 * return the object to its heap. The deallocation is an inline operator delete
 * -- the ROM's `bl 0x0203c1e8` is _ZN6Memory10DeallocateEPvP4Heap -- matching
 * BigBrickBlock's D0 (include/BigBrickBlock.h, src/_ZN13BigBrickBlockD0Ev.cpp).
 *
 * (No definition here, and none anywhere in this file: the one definition is
 * `virtual ~daObjEmmLog_c() {}` in include/daObjEmmLog_c.h, pulled in by the
 * include at the top. It emits D1 and D0 together -- and no D2 -- so this file
 * is licensed for both variants; see the banner for why the in-class spelling
 * is the only one the link accepts.) */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN13daObjEmmLog_cD1Ev, 0x021111a0, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjEmmLog_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Two vtable stores and three destructor calls, every one a consequence of
 * `struct daObjEmmLog_c : dBgActor_c`: its own vptr, then dBgActor_c's --
 * inlined, because dBgActor_c's destructor is defined in its class body --
 * then dBgActor_c's members in reverse declaration order, dBgW_KcMbg at +0x124
 * (0x021111b4: add r0, r4, #0x124) before Model at +0xd4 (0x021111c0), and then
 * dActor_c. This class adds no member with a destructor of its own
 * (mSpinAngle/mBasePosY/mBobAmplitude are plain integers).
 */
/* (no definition here: `virtual ~daObjEmmLog_c() {}` is in
 * include/daObjEmmLog_c.h. That one definition emits both D1 and D0, and only
 * the in-class spelling emits them in ROM-ascending order -- see the comment on
 * that declaration.) */
