//cpp
/**
 * daKpa_c in the Dark World's rickshaw cart (`kuruma`).
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjKuruma_c's shared ov002 helpers.
 * InitResourcesFrom loads slot 0 with dExtSimpleModel_c::LoadFile, slot 1 with
 * dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile. ov043 sinit
 * constructs those SharedFilePtrs as file IDs 1619 / 1620.
 *
 * daObjKm1_Kuruma_c_classInit is reconstructed (RTTI daObjKm1_Kuruma_c,
 * KM1_KURUMA registry). Retail does not store that spelling.
 *
 * Notes:
 * - data_ov043_02112418 is overlay data this TU does not own; the BMD/KCL
 *   SharedFilePtrs and CLPS_Block are still data_ov043_*.
 * - g_profile_KM1_KURUMA is overlay data; its definition here is a
 *   deadstripped duplicate.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_km1_kuruma_AssetId { ASSET_KM1_KURUMA_BMD = 0x0653, ASSET_KM1_KURUMA_KCL = 0x0654 };

#include "daObjKm1_Kuruma_c.h"
#include "SpawnInfo.h"
#include "SharedFilePtr.h"

/* File-scope objects at the end of this file construct the two resource
 * handles (model file 0x653, collision file 0x654). mwcc emits
 * __sinit_d_a_obj_km1_kuruma.cpp from those definitions. The wrapper names
 * are local; the handle constructors and destructors are the ROM
 * resource-family functions, aliased in the manifest. Nothing in this TU
 * names the handles directly; the Resources record data_ov043_02112418
 * points at them. */
struct Km1KurumaModelFile : SharedFilePtr {
    Km1KurumaModelFile(u32 fileID);
    ~Km1KurumaModelFile();
};

struct Km1KurumaCollisionFilePtr : SharedFilePtr {
    Km1KurumaCollisionFilePtr(u32 fileID);
    ~Km1KurumaCollisionFilePtr();
};

extern daObjKuruma_c::Resources data_ov043_02112418;

// @symbol daObjKm1_Kuruma_c_classInit
extern "C" daObjKm1_Kuruma_c *daObjKm1_Kuruma_c_classInit() {
    return new daObjKm1_Kuruma_c();
}

// @symbol g_profile_KM1_KURUMA
extern "C" SpawnInfo<daObjKm1_Kuruma_c> g_profile_KM1_KURUMA = {
    daObjKm1_Kuruma_c_classInit,
    0x0088, /* KM1_KURUMA registry id 136 */
    0x00b1,
    2,
    0,
    0x00200000,
    0x01000000,
    0};

// @symbol _ZN17daObjKm1_Kuruma_c13InitResourcesEv
s32 daObjKm1_Kuruma_c::InitResources() {
    return InitResourcesFrom(&data_ov043_02112418);
}

// @symbol _ZN17daObjKm1_Kuruma_c16CleanupResourcesEv
s32 daObjKm1_Kuruma_c::CleanupResources() {
    return CleanupResourcesFrom(&data_ov043_02112418);
}

/* Static-init globals (was the handwritten __sinit_ov043_021118d4 shard).
 * Definition order is the retail initializer's construction order. */
Km1KurumaModelFile gKm1KurumaModelFile(ASSET_KM1_KURUMA_BMD);
Km1KurumaCollisionFilePtr gKm1KurumaCollisionFile(ASSET_KM1_KURUMA_KCL);
