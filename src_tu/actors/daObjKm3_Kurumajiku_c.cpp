//cpp
/* Manually curated shadow translation unit.
 * ov047/daObjKm3_Kurumajiku_c  (5 function(s))
 *
 * NOT ENROLLED and NOT CANONICAL. The readable class and member definitions
 * are compiled only by tubuild's scratch pipeline while the five legacy
 * production sources remain authoritative.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x021111a0  src/game/actors/daObjKm3_Kurumajiku_c/_ZN21daObjKm3_Kurumajiku_cD1Ev.cpp
 *   [1] 0x021111f0  src/_ZN21daObjKm3_Kurumajiku_cD0Ev.c
 *   [2] 0x02111254  src/game/actors/daObjKm3_Kurumajiku_c/_ZN21daObjKm3_Kurumajiku_c16CleanupResourcesEv.cpp
 *   [3] 0x02111268  src/game/actors/daObjKm3_Kurumajiku_c/_ZN21daObjKm3_Kurumajiku_c13InitResourcesEv.cpp
 *   [4] 0x02111280  src/game/actors/RickshawBs/RickshawBs_Spawn.c
 */

struct ResourceDescriptor {
    void *entries[3];
};

extern "C" {
extern char data_ov047_021125e8[];
extern char data_ov047_021125e0[];
extern char data_ov047_02111b54[];
}

/* Keep this definition before the class header. mwcc emits these dedicated
 * data sections in declaration order, and retail places this descriptor
 * before the class type name, SpawnInfo, and vtable storage. */
extern "C" ResourceDescriptor data_ov047_02112258 = {
    data_ov047_021125e8,
    data_ov047_021125e0,
    data_ov047_02111b54
};

#include "daObjKm3_Kurumajiku_c.h"

struct Km3SpawnInfo {
    daObjKm3_Kurumajiku_c *(*spawn)();
    s16 behaviorPriority;
    s16 renderPriority;
    u32 flags;
    Fix12i rangeOffsetY;
    Fix12i range;
    Fix12i drawDistance;
    u32 unk_18;
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

/* ROM ordinal 4 -- RickshawBs_Spawn, 0x02111280, size 0x3c */
// @symbol RickshawBs_Spawn
/* The object table calls this historical name, but its two vptr stores prove
 * that it constructs daObjKm3_Kurumajiku_c. */
extern "C" daObjKm3_Kurumajiku_c *RickshawBs_Spawn()
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

extern "C" Km3SpawnInfo RickshawBs_SpawnInfo = {
    RickshawBs_Spawn,
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN21daObjKm3_Kurumajiku_cD1Ev, 0x021111a0, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN21daObjKm3_Kurumajiku_cD1Ev
/* The compiler emits D2, D0 and D1 from this one class definition. */
daObjKm3_Kurumajiku_c::~daObjKm3_Kurumajiku_c()
{
}
