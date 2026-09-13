//cpp
/**
 * Bowser in the Fire Sea's floating floor.
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjUkiyuka_c's shared ov002 helpers.
 * Init also stores kBobAmplitude into mBobAmplitude.
 * func_ov002_020b6584 loads slot 0 with Model::LoadFile, slot 1
 * with dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile, copies
 * mPosY to mRestY, and stores arg 3 at mBobAmplitude. ov045 sinit
 * constructs those SharedFilePtrs as file IDs 1643 / 1644.
 *
 * daObjKm2_Ukishima_c_classInit is reconstructed (RTTI
 * daObjKm2_Ukishima_c, KM2_UKISHIMA registry). Retail does not store
 * that spelling.
 *
 * deslop
 * Leftover: func_ov002_020b6584 / func_ov002_020b6424 are still the
 *   linker names of daObjUkiyuka_c Init/Cleanup (the base leaves
 *   those slots pure virtual). Naming belongs in ov002.
 * Leftover: the file table is still data_ov045_02112f08.
 * Leftover: g_profile_KM2_UKISHIMA is overlay data; this TU is
 *   text-only so the definition here is a deadstripped duplicate.
 */

#include "daObjKm2_Ukishima_c.h"
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
int func_ov002_020b6584(daObjKm2_Ukishima_c *self, ResourceDescriptor *descriptor,
                        Fix12i bobAmplitude);
int func_ov002_020b6424(daObjKm2_Ukishima_c *self, ResourceDescriptor *descriptor);
extern ResourceDescriptor data_ov045_02112f08;
}

struct UkishimaSpawnInfo {
    daObjKm2_Ukishima_c *(*classInit)();
    s16 executePriority; /* +4: also KM2_UKISHIMA registry id 0x0091 = 145 */
    s16 renderPriority;  /* +6 */
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};
typedef char UkishimaSpawnInfo_size_must_be_0x1c[
    sizeof(UkishimaSpawnInfo) == 0x1c ? 1 : -1];

/* 20.12; the shared setup helper writes this to mBobAmplitude. */
enum { kBobAmplitude = 0xf50 };

// @symbol daObjKm2_Ukishima_c_classInit
extern "C" daObjKm2_Ukishima_c *daObjKm2_Ukishima_c_classInit()
{
    return new daObjKm2_Ukishima_c();
}

// @symbol g_profile_KM2_UKISHIMA
extern "C" UkishimaSpawnInfo g_profile_KM2_UKISHIMA = {
    daObjKm2_Ukishima_c_classInit,
    0x0091,
    0x00d8,
    2,
    0x00060000,
    0x01000000,
    0x02000000,
    0
};

// @symbol _ZN19daObjKm2_Ukishima_c13InitResourcesEv
s32 daObjKm2_Ukishima_c::InitResources()
{
    return func_ov002_020b6584(this, &data_ov045_02112f08, kBobAmplitude);
}

// @symbol _ZN19daObjKm2_Ukishima_c16CleanupResourcesEv
s32 daObjKm2_Ukishima_c::CleanupResources()
{
    return func_ov002_020b6424(this, &data_ov045_02112f08);
}
