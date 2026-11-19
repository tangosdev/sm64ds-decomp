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

/* The water texture and collision bundle the file table above points
 * into. Every relocated word names the symbol it loads. */

/* 0x02112fd0..0x0211377c: water texture and collision payload. */
extern u32 data_ov032_02112ec0;
extern u32 data_ov032_02112ec4;
extern u32 data_ov032_02112ec8;
extern u32 data_ov032_02112ef4;
extern u32 data_ov032_02112f04;
extern u32 data_ov032_02112f14;
extern u32 data_ov032_02112f24;
extern u32 data_ov032_02112f4c;
extern u32 data_ov032_02112fd8;
extern u32 data_ov032_02113080;
extern u32 data_ov032_021130c8;
extern u32 data_ov032_02113148;
extern u32 data_ov032_021131c8;
extern u32 data_ov032_0211336c;
extern u32 data_ov032_0211346c;

u32 data_ov032_02112fd0[491] = {
    0xffffffff, 0xffffffff,
    0x1b8a011a, 0xe7fafa24,
    0x00000000, 0xffff0000,
    0xf8f80119, 0x0834fe0c,
    0x00000003, 0xfff10000,
    0xe63800ef, 0x0384f736,
    0x00000000, 0xffff0000,
    0x00003100, (u32)&data_ov032_0211346c,
    0x00002005, (u32)&data_ov032_0211336c,
    0x00000360, (u32)&data_ov032_02112fd8,
    0x00000180, (u32)&data_ov032_02112ef4,
    0x000001a0, (u32)&data_ov032_02112f04,
    0x000001c0, (u32)&data_ov032_02112f14,
    0x000001e0, (u32)&data_ov032_02112f24,
    0x00001b02, (u32)&data_ov032_021131c8,
    0x00000403, (u32)&data_ov032_02112f4c,
    0x00000904, (u32)&data_ov032_021130c8,
    0x0000010b, (u32)&data_ov032_02112ec4,
    0x00000801, (u32)&data_ov032_02113148,
    0x0000010c, (u32)&data_ov032_02112ec0,
    0x0000010e, (u32)&data_ov032_02112ec8,
    0x0000050a, (u32)&data_ov032_02113080,
    0xfe0c0190, 0x00000320,
    0x001a0000, 0x000011ff,
    0xfa8c0e74, 0x0000c000,
    0x11ff041a, 0xf862e9bc,
    0xc000e98a, 0x04190000,
    0xf3e400ff, 0xf1be0226,
    0x0000c000, 0x00ff0519,
    0xfa561900, 0xc000e9ee,
    0x06190000, 0x000000ff,
    0x0000ff00, 0xff9c10cc,
    0x00000000, 0xff020000,
    0xf7c6dda0, 0x00001a2c,
    0x00004000, 0xef340204,
    0x1c84f5d8, 0x80000000,
    0xff040000, 0xffc4f8f8,
    0x00000bb8, 0x00008000,
    0x0000ff07, 0xff9c0000,
    0x00000000, 0xff020000,
    0xff9c028a, 0x00000640,
    0x00008aaa, 0xe700ff02,
    0xe82cfaec, 0x2aaa0000,
    0xff020000, 0xfc181ce8,
    0x0000e8f4, 0x0000d1c8,
    0xf18cff02, 0xeed003e8,
    0x1c710000, 0x00000000,
    0xe37c0000, 0x1c84fb50,
    0x5c710000, 0x05080000,
    0xe5700000, 0x1c84fb50,
    0x5c710000, 0x05080000,
    0xe5700000, 0x1a90fb50,
    0x5c710000, 0x05080000,
    0xe37c0000, 0x1a90fb50,
    0x5c710000, 0x05080000,
    0x01900000, 0x0384fe0c,
    0x00000000, 0x00280000,
    0xe9bc0000, 0xe958f830,
    0x00000000, 0x06b00000,
    0xf3e40000, 0xf18c01f4,
    0xe38f0000, 0x06c00000,
    0x19000000, 0xe9bcfa24,
    0x1c710000, 0x06b80000,
    0x00c8ef63, 0xed98f7c1,
    0xf6c6005c, 0x002eec04,
    0xeda6ee74, 0xec0400d8,
    0xfe8df2a5, 0xfbaaed52,
    0xed60fabd, 0xf9c002f1,
    0x0b09eda6, 0xed8af830,
    0xfe0cf8f8, 0xf5d80834,
    0x0ce4fe0c, 0xfe0ced40,
    0xed400a8c, 0xfed4fe0c,
    0x0064ed40, 0xeb4cf7cc,
    0xee6c0078, 0x009ded40,
    0xf060eae8, 0xebb0ff5d,
    0xfe88f2b8, 0xfc18ecdc,
    0xecdcfaa2, 0xf9c00320,
    0x0b54ecdc, 0xeb4cf830,
    0xfa24157c, 0x189ceb4c,
    0xe7c8fa24, 0xfa241b58,
    0x1a90e764, 0x0b54fc18,
    0xfc181a90, 0x1770fa24,
    0xe890fc18, 0xfc181770,
    0x0000f830, 0x53504c43,
    0x001d0008, 0x00240fc0,
    0x000000ff, 0x019c0000,
    0x000000ff, 0x01000fc0,
    0x000000ff, 0x00040023,
    0x000000ff, 0x00730004,
    0x000000ff, 0x01040004,
    0x000000ff, 0x00744004,
    0x000000ff, 0x00040004,
    0x000000ff, 0x00030004,
    0x000000ff, 0x000300c2,
    0x000000ff, 0x00030fc2,
    0x000000ff, 0x00040002,
    0x000000ff, 0x00040fc2,
    0x000000ff, 0x00030002,
    0x000000ff, 0x00740002,
    0x000000ff, 0x00730082,
    0x000000ff, 0x00732082,
    0x000000ff, 0x00744002,
    0x000000ff, 0x00730002,
    0x000000ff, 0x00732002,
    0x000000ff, 0x005c0002,
    0x00000003, 0x005c0fc2,
    0x00000002, 0x00040008,
    0x000000ff, 0x00030005,
    0x000000ff, 0x005c0005,
    0x00000003, 0x00040005,
    0x000000ff, 0x00030fc5,
    0x000000ff, 0x01040005,
    0x000000ff, 0x01043005,
    0x000000ff, 0xffffffff,
    0xffffffff, 0xffffffff,
    0xffffffff, 0x125ce029,
    0x0899fe11, 0xed40e225,
    0xf95cffcf, 0xed40e225,
    0xf9f2ff96, 0xed40e225,
    0xfa88ff5e, 0xed40e225,
    0xfb1eff26, 0x02ebe225,
    0x08ecfe16, 0x02e8e225,
    0x098afe16, 0x02e5e225,
    0x0a2bfe16, 0x02e2e225,
    0x0ad6fe16, 0x02dbe225,
    0x0b84fe16, 0xff38e225,
    0x01900866, 0x0000e225,
    0x01900866, 0x00c8e225,
    0x01900866, 0x0190e225,
    0x01900866, 0x0258e225,
    0x01900866, 0x15aee225,
    0xf704f911, 0x15aee225,
    0xf7ccf8f8, 0x15aee225,
    0xf894f8df, 0x15aee225,
    0xf95cf8c6, 0x15aee225,
    0xfa24f8ad, 0xe7c8423f,
    0xe764f880, 0x1b58463f,
    0xe9bcfa74, 0xf9c00836,
    0x1a90f380, 0x08fc0a36,
    0x1b58f380, 0xe7000836,
    0x1004f380, 0xe9bc0a36,
    0xfb50f31c, 0x09c4443f,
    0xf7680d48, 0xe570e10c,
    0xf380f830, 0xe570e10b,
    0xf380f9c0, 0x0000ec3d,
    0xfa241004, 0xe638e029,
    0x0384f4de, 0xe638e225,
    0x0384f768, 0xe9bc00ba,
    0xe958f830, 0x00000000,
    0xffff0000, 0xf3e400ba,
    0xf18c01f4, 0x00000000,
    0xffff0000, 0x190000ba,
    0xe9bcfa24, 0x00000000,
    0xffff0000, 0x16da001f,
    0xfe0df92a, 0x00000000,
    0xff020000, 0x09c4001f,
    0xf7680d48, 0x00000000,
    0x02010000, 0x19640032,
    0x1a90f530, 0x80000000,
    0x0002c000, 0xe8740031,
    0xfaa8f255, 0xc0000000,
    0x00f30000, 0x000000bd,
    0x0dacf830, 0x00000000,
    0x00000000, 0x189c00bd,
    0x1770f830, 0x00000000,
    0x00010000, 0xfede0015,
    0x0866f84b, 0x00000000,
    0x00000000, 0xe66800ea,
    0xfc8ef380, 0x00000000,
    0xffff0000, 0x045c00ea,
    0x1a22f31c, 0x00000000,
    0xffff0000, 0xeee5000d,
    0xf7dc00c8, 0x00000000,
    0x00200000, 0xf32b00bd,
    0xeab70064, 0x00000000,
    0x00010000, 0xfe0c0024,
    0x1964f63c, 0x00000000,
    0xffff0000, 0xeb6f0024,
    0x148df4ac, 0x00000000,
    0xffff0000, 0xfed40094,
    0x1130f628, 0x00000000,
    0x00010000, 0xfcea004c,
    0x1982f63c, 0x4000ffff,
    0x06400000, 0xf736004c,
    0x07d0fe0c, 0x0000ffff,
    0x06410000, 0x19f5004c,
    0x0766f63c, 0x4000ffff,
    0x07490000, 0x00000078,
    0xfa240eec, 0x00000000,
    0xffff0000, 0x00000079,
    0x00000000, 0x00000000,
    0xffff0000, 0x16a800fe,
    0xe7c8fa24, 0x00000000,
    0x12040000, 0xf7ee012d,
    0xfd970866, 0x00000000,
    0xffff0000, 0xe7640011,
    0x157cf4ac, 0x00000000,
    0xffff0000, 0xe5700011,
    0x17d4f4ac, 0x00000000,
    0xffff0000, 0xe8900011,
    0x1af4f4ac, 0x00000000,
    0xffff0000, 0xf0c40011,
    0x0ce4fe0c, 0x00000000,
    0xffff0000, 0xf3e40011,
    0x0834fe0c, 0x00000000,
    0xffff0000, 0x189c0011,
    0x1194f63c, 0x00000000,
    0xffff0000, 0x0e740011,
    0x0a8cfe0c, 0x00000000,
    0xffff0000, 0x0e9e0011,
    0x0de8fa24, 0x00000000,
    0xffff0000, 0x1b580011,
    0xec78fa24, 0x00000000,
    0xffff0000, 0x1964001d,
    0x1a90f530, 0x00000000,
    0xff010000, 0xfd440031,
    0x04b0f9cc, 0x00000000,
    0x00f90000, 0xe82c0138,
    0xf1f0f830, 0x00000000,
    0xff010000, 0xeae80138,
    0xec78f830, 0x00000000,
    0xff010000, 0xe69c0138,
    0xeed0f830, 0x00000000,
    0xff010000, 0xe82c0138,
    0xebb0f830, 0x00000000,
    0xff010000, 0xea840138,
    0xe69cf830, 0x00000000,
    0xff010000, 0xef5200fe,
    0xf7cc02bc, 0xe0000000,
    0x11040000, 0xe82c00fe,
    0x044cf4de, 0x20000000,
    0x10040000, 0xeb7e0024,
    0xf380f830, 0x00000000,
    0xffff0000, 0xe7000024,
    0xed0ef830, 0x00000000,
    0xffff0000, 0xe5700024,
    0xf380f830, 0x00000000,
    0xffff0000, 0x0a8c004c,
    0x0c80fe0c, 0x4000ffff,
    0x07550000, 0x16a800fe,
    0xe82cfa24, 0x00000000,
    0x02040000, 0xef5200fe,
    0xf7cc02bf, 0xe0000000,
    0x01040000, 0xe82c00fe,
    0x04b0f4de, 0x20000000,
    0x00040000,
};
