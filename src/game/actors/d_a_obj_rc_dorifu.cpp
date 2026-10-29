//cpp
/**
 * Rainbow Ride's Tricky Triangles (`dorifu`). Twin of the BitDW
 * leaf (`daObjKm1_Dorifu_c`) and the BitS leaf (`daObjKm3_Dorifu_c`).
 *
 * No fields of its own. InitResources / CleanupResources hand this
 * overlay's five-plank file table to daObjDorifu_c's shared
 * overloads. InitResources loads each slot 0 with dExtSimpleModel_c::LoadFile,
 * slot 1 with dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile.
 * ov036 sinit constructs those SharedFilePtrs as file IDs
 * 1690 / 1691, 1692 / 1693, 1694 / 1695, 1696 / 1697, 1698 / 1699.
 *
 * The five-entry table is defined in this TU. Retail .data
 * order is typeinfo, type-name, profile, table, vtable.
 *
 * daObjRc_Dorifu_c_classInit is reconstructed (RTTI
 * daObjRc_Dorifu_c, RC_DORIFU registry). Retail does not
 * store that spelling.
 *
 * Note: the BMD/KCL SharedFilePtrs and CLPS_Blocks keep their
 * data_ov036_* linker names. The CLPS words sit before this TU's claimed
 * .data (typeinfo at 0x02113e4c); the SharedFilePtrs are BSS.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_rc_dorifu_AssetId {
    ASSET_RC_DORIFU0_BMD = 0x069a,
    ASSET_RC_DORIFU0_KCL = 0x069b,
    ASSET_RC_DORIFU1_BMD = 0x069c,
    ASSET_RC_DORIFU1_KCL = 0x069d,
    ASSET_RC_DORIFU2_BMD = 0x069e,
    ASSET_RC_DORIFU2_KCL = 0x069f,
    ASSET_RC_DORIFU3_BMD = 0x06a0,
    ASSET_RC_DORIFU3_KCL = 0x06a1,
    ASSET_RC_DORIFU4_BMD = 0x06a2,
    ASSET_RC_DORIFU4_KCL = 0x06a3
};

#include "daObjRc_Dorifu_c.h"
#include "SpawnInfo.h"

extern CLPS_Block data_ov036_02112be8;
extern CLPS_Block data_ov036_02112c08;
extern CLPS_Block data_ov036_02112a88;
extern CLPS_Block data_ov036_02112b88;
extern CLPS_Block data_ov036_02112ae8;

/* File-scope objects at the end of this file construct the ten resource
 * handles (model files 1690/1692/1694/1696/1698, collision files
 * 1691/1693/1695/1697/1699). mwcc emits __sinit_d_a_obj_rc_dorifu.cpp from
 * those definitions. The wrapper names are local; the handle constructors
 * and destructors are the ROM resource-family functions, aliased in the
 * manifest. */
struct RcDorifuModelFilePtr : SharedFilePtr {
    RcDorifuModelFilePtr(u32 fileID);
    ~RcDorifuModelFilePtr();
};

struct RcDorifuCollisionFilePtr : SharedFilePtr {
    RcDorifuCollisionFilePtr(u32 fileID);
    ~RcDorifuCollisionFilePtr();
};

extern RcDorifuModelFilePtr data_ov036_0211411c;
extern RcDorifuCollisionFilePtr data_ov036_021140f4;
extern RcDorifuModelFilePtr data_ov036_021140fc;
extern RcDorifuCollisionFilePtr data_ov036_021140d4;
extern RcDorifuModelFilePtr data_ov036_021140e4;
extern RcDorifuCollisionFilePtr data_ov036_021140ec;
extern RcDorifuModelFilePtr data_ov036_02114104;
extern RcDorifuCollisionFilePtr data_ov036_02114114;
extern RcDorifuModelFilePtr data_ov036_0211410c;
extern RcDorifuCollisionFilePtr data_ov036_021140dc;

// @symbol daObjRc_Dorifu_c_classInit
extern "C" daObjRc_Dorifu_c *daObjRc_Dorifu_c_classInit() {
    return new daObjRc_Dorifu_c();
}

// @symbol g_profile_RC_DORIFU
extern "C" SpawnInfo<daObjRc_Dorifu_c> g_profile_RC_DORIFU = {
    daObjRc_Dorifu_c_classInit,
    0x007b, /* RC_DORIFU registry id 123 */
    0x00fd,
    0,
    0x00500000,
    0x01000000,
    0x02000000,
    0};

daObjDorifuResources data_ov036_02113e88[5] = {
    {&data_ov036_0211411c, &data_ov036_021140f4, &data_ov036_02112be8},
    {&data_ov036_021140fc, &data_ov036_021140d4, &data_ov036_02112c08},
    {&data_ov036_021140e4, &data_ov036_021140ec, &data_ov036_02112a88},
    {&data_ov036_02114104, &data_ov036_02114114, &data_ov036_02112b88},
    {&data_ov036_0211410c, &data_ov036_021140dc, &data_ov036_02112ae8}};

// @symbol _ZN16daObjRc_Dorifu_c13InitResourcesEv
s32 daObjRc_Dorifu_c::InitResources() {
    return daObjDorifu_c::InitResources(data_ov036_02113e88);
}

// @symbol _ZN16daObjRc_Dorifu_c16CleanupResourcesEv
s32 daObjRc_Dorifu_c::CleanupResources() {
    return daObjDorifu_c::CleanupResources(data_ov036_02113e88);
}

/* Static-init globals (was the handwritten __sinit_ov036_0211279c shard).
 * Definition order is the retail initializer's construction order: the five
 * model handles (files 1690/1692/1694/1696/1698), then the five collision
 * handles (files 1691/1693/1695/1697/1699). */
RcDorifuModelFilePtr data_ov036_0211411c(ASSET_RC_DORIFU0_BMD);
RcDorifuModelFilePtr data_ov036_021140fc(ASSET_RC_DORIFU1_BMD);
RcDorifuModelFilePtr data_ov036_021140e4(ASSET_RC_DORIFU2_BMD);
RcDorifuModelFilePtr data_ov036_02114104(ASSET_RC_DORIFU3_BMD);
RcDorifuModelFilePtr data_ov036_0211410c(ASSET_RC_DORIFU4_BMD);
RcDorifuCollisionFilePtr data_ov036_021140f4(ASSET_RC_DORIFU0_KCL);
RcDorifuCollisionFilePtr data_ov036_021140d4(ASSET_RC_DORIFU1_KCL);
RcDorifuCollisionFilePtr data_ov036_021140ec(ASSET_RC_DORIFU2_KCL);
RcDorifuCollisionFilePtr data_ov036_02114114(ASSET_RC_DORIFU3_KCL);
RcDorifuCollisionFilePtr data_ov036_021140dc(ASSET_RC_DORIFU4_KCL);
