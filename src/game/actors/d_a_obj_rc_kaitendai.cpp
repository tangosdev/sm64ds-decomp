//cpp
/**
 * Rainbow Ride's spinning disc.
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjKaitendai_c's shared ov002
 * helpers. Start angle is data_ov036_02113b18, or
 * data_ov036_02113b1c when param1's low byte is 1.
 *
 * daObjRc_Kaitendai_c_classInit is reconstructed (RTTI
 * daObjRc_Kaitendai_c, RC_KAITEN registry). Retail does not store
 * that spelling.
 *
 * Notes:
 * - The two start-angle halfwords 02113b18 / 02113b1c sit 8 bytes before
 *   typeinfo at 02113b20; they are not owned here.
 * - g_profile_RC_KAITEN lives outside this TU.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_rc_kaitendai_AssetId { ASSET_RC_KAITEN_BMD = 0x06a7, ASSET_RC_KAITEN_KCL = 0x06a8 };

#include "daObjRc_Kaitendai_c.h"
#include "SharedFilePtr.h"

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

/* File-scope objects at the end of this file construct the two resource
 * handles (model file 0x6a7, collision file 0x6a8). mwcc emits
 * __sinit_d_a_obj_rc_kaitendai.cpp from those definitions. The wrapper
 * names are local; the handle constructors and destructors are the ROM
 * resource-family functions, aliased in the manifest. Nothing in this TU
 * names the handles directly; the ResourceDescriptor row above points at
 * them. */
struct RcKaitendaiModelFile : SharedFilePtr {
    RcKaitendaiModelFile(u32 fileID);
    ~RcKaitendaiModelFile();
};

struct RcKaitendaiCollisionFilePtr : SharedFilePtr {
    RcKaitendaiCollisionFilePtr(u32 fileID);
    ~RcKaitendaiCollisionFilePtr();
};

extern ResourceDescriptor data_ov036_02113b2c;
extern s16 data_ov036_02113b18;
extern s16 data_ov036_02113b1c;

// @symbol daObjRc_Kaitendai_c_classInit
extern "C" daObjRc_Kaitendai_c *daObjRc_Kaitendai_c_classInit() {
    return new daObjRc_Kaitendai_c();
}

// @symbol _ZN19daObjRc_Kaitendai_c13InitResourcesEv
s32 daObjRc_Kaitendai_c::InitResources() {
    s16 angle = data_ov036_02113b18;
    if ((param1 & 0xff) == 1)
        angle = data_ov036_02113b1c;
    return InitResourcesFrom(&data_ov036_02113b2c, angle);
}

// @symbol _ZN19daObjRc_Kaitendai_c16CleanupResourcesEv
s32 daObjRc_Kaitendai_c::CleanupResources() {
    return CleanupResourcesFrom(&data_ov036_02113b2c);
}

/* Static-init globals (was the handwritten __sinit_ov036_0211261c shard).
 * Definition order is the retail initializer's construction order. */
RcKaitendaiModelFile gRcKaitenModelFile(ASSET_RC_KAITEN_BMD);
RcKaitendaiCollisionFilePtr gRcKaitenCollisionFile(ASSET_RC_KAITEN_KCL);
