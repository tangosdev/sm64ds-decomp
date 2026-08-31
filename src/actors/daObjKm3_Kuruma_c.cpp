//cpp
/* Manually curated production translation unit.
 * ov047/daObjKm3_Kuruma_c  (5 function(s))
 *
 * One intact mwccarm object owns the five retail text contributions together
 * with this class's RTTI, type-name string, descriptor, SpawnInfo, and vtable.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in reverse source order. The destructor
 * variants are the documented compiler-chosen exception.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x021113f8  src/_ZN17daObjKm3_Kuruma_cD1Ev.cpp
 *   [1] 0x02111448  src/_ZN17daObjKm3_Kuruma_cD0Ev.cpp
 *   [2] 0x021114ac  src/_ZN17daObjKm3_Kuruma_c16CleanupResourcesEv.cpp
 *   [3] 0x021114c0  src/_ZN17daObjKm3_Kuruma_c13InitResourcesEv.cpp
 *   [4] 0x021114d4  src/RickshawPlatformBs_Spawn.c
 */

struct ResourceDescriptor {
    void *entries[3];
};

extern "C" {
extern char data_ov047_02112638[];
extern char data_ov047_02112630[];
extern char data_ov047_02111ab4[];
}

/* Retail places this descriptor between the class RTTI and type-name records.
 * Defining it before the class header preserves that declaration order. */
extern "C" ResourceDescriptor data_ov047_02112408 = {
    data_ov047_02112638,
    data_ov047_02112630,
    data_ov047_02111ab4
};

#include "daObjKm3_Kuruma_c.h"

struct KurumaSpawnInfo {
    daObjKm3_Kuruma_c *(*spawn)();
    s16 behaviorPriority;
    s16 renderPriority;
    u32 flags;
    Fix12i rangeOffsetY;
    Fix12i range;
    Fix12i drawDistance;
    u32 unk_18;
};

typedef char KurumaSpawnInfo_size_must_be_0x1c[
    sizeof(KurumaSpawnInfo) == 0x1c ? 1 : -1];

extern "C" {
extern void *_ZN7fBase_cnwEj(unsigned size);
extern void _ZN10dBgActor_cC2Ev(void *self);
extern int _ZTV13daObjKuruma_c[];
extern int _ZTV17daObjKm3_Kuruma_c[];
int func_ov002_020b68b0(daObjKm3_Kuruma_c *self,
                        ResourceDescriptor *descriptor);
int func_ov002_020b6958(daObjKm3_Kuruma_c *self,
                        ResourceDescriptor *descriptor);
}

/* ROM ordinal 4 -- RickshawPlatformBs_Spawn */
// @symbol RickshawPlatformBs_Spawn
extern "C" daObjKm3_Kuruma_c *RickshawPlatformBs_Spawn()
{
    daObjKm3_Kuruma_c *actor =
        static_cast<daObjKm3_Kuruma_c *>(_ZN7fBase_cnwEj(0x320));

    if (actor) {
        _ZN10dBgActor_cC2Ev(actor);
        *reinterpret_cast<int *>(actor) = (int)_ZTV13daObjKuruma_c;
        *reinterpret_cast<int *>(actor) =
            (int)&_ZTV17daObjKm3_Kuruma_c[2];
    }

    return actor;
}

extern "C" KurumaSpawnInfo RickshawPlatformBs_SpawnInfo = {
    RickshawPlatformBs_Spawn,
    0x0097,
    0x00de,
    2,
    0,
    0x00200000,
    0x01000000,
    0
};

/* ROM ordinal 3 -- _ZN17daObjKm3_Kuruma_c13InitResourcesEv */
// @symbol _ZN17daObjKm3_Kuruma_c13InitResourcesEv
int daObjKm3_Kuruma_c::InitResources()
{
    return func_ov002_020b6958(this, &data_ov047_02112408);
}

/* ROM ordinal 2 -- _ZN17daObjKm3_Kuruma_c16CleanupResourcesEv */
// @symbol _ZN17daObjKm3_Kuruma_c16CleanupResourcesEv
int daObjKm3_Kuruma_c::CleanupResources()
{
    return func_ov002_020b68b0(this, &data_ov047_02112408);
}
