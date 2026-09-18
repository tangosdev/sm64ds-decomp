//cpp
/**
 * Bowser in the Dark World's rickshaw cart (`kuruma`).
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjKuruma_c's shared ov002 helpers.
 * func_ov002_020b6958 loads slot 0 with Model::LoadFile, slot 1 with
 * dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile. ov043 sinit
 * constructs those SharedFilePtrs as file IDs 1619 / 1620.
 *
 * daObjKm1_Kuruma_c_classInit is reconstructed (RTTI daObjKm1_Kuruma_c,
 * KM1_KURUMA registry). Retail does not store that spelling.
 *
 * deslop
 * Leftover: func_ov002_020b6958 / func_ov002_020b68b0 are still the
 *   linker names of daObjKuruma_c Init/Cleanup (the base leaves
 *   those slots pure virtual). Naming belongs in ov002.
 * Leftover: data_ov043_02112418 is overlay data this TU does not
 *   own; the BMD/KCL SharedFilePtrs and CLPS_Block are still
 *   data_ov043_*.
 * Leftover: g_profile_KM1_KURUMA is overlay data; this TU is
 *   text-only so the definition here is a deadstripped duplicate.
 */

#include "daObjKm1_Kuruma_c.h"
#include "SharedFilePtr.h"

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[
    sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern "C" {
int func_ov002_020b6958(daObjKm1_Kuruma_c *self, ResourceDescriptor *descriptor);
int func_ov002_020b68b0(daObjKm1_Kuruma_c *self, ResourceDescriptor *descriptor);
extern ResourceDescriptor data_ov043_02112418;
}

struct KurumaSpawnInfo {
    daObjKm1_Kuruma_c *(*classInit)();
    s16 executePriority; /* +4: also KM1_KURUMA registry id 0x0088 = 136 */
    s16 renderPriority;  /* +6 */
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};
typedef char KurumaSpawnInfo_size_must_be_0x1c[
    sizeof(KurumaSpawnInfo) == 0x1c ? 1 : -1];

// @symbol daObjKm1_Kuruma_c_classInit
extern "C" daObjKm1_Kuruma_c *daObjKm1_Kuruma_c_classInit()
{
    return new daObjKm1_Kuruma_c();
}

// @symbol g_profile_KM1_KURUMA
extern "C" KurumaSpawnInfo g_profile_KM1_KURUMA = {
    daObjKm1_Kuruma_c_classInit,
    0x0088,
    0x00b1,
    2,
    0,
    0x00200000,
    0x01000000,
    0
};

// @symbol _ZN17daObjKm1_Kuruma_c13InitResourcesEv
s32 daObjKm1_Kuruma_c::InitResources()
{
    return func_ov002_020b6958(this, &data_ov043_02112418);
}

// @symbol _ZN17daObjKm1_Kuruma_c16CleanupResourcesEv
s32 daObjKm1_Kuruma_c::CleanupResources()
{
    return func_ov002_020b68b0(this, &data_ov043_02112418);
}
