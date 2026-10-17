//cpp
/**
 * daKpa_c in the Fire Sea's tilting slab.
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjGuragura_c's shared ov002 helpers.
 *
 * The three-word file table is defined in this TU. Retail .data
 * order is descriptor, type-name, profile, vtable.
 * func_ov002_020b6244 loads slot 0 with
 * Model::LoadFile, slot 1 with dBgW_Kc::LoadFile, slot 2 as CLPS
 * into SetFile. This TU's static initializer constructs those
 * SharedFilePtrs as file IDs 1631 / 1632.
 *
 * daObjKm2_Gura_c_classInit is reconstructed (RTTI daObjKm2_Gura_c,
 * KM2_GURA registry). Retail does not store that spelling.
 *
 * deslop
 * Leftover: func_ov002_020b6244 / func_ov002_020b60fc are still the
 *   linker names of daObjGuragura_c Init/Cleanup (the base leaves
 *   those slots pure virtual). Naming belongs in ov002.
 * Leftover: the BMD/KCL SharedFilePtrs and CLPS_Block are still
 *   data_ov045_*.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_km2_gura_AssetId {
    ASSET_KM2_GURA_BMD = 0x065f,
    ASSET_KM2_GURA_KCL = 0x0660
};

#include "daObjKm2_Gura_c.h"
#include "SpawnInfo.h"
#include "SharedFilePtr.h"

/* 8-byte file handles. The model uses func_02017acc / func_02017ab4 and the
 * collision file func_02017b4c / SharedFilePtr_Destruct_Clsn. The spellings
 * are local; the manifest aliases the generated names to those ROM symbols. */
struct Km2GuraModelFilePtr : SharedFilePtr {
    unsigned int words[2];

    Km2GuraModelFilePtr(unsigned int fileID);
    ~Km2GuraModelFilePtr();
};

struct Km2GuraClsnFilePtr : SharedFilePtr {
    unsigned int words[2];

    Km2GuraClsnFilePtr(unsigned int fileID);
    ~Km2GuraClsnFilePtr();
};

typedef char Km2GuraModelFilePtr_size_must_be_8[
    sizeof(Km2GuraModelFilePtr) == 8 ? 1 : -1];
typedef char Km2GuraClsnFilePtr_size_must_be_8[
    sizeof(Km2GuraClsnFilePtr) == 8 ? 1 : -1];

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[
    sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern "C" {
extern Km2GuraModelFilePtr data_ov045_02113220;
extern Km2GuraClsnFilePtr data_ov045_02113228;
extern CLPS_Block data_ov045_021124f0;
}

extern "C" ResourceDescriptor data_ov045_02112fdc = {
    &data_ov045_02113220,
    &data_ov045_02113228,
    &data_ov045_021124f0
};

extern "C" {
int func_ov002_020b6244(daObjGuragura_c *self, ResourceDescriptor *descriptor);
int func_ov002_020b60fc(daObjGuragura_c *self, ResourceDescriptor *descriptor);
}

// @symbol daObjKm2_Gura_c_classInit
extern "C" daObjKm2_Gura_c *daObjKm2_Gura_c_classInit()
{
    return new daObjKm2_Gura_c();
}

// @symbol g_profile_KM2_GURA
extern "C" SpawnInfo<daObjKm2_Gura_c> g_profile_KM2_GURA = {
    daObjKm2_Gura_c_classInit,
    0x008d, /* KM2_GURA registry id 141 */
    0x00d4,
    2,
    0x00120000,
    0x00200000,
    0x02000000,
    0,
};

// @symbol _ZN15daObjKm2_Gura_c13InitResourcesEv
s32 daObjKm2_Gura_c::InitResources()
{
    return func_ov002_020b6244(this, &data_ov045_02112fdc);
}

// @symbol _ZN15daObjKm2_Gura_c16CleanupResourcesEv
s32 daObjKm2_Gura_c::CleanupResources()
{
    return func_ov002_020b60fc(this, &data_ov045_02112fdc);
}

/* Source order is construction order: model file 1631, then the collision
 * file 1632. The compiler registers each destructor beside the object. */
Km2GuraModelFilePtr data_ov045_02113220(ASSET_KM2_GURA_BMD);
Km2GuraClsnFilePtr data_ov045_02113228(ASSET_KM2_GURA_KCL);
