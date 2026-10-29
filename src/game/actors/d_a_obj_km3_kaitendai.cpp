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
 * This TU defines the rotation increments, two-entry collision-property
 * block, and three-pointer resource descriptor. The data names describe
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

/* 8-byte file handles. The model uses func_02017acc / func_02017ab4 and the
 * collision file func_02017b4c / SharedFilePtr_Destruct_Clsn. The spellings
 * are local; the manifest aliases the generated names to those ROM symbols. */
struct Km3KaitendaiModelFilePtr : SharedFilePtr {
    Km3KaitendaiModelFilePtr(unsigned int fileID);
    ~Km3KaitendaiModelFilePtr();
};

struct Km3KaitendaiClsnFilePtr : SharedFilePtr {
    Km3KaitendaiClsnFilePtr(unsigned int fileID);
    ~Km3KaitendaiClsnFilePtr();
};

typedef char
    Km3KaitendaiModelFilePtr_size_must_be_8[sizeof(Km3KaitendaiModelFilePtr) == 8 ? 1 : -1];
typedef char Km3KaitendaiClsnFilePtr_size_must_be_8[sizeof(Km3KaitendaiClsnFilePtr) == 8 ? 1 : -1];

struct Km3KaitendaiClpsBlock {
    char magic[4];
    u16 entrySize;
    u16 entryCount;
    CLPS entries[2];
};
typedef char
    Km3KaitendaiClpsBlock_size_must_be_0x18[sizeof(Km3KaitendaiClpsBlock) == 0x18 ? 1 : -1];

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *kcl;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern Km3KaitendaiModelFilePtr data_ov047_02112610;
extern Km3KaitendaiClsnFilePtr data_ov047_02112608;
/* KCL prism attributes index these two entries. The following ROM bytes
 * have no established type or ownership and are not part of this block. */
extern const Km3KaitendaiClpsBlock gKm3KaitendaiClps = {
    {'C', 'L', 'P', 'S'}, 8, 2, {{0x00000fc0, 0x000000ff}, {0x00000fc4, 0x000000ff}}};

/* Defined here because this TU owns the descriptor in overlay .data. */
ResourceDescriptor data_ov047_02112334 = {
    &data_ov047_02112610, &data_ov047_02112608,
    reinterpret_cast<CLPS_Block *>(const_cast<Km3KaitendaiClpsBlock *>(&gKm3KaitendaiClps))};

#include "daObjKm3_Kaitendai_c.h"
#include "SpawnInfo.h"

s16 gKm3KaitendaiRotationIncrement = 312;
s16 gKm3KaitendaiAlternateRotationIncrement = 624;

// @symbol daObjKm3_Kaitendai_c_classInit
extern "C" daObjKm3_Kaitendai_c *daObjKm3_Kaitendai_c_classInit() {
    return new daObjKm3_Kaitendai_c();
}

// @symbol g_profile_KM3_KAITENDAI
extern "C" SpawnInfo<daObjKm3_Kaitendai_c> g_profile_KM3_KAITENDAI = {
    daObjKm3_Kaitendai_c_classInit,
    0x009c, /* KM3_KAITENDAI registry id 156 */
    0x00e3,
    2,
    0,
    0x00380000,
    0x02000000,
    0};

// @symbol _ZN20daObjKm3_Kaitendai_c13InitResourcesEv
s32 daObjKm3_Kaitendai_c::InitResources() {
    s16 rotationIncrement = gKm3KaitendaiRotationIncrement;
    if ((param1 & 0xff) == 1)
        rotationIncrement = gKm3KaitendaiAlternateRotationIncrement;
    return InitResourcesFrom(&data_ov047_02112334, rotationIncrement);
}

// @symbol _ZN20daObjKm3_Kaitendai_c16CleanupResourcesEv
s32 daObjKm3_Kaitendai_c::CleanupResources() {
    return CleanupResourcesFrom(&data_ov047_02112334);
}

/* Source order is construction order: model file 1661, then the collision
 * file 1662. The compiler registers each destructor beside the object. */
Km3KaitendaiModelFilePtr data_ov047_02112610(ASSET_KM3_KAITENDAI_BMD);
Km3KaitendaiClsnFilePtr data_ov047_02112608(ASSET_KM3_KAITENDAI_KCL);
