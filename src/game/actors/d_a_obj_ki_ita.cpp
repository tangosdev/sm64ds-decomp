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
 * Leftover: func_ov002_020b5e58 and data_ov016_02114b8c (shared
 * float-board setup lives in ov002; this leaf only passes the file table).
 * #pragma long_calls: ov002 call is the wrong instruction without it.
 */

#include "daObjKi_Ita_c.h"

extern "C" int func_ov002_020b5e58(void *self, void *data);
extern "C" void *data_ov016_02114b8c[];

struct KiItaSpawnInfo {
    daObjKi_Ita_c *(*classInit)();
    s16 executeOrder;      /* +4 behavior/execute priority */
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
#pragma long_calls on
int daObjKi_Ita_c::InitResources()
{
    /* Shared float-board setup: BMD, KCL, bob/sink state. */
    return func_ov002_020b5e58(this, data_ov016_02114b8c);
}
#pragma long_calls off
