//cpp
/**
 * Lethal Lava Land's tilting slab.
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjGuragura_c's shared ov002 helpers.
 * func_ov002_020b6244 loads slot 0 with Model::LoadFile, slot 1 with
 * dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile. ov064 sinit
 * constructs those SharedFilePtrs as file IDs 1523 / 1524.
 *
 * daObjFl_Gura_c_classInit is reconstructed (RTTI daObjFl_Gura_c,
 * FL_GURA registry). Retail does not store that spelling.
 *
 * deslop
 * Leftover: func_ov002_020b6244 / func_ov002_020b60fc are still
 *   linker names. Naming belongs in ov002.
 * Leftover: data_ov064_0211adb0 is overlay .rodata this TU does not
 *   own; the BMD/KCL SharedFilePtrs and CLPS_Block are still
 *   data_ov064_*.
 */

#include "daObjFl_Gura_c.h"
#include "SharedFilePtr.h"

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[
    sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern "C" {
int func_ov002_020b6244(daObjFl_Gura_c *self, ResourceDescriptor *descriptor);
int func_ov002_020b60fc(daObjFl_Gura_c *self, ResourceDescriptor *descriptor);
extern ResourceDescriptor data_ov064_0211adb0;
}

struct FlGuraSpawnInfo {
    daObjFl_Gura_c *(*classInit)();
    s16 executePriority; /* +4: also FL_GURA registry id 0x0048 = 72 */
    s16 renderPriority;  /* +6 */
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};
typedef char FlGuraSpawnInfo_size_must_be_0x1c[
    sizeof(FlGuraSpawnInfo) == 0x1c ? 1 : -1];

// @symbol daObjFl_Gura_c_classInit
extern "C" daObjFl_Gura_c *daObjFl_Gura_c_classInit()
{
    return new daObjFl_Gura_c();
}

// @symbol g_profile_FL_GURA
extern "C" FlGuraSpawnInfo g_profile_FL_GURA = {
    daObjFl_Gura_c_classInit,
    0x0048,
    0x0118,
    2,
    0x00080000,
    0x001a0000,
    0x02000000,
    0
};

// @symbol _ZN14daObjFl_Gura_c13InitResourcesEv
s32 daObjFl_Gura_c::InitResources()
{
    return func_ov002_020b6244(this, &data_ov064_0211adb0);
}

// @symbol _ZN14daObjFl_Gura_c16CleanupResourcesEv
s32 daObjFl_Gura_c::CleanupResources()
{
    return func_ov002_020b60fc(this, &data_ov064_0211adb0);
}
