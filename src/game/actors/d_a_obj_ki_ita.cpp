//cpp
/**
 * Jolly Roger Bay floating plank (`ita` = board).
 *
 * No fields. InitResources hands this overlay's model and collision
 * files to daObjFloatBoard_c's shared ov002 helper. CleanupResources
 * is the base's.
 *
 * daObjKi_Ita_c_classInit / g_profile_KI_ITA are reconstructed (RTTI
 * daObjKi_Ita_c, KI_ITA registry). Retail does not store those spellings.
 *
 * deslop
 * Leftover: func_ov002_020b5e58 is still the linker name of the shared
 *   float-board setup (slot 0 is pure virtual on daObjFloatBoard_c).
 *   Naming belongs in ov002. That helper loads slot 0 with
 *   Model::LoadFile, slot 1 with dBgW_Kc::LoadFile, slot 2 as CLPS
 *   into SetFile.
 * Leftover: data_ov016_02114b8c is still the linker name of this
 *   overlay's three-word file table. Those words sit between this
 *   class's RTTI and type-name; this TU does not own them.
 */

#include "daObjKi_Ita_c.h"
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
int func_ov002_020b5e58(daObjKi_Ita_c *self, ResourceDescriptor *descriptor);
extern ResourceDescriptor data_ov016_02114b8c;
}

struct KiItaSpawnInfo {
    daObjKi_Ita_c *(*classInit)();
    s16 executePriority; /* +4: also KI_ITA registry id 0x003c = 60 */
    s16 renderPriority;  /* +6 */
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};

typedef char KiItaSpawnInfo_size_must_be_0x1c[
    sizeof(KiItaSpawnInfo) == 0x1c ? 1 : -1];

// @symbol daObjKi_Ita_c_classInit
extern "C" daObjKi_Ita_c *daObjKi_Ita_c_classInit()
{
    return new daObjKi_Ita_c();
}

extern "C" KiItaSpawnInfo g_profile_KI_ITA = {
    daObjKi_Ita_c_classInit,
    0x003c,
    0x00b8,
    2,
    0,
    0x00250000,
    0x02000000,
    0
};

// @symbol _ZN13daObjKi_Ita_c13InitResourcesEv
int daObjKi_Ita_c::InitResources()
{
    return func_ov002_020b5e58(this, &data_ov016_02114b8c);
}
