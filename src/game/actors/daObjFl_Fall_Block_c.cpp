//cpp
/**
 * Lethal Lava Land falling block.
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjFallBlock_c's shared ov098
 * helpers.
 * daObjFallBlock_c_InitResources loads slot 0 with Model::LoadFile, slot 1
 * with dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile. ov022 sinit
 * constructs those SharedFilePtrs as file IDs 1529 / 1530.
 *
 * daObjFl_Fall_Block_c_classInit is reconstructed (RTTI
 * daObjFl_Fall_Block_c, FL_KUZURE registry). Retail does not store
 * that spelling.
 *
 * deslop
 * Leftover: func_ov098_0213a2cc is still the linker name of
 *   daObjFallBlock_c Cleanup (the base leaves those slots pure virtual).
 * Leftover: data_ov022_0211427c is overlay .data this TU does not
 *   own; the BMD/KCL SharedFilePtrs stay data_ov022_*, CLPS is
 *   data_ov064_0211ba8c.
 * Leftover: g_profile_FL_KUZURE lives outside this TU (S14).
 */

#include "daObjFl_Fall_Block_c.h"
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
int func_ov098_0213a2cc(daObjFl_Fall_Block_c *self, ResourceDescriptor *descriptor);
int daObjFallBlock_c_InitResources(daObjFallBlock_c *self, ResourceDescriptor *descriptor);
extern ResourceDescriptor data_ov022_0211427c;
}

// @symbol daObjFl_Fall_Block_c_classInit
extern "C" daObjFl_Fall_Block_c *daObjFl_Fall_Block_c_classInit()
{
    return new daObjFl_Fall_Block_c();
}

// @symbol _ZN20daObjFl_Fall_Block_c13InitResourcesEv
int daObjFl_Fall_Block_c::InitResources()
{
    return daObjFallBlock_c_InitResources(this, &data_ov022_0211427c);
}

// @symbol _ZN20daObjFl_Fall_Block_c16CleanupResourcesEv
int daObjFl_Fall_Block_c::CleanupResources()
{
    return func_ov098_0213a2cc(this, &data_ov022_0211427c);
}

// @symbol _ZN20daObjFl_Fall_Block_cD1Ev
// @symbol _ZN20daObjFl_Fall_Block_cD0Ev
