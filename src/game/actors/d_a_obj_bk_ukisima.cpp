//cpp
/**
 * Whomp's Fortress spinning disc.
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjKaitendai_c's shared ov002
 * helpers, plus a starting angle at data_ov015_02114794.
 * InitResourcesFrom types the slots as SharedFilePtr *m[2]
 * and CLPS_Block *clps.
 *
 * daObjBk_Ukisima_c_classInit is reconstructed (RTTI daObjBk_Ukisima_c,
 * BK_UKISIMA registry). Retail does not store that spelling.
 *
 * Notes:
 * - InitResourcesFrom / CleanupResourcesFrom are daObjKaitendai_c's
 *   Init/Cleanup helpers (the base leaves those slots pure virtual),
 *   defined in d_a_obj_kaitendai.cpp.
 * - data_ov015_02114794 is the start-angle halfword.
 * - g_profile_BK_UKISIMA lives outside this TU (S14).
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_bk_ukisima_AssetId { ASSET_BK_UKISIMA_BMD = 0x05a1, ASSET_BK_UKISIMA_KCL = 0x05a2 };

#include "daObjBk_Ukisima_c.h"
#include "SharedFilePtr.h"

/* The model handle constructs through func_02017acc and destroys through
 * func_02017ab4. The collision handle constructs through func_02017b4c and
 * destroys through SharedFilePtr_Destruct_Clsn. The descriptor stays ROM data. */
struct UkisimaModelFilePtr : SharedFilePtr {
    UkisimaModelFilePtr(unsigned int fileId);
    ~UkisimaModelFilePtr();
};
struct UkisimaClsnFileHandle : SharedFilePtr {
    UkisimaClsnFileHandle(unsigned int fileId);
    ~UkisimaClsnFileHandle();
};
typedef char UkisimaModelFilePtr_size_must_be_8[sizeof(UkisimaModelFilePtr) == 8 ? 1 : -1];
typedef char UkisimaClsnFileHandle_size_must_be_8[sizeof(UkisimaClsnFileHandle) == 8 ? 1 : -1];

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern ResourceDescriptor data_ov015_021147a4;
extern s16 data_ov015_02114794;

// @symbol daObjBk_Ukisima_c_classInit
extern "C" daObjBk_Ukisima_c *daObjBk_Ukisima_c_classInit() {
    return new daObjBk_Ukisima_c();
}

// @symbol _ZN17daObjBk_Ukisima_c13InitResourcesEv
s32 daObjBk_Ukisima_c::InitResources() {
    return InitResourcesFrom(&data_ov015_021147a4, data_ov015_02114794);
}

// @symbol _ZN17daObjBk_Ukisima_c16CleanupResourcesEv
s32 daObjBk_Ukisima_c::CleanupResources() {
    return CleanupResourcesFrom(&data_ov015_021147a4);
}

/* Retail construction order. mwcc emits __sinit_d_a_obj_bk_ukisima.cpp. */
UkisimaModelFilePtr gBkUkisimaModelFile(ASSET_BK_UKISIMA_BMD);
UkisimaClsnFileHandle gBkUkisimaCollisionFile(ASSET_BK_UKISIMA_KCL);
