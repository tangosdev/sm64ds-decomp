//cpp
/**
 * daKpa_c in the Dark World's rickshaw cart (`kuruma`).
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
 * Leftover: g_profile_KM1_KURUMA is overlay data; its definition here
 *   is a deadstripped duplicate.
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

/* File-scope objects at the end of this file construct the two resource
 * handles (model file 0x653, collision file 0x654). mwcc emits
 * __sinit_d_a_obj_km1_kuruma.cpp from those definitions. The wrapper names
 * are local; the handle constructors and destructors are the ROM
 * resource-family functions, aliased in the manifest. Nothing in this TU
 * names the handles directly; the ResourceDescriptor row above points at
 * them. */
struct Km1KurumaModelFile : SharedFilePtr {
    u32 words[2];

    Km1KurumaModelFile(u32 fileID);
    ~Km1KurumaModelFile();
};

struct Km1KurumaCollisionFilePtr : SharedFilePtr {
    u32 words[2];

    Km1KurumaCollisionFilePtr(u32 fileID);
    ~Km1KurumaCollisionFilePtr();
};

extern "C" {
int func_ov002_020b6958(daObjKuruma_c *self, ResourceDescriptor *descriptor);
int func_ov002_020b68b0(daObjKuruma_c *self, ResourceDescriptor *descriptor);
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

/* Static-init globals (was the handwritten __sinit_ov043_021118d4 shard).
 * Definition order is the retail initializer's construction order. */
Km1KurumaModelFile data_ov043_02112638(0x653);
Km1KurumaCollisionFilePtr data_ov043_02112630(0x654);
