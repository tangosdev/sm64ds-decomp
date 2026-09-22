//cpp
/* Production translation unit for ov012/daObjC0_Switch_c, 8 function(s),
 * .text 0x021111a0..0x02111450.
 *
 * Castle basement switch pillar (profile C0_SWITCH / SWITCH_PILLAR, actor 34).
 * ov012 also has BASEMENT_WATER(35); this class is the pillar, not the water.
 *
 * WHAT THE CARTRIDGE PROVES ABOUT THE NAME AND THE SHAPE:
 *   _ZTS  ov012 0x0211230c  "16daObjC0_Switch_c"
 *   _ZTI  ov012 0x02112300  __si_class_type_info; +8 -> _ZTI10dBgActor_c
 *                           (ov002 0x021089ec), so the DIRECT base is
 *                           dBgActor_c and nothing else.
 *   _ZTV  ov012 0x02112344  the ADDRESS POINT: V-8 is a zero offset-to-top,
 *                           V-4 is &_ZTI, V+0 is slot 0 (InitResources,
 *                           0x02111370).
 *   size  0x320             daObjC0_Switch_c_classInit's own literal, which is
 *                           dBgActor_c's own size too: mPressed lives in the
 *                           base's tail padding at 0x31e.
 * The coined `SwitchPillar` alias that used to sit on this vtable is gone; the
 * ROM's own type string is where the class name now comes from.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder. The
 * destructor is not written here at all; it is inline in
 * include/daObjC0_Switch_c.h, declared last, and so emits first, which is
 * where the cartridge has it.
 *
 * deslop leftovers:
 * - dBgW_KcMbg::SetFile 6az: InitResources' by-value Fix12<int> scale makes
 *   the header method form size-DIFF.
 * - dBgActor_c::IsClsnInRange 6az: Behavior's two by-value Fix12<int>
 *   parameters; the header method form is refused by the bytes
 *   (include/dBgActor_c.h).
 * - data_ov012_021124a8 / 021124a0 are this overlay's BMD/KCL handles
 *   (sinit-owned BSS). data_ov012_02111cd0 is the CLPS block this TU does
 *   not own. data_0209caa0 is an arm9 shared-flags global.
 *
 * Consolidated from these legacy one-function sources (ROM address order):
 *   [0] 0x021111a0  src/_ZN16daObjC0_Switch_cD1Ev.cpp
 *   [1] 0x021111e4  src/_ZN16daObjC0_Switch_cD0Ev.cpp
 *   [2] 0x0211123c  src/_ZN16daObjC0_Switch_c15OnGroundPoundedER8dActor_c.cpp
 *   [3] 0x021112ec  src/_ZN16daObjC0_Switch_c16CleanupResourcesEv.cpp
 *   [4] 0x02111324  src/_ZN16daObjC0_Switch_c6RenderEv.cpp
 *   [5] 0x0211134c  src/_ZN16daObjC0_Switch_c8BehaviorEv.cpp
 *   [6] 0x02111370  src/_ZN16daObjC0_Switch_c13InitResourcesEv.cpp
 *   [7] 0x02111420  src/daObjC0_Switch_c_Spawn.c
 */

/* daObjC0_Switch_c.h FIRST: it pulls in dBgActor_c.h, which must reach
   common.h ahead of Model.h or the wrong Matrix4x3 spelling wins. */
#include "daObjC0_Switch_c.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr data_ov012_021124a8;
extern SharedFilePtr data_ov012_021124a0;
extern int data_ov012_02111cd0[];
extern int data_0209caa0[];

/* dBgActor_c::IsClsnInRange(Fix12<int>, Fix12<int>) -- wall 6az on Behavior. */
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
/* dBgW_KcMbg::SetFile -- wall 6az on InitResources. */
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat, int scale,
    s16 angY, void *clps);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjC0_Switch_c_classInit
/* Reconstructed source-style name: SM64DS proves daObjC0_Switch_c through RTTI,
 * allocation size, vtable identity, and the C0_SWITCH registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: daObjC0_Switch_c_Spawn.
 *
 * Every instruction the cartridge has here falls out of the one `new`.
 * 0x02111424 loads 800 = 0x320 -- the class's own size -- into the header's
 * leaf operator new; 0x02111434 calls dBgActor_c's C2 and the store at
 * 0x02111440 lays down this class's vptr. The null check is the one `new`
 * itself emits. The implicit constructor is what keeps the intervening
 * dBgActor_c construction inlined; declaring a constructor of our own would
 * emit a `bl` the factory does not have. */
extern "C" daObjC0_Switch_c *daObjC0_Switch_c_classInit()
{
    return new daObjC0_Switch_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjC0_Switch_c13InitResourcesEv
/* daObjC0_Switch_c::InitResources -- vtable slot 0, ov012 0x02111370.
 *
 * mModel/mMeshCollider/mClsnMat/mAngleY are dBgActor_c's/dActor_c's own named
 * fields. Model::LoadFile, ModelBase::SetFile, dBgW_Kc::LoadFile,
 * UpdateModelPosAndRotY and UpdateClsnPosAndRot are real method calls.
 * dBgW_KcMbg::SetFile stays mangled -- it takes Fix12<int> BY VALUE
 * (mwccarm-codegen.md 6az).
 *
 * mPosY -= 0x64000 and mPressed = 1 only run when data_0209caa0[2] & 0x80000
 * is set -- some other pillar in the group already reported pressed (see
 * OnGroundPounded), so this one starts sunk and pre-flagged. */
int daObjC0_Switch_c::InitResources()
{
    void *mdl = Model::LoadFile(data_ov012_021124a8);
    mModel.SetFile((BMD_File *)mdl, 1, -1);
    if (data_0209caa0[2] & 0x80000) {
        mPosY -= 0x64000;
        mPressed = 1;
    }
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    void *kcl = dBgW_Kc::LoadFile(data_ov012_021124a0);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, (KCL_File *)kcl, &mClsnMat, 0x199, mAngleY,
        data_ov012_02111cd0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjC0_Switch_c8BehaviorEv
s32 daObjC0_Switch_c::Behavior()
{
    _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjC0_Switch_c6RenderEv
s32 daObjC0_Switch_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjC0_Switch_c16CleanupResourcesEv
s32 daObjC0_Switch_c::CleanupResources()
{
    mMeshCollider.Disable();
    data_ov012_021124a8.Release();
    data_ov012_021124a0.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjC0_Switch_c15OnGroundPoundedER8dActor_c
/* daObjC0_Switch_c::OnGroundPounded -- vtable slot 21, ov012 0x0211123c.
 *
 * mPosY and mPressed are the same fields InitResources touches.
 * dActor_c::FindWithActorID walks every actor of a given actorID (0x22, this
 * class's own -- SWITCH_PILLAR 34). This one no-ops if already pressed;
 * otherwise it sinks, marks itself pressed, then returns at the first
 * non-this actorID-0x22 pillar and, if that one is already pressed, sets
 * the shared group flag in data_0209caa0[2] that InitResources reads. */
void daObjC0_Switch_c::OnGroundPounded(dActor_c &other)
{
    if (mPressed) return;
    mPosY -= 0x64000;
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    mPressed = 1;
    dActor_c *p = dActor_c::FindWithActorID(0x22, 0);
    if (p == 0) return;
check:
    if (p != (dActor_c *)this) {
        if (((daObjC0_Switch_c *)p)->mPressed) {
            data_0209caa0[2] |= 0x80000;
        }
        return;
    }
    p = dActor_c::FindWithActorID(0x22, p);
    if (p != 0) goto check;
}

/* -------------------------------------------------------------------------- */
/* _ZN16daObjC0_Switch_cD0Ev 0x021111e4 size 0x58 -- are NOT written here.      */
/*                                                                             */
/* The destructor body is INLINE in include/daObjC0_Switch_c.h and declared     */
/* LAST. Two measurements force that:                                           */
/*   - out of line, mwcc emits D0 ahead of D1 and the cartridge has D1 first,   */
/*     which rombuild refuses outright;                                         */
/*   - out of line it also emits the D2 base-object variant, which the ROM      */
/*     never carried.                                                           */
/* Declaring it last, with the factory's `new` instantiating the class, is      */
/* what emits the used D1/D0 pair in cartridge order and keeps this TU the      */
/* vtable's home. The body is empty in the ROM too: D1 at 0x021111a0 stores     */
/* the vtable and tail-calls ~dBgActor_c, with no member teardown.              */
/* -------------------------------------------------------------------------- */
