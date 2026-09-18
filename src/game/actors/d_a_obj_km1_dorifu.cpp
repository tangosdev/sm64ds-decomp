//cpp
/**
 * Bowser in the Dark World's drifting stairs (`dorifu`).
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
 * daObjKm1_Dorifu_c_classInit is reconstructed (RTTI
 * daObjKm1_Dorifu_c, KM1_DORIFU registry). Retail does not
 * store that spelling.
 *
 * deslop
 * Leftover: the BMD/KCL SharedFilePtrs and CLPS_Block are still
 *   data_ov043_*.
 */

#include "daObjKm1_Dorifu_c.h"

struct DorifuSpawnInfo {
    daObjKm1_Dorifu_c *(*classInit)();
    s16 executePriority; /* +4: also KM1_DORIFU registry id 0x0086 = 134 */
    s16 renderPriority;  /* +6 */
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};
typedef char DorifuSpawnInfo_size_must_be_0x1c[
    sizeof(DorifuSpawnInfo) == 0x1c ? 1 : -1];

extern "C" {
extern SharedFilePtr data_ov043_021126a0;
extern SharedFilePtr data_ov043_02112678;
extern CLPS_Block data_ov043_02111c40;
extern SharedFilePtr data_ov043_02112680;
extern SharedFilePtr data_ov043_02112658;
extern CLPS_Block data_ov043_02111b60;
extern SharedFilePtr data_ov043_02112668;
extern SharedFilePtr data_ov043_02112670;
extern CLPS_Block data_ov043_02111ba0;
extern SharedFilePtr data_ov043_02112688;
extern SharedFilePtr data_ov043_02112698;
extern CLPS_Block data_ov043_02111c80;
extern SharedFilePtr data_ov043_02112690;
extern SharedFilePtr data_ov043_02112660;
extern CLPS_Block data_ov043_02111c60;
}

// @symbol daObjKm1_Dorifu_c_classInit
extern "C" daObjKm1_Dorifu_c *daObjKm1_Dorifu_c_classInit()
{
    return new daObjKm1_Dorifu_c();
}

// @symbol g_profile_KM1_DORIFU
extern "C" DorifuSpawnInfo g_profile_KM1_DORIFU = {
    daObjKm1_Dorifu_c_classInit,
    0x0086,
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
