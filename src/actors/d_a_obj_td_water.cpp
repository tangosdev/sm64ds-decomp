//cpp
/* Production translation unit for ov032/daObjTdWater_c, hand-curated.
 * 6 function(s), .text 0x02112698..0x021128b8.
 *
 * WHAT THE CARTRIDGE PROVES ABOUT THE NAME AND THE SHAPE:
 *   _ZTS  ov032 0x0211396c  "14daObjTdWater_c"
 *   _ZTI  ov032 0x02113960  __si_class_type_info; +8 -> _ZTI10dBgActor_c
 *                           (ov002 0x021089ec), so the DIRECT base is
 *                           dBgActor_c and nothing else.
 *   _ZTV  ov032 0x021139a4  the ADDRESS POINT itself: V-8 is a zero
 *                           offset-to-top, V-4 is &_ZTI, V+0 is slot 0
 *                           (InitResources, 0x021127f0). The vptr store is
 *                           addend-0 against this symbol.
 *   size  0x334             the factory's own literal.
 * The coined HugeWater alias that used to sit on this vtable is gone; the
 * ROM's own type string is where the class name now comes from.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder.
 *
 * Consolidated from these legacy one-function sources (ROM address order):
 *   [0] 0x02112698  src/_ZN14daObjTdWater_cD1Ev.cpp
 *   [1] 0x021126e4  src/_ZN14daObjTdWater_cD0Ev.cpp
 *   [2] 0x02112744  src/_ZN14daObjTdWater_c16CleanupResourcesEv.cpp
 *   [3] 0x02112788  src/_ZN14daObjTdWater_c6RenderEv.cpp
 *   [4] 0x021127bc  src/_ZN14daObjTdWater_c8BehaviorEv.cpp
 *   [5] 0x021127f0  src/_ZN14daObjTdWater_c13InitResourcesEv.cpp
 */

#include "daObjTdWater_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "dBgW_Kc.h"

/* Externs: the union of the legacy files', kept at their legacy spelling.
 *
 * The two SharedFilePtrs are real objects in ov032's data, not a pool: the BMD
 * pair lives at 0x02113afc and the KCL's at 0x02113af4, and each call site
 * loads its own address.
 *
 * dBgW_KcMbg::SetFile and TextureTransformer::SetFile keep their mangled free
 * spelling: both carry Fix12<int> in the signature (wall 6az homes class-typed
 * by-value parameters), so their definitions are not methods yet. */
struct KCL_File;
struct CLPS_Block;

extern "C" {
extern SharedFilePtr data_ov032_02113afc;   /* the water model's BMD */
extern SharedFilePtr data_ov032_02113af4;   /* the collision KCL */
extern BTA_File      data_ov032_02112f64;   /* the scrolling texture animation */
extern CLPS_Block    data_ov032_02112fb8;

void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
    TextureTransformer *self, BTA_File &animFile, int flags, int speed,
    u32 startFrame);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 &mat, int scale,
    s16 angleY, CLPS_Block &clps);

/* Event has no shared header anywhere in the tree -- every caller declares it
   locally, so this matches the house spelling rather than inventing one. */
int _ZN5Event6GetBitEj(u32 bit);
}

/* ROM ordinal 5 -- ...13InitResourcesEv, 0x021127f0, size 0xc8 */
// @symbol _ZN14daObjTdWater_c13InitResourcesEv
s32 daObjTdWater_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov032_02113afc), 1, 0x14);

    /* The BMD the SharedFilePtr is holding, at +4: the ROM re-reads it out of
       the descriptor rather than reusing LoadFile's return value. The class has
       no recovered layout (include/SharedFilePtr.h says so), so the offset is
       spelled out instead of being named. */
    TextureTransformer::Prepare(
        **(BMD_File **)((char *)&data_ov032_02113afc + 4), data_ov032_02112f64);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
        &mTextureTransformer, data_ov032_02112f64, 0, 0x1000, 0);

    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(data_ov032_02113af4);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, mClsnMat, 0x1000, mAngleY, data_ov032_02112fb8);
    mMeshCollider.Enable(this);

    /* The water only survives the spawn while the level's event bit 0xe is
       clear -- returning 0 makes fBase_c drop the actor on the spot. */
    return _ZN5Event6GetBitEj(0xe) == 0;
}

/* ROM ordinal 4 -- ...8BehaviorEv, 0x021127bc, size 0x34 */
// @symbol _ZN14daObjTdWater_c8BehaviorEv
s32 daObjTdWater_c::Behavior()
{
    /* Rewritten every frame, not just at spawn: the ROM stores the literal
       here, so the field is set unconditionally before Advance reads it. */
    mTextureTransformer.speed = 0x1000;
    mTextureTransformer.Advance();

    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    return 1;
}

/* ROM ordinal 3 -- ...6RenderEv, 0x02112788, size 0x34 */
// @symbol _ZN14daObjTdWater_c6RenderEv
s32 daObjTdWater_c::Render()
{
    /* Update first, then draw: the scroll for THIS frame has to be folded into
       the model's components before Model::Render walks them. */
    mTextureTransformer.Update(mModel.data);
    mModel.Render(0);
    return 1;
}

/* ROM ordinal 2 -- ...16CleanupResourcesEv, 0x02112744, size 0x44 */
// @symbol _ZN14daObjTdWater_c16CleanupResourcesEv
s32 daObjTdWater_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov032_02113afc.Release();
    data_ov032_02113af4.Release();
    return 1;
}

/* ROM ordinals 1 and 0 -- D0 0x021126e4 size 0x60, D1 0x02112698 size 0x4c */
// @symbol _ZN14daObjTdWater_cD0Ev
// @symbol _ZN14daObjTdWater_cD1Ev
/* NOT WRITTEN HERE ON PURPOSE. The destructor body is INLINE in
 * include/daObjTdWater_c.h and declared FIRST. Two measurements force that:
 *   - out of line, mwcc emits D0 ahead of D1 and the cartridge has D1 first,
 *     which rombuild refuses outright;
 *   - out of line it also emits the D2 base-object variant, which the ROM
 *     never carried.
 * Declaring it first is what makes this TU the vtable's home, so _ZTV and the
 * RTTI pair land here rather than in whichever other TU happens to name them.
 */
