//cpp
/**
 * Big Boo's Haunt falling block.
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjFallBlock_c's shared ov098
 * helpers.
 * InitResourcesFrom loads slot 0 with dExtSimpleModel_c::LoadFile, slot 1
 * with dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile. ov063 sinit
 * constructs those SharedFilePtrs as file IDs 1721 / 1722.
 *
 * daObjTh_Fall_Block_c_classInit is reconstructed (RTTI
 * daObjTh_Fall_Block_c, TH_DOWN_B registry). Retail does not store
 * that spelling.
 *
 * data_ov063_0211eb10 is overlay .data this TU does not own; the
 * CLPS_Block it points at is still data_ov063_*. g_profile_TH_DOWN_B
 * lives outside this TU (S14).
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_th_fall_block_AssetId { ASSET_TH_DOWN_B_BMD = 0x06b9, ASSET_TH_DOWN_B_KCL = 0x06ba };

#include "daObjTh_Fall_Block_c.h"
#include "SharedFilePtr.h"

#include "ResourceDescriptor.h"

extern ResourceDescriptor data_ov063_0211eb10;

daObjTh_Fall_Block_c *daObjTh_Fall_Block_c_classInit() {
    return new daObjTh_Fall_Block_c();
}

s32 daObjTh_Fall_Block_c::InitResources() {
    return InitResourcesFrom(&data_ov063_0211eb10);
}

s32 daObjTh_Fall_Block_c::CleanupResources() {
    return CleanupResourcesFrom(&data_ov063_0211eb10);
}


/* File-scope resource handles: the ctor/dtor are the ROM's SharedFilePtr
 * veneer pairs (func_02017acc / func_02017ab4 for the model, func_02017b4c /
 * SharedFilePtr_Destruct_Clsn for the collision), spelled through
 * declared-only subclasses so the static initializer names the real entry
 * points. */
struct ThFallBlockModelFilePtr : SharedFilePtr {
    ThFallBlockModelFilePtr(u32 fileID);
    ~ThFallBlockModelFilePtr();
};
struct ThFallBlockCollisionFilePtr : SharedFilePtr {
    ThFallBlockCollisionFilePtr(u32 fileID);
    ~ThFallBlockCollisionFilePtr();
};

/* The retail initializer constructs the model first, then the collision
 * file. */
ThFallBlockModelFilePtr gThDownBModelFile(ASSET_TH_DOWN_B_BMD);         /* model */
ThFallBlockCollisionFilePtr gThDownBCollisionFile(ASSET_TH_DOWN_B_KCL); /* collision */
