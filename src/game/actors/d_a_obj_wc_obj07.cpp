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
 * The CLPS block is data_ov029_021130ac, defined in the stage-map TU
 * (d_s_map_ov029.c). The start-angle slot 02113fc4 sits 4 bytes before
 * this class's typeinfo at 02113fc8.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_wc_obj07_AssetId { ASSET_WC_OBJ07_BMD = 0x06d7, ASSET_WC_OBJ07_KCL = 0x06d8 };

#include "SpawnInfo.h"
#include "daObjWc_Obj07_c.h"
#include "SharedFilePtr.h"

#include "ResourceDescriptor.h"

extern CLPS_Block data_ov029_021130ac;

extern ModelFile gWcObj07ModelFile;
extern ClsnFile gWcObj07CollisionFile;

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
ModelFile gWcObj07ModelFile(ASSET_WC_OBJ07_BMD);
ClsnFile gWcObj07CollisionFile(ASSET_WC_OBJ07_KCL);

extern "C" SpawnInfo<daObjWc_Obj07_c> g_profile_WC_UKISIMA = {
    daObjWc_Obj07_c_classInit, 94, 270, 0x00000002, 0x00000000, 0x00110000, 0x01c20000, 0x00640000};
