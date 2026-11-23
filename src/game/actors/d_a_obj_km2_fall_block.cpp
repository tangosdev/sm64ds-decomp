//cpp
/**
 * daKpa_c in the Fire Sea's falling block.
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjFallBlock_c's shared ov098
 * helpers.
 *
 * daObjKm2_Fall_Block_c_classInit is reconstructed (RTTI
 * daObjKm2_Fall_Block_c, KM2_KUZURE registry). Retail does not store
 * that spelling.
 *
 * InitResourcesFrom loads slot 0 with dExtSimpleModel_c::LoadFile,
 * slot 1 with dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile. The
 * CLPS_Block is still data_ov045_*. g_profile_KM2_KUZURE lives outside
 * this TU (S14).
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_km2_fall_block_AssetId {
    ASSET_KM2_KUZURE_BMD = 0x0661,
    ASSET_KM2_KUZURE_KCL = 0x0662
};

#include "daObjKm2_Fall_Block_c.h"
#include "SharedFilePtr.h"

/* The model handle constructs through func_02017acc and destroys through
 * func_02017ab4. The collision handle constructs through func_02017b4c and
 * destroys through SharedFilePtr_Destruct_Clsn. The descriptor stays ROM data. */
struct Km2FallModelFilePtr : SharedFilePtr {
    Km2FallModelFilePtr(unsigned int fileId);
    ~Km2FallModelFilePtr();
};
struct Km2FallClsnFileHandle : SharedFilePtr {
    Km2FallClsnFileHandle(unsigned int fileId);
    ~Km2FallClsnFileHandle();
};
typedef char Km2FallModelFilePtr_size_must_be_8[sizeof(Km2FallModelFilePtr) == 8 ? 1 : -1];
typedef char Km2FallClsnFileHandle_size_must_be_8[sizeof(Km2FallClsnFileHandle) == 8 ? 1 : -1];

#include "ResourceDescriptor.h"

extern Km2FallModelFilePtr gKm2KuzureModelFile;
extern Km2FallClsnFileHandle gKm2KuzureCollisionFile;
/* Collision properties: the 'CLPS' tag, the 8-byte stride header with one
 * entry, then the default entries. */
struct CLPS_Block {
    u32 tag;
    u32 header;
    u32 entries[6];
};
CLPS_Block data_ov045_021125f0 = {
    0x53504c43,
    0x00010008,
    {0x00000fc0, 0x000000ff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff},
};

ResourceDescriptor data_ov045_021130ac = {
    &gKm2KuzureModelFile,
    &gKm2KuzureCollisionFile,
    &data_ov045_021125f0,
};

daObjKm2_Fall_Block_c *daObjKm2_Fall_Block_c_classInit() {
    return new daObjKm2_Fall_Block_c();
}

s32 daObjKm2_Fall_Block_c::InitResources() {
    return InitResourcesFrom(&data_ov045_021130ac);
}

s32 daObjKm2_Fall_Block_c::CleanupResources() {
    return CleanupResourcesFrom(&data_ov045_021130ac);
}

/* Retail construction order: model 1633, then collision 1634.
 * mwcc emits __sinit_d_a_obj_km2_fall_block.cpp. */
Km2FallModelFilePtr gKm2KuzureModelFile(ASSET_KM2_KUZURE_BMD);
Km2FallClsnFileHandle gKm2KuzureCollisionFile(ASSET_KM2_KUZURE_KCL);

/* ROM picture of the unclaimed run. Interior labels stay in symbols.txt. */
u8 data_ov045_02113270[0x10];
