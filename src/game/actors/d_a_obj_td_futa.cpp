//cpp
/* Production translation unit for ov032/daObjTdFuta_c, hand-curated.
 * 7 function(s), .text 0x021124a8..0x02112698.
 *
 * Tiny-Huge Island's lid ("futa") -- the ROM's own RTTI spells the class
 * daObjTdFuta_c; the coined HugeCover alias that shared vtable 0x021138e0 in
 * ov032's symbols.txt has been deleted. It is a dBgActor_c with no storage of
 * its own (the factory's literal 0x320 is exactly the base's size) that fills
 * four vtable slots its base leaves to the derived class: InitResources (0),
 * CleanupResources (3), Behavior (6) and Render (9).
 *
 * InitResources loads the model and the KCL, hands the mesh collider the
 * measured 0x199 scale and this actor's Y angle, and reports whether event bit
 * 0xe is still clear. CleanupResources disables the mesh and releases both
 * file handles. The neighbouring 0x021139a4 table is a DIFFERENT class --
 * RTTI names it daObjTdWater_c -- and its members start at 0x02112698, past
 * this TU's last byte.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder:
 * tools/rombuild.py refuses the object outright when the emitted order and the
 * ROM's disagree.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x021124a8  src/_ZN13daObjTdFuta_cD1Ev.cpp
 *   [1] 0x021124ec  src/_ZN13daObjTdFuta_cD0Ev.cpp
 *   [2] 0x02112544  src/_ZN13daObjTdFuta_c16CleanupResourcesEv.cpp
 *   [3] 0x02112588  src/_ZN13daObjTdFuta_c6RenderEv.cpp
 *   [4] 0x021125b0  src/_ZN13daObjTdFuta_c8BehaviorEv.cpp
 *   [5] 0x021125d4  src/_ZN13daObjTdFuta_c13InitResourcesEv.cpp
 *   [6] 0x02112668  src/daObjTdFuta_c_classInit.c
 *
 * THE SEVENTH IS THE FACTORY. daObjTdFuta_c_classInit (historical alias
 * HugeCover_Spawn) is the TD_FUTA registry profile's spawn function and sits
 * immediately after InitResources in the ROM's own .text order, so it is part
 * of this TU. It was outside it only because the promotion predated the
 * profile-reconstruction campaign. It keeps C linkage and is written first
 * here, being the highest-address member.
 */

/* Includes and externs: the union of the legacy files', kept at their legacy
 * spelling. The two SharedFilePtr globals were declared `extern "C"` by the
 * destructor/cleanup source and plain `extern` by InitResources; both name the
 * same unmangled ov032 data symbol, so the C-linkage spelling is the one that
 * is true and the tubuild "conflict" was cosmetic. */
#include "daObjTdFuta_c.h"
#include "SharedFilePtr.h"

/* Event::GetBit has no shared header anywhere in the tree -- src/_ZN5Event6GetBitEj.cpp
 * defines it and every caller declares it locally, so this matches the house
 * spelling rather than inventing an include. */
namespace Event { s32 GetBit(u32 bit); }

extern "C" {
extern SharedFilePtr data_ov032_02113acc;
extern SharedFilePtr data_ov032_02113ad4;
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, void *mtx, int scale, short angleY, void *clps);
extern char data_ov032_02112f98;

/* The factory's own dependencies, restated here rather than pulled in through
   a decl_*.h as the legacy file did -- this TU declares in place, and reaching
   a shared header would change what the rest of the TU sees. */
extern void *_ZN7fBase_cnwEj(unsigned size);
extern void _ZN10dBgActor_cC2Ev(void *self);
extern int _ZTV13daObjTdFuta_c[];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- daObjTdFuta_c_classInit, 0x02112668, size 0x30            */
/* -------------------------------------------------------------------------- */
// @symbol daObjTdFuta_c_classInit
/* Reconstructed source-style name: SM64DS proves daObjTdFuta_c through RTTI,
   allocation size, vtable identity, and the TD_FUTA registry profile; later
   EAD lineage supplies classInit. Exact original spelling is not preserved.
   Historical alias: HugeCover_Spawn.

   The factory remains in its measured C ABI form. mwccarm rejects fBase_c's
   actor allocator as an in-class operator new declaration (see fBase_c.h), so
   a source-level `new daObjTdFuta_c` binds the wrong global allocator; the ROM
   calls the actor allocator, the base constructor and the derived vptr store
   explicitly. 800 = 0x320 = the whole object; this class adds no fields. */
extern "C" int *daObjTdFuta_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(800);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV13daObjTdFuta_c; }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN13daObjTdFuta_c13InitResourcesEv, 0x021125d4, size 0x94 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjTdFuta_c13InitResourcesEv
/* dBgW_KcMbg::SetFile takes Fix12<int> by value. An ordinary member call
 * triggers mwccarm's by-value-class parameter homing and changes the ROM ABI,
 * so this one call deliberately retains the measured register-level view. */
s32 daObjTdFuta_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov032_02113ad4), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    void *kcl = dBgW_Kc::LoadFile(data_ov032_02113acc);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY,
        &data_ov032_02112f98);

    return Event::GetBit(0xe) == 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN13daObjTdFuta_c8BehaviorEv, 0x021125b0, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjTdFuta_c8BehaviorEv
/* The ROM passes both Fix12<int> values directly in registers. Defining the
 * imported function as an ordinary C++ member homes the by-value class
 * parameters to the stack under mwccarm 2004/b56, so keep the measured ABI
 * spelling until that shared compiler wall is solved. */
s32 daObjTdFuta_c::Behavior()
{
    _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN13daObjTdFuta_c6RenderEv, 0x02112588, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjTdFuta_c6RenderEv
s32 daObjTdFuta_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN13daObjTdFuta_c16CleanupResourcesEv, 0x02112544, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjTdFuta_c16CleanupResourcesEv
s32 daObjTdFuta_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov032_02113ad4.Release();
    data_ov032_02113acc.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN13daObjTdFuta_cD0Ev, 0x021124ec, size 0x58            */
/* ROM ordinal 0 -- _ZN13daObjTdFuta_cD1Ev, 0x021124a8, size 0x44            */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjTdFuta_cD1Ev
// @symbol _ZN13daObjTdFuta_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjTdFuta_c() {}` in the header
   is the whole source of both variants: from an inline body mwcc emits D1 and
   then D0 -- the cartridge's own order -- and no D2, which is the order and
   the set the ROM carries. Written out of line here instead, mwcc emits D0
   ahead of D1 and rombuild refuses the object outright. */
