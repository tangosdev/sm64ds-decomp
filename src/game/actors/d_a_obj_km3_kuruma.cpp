//cpp
/**
 * daKpa_c in the Sky's rickshaw cart (`kuruma`).
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjKuruma_c's shared ov002 helpers.
 * InitResourcesFrom loads slot 0 with dExtSimpleModel_c::LoadFile, slot 1 with
 * dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile. This TU's static
 * initializer constructs those SharedFilePtrs as file IDs 1663 / 1664.
 *
 * The three-word file table is defined in this TU. Retail .data
 * order is typeinfo, descriptor, type-name, profile, vtable.
 *
 * daObjKm3_Kuruma_c_classInit is reconstructed (RTTI daObjKm3_Kuruma_c,
 * KM3_KURUMA registry). Retail does not store that spelling.
 *
 * The resource names describe observed uses. The CLPS block is stage-map data.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_km3_kuruma_AssetId { ASSET_KM3_KURUMA_BMD = 0x067f, ASSET_KM3_KURUMA_KCL = 0x0680 };

#include "SharedFilePtr.h"
#include "CLPS.h"
#include "daObjKuruma_c.h"

extern ModelFile gKm3KurumaModelFile;
extern ClsnFile gKm3KurumaCollisionFile;

extern CLPS_Block gKm3KurumaClps;

/* Defined here because this TU owns the descriptor in overlay .data. */
daObjKuruma_c::Resources gKm3KurumaResources = {
    &gKm3KurumaModelFile, &gKm3KurumaCollisionFile,
    /* SetFile retains this read-only block through its opaque legacy API. */
    &gKm3KurumaClps};

#include "daObjKm3_Kuruma_c.h"
#include "SpawnInfo.h"

daObjKm3_Kuruma_c *daObjKm3_Kuruma_c_classInit() {
    return new daObjKm3_Kuruma_c();
}

extern "C" SpawnInfo<daObjKm3_Kuruma_c> g_profile_KM3_KURUMA = {
    daObjKm3_Kuruma_c_classInit,
    0x0097, /* KM3_KURUMA registry id 151 */
    0x00de,
    2,
    0,
    0x00200000,
    0x01000000,
    0};

s32 daObjKm3_Kuruma_c::InitResources() {
    return InitResourcesFrom(&gKm3KurumaResources);
}

s32 daObjKm3_Kuruma_c::CleanupResources() {
    return CleanupResourcesFrom(&gKm3KurumaResources);
}

/* Source order is construction order: model file 1663, then the collision
 * file 1664. The compiler registers each destructor beside the object. */
ModelFile gKm3KurumaModelFile(ASSET_KM3_KURUMA_BMD);
ClsnFile gKm3KurumaCollisionFile(ASSET_KM3_KURUMA_KCL);
