//cpp
/**
 * daKpa_c in the Sky's spinning disc.
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjKaitendai_c's shared ov002
 * helpers. The default rotation increment is 312 angle units per frame,
 * or 624 when param1's low byte is 1. The shared setup stores the
 * increment and Behavior adds it each frame; it is not a start angle.
 * InitResourcesFrom loads slot 0 with dExtSimpleModel_c::LoadFile, slot 1 with
 * dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile. This TU's static
 * initializer constructs those SharedFilePtrs as file IDs 1661 / 1662.
 *
 * This TU defines the rotation increments and three-pointer resource
 * descriptor. The collision-property block is stage-map data. The data names describe
 * measured consumers; their original source spellings are not recovered.
 *
 * daObjKm3_Kaitendai_c_classInit is reconstructed (RTTI
 * daObjKm3_Kaitendai_c, KM3_KAITENDAI registry). Retail does not
 * store that spelling.
 *
 * What stays: the BMD/KCL handles and descriptor retain their address names.
 *   The shared setup/cleanup interface still takes an opaque mutable
 *   CLPS_Block pointer, although its collision-property consumers only read it.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_km3_kaitendai_AssetId {
    ASSET_KM3_KAITENDAI_BMD = 0x067d,
    ASSET_KM3_KAITENDAI_KCL = 0x067e
};

#include "SharedFilePtr.h"
#include "CLPS.h"

#include "ResourceDescriptor.h"

extern ModelFile data_ov047_02112610;
extern ClsnFile data_ov047_02112608;

extern CLPS_Block gKm3KaitendaiClps;

/* Defined here because this TU owns the descriptor in overlay .data. */
ResourceDescriptor data_ov047_02112334 = {&data_ov047_02112610, &data_ov047_02112608,
                                          &gKm3KaitendaiClps};

#include "daObjKm3_Kaitendai_c.h"
#include "SpawnInfo.h"

s16 gKm3KaitendaiRotationIncrement = 312;
s16 gKm3KaitendaiAlternateRotationIncrement = 624;

daObjKm3_Kaitendai_c *daObjKm3_Kaitendai_c_classInit() {
    return new daObjKm3_Kaitendai_c();
}

extern "C" SpawnInfo<daObjKm3_Kaitendai_c> g_profile_KM3_KAITENDAI = {
    daObjKm3_Kaitendai_c_classInit,
    0x009c, /* KM3_KAITENDAI registry id 156 */
    0x00e3,
    2,
    0,
    0x00380000,
    0x02000000,
    0};

s32 daObjKm3_Kaitendai_c::InitResources() {
    s16 rotationIncrement = gKm3KaitendaiRotationIncrement;
    if ((param1 & 0xff) == 1)
        rotationIncrement = gKm3KaitendaiAlternateRotationIncrement;
    return InitResourcesFrom(&data_ov047_02112334, rotationIncrement);
}

s32 daObjKm3_Kaitendai_c::CleanupResources() {
    return CleanupResourcesFrom(&data_ov047_02112334);
}

/* Source order is construction order: model file 1661, then the collision
 * file 1662. The compiler registers each destructor beside the object. */
ModelFile data_ov047_02112610(ASSET_KM3_KAITENDAI_BMD);
ClsnFile data_ov047_02112608(ASSET_KM3_KAITENDAI_KCL);
