//cpp
/**
 * daKpa_c in the Dark World's drifting stairs (`dorifu`).
 *
 * No fields of its own. InitResources / CleanupResources hand this
 * overlay's five-plank file table to daObjDorifu_c's shared
 * overloads. Each daObjDorifuResources row is modelFile, clsnFile,
 * clps: InitResources loads modelFile with Model::LoadFile,
 * clsnFile with dBgW_Kc::LoadFile, clps as CLPS into SetFile.
 * ov043 sinit constructs those SharedFilePtrs as file IDs
 * 1609 / 1610, 1611 / 1612, 1613 / 1614, 1615 / 1616, 1617 / 1618.
 *
 * The five-entry table is defined in this TU (retail places it
 * after the profile, before the vtable).
 *
 * The ten file handles at 02112658..021126a8 are defined here too.
 * Model handles construct through func_02017acc and destroy through
 * func_02017ab4; collision handles construct through func_02017b4c
 * and destroy through SharedFilePtr_Destruct_Clsn. The manifest
 * aliases those undefined members onto the ROM symbols, and mwcc
 * emits __sinit_d_a_obj_km1_dorifu.cpp from the definitions at the
 * bottom (retail initializer order, file IDs 0x649..0x652).
 *
 * daObjKm1_Dorifu_c_classInit is reconstructed (RTTI
 * daObjKm1_Dorifu_c, KM1_DORIFU registry). Retail does not
 * store that spelling.
 *
 * deslop
 * Leftover: the CLPS_Block globals are still data_ov043_* linker
 *   names; the handles keep data_ov043_* names as well.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_km1_dorifu_AssetId {
    ASSET_KM1_DORIFU0_BMD = 0x0649,
    ASSET_KM1_DORIFU0_KCL = 0x064a,
    ASSET_KM1_DORIFU1_BMD = 0x064b,
    ASSET_KM1_DORIFU1_KCL = 0x064c,
    ASSET_KM1_DORIFU2_BMD = 0x064d,
    ASSET_KM1_DORIFU2_KCL = 0x064e,
    ASSET_KM1_DORIFU3_BMD = 0x064f,
    ASSET_KM1_DORIFU3_KCL = 0x0650,
    ASSET_KM1_DORIFU4_BMD = 0x0651,
    ASSET_KM1_DORIFU4_KCL = 0x0652
};

#include "daObjKm1_Dorifu_c.h"
#include "SpawnInfo.h"

struct DorifuModelFilePtr : SharedFilePtr {
    u32 words[2];

    DorifuModelFilePtr(u32 fileID);
    ~DorifuModelFilePtr();
};

struct DorifuCollisionFilePtr : SharedFilePtr {
    u32 words[2];

    DorifuCollisionFilePtr(u32 fileID);
    ~DorifuCollisionFilePtr();
};

extern "C" {
extern DorifuModelFilePtr data_ov043_021126a0;
extern DorifuCollisionFilePtr data_ov043_02112678;
extern CLPS_Block data_ov043_02111c40;
extern DorifuModelFilePtr data_ov043_02112680;
extern DorifuCollisionFilePtr data_ov043_02112658;
extern CLPS_Block data_ov043_02111b60;
extern DorifuModelFilePtr data_ov043_02112668;
extern DorifuCollisionFilePtr data_ov043_02112670;
extern CLPS_Block data_ov043_02111ba0;
extern DorifuModelFilePtr data_ov043_02112688;
extern DorifuCollisionFilePtr data_ov043_02112698;
extern CLPS_Block data_ov043_02111c80;
extern DorifuModelFilePtr data_ov043_02112690;
extern DorifuCollisionFilePtr data_ov043_02112660;
extern CLPS_Block data_ov043_02111c60;
}

// @symbol daObjKm1_Dorifu_c_classInit
extern "C" daObjKm1_Dorifu_c *daObjKm1_Dorifu_c_classInit()
{
    return new daObjKm1_Dorifu_c();
}

// @symbol g_profile_KM1_DORIFU
extern "C" SpawnInfo<daObjKm1_Dorifu_c> g_profile_KM1_DORIFU = {
    daObjKm1_Dorifu_c_classInit,
    0x0086, /* KM1_DORIFU registry id 134 */
    0x00af,
    0,
    0x00500000,
    0x01000000,
    0x02000000,
    0
};

extern "C" daObjDorifuResources data_ov043_02112518[5] = {
    {&data_ov043_021126a0, &data_ov043_02112678, &data_ov043_02111c40},
    {&data_ov043_02112680, &data_ov043_02112658, &data_ov043_02111b60},
    {&data_ov043_02112668, &data_ov043_02112670, &data_ov043_02111ba0},
    {&data_ov043_02112688, &data_ov043_02112698, &data_ov043_02111c80},
    {&data_ov043_02112690, &data_ov043_02112660, &data_ov043_02111c60}
};

// @symbol _ZN17daObjKm1_Dorifu_c13InitResourcesEv
s32 daObjKm1_Dorifu_c::InitResources()
{
    return daObjDorifu_c::InitResources(data_ov043_02112518);
}

// @symbol _ZN17daObjKm1_Dorifu_c16CleanupResourcesEv
s32 daObjKm1_Dorifu_c::CleanupResources()
{
    return daObjDorifu_c::CleanupResources(data_ov043_02112518);
}

/* Order is the retail initializer: models 0x649/0x64b/0x64d/0x64f/0x651,
 * then collisions 0x64a/0x64c/0x64e/0x650/0x652. mwcc emits
 * __sinit_d_a_obj_km1_dorifu.cpp from these ten definitions. */
DorifuModelFilePtr data_ov043_021126a0(ASSET_KM1_DORIFU0_BMD);
DorifuModelFilePtr data_ov043_02112680(ASSET_KM1_DORIFU1_BMD);
DorifuModelFilePtr data_ov043_02112668(ASSET_KM1_DORIFU2_BMD);
DorifuModelFilePtr data_ov043_02112688(ASSET_KM1_DORIFU3_BMD);
DorifuModelFilePtr data_ov043_02112690(ASSET_KM1_DORIFU4_BMD);
DorifuCollisionFilePtr data_ov043_02112678(ASSET_KM1_DORIFU0_KCL);
DorifuCollisionFilePtr data_ov043_02112658(ASSET_KM1_DORIFU1_KCL);
DorifuCollisionFilePtr data_ov043_02112670(ASSET_KM1_DORIFU2_KCL);
DorifuCollisionFilePtr data_ov043_02112698(ASSET_KM1_DORIFU3_KCL);
DorifuCollisionFilePtr data_ov043_02112660(ASSET_KM1_DORIFU4_KCL);
