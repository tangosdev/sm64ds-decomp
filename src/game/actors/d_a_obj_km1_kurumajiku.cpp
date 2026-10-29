//cpp
/**
 * daKpa_c in the Dark World's rickshaw axle (`kurumajiku`).
 *
 * No fields of its own. InitResources / CleanupResources hand this
 * overlay's model and collision files to daObjKurumajiku_c's shared
 * ov002 helpers. InitResourcesFrom loads slot 0 with dExtSimpleModel_c::LoadFile,
 * slot 1 with dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile, then
 * Spawns four carts (actor id KM1_KURUMA) and stores their uniqueIDs
 * in mMountedActorIds. ov043 sinit constructs those SharedFilePtrs as
 * file IDs 1621 / 1622.
 *
 * daObjKm1_Kurumajiku_c_classInit is reconstructed (RTTI
 * daObjKm1_Kurumajiku_c, KM1_KURUMAJIKU registry). Retail does not
 * store that spelling.
 *
 * Notes:
 * - data_ov043_02112344 is overlay data this TU does not own; the BMD/KCL
 *   SharedFilePtrs and CLPS_Block are still data_ov043_*.
 * - g_profile_KM1_KURUMAJIKU is overlay data; this TU is text-only so the
 *   definition here is a deadstripped duplicate.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_km1_kurumajiku_AssetId {
    ASSET_KM1_KURUMAJIKU_BMD = 0x0655,
    ASSET_KM1_KURUMAJIKU_KCL = 0x0656
};

#include "daObjKm1_Kurumajiku_c.h"
#include "SpawnInfo.h"
#include "SharedFilePtr.h"

/* File-scope resource handles: the ctor/dtor are the ROM's SharedFilePtr
 * veneer pairs (func_02017acc / func_02017ab4 for the model, func_02017b4c /
 * SharedFilePtr_Destruct_Clsn for the collision), spelled through
 * declared-only subclasses so the static initializer names the real entry
 * points. */
struct Km1KurumajikuModelFilePtr : SharedFilePtr {
    Km1KurumajikuModelFilePtr(u32 fileID);
    ~Km1KurumajikuModelFilePtr();
};
struct Km1KurumajikuCollisionFilePtr : SharedFilePtr {
    Km1KurumajikuCollisionFilePtr(u32 fileID);
    ~Km1KurumajikuCollisionFilePtr();
};

extern daObjKurumajiku_c::Resources data_ov043_02112344;

/* KM1_KURUMA: the cart this axle drives. InitResourcesFrom spawns four. */
enum { kCartActorId = 0x88 };

// @symbol daObjKm1_Kurumajiku_c_classInit
extern "C" daObjKm1_Kurumajiku_c *daObjKm1_Kurumajiku_c_classInit() {
    return new daObjKm1_Kurumajiku_c();
}

// @symbol g_profile_KM1_KURUMAJIKU
extern "C" SpawnInfo<daObjKm1_Kurumajiku_c> g_profile_KM1_KURUMAJIKU = {
    daObjKm1_Kurumajiku_c_classInit,
    0x0089, /* KM1_KURUMAJIKU registry id 137 */
    0x00b2,
    2,
    0,
    0x00300000,
    0x01000000,
    0};

// @symbol _ZN21daObjKm1_Kurumajiku_c13InitResourcesEv
s32 daObjKm1_Kurumajiku_c::InitResources() {
    return InitResourcesFrom(&data_ov043_02112344, kCartActorId);
}

// @symbol _ZN21daObjKm1_Kurumajiku_c16CleanupResourcesEv
s32 daObjKm1_Kurumajiku_c::CleanupResources() {
    return CleanupResourcesFrom(&data_ov043_02112344);
}

/* The axle's model and collision file handles; data_ov043_02112344.model
 * and .kcl point at them. */
Km1KurumajikuModelFilePtr gKm1KurumajikuModelFile(ASSET_KM1_KURUMAJIKU_BMD);
Km1KurumajikuCollisionFilePtr gKm1KurumajikuCollisionFile(ASSET_KM1_KURUMAJIKU_KCL);
