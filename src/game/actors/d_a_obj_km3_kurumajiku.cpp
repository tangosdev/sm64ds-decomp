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
 * The resource descriptor and single-entry collision-property block are
 * defined here. Retail .data order is typeinfo, descriptor, type-name,
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

/* The constructor resolves each runtime asset handle to the stored ID.
 * The model uses func_02017acc / func_02017ab4 and the
 * collision file func_02017b4c / SharedFilePtr_Destruct_Clsn. The spellings
 * are local; the manifest aliases the generated names to those ROM symbols. */
struct Km3KurumajikuModelFilePtr : SharedFilePtr {
    Km3KurumajikuModelFilePtr(unsigned int fileID);
    ~Km3KurumajikuModelFilePtr();
};

struct Km3KurumajikuClsnFilePtr : SharedFilePtr {
    Km3KurumajikuClsnFilePtr(unsigned int fileID);
    ~Km3KurumajikuClsnFilePtr();
};

typedef char
    Km3KurumajikuModelFilePtr_size_must_be_8[sizeof(Km3KurumajikuModelFilePtr) == 8 ? 1 : -1];
typedef char
    Km3KurumajikuClsnFilePtr_size_must_be_8[sizeof(Km3KurumajikuClsnFilePtr) == 8 ? 1 : -1];

struct Km3KurumajikuClpsBlock {
    char magic[4];
    unsigned short entrySize;
    unsigned short entryCount;
    CLPS entries[1];
};
typedef char Km3KurumajikuClpsBlock_size_must_be_16[sizeof(Km3KurumajikuClpsBlock) == 16 ? 1 : -1];

extern Km3KurumajikuModelFilePtr gKm3KurumajikuModelFile;
extern Km3KurumajikuClsnFilePtr gKm3KurumajikuCollisionFile;
/* All sixteen real KCL prisms use entry zero. The following sixteen FF
 * bytes are outside this native block and remain ROM-owned. */
extern const Km3KurumajikuClpsBlock gKm3KurumajikuClps = {
    {'C', 'L', 'P', 'S'}, 8, 1, {{0x00000fc0, 0x000000ff}}};

/* Retail data order: own typeinfo, this descriptor, type name, profile, vtable. */
daObjKurumajiku_c::Resources gKm3KurumajikuResources = {
    &gKm3KurumajikuModelFile, &gKm3KurumajikuCollisionFile,
    /* The legacy SetFile API retains this read-only collision-property block. */
    (CLPS_Block *)&gKm3KurumajikuClps};

#include "daObjKm3_Kurumajiku_c.h"
#include "SpawnInfo.h"

/* KM3_KURUMA: the cart this axle drives. InitResourcesFrom spawns four. */
enum { kCartActorId = 0x97 };

// @symbol daObjKm3_Kurumajiku_c_classInit
extern "C" daObjKm3_Kurumajiku_c *daObjKm3_Kurumajiku_c_classInit() {
    return new daObjKm3_Kurumajiku_c();
}

// @symbol g_profile_KM3_KURUMAJIKU
extern "C" SpawnInfo<daObjKm3_Kurumajiku_c> g_profile_KM3_KURUMAJIKU = {
    daObjKm3_Kurumajiku_c_classInit,
    0x0098, /* KM3_KURUMAJIKU registry id 152 */
    0x00df,
    2,
    0,
    0x00300000,
    0x01000000,
    0};

// @symbol _ZN21daObjKm3_Kurumajiku_c13InitResourcesEv
s32 daObjKm3_Kurumajiku_c::InitResources() {
    return InitResourcesFrom(&gKm3KurumajikuResources, kCartActorId);
}

// @symbol _ZN21daObjKm3_Kurumajiku_c16CleanupResourcesEv
s32 daObjKm3_Kurumajiku_c::CleanupResources() {
    return CleanupResourcesFrom(&gKm3KurumajikuResources);
}

/* Source order is construction order: model file 1665, then the collision
 * file 1666. The compiler registers each destructor beside the object. */
Km3KurumajikuModelFilePtr gKm3KurumajikuModelFile(ASSET_KM3_KURUMAJIKU_BMD);
Km3KurumajikuClsnFilePtr gKm3KurumajikuCollisionFile(ASSET_KM3_KURUMAJIKU_KCL);
