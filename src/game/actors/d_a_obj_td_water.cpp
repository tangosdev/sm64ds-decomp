//cpp
/* Production translation unit for ov032/daObjTdWater_c, hand-curated.
 * 7 function(s), .text 0x02112698..0x021128f0.
 *
 * Huge water plane (profile HUGE_WATER / TD_WATER 107). ov032 also has
 * HUGE_COVER(106) / daObjTdFuta_c. RTTI ov032:0x02113960 names
 * daObjTdWater_c; g_profile_TD_WATER is at 0x02113980.
 *
 * WHAT THE CARTRIDGE PROVES ABOUT THE NAME AND THE SHAPE:
 *   _ZTS  ov032 0x0211396c  "14daObjTdWater_c"
 *   _ZTI  ov032 0x02113960  __si_class_type_info; +8 -> _ZTI10dBgActor_c
 *                           (ov002 0x021089ec), so the DIRECT base is
 *                           dBgActor_c and nothing else.
 *   _ZTV  ov032 0x021139a4  the ADDRESS POINT itself.
 *   size  0x334             the factory's own literal.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder. The
 * inline destructor in include/daObjTdWater_c.h emits the retail D1/D0 pair
 * first and emits no D2 body.
 *
 * deslop leftovers:
 * - TextureTransformer::SetFile / dBgW_KcMbg::SetFile 6az: InitResources
 *   passes Fix12<int> by value; the header method form size-DIFFs.
 * - SharedFilePtr +4: InitResources' TextureTransformer::Prepare reads the
 *   BMD at data_ov032_02113afc +4; SharedFilePtr.h has no fields.
 * - data_ov032_02113afc / 02113af4 / 02112f64 / 02112fb8 BMD / KCL / BTA /
 *   CLPS handles; this TU consumes them, overlay .data owns them.
 * - S14: g_profile_TD_WATER stays outside the licensed .text.
 * - Event::GetBit has no shared header; every caller declares it locally.
 * - common.h first (dBgActor_c.h must see common.h ahead of Model.h).
 * - no Player.h / Camera.h.
 *
 * Consolidated from these legacy one-function sources (ROM address order):
 *   [0] 0x02112698  src/_ZN14daObjTdWater_cD1Ev.cpp
 *   [1] 0x021126e4  src/_ZN14daObjTdWater_cD0Ev.cpp
 *   [2] 0x02112744  src/_ZN14daObjTdWater_c16CleanupResourcesEv.cpp
 *   [3] 0x02112788  src/_ZN14daObjTdWater_c6RenderEv.cpp
 *   [4] 0x021127bc  src/_ZN14daObjTdWater_c8BehaviorEv.cpp
 *   [5] 0x021127f0  src/_ZN14daObjTdWater_c13InitResourcesEv.cpp
 *   [6] 0x021128b8  src/daObjTdWater_c_classInit.c
 */

#include "common.h"
#include "daObjTdWater_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "dBgW_Kc.h"

struct KCL_File;
struct CLPS_Block;

extern "C" {
extern SharedFilePtr data_ov032_02113afc;   /* the water model's BMD */
extern SharedFilePtr data_ov032_02113af4;   /* the collision KCL */
extern BTA_File      data_ov032_02112f64;   /* the scrolling texture animation */
extern CLPS_Block    data_ov032_02112fb8;

/* 6az: both SetFile calls take Fix12<int> by value. */
void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
    TextureTransformer *self, BTA_File &animFile, int flags, int speed,
    u32 startFrame);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 &mat, int scale,
    s16 angleY, CLPS_Block &clps);
}

/* Event has no shared header anywhere in the tree -- every caller declares it
   locally, so this matches the house spelling rather than inventing one. */
namespace Event { s32 GetBit(u32 bit); }

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjTdWater_c_classInit
/* Reconstructed source-style name. Historical alias: HugeWater_Spawn.
 *
 * Every instruction the cartridge has here falls out of the one `new`.
 * 0x021128bc loads 820 = 0x334 -- the class's own size -- into the header's
 * inline operator new; dBgActor_c's C2, the derived vptr store, and
 * TextureTransformer at 0x320 are the implicit constructor. The null check
 * is the one `new` itself emits. */
extern "C" daObjTdWater_c *daObjTdWater_c_classInit(void)
{
    return new daObjTdWater_c();
}

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
    /* 6az: header SetFile takes Fix12<int> by value. */
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
        &mTextureTransformer, data_ov032_02112f64, 0, 0x1000, 0);

    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(data_ov032_02113af4);
    /* 6az: header SetFile takes Fix12<int> by value. */
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, mClsnMat, 0x1000, mAngleY, data_ov032_02112fb8);
    mMeshCollider.Enable(this);

    /* The water only survives the spawn while the level's event bit 0xe is
       clear -- returning 0 makes fBase_c drop the actor on the spot. */
    return Event::GetBit(0xe) == 0;
}

// @symbol _ZN14daObjTdWater_c8BehaviorEv
s32 daObjTdWater_c::Behavior()
{
    mTextureTransformer.speed = 0x1000;
    mTextureTransformer.Advance();

    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    return 1;
}

// @symbol _ZN14daObjTdWater_c6RenderEv
s32 daObjTdWater_c::Render()
{
    mTextureTransformer.Update(mModel.data);
    mModel.Render(0);
    return 1;
}

// @symbol _ZN14daObjTdWater_c16CleanupResourcesEv
s32 daObjTdWater_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov032_02113afc.Release();
    data_ov032_02113af4.Release();
    return 1;
}

// @symbol _ZN14daObjTdWater_cD0Ev
// @symbol _ZN14daObjTdWater_cD1Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjTdWater_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no leaf D2. Writing the
   body out of line here instead flips them to D0-before-D1 and the isolation
   step rejects the object.
 */
