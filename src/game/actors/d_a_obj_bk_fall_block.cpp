//cpp
/**
 * Whomp's Fortress falling block.
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjFallBlock_c's shared ov098
 * helpers.
 * InitResourcesFrom loads slot 0 with dExtSimpleModel_c::LoadFile,
 * slot 1 with dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile. ov015
 * sinit constructs those SharedFilePtrs as file IDs 1423 / 1424.
 *
 * daObjBk_Fall_Block_c_classInit is reconstructed (RTTI
 * daObjBk_Fall_Block_c, BK_DOWN_B registry). Retail does not store
 * that spelling.
 *
 * data_ov015_02114880 is this TU's model, collision, and CLPS triple.
 * g_profile_BK_DOWN_B
 * lives outside this TU (S14).
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_bk_fall_block_AssetId { ASSET_BK_DOWN_B_BMD = 0x058f, ASSET_BK_DOWN_B_KCL = 0x0590 };

#include "daObjBk_Fall_Block_c.h"
#include "SharedFilePtr.h"

/* The model handle constructs through func_02017acc and destroys through
 * func_02017ab4. The collision handle constructs through func_02017b4c and
 * destroys through SharedFilePtr_Destruct_Clsn. */
struct FallBlockModelFilePtr : SharedFilePtr {
    FallBlockModelFilePtr(unsigned int fileId);
    ~FallBlockModelFilePtr();
};
struct FallBlockClsnFileHandle : SharedFilePtr {
    FallBlockClsnFileHandle(unsigned int fileId);
    ~FallBlockClsnFileHandle();
};
typedef char FallBlockModelFilePtr_size_must_be_8[sizeof(FallBlockModelFilePtr) == 8 ? 1 : -1];
typedef char FallBlockClsnFileHandle_size_must_be_8[sizeof(FallBlockClsnFileHandle) == 8 ? 1 : -1];

#include "ResourceDescriptor.h"

extern ResourceDescriptor data_ov015_02114880;

daObjBk_Fall_Block_c *daObjBk_Fall_Block_c_classInit() {
    return new daObjBk_Fall_Block_c();
}

s32 daObjBk_Fall_Block_c::InitResources() {
    return InitResourcesFrom(&data_ov015_02114880);
}

s32 daObjBk_Fall_Block_c::CleanupResources() {
    return CleanupResourcesFrom(&data_ov015_02114880);
}

/* Retail construction order: model 1423, then collision 1424.
 * mwcc emits __sinit_d_a_obj_bk_fall_block.cpp. */
FallBlockModelFilePtr gBkDownBModelFile(ASSET_BK_DOWN_B_BMD);
FallBlockClsnFileHandle gBkDownBCollisionFile(ASSET_BK_DOWN_B_KCL);

struct CLPS_Block {
    u32 tag;
    u32 hdr;
    u32 w0;
    u32 w1;
    u32 w2;
    u32 w3;
    u32 tail0;
    u32 tail1;
};
CLPS_Block data_ov015_021136b4 = {0x53504c43, 0x00010008, 0x00000fc4, 0x000000ff,
                                  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};
ResourceDescriptor data_ov015_02114880 = {
    &gBkDownBModelFile,
    &gBkDownBCollisionFile,
    &data_ov015_021136b4,
};

extern "C" {
u32 data_ov015_02114afc[1];
}

/* The read-only table that begins where this class's code ends. */
extern u32 data_ov015_02113458[];
extern const u32 data_ov015_02112e0c[] = {
    0x00000000, 0x00001000, 0x0000ffff, (u32)data_ov015_02113458,
    0x00000001, 0x00000001, 0x00000001, 0x0000005b,
    0x0000005b, 0x00000000, 0x0000002e, 0x0000005b,
    0x00000089, 0x000000b6, 0x000000e4, 0x00000111,
    0x0000013f, 0x0000016c, 0x0000019a, 0x000001c7,
    0x000001f5, 0x00000222, 0x00000250, 0x0000027d,
    0x000002ab, 0x000002d8, 0x00000306, 0x00000333,
    0x00000361, 0x0000038e, 0x000003bc, 0x000003e9,
    0x00000417, 0x00000444, 0x00000472, 0x0000049f,
    0x000004cd, 0x000004fa, 0x00000528, 0x00000555,
    0x00000583, 0x000005b0, 0x000005de, 0x0000060b,
    0x00000639, 0x00000666, 0x00000694, 0x000006c1,
    0x000006ef, 0x0000071c, 0x0000074a, 0x00000777,
    0x000007a5, 0x000007d2, 0x00000800, 0x0000082e,
    0x0000085b, 0x00000889, 0x000008b6, 0x000008e4,
    0x00000911, 0x0000093f, 0x0000096c, 0x0000099a,
    0x000009c7, 0x000009f5, 0x00000a22, 0x00000a50,
    0x00000a7d, 0x00000aab, 0x00000ad8, 0x00000b06,
    0x00000b33, 0x00000b61, 0x00000b8e, 0x00000bbc,
    0x00000be9, 0x00000c17, 0x00000c44, 0x00000c72,
    0x00000c9f, 0x00000ccd, 0x00000cfa, 0x00000d28,
    0x00000d55, 0x00000d83, 0x00000db0, 0x00000dde,
    0x00000e0b, 0x00000e39, 0x00000e66, 0x00000e94,
    0x00000ec1, 0x00000eef, 0x00000f1c, 0x00000f4a,
    0x00000f77, 0x00000fa5, 0x00000fd2, 0x00001000,
};
