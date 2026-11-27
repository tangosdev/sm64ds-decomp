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

/* The model handle constructs through ModelFile::ModelFile and destroys through
 * ModelFile::~ModelFile. The collision handle constructs through ClsnFile::ClsnFile and
 * destroys through ClsnFile::~ClsnFile. */
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

extern struct CLPS_Block data_ov015_021136b4;

ResourceDescriptor data_ov015_02114880 = {
    &gBkDownBModelFile,
    &gBkDownBCollisionFile,
    &data_ov015_021136b4,
};

extern "C" {
u32 data_ov015_02114afc[1];
}

/* The read-only table that begins where this class's code ends. */
