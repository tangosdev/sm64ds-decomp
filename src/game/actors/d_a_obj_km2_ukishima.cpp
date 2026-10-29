//cpp
/**
 * daKpa_c in the Fire Sea's floating floor.
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjUkiyuka_c's shared ov002 helpers.
 * Init also stores kBobAmplitude into mBobAmplitude.
 * InitResourcesFrom loads slot 0 with dExtSimpleModel_c::LoadFile, slot 1
 * with dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile, copies
 * mPosY to mRestY, and stores arg 3 at mBobAmplitude. ov045 sinit
 * constructs those SharedFilePtrs as file IDs 1643 / 1644.
 *
 * daObjKm2_Ukishima_c_classInit is reconstructed (RTTI
 * daObjKm2_Ukishima_c, KM2_UKISHIMA registry). Retail does not store
 * that spelling.
 *
 * Notes:
 * - The file table is still data_ov045_02112f08.
 * - g_profile_KM2_UKISHIMA is overlay data; this TU is text-only so the
 *   definition here is a deadstripped duplicate.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_km2_ukishima_AssetId {
    ASSET_KM2_UKISHIMA_BMD = 0x066b,
    ASSET_KM2_UKISHIMA_KCL = 0x066c
};

#include "daObjKm2_Ukishima_c.h"
#include "SpawnInfo.h"
#include "SharedFilePtr.h"

/* Retail constructs these 8-byte handles in this order and registers
 * their destructors. The wrapper names are local; the ctor/dtor
 * addresses are the ROM resource-family functions. */
struct UkishimaModelFilePtr : SharedFilePtr {
    UkishimaModelFilePtr(u32 fileID);
    ~UkishimaModelFilePtr();
};

struct UkishimaCollisionFilePtr : SharedFilePtr {
    UkishimaCollisionFilePtr(u32 fileID);
    ~UkishimaCollisionFilePtr();
};

extern daObjUkiyuka_c::Resources data_ov045_02112f08;
extern UkishimaModelFilePtr gKm2UkishimaModelFile;         /* BMD, file 1643 */
extern UkishimaCollisionFilePtr gKm2UkishimaCollisionFile; /* KCL, file 1644 */

/* 20.12; InitResourcesFrom writes this to mBobAmplitude. */
enum { kBobAmplitude = 0xf50 };

// @symbol daObjKm2_Ukishima_c_classInit
extern "C" daObjKm2_Ukishima_c *daObjKm2_Ukishima_c_classInit() {
    return new daObjKm2_Ukishima_c();
}

// @symbol g_profile_KM2_UKISHIMA
extern "C" SpawnInfo<daObjKm2_Ukishima_c> g_profile_KM2_UKISHIMA = {
    daObjKm2_Ukishima_c_classInit,
    0x0091, /* KM2_UKISHIMA registry id 145 */
    0x00d8,
    2,
    0x00060000,
    0x01000000,
    0x02000000,
    0};

// @symbol _ZN19daObjKm2_Ukishima_c13InitResourcesEv
s32 daObjKm2_Ukishima_c::InitResources() {
    return InitResourcesFrom(&data_ov045_02112f08, kBobAmplitude);
}

// @symbol _ZN19daObjKm2_Ukishima_c16CleanupResourcesEv
s32 daObjKm2_Ukishima_c::CleanupResources() {
    return CleanupResourcesFrom(&data_ov045_02112f08);
}

UkishimaModelFilePtr gKm2UkishimaModelFile(ASSET_KM2_UKISHIMA_BMD);
UkishimaCollisionFilePtr gKm2UkishimaCollisionFile(ASSET_KM2_UKISHIMA_KCL);
