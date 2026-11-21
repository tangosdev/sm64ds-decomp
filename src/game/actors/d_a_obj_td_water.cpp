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
 * What stays:
 * - data_ov032_02113afc / 02113af4 / 02112f64 / 02112fb8 BMD / KCL / BTA /
 *   CLPS handles; this TU consumes them, overlay .data owns them.
 * - S14: g_profile_TD_WATER stays outside the licensed .text.
 * - no daPly_c.h / dCamera_c.h.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_td_water_AssetId { ASSET_TD_OBJ_WATER_BMD = 0x06b7, ASSET_TD_OBJ_WATER_KCL = 0x06b8 };

#include "common.h"
#include "daObjTdWater_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "dBgW_Kc.h"
#include "Event.h"

#include "private/BTA_File.h"
#include "SpawnInfo.h"

struct KCL_File;

struct CLPS_Block {
    u32 words[6];
};

/* dExtSimpleModel_c handles construct through func_02017acc and destroy through
 * func_02017ab4; collision handles construct through func_02017b4c and
 * destroy through SharedFilePtr_Destruct_Clsn. The manifest aliases those
 * undefined members onto the ROM symbols. */
struct TdWaterModelFilePtr : SharedFilePtr {
    TdWaterModelFilePtr(u32 fileID);
    ~TdWaterModelFilePtr();
};

struct TdWaterClsnFileHandle : SharedFilePtr {
    TdWaterClsnFileHandle(u32 fileID);
    ~TdWaterClsnFileHandle();
};

extern TdWaterModelFilePtr data_ov032_02113afc;   /* the water model's BMD */
extern TdWaterClsnFileHandle data_ov032_02113af4; /* the collision KCL */
extern const fx32 data_ov032_021128fc[];
extern const u16 data_ov032_021128f0[];
extern const fx32 data_ov032_02112aa4[];
extern const BTAMaterialRecord data_ov032_0211291c[];
extern char data_ov032_02112edc[];

BTA_File data_ov032_02112f64 = {
    91,
    0,
    (fx32 *)data_ov032_021128fc,
    (u16 *)data_ov032_021128f0,
    (fx32 *)data_ov032_02112aa4,
    1,
    0,
    (BTAMaterialRecord *)data_ov032_0211291c,
};

/* The keys and the material record of the file table above. Retail keeps
 * them in .rodata, interleaved with the second water BTA file's keys, so each
 * one gets its own named section. func_02046b64 patches the record's material
 * id in place, so the file table takes its address through a non-const
 * pointer. */

/* Both keys are the identity value: no texture rotation, no scaling. */
const u16 data_ov032_021128f0[2] = {0, 0};
const fx32 data_ov032_021128fc[1] = {0x1000};

const BTAMaterialRecord data_ov032_0211291c[1] = {
    {0xffff, 0, data_ov032_02112edc, {1, 0}, {1, 0}, {1, 0}, {91, 0}, {91, 0}},
};

/* Texture translation ramps linearly from 0 to 0x1000 across the 91-frame
 * loop, so the surface scrolls exactly one texture width per cycle. */
const fx32 data_ov032_02112aa4[91] = {
    0x00000000, 0x0000002e, 0x0000005b, 0x00000089, 0x000000b6, 0x000000e4, 0x00000111, 0x0000013f,
    0x0000016c, 0x0000019a, 0x000001c7, 0x000001f5, 0x00000222, 0x00000250, 0x0000027d, 0x000002ab,
    0x000002d8, 0x00000306, 0x00000333, 0x00000361, 0x0000038e, 0x000003bc, 0x000003e9, 0x00000417,
    0x00000444, 0x00000472, 0x0000049f, 0x000004cd, 0x000004fa, 0x00000528, 0x00000555, 0x00000583,
    0x000005b0, 0x000005de, 0x0000060b, 0x00000639, 0x00000666, 0x00000694, 0x000006c1, 0x000006ef,
    0x0000071c, 0x0000074a, 0x00000777, 0x000007a5, 0x000007d2, 0x00000800, 0x0000082e, 0x0000085b,
    0x00000889, 0x000008b6, 0x000008e4, 0x00000911, 0x0000093f, 0x0000096c, 0x0000099a, 0x000009c7,
    0x000009f5, 0x00000a22, 0x00000a50, 0x00000a7d, 0x00000aab, 0x00000ad8, 0x00000b06, 0x00000b33,
    0x00000b61, 0x00000b8e, 0x00000bbc, 0x00000be9, 0x00000c17, 0x00000c44, 0x00000c72, 0x00000c9f,
    0x00000ccd, 0x00000cfa, 0x00000d28, 0x00000d55, 0x00000d83, 0x00000db0, 0x00000dde, 0x00000e0b,
    0x00000e39, 0x00000e66, 0x00000e94, 0x00000ec1, 0x00000eef, 0x00000f1c, 0x00000f4a, 0x00000f77,
    0x00000fa5, 0x00000fd2, 0x00001000,
};

CLPS_Block data_ov032_02112fb8 = {
    0x53504c43, 0x00010008, 0x00000fe0, 0x000000ff, 0xffffffff, 0xffffffff,
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* Reconstructed source-style name. Historical alias: HugeWater_Spawn.
 *
 * Every instruction the cartridge has here falls out of the one `new`.
 * 0x021128bc loads 820 = 0x334 -- the class's own size -- into the header's
 * inline operator new; dBgActor_c's C2, the derived vptr store, and
 * dExtAnmTexSRT_c at 0x320 are the implicit constructor. The null check
 * is the one `new` itself emits. */
daObjTdWater_c *daObjTdWater_c_classInit(void) {
    return new daObjTdWater_c();
}

s32 daObjTdWater_c::InitResources() {
    mModel.SetFile(dExtSimpleModel_c::LoadFile(data_ov032_02113afc), 1, 0x14);

    /* The BMD the SharedFilePtr is holding: the ROM re-reads it out of the
       descriptor rather than reusing LoadFile's return value. */
    dExtAnmTexSRT_c::Prepare(*(BMD_File *)data_ov032_02113afc.filePtr, data_ov032_02112f64);
    mTextureTransformer.SetFile(data_ov032_02112f64, 0, 0x1000, 0);

    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    KCL_File *kcl = dBgW_Kc::LoadFile(data_ov032_02113af4);
    mMeshCollider.SetFile(kcl, mClsnMat, 0x1000, mAngle.y, data_ov032_02112fb8);
    mMeshCollider.Enable(this);

    /* The water only survives the spawn while the level's event bit 0xe is
       clear -- returning 0 makes fBase_c drop the actor on the spot. */
    return Event::GetBit(0xe) == 0;
}

s32 daObjTdWater_c::Behavior() {
    mTextureTransformer.speed = 0x1000;
    mTextureTransformer.Advance();

    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    return 1;
}

s32 daObjTdWater_c::Render() {
    mTextureTransformer.Update(mModel.data);
    mModel.Render(0);
    return 1;
}

s32 daObjTdWater_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov032_02113afc.Release();
    data_ov032_02113af4.Release();
    return 1;
}

/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjTdWater_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no leaf D2. Writing the
   body out of line here instead flips them to D0-before-D1 and the isolation
   step rejects the object.
 */

/* File-scope objects, in __sinit_d_a_obj_td_water.cpp construction order:
 * the model's SharedFilePtr, then the collision file's. */
TdWaterModelFilePtr data_ov032_02113afc(ASSET_TD_OBJ_WATER_BMD);
TdWaterClsnFileHandle data_ov032_02113af4(ASSET_TD_OBJ_WATER_KCL);

/* The BMD material the texture animation above drives. */
char data_ov032_02112edc[12] = "water_mat";

extern "C" {
u32 data_ov032_02113b1c[1];
}

extern "C" SpawnInfo<daObjTdWater_c> g_profile_TD_WATER = {
    daObjTdWater_c_classInit, 107, 269, 0x00000000, 0x00064000, 0x00258000, 0x01f40000, 0x00050000};

#pragma explicit_zero_data on
extern "C" u32 data_ov032_02113a24[7] = {0, 0, 0, 0, 0, 0, 0};
#pragma explicit_zero_data off
