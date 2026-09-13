//cpp
/**
 * Bowser in the Sky's drifting stairs (`dorifu`). Twin of the BitDW
 * leaf (`daObjKm1_Dorifu_c`).
 *
 * No fields of its own. InitResources / CleanupResources hand this
 * overlay's five-plank file table to daObjDorifu_c's shared
 * overloads. InitResources loads each slot 0 with Model::LoadFile,
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
 * deslop
 * Leftover: data_ov047_02112508 and g_profile_KM3_DORIFU are
 *   overlay data this TU does not own; the BMD/KCL SharedFilePtrs
 *   and CLPS_Block are still data_ov047_*.
 */

#include "daObjKm3_Dorifu_c.h"

extern "C" {
extern daObjDorifuResources data_ov047_02112508[5];
}

// @symbol daObjKm3_Dorifu_c_classInit
extern "C" daObjKm3_Dorifu_c *daObjKm3_Dorifu_c_classInit()
{
    return new daObjKm3_Dorifu_c();
}

// @symbol _ZN17daObjKm3_Dorifu_c13InitResourcesEv
s32 daObjKm3_Dorifu_c::InitResources()
{
    return daObjDorifu_c::InitResources(data_ov047_02112508);
}

// @symbol _ZN17daObjKm3_Dorifu_c16CleanupResourcesEv
s32 daObjKm3_Dorifu_c::CleanupResources()
{
    return daObjDorifu_c::CleanupResources(data_ov047_02112508);
}
