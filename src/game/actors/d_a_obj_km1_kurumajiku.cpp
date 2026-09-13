//cpp
/**
 * Bowser in the Dark World's rickshaw axle (`kurumajiku`).
 *
 * No fields of its own. InitResources / CleanupResources hand this
 * overlay's model and collision files to daObjKurumajiku_c's shared
 * ov002 helpers. func_ov002_020b6c54 loads slot 0 with Model::LoadFile,
 * slot 1 with dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile, then
 * Spawns four carts (actor id KM1_KURUMA) and stores their uniqueIDs
 * in mMountedActorIds. ov043 sinit constructs those SharedFilePtrs as
 * file IDs 1621 / 1622.
 *
 * daObjKm1_Kurumajiku_c_classInit is reconstructed (RTTI
 * daObjKm1_Kurumajiku_c, KM1_KURUMAJIKU registry). Retail does not
 * store that spelling.
 *
 * deslop
 * Leftover: func_ov002_020b6c54 / func_ov002_020b6ac8 are still the
 *   linker names of daObjKurumajiku_c Init/Cleanup (the base leaves
 *   those slots pure virtual). Naming belongs in ov002.
 * Leftover: data_ov043_02112344 is overlay data this TU does not
 *   own; the BMD/KCL SharedFilePtrs and CLPS_Block are still
 *   data_ov043_*.
 * Leftover: g_profile_KM1_KURUMAJIKU is overlay data; this TU is
 *   text-only so the definition here is a deadstripped duplicate.
 */

#include "daObjKm1_Kurumajiku_c.h"
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
int func_ov002_020b6c54(daObjKm1_Kurumajiku_c *self,
                        ResourceDescriptor *descriptor, unsigned actorID);
int func_ov002_020b6ac8(daObjKm1_Kurumajiku_c *self,
                        ResourceDescriptor *descriptor);
extern ResourceDescriptor data_ov043_02112344;
}

struct KurumajikuSpawnInfo {
    daObjKm1_Kurumajiku_c *(*classInit)();
    s16 executePriority; /* +4: also KM1_KURUMAJIKU registry id 0x0089 = 137 */
    s16 renderPriority;  /* +6 */
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};
typedef char KurumajikuSpawnInfo_size_must_be_0x1c[
    sizeof(KurumajikuSpawnInfo) == 0x1c ? 1 : -1];

/* KM1_KURUMA: the cart this axle drives. The helper Spawns four of them. */
enum { kCartActorId = 0x88 };

// @symbol daObjKm1_Kurumajiku_c_classInit
extern "C" daObjKm1_Kurumajiku_c *daObjKm1_Kurumajiku_c_classInit()
{
    return new daObjKm1_Kurumajiku_c();
}

// @symbol g_profile_KM1_KURUMAJIKU
extern "C" KurumajikuSpawnInfo g_profile_KM1_KURUMAJIKU = {
    daObjKm1_Kurumajiku_c_classInit,
    0x0089,
    0x00b2,
    2,
    0,
    0x00300000,
    0x01000000,
    0
};

// @symbol _ZN21daObjKm1_Kurumajiku_c13InitResourcesEv
s32 daObjKm1_Kurumajiku_c::InitResources()
{
    return func_ov002_020b6c54(this, &data_ov043_02112344, kCartActorId);
}

// @symbol _ZN21daObjKm1_Kurumajiku_c16CleanupResourcesEv
s32 daObjKm1_Kurumajiku_c::CleanupResources()
{
    return func_ov002_020b6ac8(this, &data_ov043_02112344);
}
