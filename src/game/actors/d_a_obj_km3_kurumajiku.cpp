//cpp
/* Reconstructed production translation unit.
 * ov047/daObjKm3_Kurumajiku_c  (5 function(s))
 *
 * This one compiler invocation owns the retail D1/D0 pair, both resource
 * methods, the factory, resource descriptor, type-name string, SpawnInfo, and
 * vtable storage. Exact per-member authorship remains in attribution.json.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (destructor variants have compiler-chosen order). The destructor
 * lives inline in the class definition so mwccarm emits only the retail D1/D0
 * pair, in retail order.
 *
 * The manifest records the five absorbed intake sources and the exact text/data
 * ownership proof; do not recreate per-function production files for this class.
 */

#include "daObjKm3_Kurumajiku_c.h"

struct ResourceDescriptor {
    void *entries[3];
};

extern "C" {
extern char data_ov047_021125e8[];
extern char data_ov047_021125e0[];
extern char data_ov047_02111b54[];
}

/* Keep this definition before the SpawnInfo and method definitions. Retail
 * places the descriptor between the class typeinfo and type-name records. */
extern "C" ResourceDescriptor data_ov047_02112258 = {
    data_ov047_021125e8,
    data_ov047_021125e0,
    data_ov047_02111b54
};

struct Km3SpawnInfo {
    daObjKm3_Kurumajiku_c *(*classInit)();
    s16 profileIDAndExecuteOrder;
    s16 drawOrder;
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};

typedef char Km3SpawnInfo_size_must_be_0x1c[
    sizeof(Km3SpawnInfo) == 0x1c ? 1 : -1];

extern "C" {
extern void *_ZN7fBase_cnwEj(unsigned size);
extern void _ZN10dBgActor_cC2Ev(void *self);
extern int _ZTV17daObjKurumajiku_c[];
extern int _ZTV21daObjKm3_Kurumajiku_c[];
int func_ov002_020b6ac8(daObjKm3_Kurumajiku_c *self,
                        ResourceDescriptor *descriptor);
int func_ov002_020b6c54(daObjKm3_Kurumajiku_c *self,
                        ResourceDescriptor *descriptor, unsigned actorID);
}

/* Reconstructed source-style names: SM64DS proves the RTTI class,
 * KM3_KURUMAJIKU registry ID, descriptor/factory relationship, and object
 * shape; later EAD lineage supplies the spelling prior. Exact original
 * SM64DS symbols are not preserved. Historical project aliases:
 * RickshawBs_Spawn and RickshawBs_SpawnInfo. */
/* ROM ordinal 4 -- class initializer, 0x02111280, size 0x3c */
// @symbol daObjKm3_Kurumajiku_c_classInit
extern "C" daObjKm3_Kurumajiku_c *daObjKm3_Kurumajiku_c_classInit()
{
    daObjKm3_Kurumajiku_c *actor =
        static_cast<daObjKm3_Kurumajiku_c *>(_ZN7fBase_cnwEj(0x330));

    if (actor) {
        _ZN10dBgActor_cC2Ev(actor);
        *reinterpret_cast<int *>(actor) = (int)_ZTV17daObjKurumajiku_c;
        *reinterpret_cast<int *>(actor) =
            (int)&_ZTV21daObjKm3_Kurumajiku_c[2];
    }

    return actor;
}

extern "C" Km3SpawnInfo g_profile_KM3_KURUMAJIKU = {
    daObjKm3_Kurumajiku_c_classInit,
    0x0098,
    0x00df,
    2,
    0,
    0x00300000,
    0x01000000,
    0
};

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN21daObjKm3_Kurumajiku_c13InitResourcesEv, 0x02111268, size 0x18 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN21daObjKm3_Kurumajiku_c13InitResourcesEv
/* Delegates to the shared setup with this class's three-word descriptor and
 * the actor ID of the cart it drives. */
int daObjKm3_Kurumajiku_c::InitResources()
{
    return func_ov002_020b6c54(this, &data_ov047_02112258, 0x97);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN21daObjKm3_Kurumajiku_c16CleanupResourcesEv, 0x02111254, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN21daObjKm3_Kurumajiku_c16CleanupResourcesEv
int daObjKm3_Kurumajiku_c::CleanupResources()
{
    return func_ov002_020b6ac8(this, &data_ov047_02112258);
}
