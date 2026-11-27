//cpp
/**
 * daKpa_c in the Sky's rickshaw axle (`kurumajiku`).
 *
 * No fields of its own. InitResources / CleanupResources hand this
 * overlay's model and collision files to daObjKurumajiku_c's shared
 * ov002 helpers. InitResourcesFrom loads slot 0 with dExtSimpleModel_c::LoadFile,
 * slot 1 with dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile, then
 * spawns four carts (actor id KM3_KURUMA) and stores their uniqueIDs
 * in mMountedActorIds. ov047 sinit constructs those SharedFilePtrs as
 * runtime handles 1665 / 1666; their stored file IDs are resolved at construction.
 *
 * The resource descriptor is defined here; the collision-property block is
 * stage-map data. Retail .data order is typeinfo, descriptor, type-name,
 * profile, vtable.
 *
 * daObjKm3_Kurumajiku_c_classInit is reconstructed (RTTI
 * daObjKm3_Kurumajiku_c, KM3_KURUMAJIKU registry). Retail does not
 * store that spelling.
 *
 * Resource names describe the asset/consumer roles, not recovered identifiers.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_km3_kurumajiku_AssetId {
    ASSET_KM3_KURUMAJIKU_BMD = 0x0681,
    ASSET_KM3_KURUMAJIKU_KCL = 0x0682
};

#include "SharedFilePtr.h"
#include "CLPS.h"
#include "daObjKurumajiku_c.h"

extern ModelFile gKm3KurumajikuModelFile;
extern ClsnFile gKm3KurumajikuCollisionFile;
extern CLPS_Block gKm3KurumajikuClps;

/* Retail data order: own typeinfo, this descriptor, type name, profile, vtable. */
daObjKurumajiku_c::Resources gKm3KurumajikuResources = {
    &gKm3KurumajikuModelFile, &gKm3KurumajikuCollisionFile,
    /* The legacy SetFile API retains this read-only collision-property block. */
    &gKm3KurumajikuClps};

#include "daObjKm3_Kurumajiku_c.h"
#include "SpawnInfo.h"

/* KM3_KURUMA: the cart this axle drives. InitResourcesFrom spawns four. */
enum { kCartActorId = 0x97 };

daObjKm3_Kurumajiku_c *daObjKm3_Kurumajiku_c_classInit() {
    return new daObjKm3_Kurumajiku_c();
}

extern "C" SpawnInfo<daObjKm3_Kurumajiku_c> g_profile_KM3_KURUMAJIKU = {
    daObjKm3_Kurumajiku_c_classInit,
    0x0098, /* KM3_KURUMAJIKU registry id 152 */
    0x00df,
    2,
    0,
    0x00300000,
    0x01000000,
    0};

s32 daObjKm3_Kurumajiku_c::InitResources() {
    return InitResourcesFrom(&gKm3KurumajikuResources, kCartActorId);
}

s32 daObjKm3_Kurumajiku_c::CleanupResources() {
    return CleanupResourcesFrom(&gKm3KurumajikuResources);
}

/* Source order is construction order: model file 1665, then the collision
 * file 1666. The compiler registers each destructor beside the object. */
ModelFile gKm3KurumajikuModelFile(ASSET_KM3_KURUMAJIKU_BMD);
ClsnFile gKm3KurumajikuCollisionFile(ASSET_KM3_KURUMAJIKU_KCL);
