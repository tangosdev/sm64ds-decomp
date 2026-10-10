//cpp
/**
 * Big Boo's Haunt falling block.
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjFallBlock_c's shared ov098
 * helpers.
 * daObjFallBlock_c_InitResources loads slot 0 with Model::LoadFile, slot 1
 * with dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile. ov063 sinit
 * constructs those SharedFilePtrs as file IDs 1721 / 1722.
 *
 * daObjTh_Fall_Block_c_classInit is reconstructed (RTTI
 * daObjTh_Fall_Block_c, TH_DOWN_B registry). Retail does not store
 * that spelling.
 *
 * deslop
 * Leftover: func_ov098_0213a2cc is still the linker name of
 *   daObjFallBlock_c Cleanup (the base leaves those slots pure virtual).
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
int daObjFallBlock_c_InitResources(daObjFallBlock_c *self, ResourceDescriptor *descriptor);
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
    return daObjFallBlock_c_InitResources(this, &data_ov063_0211eb10);
}

// @symbol _ZN20daObjTh_Fall_Block_c16CleanupResourcesEv
s32 daObjTh_Fall_Block_c::CleanupResources()
{
    return func_ov098_0213a2cc(this, &data_ov063_0211eb10);
}

// @symbol _ZN20daObjTh_Fall_Block_cD1Ev
// @symbol _ZN20daObjTh_Fall_Block_cD0Ev

/* File-scope resource handles: the ctor/dtor are the ROM's SharedFilePtr
 * veneer pairs (func_02017acc / func_02017ab4 for the model, func_02017b4c /
 * SharedFilePtr_Destruct_Clsn for the collision), spelled through
 * declared-only subclasses so the static initializer names the real entry
 * points. */
struct ThFallBlockModelFilePtr : SharedFilePtr {
    u32 words[2];
    ThFallBlockModelFilePtr(u32 fileID);
    ~ThFallBlockModelFilePtr();
};
struct ThFallBlockCollisionFilePtr : SharedFilePtr {
    u32 words[2];
    ThFallBlockCollisionFilePtr(u32 fileID);
    ~ThFallBlockCollisionFilePtr();
};

// @symbol __sinit_d_a_obj_th_fall_block.cpp
/* The retail initializer constructs the model first, then the collision
 * file. */
ThFallBlockModelFilePtr data_ov063_0211ef60(0x6b9);      /* model */
ThFallBlockCollisionFilePtr data_ov063_0211ef58(0x6ba);  /* collision */
