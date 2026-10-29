//cpp
/**
 * daKpa_c in the Sky's drifting stairs (`dorifu`). Twin of the BitDW
 * leaf (`daObjKm1_Dorifu_c`).
 *
 * No fields of its own. InitResources / CleanupResources hand this
 * overlay's five-plank file table to daObjDorifu_c's shared
 * overloads. InitResources loads each slot 0 with dExtSimpleModel_c::LoadFile,
 * slot 1 with dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile.
 * ov047 sinit constructs those SharedFilePtrs as file IDs
 * 1647 / 1648, 1649 / 1650, 1651 / 1652, 1653 / 1654, 1655 / 1656.
 *
 * daObjKm3_Dorifu_c_classInit is reconstructed (RTTI
 * daObjKm3_Dorifu_c, KM3_DORIFU registry). Retail does not
 * store that spelling.
 *
 * This TU is text-only. Retail overlay data order is typeinfo,
 * type-name, profile, five-entry file table, vtable.
 *
 * Note: data_ov047_02112508 and g_profile_KM3_DORIFU are overlay data
 * this TU does not own; the BMD/KCL SharedFilePtrs and CLPS_Block are
 * still data_ov047_*.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_km3_dorifu_AssetId {
    ASSET_KM3_DAN0_BMD = 0x066f,
    ASSET_KM3_DAN0_KCL = 0x0670,
    ASSET_KM3_DAN1_BMD = 0x0671,
    ASSET_KM3_DAN1_KCL = 0x0672,
    ASSET_KM3_DAN2_BMD = 0x0673,
    ASSET_KM3_DAN2_KCL = 0x0674,
    ASSET_KM3_DAN3_BMD = 0x0675,
    ASSET_KM3_DAN3_KCL = 0x0676,
    ASSET_KM3_DAN4_BMD = 0x0677,
    ASSET_KM3_DAN4_KCL = 0x0678
};

#include "daObjKm3_Dorifu_c.h"
#include "SharedFilePtr.h"

/* Five model handles construct through func_02017acc and destroy through
 * func_02017ab4. Five collision handles construct through func_02017b4c and
 * destroy through SharedFilePtr_Destruct_Clsn. The five-plank table stays
 * ROM data. */
struct DorifuModelFilePtr : SharedFilePtr {
    DorifuModelFilePtr(unsigned int fileId);
    ~DorifuModelFilePtr();
};
struct DorifuClsnFileHandle : SharedFilePtr {
    DorifuClsnFileHandle(unsigned int fileId);
    ~DorifuClsnFileHandle();
};
typedef char DorifuModelFilePtr_size_must_be_8[sizeof(DorifuModelFilePtr) == 8 ? 1 : -1];
typedef char DorifuClsnFileHandle_size_must_be_8[sizeof(DorifuClsnFileHandle) == 8 ? 1 : -1];

extern daObjDorifuResources data_ov047_02112508[5];

// @symbol daObjKm3_Dorifu_c_classInit
extern "C" daObjKm3_Dorifu_c *daObjKm3_Dorifu_c_classInit() {
    return new daObjKm3_Dorifu_c();
}

// @symbol _ZN17daObjKm3_Dorifu_c13InitResourcesEv
s32 daObjKm3_Dorifu_c::InitResources() {
    return daObjDorifu_c::InitResources(data_ov047_02112508);
}

// @symbol _ZN17daObjKm3_Dorifu_c16CleanupResourcesEv
s32 daObjKm3_Dorifu_c::CleanupResources() {
    return daObjDorifu_c::CleanupResources(data_ov047_02112508);
}

/* Retail construction order. mwcc emits __sinit_d_a_obj_km3_dorifu.cpp. */
DorifuModelFilePtr gKm3Dan0ModelFile(ASSET_KM3_DAN0_BMD);
DorifuModelFilePtr gKm3Dan1ModelFile(ASSET_KM3_DAN1_BMD);
DorifuModelFilePtr gKm3Dan2ModelFile(ASSET_KM3_DAN2_BMD);
DorifuModelFilePtr gKm3Dan3ModelFile(ASSET_KM3_DAN3_BMD);
DorifuModelFilePtr gKm3Dan4ModelFile(ASSET_KM3_DAN4_BMD);
DorifuClsnFileHandle gKm3Dan0CollisionFile(ASSET_KM3_DAN0_KCL);
DorifuClsnFileHandle gKm3Dan1CollisionFile(ASSET_KM3_DAN1_KCL);
DorifuClsnFileHandle gKm3Dan2CollisionFile(ASSET_KM3_DAN2_KCL);
DorifuClsnFileHandle gKm3Dan3CollisionFile(ASSET_KM3_DAN3_KCL);
DorifuClsnFileHandle gKm3Dan4CollisionFile(ASSET_KM3_DAN4_KCL);
