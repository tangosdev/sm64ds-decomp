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
 * Leftover: the BMD/KCL SharedFilePtrs and CLPS_Block are still
 *   data_ov047_*.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_km3_kuruma_AssetId {
    ASSET_KM3_KURUMA_BMD = 0x067f,
    ASSET_KM3_KURUMA_KCL = 0x0680
};

#include "SharedFilePtr.h"

/* 8-byte file handles. The model uses func_02017acc / func_02017ab4 and the
 * collision file func_02017b4c / SharedFilePtr_Destruct_Clsn. The spellings
 * are local; the manifest aliases the generated names to those ROM symbols. */
struct Km3KurumaModelFilePtr : SharedFilePtr {
    unsigned int words[2];

    Km3KurumaModelFilePtr(unsigned int fileID);
    ~Km3KurumaModelFilePtr();
};

struct Km3KurumaClsnFilePtr : SharedFilePtr {
    unsigned int words[2];

    Km3KurumaClsnFilePtr(unsigned int fileID);
    ~Km3KurumaClsnFilePtr();
};

typedef char Km3KurumaModelFilePtr_size_must_be_8[
    sizeof(Km3KurumaModelFilePtr) == 8 ? 1 : -1];
typedef char Km3KurumaClsnFilePtr_size_must_be_8[
    sizeof(Km3KurumaClsnFilePtr) == 8 ? 1 : -1];

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *kcl;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[
    sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern "C" {
extern Km3KurumaModelFilePtr data_ov047_02112638;
extern Km3KurumaClsnFilePtr data_ov047_02112630;
extern CLPS_Block data_ov047_02111ab4;
}

/* Defined here because this TU owns the descriptor in overlay .data. */
extern "C" ResourceDescriptor data_ov047_02112408 = {
    &data_ov047_02112638,
    &data_ov047_02112630,
    &data_ov047_02111ab4
};

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
    return func_ov002_020b6958(this, &data_ov047_02112408);
}

// @symbol _ZN17daObjKm3_Kuruma_c16CleanupResourcesEv
s32 daObjKm3_Kuruma_c::CleanupResources()
{
    return func_ov002_020b68b0(this, &data_ov047_02112408);
}

/* Source order is construction order: model file 1663, then the collision
 * file 1664. The compiler registers each destructor beside the object. */
Km3KurumaModelFilePtr data_ov047_02112638(ASSET_KM3_KURUMA_BMD);
Km3KurumaClsnFilePtr data_ov047_02112630(ASSET_KM3_KURUMA_KCL);
