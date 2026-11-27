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

#include "CLPS.h"
#include "daObjFl_Gura_c.h"
#include "SpawnInfo.h"
#include "SharedFilePtr.h"

#include "ResourceDescriptor.h"

extern const ResourceDescriptor data_ov064_0211adb0;

daObjFl_Gura_c *daObjFl_Gura_c_classInit() {
    return new daObjFl_Gura_c();
}

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

s32 daObjFl_Gura_c::InitResources() {
    return InitResourcesFrom((ResourceDescriptor *)&data_ov064_0211adb0);
}

s32 daObjFl_Gura_c::CleanupResources() {
    return CleanupResourcesFrom((ResourceDescriptor *)&data_ov064_0211adb0);
}

/* File-scope resource handles: the ctor/dtor are the ROM's SharedFilePtr
 * veneer pairs (ModelFile::ModelFile / ModelFile::~ModelFile for the model, ClsnFile::ClsnFile /
 * ClsnFile::~ClsnFile for the collision), spelled through
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

/* The retail initializer constructs the model first, then the collision
 * file. */
FlGuraModelFilePtr gFlGuraModelFile(ASSET_FL_GURA_BMD);         /* model */
FlGuraCollisionFilePtr gFlGuraCollisionFile(ASSET_FL_GURA_KCL); /* collision */

CLPS_Block data_ov064_0211bb8c = {
    0x53504c43,
    0x0008,
    0x0002,
    {{0x00002fc0, 0x000000ff}, {0x00000fc0, 0x000000ff}, {0xffffffff, 0xffffffff}},
};

extern const ResourceDescriptor data_ov064_0211adb0 = {&gFlGuraModelFile, &gFlGuraCollisionFile,
                                                       &data_ov064_0211bb8c};
