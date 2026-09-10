//cpp
/**
 * Jolly Roger Bay floating plank (`ita` = board).
 *
 * No model of its own in this file: InitResources hands the shared
 * float-board setup this stage's model and collision files.
 *
 * daObjKi_Ita_c_classInit / g_profile_KI_ITA are reconstructed (RTTI
 * daObjKi_Ita_c, KI_ITA registry). Retail does not store those spellings.
 *
 * deslop
 * Leftover: func_ov002_020b5e58 is still the linker name of
 *   daObjFloatBoard_c Init (slot 0 is pure virtual on the base).
 *   Naming belongs in ov002. That helper loads **(SharedFilePtr**)fp,
 *   fp+4, fp+8 as model/KCL/CLPS.
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

extern "C" int func_ov002_020b5e58(void *self, ResourceDescriptor *data);
extern "C" ResourceDescriptor data_ov016_02114b8c;

struct KiItaSpawnInfo {
    daObjKi_Ita_c *(*classInit)();
    s16 executeOrder;      /* +4: also KI_ITA registry id 0x003c = 60 */
    s16 drawOrder;         /* +6 render priority */
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
    0x003c,       /* behavior/execute priority */
    0x00b8,       /* render priority */
    2,            /* actorFlags */
    0,
    0x00250000,   /* clip radius */
    0x02000000,   /* clip distance */
    0
};

// @symbol _ZN13daObjKi_Ita_c13InitResourcesEv
int daObjKi_Ita_c::InitResources()
{
    /* Shared float-board setup: BMD, KCL, bob/sink state. */
    return func_ov002_020b5e58(this, &data_ov016_02114b8c);
}
