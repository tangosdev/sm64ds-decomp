//cpp
/* Production translation unit for ov032/daObjTdFuta_c, hand-curated.
 * 7 function(s), .text 0x021124a8..0x02112698.
 *
 * Tiny-Huge Island's lid ("futa", HUGE_COVER, actor 106): a mesh collider
 * that sits over the huge water until event bit 0xe is set.
 *
 * WHAT THE CARTRIDGE PROVES ABOUT THE NAME AND THE SHAPE:
 *   _ZTS  ov032 0x021138ac  "13daObjTdFuta_c"
 *   _ZTI  ov032 0x021138a0  __si_class_type_info; +8 -> _ZTI10dBgActor_c
 *                           (ov002 0x021089ec), so the DIRECT base is
 *                           dBgActor_c and nothing else.
 *   _ZTV  ov032 0x021138e0  the ADDRESS POINT: V-8 is a zero offset-to-top,
 *                           V-4 is &_ZTI, V+0 is slot 0 (InitResources,
 *                           0x021125d4). 32 slots.
 *   size  0x320             daObjTdFuta_c_classInit's own literal, which is
 *                           dBgActor_c's own size too: this class adds no
 *                           fields.
 * The coined HugeCover alias that used to sit on this vtable is gone; the
 * ROM's own type string is where the class name now comes from.
 *
 * Four slots point inside ov032 -- 0, 3, 6, 9 -- plus the destructor pair at
 * 16/17, and every other slot still holds dBgActor_c's arm9 word. The
 * neighbouring 0x021139a4 table is a different class -- RTTI names it
 * daObjTdWater_c -- and its members start at 0x02112698, past this TU's last
 * byte.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder. The
 * destructor is not written here at all; it is inline in
 * include/daObjTdFuta_c.h, declared last, and so emits first, which is
 * where the cartridge has it.
 *
 * Consolidated from these legacy one-function sources (ROM address order):
 *   [0] 0x021124a8  src/_ZN13daObjTdFuta_cD1Ev.cpp
 *   [1] 0x021124ec  src/_ZN13daObjTdFuta_cD0Ev.cpp
 *   [2] 0x02112544  src/_ZN13daObjTdFuta_c16CleanupResourcesEv.cpp
 *   [3] 0x02112588  src/_ZN13daObjTdFuta_c6RenderEv.cpp
 *   [4] 0x021125b0  src/_ZN13daObjTdFuta_c8BehaviorEv.cpp
 *   [5] 0x021125d4  src/_ZN13daObjTdFuta_c13InitResourcesEv.cpp
 *   [6] 0x02112668  src/daObjTdFuta_c_classInit.c
 */

#include "daObjTdFuta_c.h"
#include "SharedFilePtr.h"

/* Event::GetBit has no shared header anywhere in the tree -- every caller
 * declares it locally. */
namespace Event { s32 GetBit(u32 bit); }

struct KCL_File;
struct CLPS_Block;

extern "C" {
extern SharedFilePtr data_ov032_02113acc;
extern SharedFilePtr data_ov032_02113ad4;
extern CLPS_Block data_ov032_02112f98;

/* dBgActor_c::IsClsnInRange takes two Fix12<int> by value. A real member
 * call homes those arguments to the stack under mwccarm 2004/b56 (wall 6az). */
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);

/* dBgW_KcMbg::SetFile likewise takes Fix12<int> by value. A real member
 * call on this TU's InitResources DIFFs. */
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat, int scale,
    s16 angY, CLPS_Block *clps);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjTdFuta_c_classInit
/* Every instruction the cartridge has here falls out of the one `new`.
 * 0x0211266c loads 0x320 -- the class's own size -- into the header's leaf
 * operator new; the following call is dBgActor_c's C2 and the store lays
 * down this class's vptr. The null check is the one `new` itself emits.
 * The implicit constructor is what keeps the intervening dBgActor_c
 * construction inlined; declaring a constructor of our own would emit a `bl`
 * the factory does not have. */
extern "C" daObjTdFuta_c *daObjTdFuta_c_classInit()
{
    return new daObjTdFuta_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjTdFuta_c13InitResourcesEv
s32 daObjTdFuta_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov032_02113ad4), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(data_ov032_02113acc);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY,
        &data_ov032_02112f98);

    return Event::GetBit(0xe) == 0;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjTdFuta_c8BehaviorEv
s32 daObjTdFuta_c::Behavior()
{
    _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjTdFuta_c6RenderEv
s32 daObjTdFuta_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
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
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjTdFuta_cD1Ev
// @symbol _ZN13daObjTdFuta_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The destructor body is INLINE in
 * include/daObjTdFuta_c.h and declared LAST. Two measurements force that:
 *   - out of line, mwcc emits D0 ahead of D1 and the cartridge has D1 first,
 *     which rombuild refuses outright;
 *   - out of line it also emits the D2 base-object variant, which the ROM
 *     never carried.
 * Declaring it last, with the factory's `new` instantiating the class, is
 * what emits the used D1/D0 pair in cartridge order and keeps this TU the
 * vtable's home.
 *
 * Both bodies are short because the chain is short and this class adds no
 * member of its own: this class's vptr store, then dBgActor_c's -- inlined,
 * which destroys the Model at 0xd4 and the dBgW_KcMbg at 0x124 -- then
 * dActor_c. */
