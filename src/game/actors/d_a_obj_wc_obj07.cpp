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
 * What stays: the BMD/KCL SharedFilePtrs and CLPS_Block are still
 *   data_ov029_*. The start-angle halfword 02113fc4 sits 4 bytes
 *   before this class's typeinfo at 02113fc8; it is not owned here.
 * What stays: g_profile_WC_UKISIMA lives outside this TU (S14).
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_wc_obj07_AssetId { ASSET_WC_OBJ07_BMD = 0x06d7, ASSET_WC_OBJ07_KCL = 0x06d8 };

#include "daObjWc_Obj07_c.h"
#include "SharedFilePtr.h"

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern ResourceDescriptor data_ov029_02113fd4;
extern s16 data_ov029_02113fc4;

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

// @symbol daObjWc_Obj07_c_classInit
extern "C" daObjWc_Obj07_c *daObjWc_Obj07_c_classInit() {
    return new daObjWc_Obj07_c();
}

// @symbol _ZN15daObjWc_Obj07_c13InitResourcesEv
s32 daObjWc_Obj07_c::InitResources() {
    return InitResourcesFrom(&data_ov029_02113fd4, data_ov029_02113fc4);
}

// @symbol _ZN15daObjWc_Obj07_c16CleanupResourcesEv
s32 daObjWc_Obj07_c::CleanupResources() {
    return CleanupResourcesFrom(&data_ov029_02113fd4);
}

/* The static-init globals -- mwcc emits __sinit_d_a_obj_wc_obj07.cpp
   from these: one ctor veneer plus destructor registration per handle. */
WcObj07ModelFilePtr gWcObj07ModelFile(ASSET_WC_OBJ07_BMD);
WcObj07CollisionFilePtr gWcObj07CollisionFile(ASSET_WC_OBJ07_KCL);
