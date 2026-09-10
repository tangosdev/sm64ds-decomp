//cpp
/* Jolly Roger Bay's floating plank. */

#include "daObjKi_Ita_c.h"

extern "C" int func_ov002_020b5e58(char *self, char *files);
extern "C" char data_ov016_02114b8c;

struct KiItaSpawnInfo {
    daObjKi_Ita_c *(*classInit)();
    u16 executeOrder;
    u16 drawOrder;
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
    return func_ov002_020b5e58(reinterpret_cast<char *>(this),
                              &data_ov016_02114b8c);
}
