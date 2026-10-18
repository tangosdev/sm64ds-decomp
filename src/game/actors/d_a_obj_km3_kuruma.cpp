//cpp
/**
 * daKpa_c in the Sky's rickshaw cart (`kuruma`).
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjKuruma_c's shared ov002 helpers.
 * func_ov002_020b6958 loads slot 0 with Model::LoadFile, slot 1 with
 * dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile. This TU's static
 * initializer constructs those SharedFilePtrs as file IDs 1663 / 1664.
 *
 * The three-word file table is defined in this TU. Retail .data
 * order is typeinfo, descriptor, type-name, profile, vtable.
 *
 * daObjKm3_Kuruma_c_classInit is reconstructed (RTTI daObjKm3_Kuruma_c,
 * KM3_KURUMA registry). Retail does not store that spelling.
 *
 * deslop
 * Leftover: func_ov002_020b6958 / func_ov002_020b68b0 are still the
 *   linker names of daObjKuruma_c Init/Cleanup (the base leaves
 *   those slots pure virtual). Naming belongs in ov002.
 * The resource names describe observed uses. The CLPS block is grouped
 * with its consumer here; its original source-file ownership is unknown.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_km3_kuruma_AssetId {
    ASSET_KM3_KURUMA_BMD = 0x067f,
    ASSET_KM3_KURUMA_KCL = 0x0680
};

#include "SharedFilePtr.h"
#include "CLPS.h"

/* Local layout follows SharedFilePtr.cpp: fileID at +0, numRefs at +2,
 * implicit padding at +3, filePtr at +4. The shared header is fieldless.
 * 8-byte file handles. The model uses func_02017acc / func_02017ab4 and the
 * collision file func_02017b4c / SharedFilePtr_Destruct_Clsn. The spellings
 * are local; the manifest aliases the generated names to those ROM symbols. */
struct Km3KurumaModelFilePtr : SharedFilePtr {
    unsigned short fileID;
    unsigned char numRefs;
    void *filePtr;

    Km3KurumaModelFilePtr(unsigned int fileID);
    ~Km3KurumaModelFilePtr();
};

struct Km3KurumaClsnFilePtr : SharedFilePtr {
    unsigned short fileID;
    unsigned char numRefs;
    void *filePtr;

    Km3KurumaClsnFilePtr(unsigned int fileID);
    ~Km3KurumaClsnFilePtr();
};

typedef char Km3KurumaModelFilePtr_size_must_be_8[
    sizeof(Km3KurumaModelFilePtr) == 8 ? 1 : -1];
typedef char Km3KurumaClsnFilePtr_size_must_be_8[
    sizeof(Km3KurumaClsnFilePtr) == 8 ? 1 : -1];

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *kcl;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[
    sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern "C" {
extern Km3KurumaModelFilePtr gKm3KurumaModelFile;
extern Km3KurumaClsnFilePtr gKm3KurumaCollisionFile;
}

struct Km3KurumaClpsBlock {
    char magic[4];
    unsigned short entrySize;
    unsigned short entryCount;
    CLPS entries[1];
};
typedef char Km3KurumaClpsBlock_size_must_be_16[sizeof(Km3KurumaClpsBlock) == 16 ? 1 : -1];

/* One property entry: all twelve KCL prisms use index zero. The +6
 * halfword is interpreted as the count; the lookup checks only entrySize. */
extern "C" const Km3KurumaClpsBlock gKm3KurumaClps = {
    {'C', 'L', 'P', 'S'}, 8, 1, {{0x00000fc0, 0x000000ff}}};

/* Defined here because this TU owns the descriptor in overlay .data. */
extern "C" ResourceDescriptor gKm3KurumaResources = {
    &gKm3KurumaModelFile, &gKm3KurumaCollisionFile,
    /* SetFile retains this read-only block through its opaque legacy API. */
    (CLPS_Block *)&gKm3KurumaClps};

#include "daObjKm3_Kuruma_c.h"
#include "SpawnInfo.h"

extern "C" {
s32 func_ov002_020b6958(daObjKuruma_c *self, ResourceDescriptor *descriptor);
s32 func_ov002_020b68b0(daObjKuruma_c *self, ResourceDescriptor *descriptor);
}

// @symbol daObjKm3_Kuruma_c_classInit
extern "C" daObjKm3_Kuruma_c *daObjKm3_Kuruma_c_classInit()
{
    return new daObjKm3_Kuruma_c();
}

// @symbol g_profile_KM3_KURUMA
extern "C" SpawnInfo<daObjKm3_Kuruma_c> g_profile_KM3_KURUMA = {
    daObjKm3_Kuruma_c_classInit,
    0x0097, /* KM3_KURUMA registry id 151 */
    0x00de,
    2,
    0,
    0x00200000,
    0x01000000,
    0
};

// @symbol _ZN17daObjKm3_Kuruma_c13InitResourcesEv
s32 daObjKm3_Kuruma_c::InitResources()
{
    return func_ov002_020b6958(this, &gKm3KurumaResources);
}

// @symbol _ZN17daObjKm3_Kuruma_c16CleanupResourcesEv
s32 daObjKm3_Kuruma_c::CleanupResources()
{
    return func_ov002_020b68b0(this, &gKm3KurumaResources);
}

/* Source order is construction order: model file 1663, then the collision
 * file 1664. The compiler registers each destructor beside the object. */
Km3KurumaModelFilePtr gKm3KurumaModelFile(ASSET_KM3_KURUMA_BMD);
Km3KurumaClsnFilePtr gKm3KurumaCollisionFile(ASSET_KM3_KURUMA_KCL);
