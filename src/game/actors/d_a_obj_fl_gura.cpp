//cpp
/**
 * Lethal Lava Land's tilting slab.
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjGuragura_c's shared ov002 helpers.
 * InitResourcesFrom loads slot 0 with dExtSimpleModel_c::LoadFile, slot 1 with
 * dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile. ov064 sinit
 * constructs those SharedFilePtrs as file IDs 1523 / 1524.
 *
 * daObjFl_Gura_c_classInit is reconstructed (RTTI daObjFl_Gura_c,
 * FL_GURA registry). Retail does not store that spelling.
 *
 * What stays: data_ov064_0211adb0 is overlay .rodata this TU does not
 * own; the BMD/KCL SharedFilePtrs and CLPS_Block are still data_ov064_*.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_fl_gura_AssetId { ASSET_FL_GURA_BMD = 0x05f3, ASSET_FL_GURA_KCL = 0x05f4 };

#include "daObjFl_Gura_c.h"
#include "SpawnInfo.h"
#include "SharedFilePtr.h"

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern ResourceDescriptor data_ov064_0211adb0;

// @symbol daObjFl_Gura_c_classInit
extern "C" daObjFl_Gura_c *daObjFl_Gura_c_classInit() {
    return new daObjFl_Gura_c();
}

// @symbol g_profile_FL_GURA
extern "C" SpawnInfo<daObjFl_Gura_c> g_profile_FL_GURA = {
    daObjFl_Gura_c_classInit,
    0x0048, /* FL_GURA registry id 72 */
    0x0118,
    2,
    0x00080000,
    0x001a0000,
    0x02000000,
    0,
};

// @symbol _ZN14daObjFl_Gura_c13InitResourcesEv
s32 daObjFl_Gura_c::InitResources() {
    return InitResourcesFrom(&data_ov064_0211adb0);
}

// @symbol _ZN14daObjFl_Gura_c16CleanupResourcesEv
s32 daObjFl_Gura_c::CleanupResources() {
    return CleanupResourcesFrom(&data_ov064_0211adb0);
}

/* File-scope resource handles: the ctor/dtor are the ROM's SharedFilePtr
 * veneer pairs (func_02017acc / func_02017ab4 for the model, func_02017b4c /
 * SharedFilePtr_Destruct_Clsn for the collision), spelled through
 * declared-only subclasses so the static initializer names the real entry
 * points. */
struct FlGuraModelFilePtr : SharedFilePtr {
    FlGuraModelFilePtr(u32 fileID);
    ~FlGuraModelFilePtr();
};
struct FlGuraCollisionFilePtr : SharedFilePtr {
    FlGuraCollisionFilePtr(u32 fileID);
    ~FlGuraCollisionFilePtr();
};

// @symbol __sinit_d_a_obj_fl_gura.cpp
/* The retail initializer constructs the model first, then the collision
 * file. */
FlGuraModelFilePtr gFlGuraModelFile(ASSET_FL_GURA_BMD);         /* model */
FlGuraCollisionFilePtr gFlGuraCollisionFile(ASSET_FL_GURA_KCL); /* collision */
