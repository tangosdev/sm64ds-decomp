//cpp
/**
 * Lethal Lava Land falling block (FL_KUZURE).
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model, KCL, and CLPS to the shared ov098 helpers.
 * InitResourcesFrom loads the model with dExtSimpleModel_c::LoadFile,
 * the KCL with dBgW_Kc::LoadFile, and the CLPS into SetFile. The ov022
 * sinit constructs those SharedFilePtrs as file IDs 1529 and 1530.
 *
 * daObjFl_Fall_Block_c_classInit is reconstructed (RTTI
 * daObjFl_Fall_Block_c, FL_KUZURE registry). Retail does not store
 * that spelling.
 *
 */
/* CleanupResources is a call to CleanupResourcesFrom. Retail
 * 0x02112434 is 0x14 bytes and only loads that symbol plus
 * data_ov022_0211427c. The callee calls dBgW::IsEnabled and
 * dBgW::Disable on mMeshCollider, then SharedFilePtr::Release on the
 * model and KCL slots. Those three calls are the body in ov098, not
 * this leaf. data_ov022_0211427c is overlay .data this TU does not own.
 * It points at data_ov022_02114648 (model), data_ov022_02114640 (KCL)
 * and data_ov064_0211ba8c (CLPS). g_profile_FL_KUZURE lives outside
 * this TU.
 */

#include "daObjFl_Fall_Block_c.h"
#include "SharedFilePtr.h"

struct CLPS_Block;

/* File-scope objects at the end of this file construct the two resource
 * handles (model file 0x5f9, collision file 0x5fa). Those definitions
 * are this file's static initializer. The wrapper names are local; the
 * handle constructors and destructors are the ROM resource-family
 * functions, aliased in the manifest. */
struct FallBlockModelFile : SharedFilePtr {
    FallBlockModelFile(u32 fileID);
    ~FallBlockModelFile();
};

struct FallBlockCollisionFilePtr : SharedFilePtr {
    FallBlockCollisionFilePtr(u32 fileID);
    ~FallBlockCollisionFilePtr();
};

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern ResourceDescriptor data_ov022_0211427c;

// @symbol daObjFl_Fall_Block_c_classInit
extern "C" daObjFl_Fall_Block_c *daObjFl_Fall_Block_c_classInit() {
    return new daObjFl_Fall_Block_c();
}

// @symbol _ZN20daObjFl_Fall_Block_c13InitResourcesEv
int daObjFl_Fall_Block_c::InitResources() {
    return InitResourcesFrom(&data_ov022_0211427c);
}

// @symbol _ZN20daObjFl_Fall_Block_c16CleanupResourcesEv
int daObjFl_Fall_Block_c::CleanupResources() {
    return CleanupResourcesFrom(&data_ov022_0211427c);
}

// @symbol _ZN20daObjFl_Fall_Block_cD1Ev
// @symbol _ZN20daObjFl_Fall_Block_cD0Ev

/* Static-init globals (was the handwritten __sinit_ov022_02112fe4 shard).
 * Definition order is the retail initializer's construction order: the
 * model handle (file 0x5f9), then the collision handle (file 0x5fa). */
FallBlockModelFile data_ov022_02114648(0x5f9);
FallBlockCollisionFilePtr data_ov022_02114640(0x5fa);
