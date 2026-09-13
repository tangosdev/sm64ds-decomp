//cpp
/**
 * Bowser in the Sky's rickshaw axle (`kurumajiku`).
 *
 * No fields of its own. InitResources / CleanupResources hand this
 * overlay's model and collision files to daObjKurumajiku_c's shared
 * ov002 helpers. func_ov002_020b6c54 loads slot 0 with Model::LoadFile,
 * slot 1 with dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile, then
 * Spawns four carts (actor id KM3_KURUMA) and stores their uniqueIDs
 * in mMountedActorIds. ov047 sinit constructs those SharedFilePtrs as
 * file IDs 1665 / 1666.
 *
 * The three-word file table is defined in this TU. Retail .data
 * order is descriptor, type-name, profile, vtable.
 *
 * daObjKm3_Kurumajiku_c_classInit is reconstructed (RTTI
 * daObjKm3_Kurumajiku_c, KM3_KURUMAJIKU registry). Retail does not
 * store that spelling.
 *
 * deslop
 * Leftover: func_ov002_020b6c54 / func_ov002_020b6ac8 are still the
 *   linker names of the shared ov002 setup/teardown helpers. Naming
 *   belongs in ov002.
 * Leftover: the BMD/KCL SharedFilePtrs and CLPS_Block are still
 *   data_ov047_*.
 */

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
extern SharedFilePtr data_ov047_021125e8;
extern SharedFilePtr data_ov047_021125e0;
extern CLPS_Block data_ov047_02111b54;
}

/* Retail data order: this descriptor, then type name, profile, vtable. */
extern "C" ResourceDescriptor data_ov047_02112258 = {
    &data_ov047_021125e8,
    &data_ov047_021125e0,
    &data_ov047_02111b54
};

#include "daObjKm3_Kurumajiku_c.h"

extern "C" {
int func_ov002_020b6c54(daObjKm3_Kurumajiku_c *self,
                        ResourceDescriptor *descriptor, unsigned actorID);
int func_ov002_020b6ac8(daObjKm3_Kurumajiku_c *self,
                        ResourceDescriptor *descriptor);
}

struct KurumajikuSpawnInfo {
    daObjKm3_Kurumajiku_c *(*classInit)();
    s16 executePriority; /* +4: also KM3_KURUMAJIKU registry id 0x0098 = 152 */
    s16 renderPriority;  /* +6 */
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};
typedef char KurumajikuSpawnInfo_size_must_be_0x1c[
    sizeof(KurumajikuSpawnInfo) == 0x1c ? 1 : -1];

/* KM3_KURUMA: the cart this axle drives. The helper Spawns four of them. */
enum { kCartActorId = 0x97 };

// @symbol daObjKm3_Kurumajiku_c_classInit
extern "C" daObjKm3_Kurumajiku_c *daObjKm3_Kurumajiku_c_classInit()
{
    return new daObjKm3_Kurumajiku_c();
}

// @symbol g_profile_KM3_KURUMAJIKU
extern "C" KurumajikuSpawnInfo g_profile_KM3_KURUMAJIKU = {
    daObjKm3_Kurumajiku_c_classInit,
    0x0098,
    0x00df,
    2,
    0,
    0x00300000,
    0x01000000,
    0
};

// @symbol _ZN21daObjKm3_Kurumajiku_c13InitResourcesEv
s32 daObjKm3_Kurumajiku_c::InitResources()
{
    return func_ov002_020b6c54(this, &data_ov047_02112258, kCartActorId);
}

// @symbol _ZN21daObjKm3_Kurumajiku_c16CleanupResourcesEv
s32 daObjKm3_Kurumajiku_c::CleanupResources()
{
    return func_ov002_020b6ac8(this, &data_ov047_02112258);
}
