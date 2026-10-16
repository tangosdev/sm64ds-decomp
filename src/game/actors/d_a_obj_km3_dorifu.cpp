//cpp
/**
 * daKpa_c in the Sky's drifting stairs (`dorifu`). Twin of the BitDW
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
 * Leftover: g_profile_KM3_DORIFU is overlay data this TU does not
 *   own; the CLPS_Block targets are still data_ov047_*.
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
 * destroy through SharedFilePtr_Destruct_Clsn. */
struct DorifuModelFilePtr : SharedFilePtr {
    DorifuModelFilePtr(unsigned int fileId);
    ~DorifuModelFilePtr();
};
struct DorifuClsnFileHandle : SharedFilePtr {
    DorifuClsnFileHandle(unsigned int fileId);
    ~DorifuClsnFileHandle();
};
typedef char DorifuModelFilePtr_size_must_be_8[
    sizeof(DorifuModelFilePtr) == 8 ? 1 : -1];
typedef char DorifuClsnFileHandle_size_must_be_8[
    sizeof(DorifuClsnFileHandle) == 8 ? 1 : -1];

struct CLPS_Block;

extern "C" {
extern DorifuModelFilePtr gKm3Dan0ModelFile;
extern DorifuModelFilePtr gKm3Dan1ModelFile;
extern DorifuModelFilePtr gKm3Dan2ModelFile;
extern DorifuModelFilePtr gKm3Dan3ModelFile;
extern DorifuModelFilePtr gKm3Dan4ModelFile;
extern DorifuClsnFileHandle gKm3Dan0CollisionFile;
extern DorifuClsnFileHandle gKm3Dan1CollisionFile;
extern DorifuClsnFileHandle gKm3Dan2CollisionFile;
extern DorifuClsnFileHandle gKm3Dan3CollisionFile;
extern DorifuClsnFileHandle gKm3Dan4CollisionFile;
extern CLPS_Block data_ov047_02111a74;
extern CLPS_Block data_ov047_02111a94;
extern CLPS_Block data_ov047_02111b94;
extern CLPS_Block data_ov047_02111bb4;
extern CLPS_Block data_ov047_02111bd4;
}

/* Five-plank file table; defined here because this TU owns it in overlay
 * .data. Each row hands Model::LoadFile the BMD handle, dBgW_Kc::LoadFile the
 * KCL handle, and SetFile that plank's CLPS block (still ROM data).
 * symbols.txt names the tenth word data_ov047_02112530, inside plank 3's
 * row, so the emit splits into two word runs there and the methods below
 * read the table through a daObjDorifuResources view. */
extern "C" void *data_ov047_02112508[10] = {
    &gKm3Dan0ModelFile, &gKm3Dan0CollisionFile, &data_ov047_02111bb4,
    &gKm3Dan1ModelFile, &gKm3Dan1CollisionFile, &data_ov047_02111a74,
    &gKm3Dan2ModelFile, &gKm3Dan2CollisionFile, &data_ov047_02111a94,
    &gKm3Dan3ModelFile,
};
extern "C" void *data_ov047_02112530[5] = {
    &gKm3Dan3CollisionFile, &data_ov047_02111b94,
    &gKm3Dan4ModelFile, &gKm3Dan4CollisionFile, &data_ov047_02111bd4,
};

// @symbol daObjKm3_Dorifu_c_classInit
extern "C" daObjKm3_Dorifu_c *daObjKm3_Dorifu_c_classInit()
{
    return new daObjKm3_Dorifu_c();
}

// @symbol _ZN17daObjKm3_Dorifu_c13InitResourcesEv
s32 daObjKm3_Dorifu_c::InitResources()
{
    return daObjDorifu_c::InitResources(
        (daObjDorifuResources *)data_ov047_02112508);
}

// @symbol _ZN17daObjKm3_Dorifu_c16CleanupResourcesEv
s32 daObjKm3_Dorifu_c::CleanupResources()
{
    return daObjDorifu_c::CleanupResources(
        (daObjDorifuResources *)data_ov047_02112508);
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
