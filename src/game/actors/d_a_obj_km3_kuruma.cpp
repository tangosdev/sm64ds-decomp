//cpp
/**
 * Bowser in the Sky's rickshaw cart (`kuruma`).
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjKuruma_c's shared ov002 helpers.
 * func_ov002_020b6958 loads slot 0 with Model::LoadFile, slot 1 with
 * dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile. ov047 sinit
 * constructs those SharedFilePtrs as file IDs 1663 / 1664.
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

#include "SharedFilePtr.h"

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *kcl;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[
    sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern "C" {
extern SharedFilePtr data_ov047_02112638;
extern SharedFilePtr data_ov047_02112630;
extern CLPS_Block data_ov047_02111ab4;
}

/* Defined here because this TU owns the descriptor in overlay .data. */
extern "C" ResourceDescriptor data_ov047_02112408 = {
    &data_ov047_02112638,
    &data_ov047_02112630,
    &data_ov047_02111ab4
};

#include "daObjKm3_Kuruma_c.h"

extern "C" {
s32 func_ov002_020b6958(daObjKm3_Kuruma_c *self, ResourceDescriptor *descriptor);
s32 func_ov002_020b68b0(daObjKm3_Kuruma_c *self, ResourceDescriptor *descriptor);
}

struct KurumaSpawnInfo {
    daObjKm3_Kuruma_c *(*classInit)();
    s16 executePriority; /* +4: also KM3_KURUMA registry id 0x0097 = 151 */
    s16 renderPriority;  /* +6 */
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};
typedef char KurumaSpawnInfo_size_must_be_0x1c[
    sizeof(KurumaSpawnInfo) == 0x1c ? 1 : -1];

// @symbol daObjKm3_Kuruma_c_classInit
extern "C" daObjKm3_Kuruma_c *daObjKm3_Kuruma_c_classInit()
{
    return new daObjKm3_Kuruma_c();
}

// @symbol g_profile_KM3_KURUMA
extern "C" KurumaSpawnInfo g_profile_KM3_KURUMA = {
    daObjKm3_Kuruma_c_classInit,
    0x0097,
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
