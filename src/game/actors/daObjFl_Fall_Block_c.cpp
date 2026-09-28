//cpp
/**
 * Lethal Lava Land falling block (FL_KUZURE).
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model, KCL, and CLPS to the shared ov098 helpers.
 * daObjFallBlock_c_InitResources loads the model with Model::LoadFile,
 * the KCL with dBgW_Kc::LoadFile, and the CLPS into SetFile. The ov022
 * sinit constructs those SharedFilePtrs as file IDs 1529 and 1530.
 *
 * daObjFl_Fall_Block_c_classInit is reconstructed (RTTI
 * daObjFl_Fall_Block_c, FL_KUZURE registry). Retail does not store
 * that spelling.
 *
 * Leftover: CleanupResources stays a call to func_ov098_0213a2cc.
 *   Retail 0x02112434 is 0x14 bytes and only loads that symbol
 *   (0x0213a2cc) plus data_ov022_0211427c. The callee itself calls
 *   dBgW::IsEnabled and dBgW::Disable on mMeshCollider (this+0x124),
 *   then SharedFilePtr::Release on the model and KCL slots. Those
 *   three calls already match the named methods, but they are the
 *   0x48 body in ov098, not this leaf. The base slot is pure virtual,
 *   so the free-function name stays with that body.
 * Leftover: data_ov022_0211427c is overlay .data this TU does not own.
 *   It points at data_ov022_02114648 (model), data_ov022_02114640
 *   (KCL), and data_ov064_0211ba8c (CLPS).
 * Leftover: g_profile_FL_KUZURE lives outside this TU.
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
