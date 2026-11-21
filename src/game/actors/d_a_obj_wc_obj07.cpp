//cpp
/**
 * Wet-Dry World's spinning disc.
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjKaitendai_c's shared ov002
 * helpers, plus a starting angle at data_ov029_02113fc4.
 * InitResourcesFrom types the slots as SharedFilePtr *m[2]
 * and CLPS_Block *clps.
 *
 * daObjWc_Obj07_c_classInit is reconstructed (RTTI daObjWc_Obj07_c,
 * WC_UKISIMA registry). Retail does not store that spelling.
 *
 * The CLPS block is data_ov029_021130ac, defined here. The start-angle
 * slot 02113fc4 sits 4 bytes before this class's typeinfo at 02113fc8.
 * What stays: g_profile_WC_UKISIMA lives outside this TU (S14).
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_wc_obj07_AssetId { ASSET_WC_OBJ07_BMD = 0x06d7, ASSET_WC_OBJ07_KCL = 0x06d8 };

#include "SpawnInfo.h"
#include "daObjWc_Obj07_c.h"
#include "SharedFilePtr.h"

#include "ResourceDescriptor.h"

/* Collision properties: the 'CLPS' tag, the 8-byte stride header with two
 * entries, then the default entries. */
struct CLPS_Block {
    u32 tag;
    u32 header;
    u32 entries[6];
};
CLPS_Block data_ov029_021130ac = {
    0x53504c43,
    0x00020008,
    {0x01000fc4, 0x000000ff, 0x01000fc0, 0x000000ff, 0xffffffff, 0xffffffff},
};

/* Retail constructs these 8-byte handles in this order and registers
 * their destructors. The wrapper names are local; the ctor/dtor
 * addresses are the ROM resource-family functions. */
struct WcObj07ModelFilePtr : SharedFilePtr {
    WcObj07ModelFilePtr(u32 fileID);
    ~WcObj07ModelFilePtr();
};

struct WcObj07CollisionFilePtr : SharedFilePtr {
    WcObj07CollisionFilePtr(u32 fileID);
    ~WcObj07CollisionFilePtr();
};

extern WcObj07ModelFilePtr gWcObj07ModelFile;
extern WcObj07CollisionFilePtr gWcObj07CollisionFile;

ResourceDescriptor data_ov029_02113fd4 = {
    &gWcObj07ModelFile,
    &gWcObj07CollisionFile,
    &data_ov029_021130ac,
};

/* The starting angle InitResourcesFrom feeds its rotation clamp. Retail gives
   it a 4-byte data slot; the tail halfword is zero. */
s16 data_ov029_02113fc4[2] = {0x444};

daObjWc_Obj07_c *daObjWc_Obj07_c_classInit() {
    return new daObjWc_Obj07_c();
}

s32 daObjWc_Obj07_c::InitResources() {
    return InitResourcesFrom(&data_ov029_02113fd4, data_ov029_02113fc4[0]);
}

s32 daObjWc_Obj07_c::CleanupResources() {
    return CleanupResourcesFrom(&data_ov029_02113fd4);
}

/* The static-init globals -- mwcc emits __sinit_d_a_obj_wc_obj07.cpp
   from these: one ctor veneer plus destructor registration per handle. */
WcObj07ModelFilePtr gWcObj07ModelFile(ASSET_WC_OBJ07_BMD);
WcObj07CollisionFilePtr gWcObj07CollisionFile(ASSET_WC_OBJ07_KCL);

extern "C" SpawnInfo<daObjWc_Obj07_c> g_profile_WC_UKISIMA = {
    daObjWc_Obj07_c_classInit, 94, 270, 0x00000002, 0x00000000, 0x00110000, 0x01c20000, 0x00640000};
