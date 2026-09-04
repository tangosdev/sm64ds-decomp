//cpp
/* Reconstructed ov064/daObjFl_Gura_c translation unit.
 *
 * tu_map identifies the RTTI-backed four-function class run at
 * 0x02118020..0x021180fc. The registry-backed factory immediately after it
 * allocates sizeof(daObjFl_Gura_c), constructs the direct base, installs the
 * class vptr, and ends at the next class's D1. That evidence supports testing
 * the combined five-function interval 0x02118020..0x02118138; tu_map alone
 * does not prove the factory join.
 *
 * mwccarm emits ordinary function sections in reverse source order. Keep the
 * factory first. The inline destructor declared last in daObjFl_Gura_c emits
 * the retail D1/D0 pair first and emits no leaf D2 body.
 */

#include "daObjFl_Gura_c.h"

extern "C" int func_ov002_020b60fc(void *self, void *data);
extern "C" int func_ov002_020b6244(void *self, void *data);
extern "C" char data_ov064_0211adb0[];

struct FlGuraProfile {
    daObjFl_Gura_c *(*classInit)();
    s16 profileID;
    s16 groupFlags;
    u32 actorFlags;
    Fix12i cullRadiusX;
    Fix12i cullRadiusY;
    u32 executeOrder;
    u32 drawOrder;
};

typedef char FlGuraProfile_size_must_be_0x1c[
    sizeof(FlGuraProfile) == 0x1c ? 1 : -1];

/* SM64DS directly preserves the class RTTI, FL_GURA ID, descriptor
 * relationship, and factory behavior. The profile struct name is local to
 * this reconstruction. */
// @symbol daObjFl_Gura_c_classInit
extern "C" daObjFl_Gura_c *daObjFl_Gura_c_classInit()
{
    return new daObjFl_Gura_c();
}

extern "C" FlGuraProfile g_profile_FL_GURA = {
    daObjFl_Gura_c_classInit,
    0x0048,
    0x0118,
    2,
    0x00080000,
    0x001a0000,
    0x02000000,
    0
};

// @symbol _ZN14daObjFl_Gura_c13InitResourcesEv
#pragma long_calls on
int daObjFl_Gura_c::InitResources()
{
    return func_ov002_020b6244(this, data_ov064_0211adb0);
}
#pragma long_calls off

// @symbol _ZN14daObjFl_Gura_c16CleanupResourcesEv
#pragma long_calls on
int daObjFl_Gura_c::CleanupResources()
{
    return func_ov002_020b60fc(this, data_ov064_0211adb0);
}
#pragma long_calls off
