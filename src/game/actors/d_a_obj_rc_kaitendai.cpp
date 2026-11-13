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

#include "ResourceDescriptor.h"

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

extern s16 data_ov036_02113b18;
extern ResourceDescriptor data_ov036_02113b2c;
extern struct CLPS_Block data_ov036_02112bc8;
extern s16 data_ov036_02113b1c;

 daObjRc_Kaitendai_c *daObjRc_Kaitendai_c_classInit() {
    return new daObjRc_Kaitendai_c();
}

s32 daObjRc_Kaitendai_c::InitResources() {
    s16 angle = data_ov036_02113b18;
    if ((param1 & 0xff) == 1)
        angle = data_ov036_02113b1c;
    return InitResourcesFrom(&data_ov036_02113b2c, angle);
}

s32 daObjRc_Kaitendai_c::CleanupResources() {
    return CleanupResourcesFrom(&data_ov036_02113b2c);
}

/* Static-init globals (was the handwritten __sinit_ov036_0211261c shard).
 * Definition order is the retail initializer's construction order. */
RcKaitendaiModelFile gRcKaitenModelFile(ASSET_RC_KAITEN_BMD);
RcKaitendaiCollisionFilePtr gRcKaitenCollisionFile(ASSET_RC_KAITEN_KCL);

ResourceDescriptor data_ov036_02113b2c = {
    &gRcKaitenModelFile,
    &gRcKaitenCollisionFile,
    &data_ov036_02112bc8,
};
