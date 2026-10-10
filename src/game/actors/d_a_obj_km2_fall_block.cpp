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
 * deslop
 * Leftover: func_ov098_0213a2cc is still the linker name of
 *   daObjFallBlock_c's Cleanup (the base leaves that slot pure
 *   virtual). Naming belongs in ov098.
 *   daObjFallBlock_c_InitResources loads slot 0 with Model::LoadFile,
 *   slot 1 with dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile.
 * Leftover: the BMD/KCL SharedFilePtrs and CLPS_Block are still
 *   data_ov045_*. g_profile_KM2_KUZURE lives outside this TU (S14).
 */

#include "daObjKm2_Fall_Block_c.h"
#include "SharedFilePtr.h"

/* The model handle constructs through func_02017acc and destroys through
 * func_02017ab4. The collision handle constructs through func_02017b4c and
 * destroys through SharedFilePtr_Destruct_Clsn. The descriptor stays ROM data. */
struct Km2FallModelFilePtr : SharedFilePtr {
    unsigned int words[2];
    Km2FallModelFilePtr(unsigned int fileId);
    ~Km2FallModelFilePtr();
};
struct Km2FallClsnFileHandle : SharedFilePtr {
    unsigned int words[2];
    Km2FallClsnFileHandle(unsigned int fileId);
    ~Km2FallClsnFileHandle();
};
typedef char Km2FallModelFilePtr_size_must_be_8[
    sizeof(Km2FallModelFilePtr) == 8 ? 1 : -1];
typedef char Km2FallClsnFileHandle_size_must_be_8[
    sizeof(Km2FallClsnFileHandle) == 8 ? 1 : -1];

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[
    sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern "C" {
int daObjFallBlock_c_InitResources(daObjFallBlock_c *self, ResourceDescriptor *descriptor);
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
    return daObjFallBlock_c_InitResources(this, &data_ov045_021130ac);
}

// @symbol _ZN21daObjKm2_Fall_Block_c16CleanupResourcesEv
s32 daObjKm2_Fall_Block_c::CleanupResources()
{
    return func_ov098_0213a2cc(this, &data_ov045_021130ac);
}

/* Retail construction order: model 1633, then collision 1634.
 * mwcc emits __sinit_d_a_obj_km2_fall_block.cpp. */
Km2FallModelFilePtr data_ov045_02113250(1633);
Km2FallClsnFileHandle data_ov045_02113248(1634);
