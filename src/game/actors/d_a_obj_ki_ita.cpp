//cpp
/* Reconstructed ov016/daObjKi_Ita_c translation unit.
 *
 * tu_map identifies the RTTI-backed three-function class run at
 * 0x02112ef4..0x02112fbc. The registry-backed factory immediately after it
 * allocates sizeof(daObjKi_Ita_c), constructs the direct base, installs the
 * class vptr, and ends at the next class's D1. That evidence supports testing
 * the combined four-function interval 0x02112ef4..0x02112ff8; tu_map alone
 * does not prove the factory join.
 *
 * mwccarm emits ordinary function sections in reverse source order. Keep the
 * factory first. The inline destructor declared last in daObjKi_Ita_c emits
 * the retail D1/D0 pair first and emits no leaf D2 body.
 */

#include "daObjKi_Ita_c.h"

extern "C" int func_ov002_020b5e58(void *self, void *data);
extern "C" void *data_ov016_02114b8c[];

struct KiItaProfile {
    daObjKi_Ita_c *(*classInit)();
    s16 profileID;
    s16 groupFlags;
    u32 actorFlags;
    Fix12i cullRadiusX;
    Fix12i cullRadiusY;
    u32 executeOrder;
    u32 drawOrder;
};

typedef char KiItaProfile_size_must_be_0x1c[
    sizeof(KiItaProfile) == 0x1c ? 1 : -1];

/* Reconstructed source-style names. SM64DS directly preserves the class RTTI,
 * KI_ITA ID, descriptor relationship, and factory behavior. */
// @symbol daObjKi_Ita_c_classInit
extern "C" daObjKi_Ita_c *daObjKi_Ita_c_classInit()
{
    return new daObjKi_Ita_c();
}

extern "C" KiItaProfile g_profile_KI_ITA = {
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
#pragma long_calls on
int daObjKi_Ita_c::InitResources()
{
    return func_ov002_020b5e58(this, data_ov016_02114b8c);
}
#pragma long_calls off
