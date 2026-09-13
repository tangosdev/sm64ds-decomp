//cpp
/**
 * Bowser in the Fire Sea's falling block.
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjFallBlock_c's shared ov098
 * helpers.
 *
 * daObjKm2_Fall_Block_c_classInit is reconstructed (RTTI
 * daObjKm2_Fall_Block_c, KM2_KUZURE registry). Retail does not store
 * that spelling.
 *
 * deslop
 * Leftover: func_ov098_0213a794 / func_ov098_0213a2cc are still the
 *   linker names of daObjFallBlock_c Init/Cleanup (the base leaves
 *   those slots pure virtual). Naming belongs in ov098. That helper
 *   loads slot 0 with Model::LoadFile, slot 1 with dBgW_Kc::LoadFile,
 *   slot 2 as CLPS into SetFile.
 * Leftover: the BMD/KCL SharedFilePtrs and CLPS_Block are still
 *   data_ov045_*. g_profile_KM2_KUZURE lives outside this TU (S14).
 */

#include "daObjKm2_Fall_Block_c.h"
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
int func_ov098_0213a794(daObjKm2_Fall_Block_c *self, ResourceDescriptor *descriptor);
int func_ov098_0213a2cc(daObjKm2_Fall_Block_c *self, ResourceDescriptor *descriptor);
extern ResourceDescriptor data_ov045_021130ac;
}

// @symbol daObjKm2_Fall_Block_c_classInit
extern "C" daObjKm2_Fall_Block_c *daObjKm2_Fall_Block_c_classInit()
{
    return new daObjKm2_Fall_Block_c();
}

// @symbol _ZN21daObjKm2_Fall_Block_c13InitResourcesEv
s32 daObjKm2_Fall_Block_c::InitResources()
{
    return func_ov098_0213a794(this, &data_ov045_021130ac);
}

// @symbol _ZN21daObjKm2_Fall_Block_c16CleanupResourcesEv
s32 daObjKm2_Fall_Block_c::CleanupResources()
{
    return func_ov098_0213a2cc(this, &data_ov045_021130ac);
}
