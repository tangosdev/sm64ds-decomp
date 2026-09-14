//cpp
/**
 * Big Boo's Haunt falling block.
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjFallBlock_c's shared ov098
 * helpers.
 * func_ov098_0213a794 loads slot 0 with Model::LoadFile, slot 1
 * with dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile. ov063 sinit
 * constructs those SharedFilePtrs as file IDs 1721 / 1722.
 *
 * daObjTh_Fall_Block_c_classInit is reconstructed (RTTI
 * daObjTh_Fall_Block_c, TH_DOWN_B registry). Retail does not store
 * that spelling.
 *
 * deslop
 * Leftover: func_ov098_0213a794 / func_ov098_0213a2cc are still the
 *   linker names of daObjFallBlock_c Init/Cleanup (the base leaves
 *   those slots pure virtual). Naming belongs in ov098.
 * Leftover: data_ov063_0211eb10 is overlay .data this TU does not
 *   own; the BMD/KCL SharedFilePtrs and CLPS_Block are still
 *   data_ov063_*.
 * Leftover: g_profile_TH_DOWN_B lives outside this TU (S14).
 */

#include "daObjTh_Fall_Block_c.h"
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
int func_ov098_0213a794(daObjTh_Fall_Block_c *self, ResourceDescriptor *descriptor);
int func_ov098_0213a2cc(daObjTh_Fall_Block_c *self, ResourceDescriptor *descriptor);
extern ResourceDescriptor data_ov063_0211eb10;
}

// @symbol daObjTh_Fall_Block_c_classInit
extern "C" daObjTh_Fall_Block_c *daObjTh_Fall_Block_c_classInit()
{
    return new daObjTh_Fall_Block_c();
}

// @symbol _ZN20daObjTh_Fall_Block_c13InitResourcesEv
s32 daObjTh_Fall_Block_c::InitResources()
{
    return func_ov098_0213a794(this, &data_ov063_0211eb10);
}

// @symbol _ZN20daObjTh_Fall_Block_c16CleanupResourcesEv
s32 daObjTh_Fall_Block_c::CleanupResources()
{
    return func_ov098_0213a2cc(this, &data_ov063_0211eb10);
}

// @symbol _ZN20daObjTh_Fall_Block_cD1Ev
// @symbol _ZN20daObjTh_Fall_Block_cD0Ev
