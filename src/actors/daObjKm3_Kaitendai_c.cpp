//cpp
/* Manually curated production translation unit.
 * ov047/daObjKm3_Kaitendai_c  (5 function(s))
 *
 * One intact mwccarm object owns the five retail text contributions together
 * with this class's RTTI, type-name string, descriptors, and vtable storage.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in reverse source order. The destructor
 * variants are the documented compiler-chosen exception.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x021112bc  src/_ZN20daObjKm3_Kaitendai_cD1Ev.cpp
 *   [1] 0x0211130c  src/_ZN20daObjKm3_Kaitendai_cD0Ev.cpp
 *   [2] 0x02111370  src/_ZN20daObjKm3_Kaitendai_c16CleanupResourcesEv.cpp
 *   [3] 0x02111384  src/_ZN20daObjKm3_Kaitendai_c13InitResourcesEv.cpp
 *   [4] 0x021113bc  src/func_ov047_021113bc.c
 */

struct ResourceDescriptor {
    void *entries[3];
};

extern "C" {
extern char data_ov047_02112610[];
extern char data_ov047_02112608[];
extern char data_ov047_02111bf4[];
}

/* Keep this definition before the class header. mwcc emits these dedicated
 * data sections in declaration order, and retail places this descriptor
 * before the class type name, SpawnInfo, and vtable storage. */
extern "C" ResourceDescriptor data_ov047_02112334 = {
    data_ov047_02112610,
    data_ov047_02112608,
    data_ov047_02111bf4
};

#include "daObjKm3_Kaitendai_c.h"

struct KaitendaiSpawnInfo {
    daObjKm3_Kaitendai_c *(*spawn)();
    s16 behaviorPriority;
    s16 renderPriority;
    u32 flags;
    Fix12i rangeOffsetY;
    Fix12i range;
    Fix12i drawDistance;
    u32 unk_18;
};

typedef char KaitendaiSpawnInfo_size_must_be_0x1c[
    sizeof(KaitendaiSpawnInfo) == 0x1c ? 1 : -1];

extern "C" {
extern void *_ZN7fBase_cnwEj(unsigned size);
extern void _ZN10dBgActor_cC2Ev(void *self);
extern int _ZTV16daObjKaitendai_c[];
extern int _ZTV20daObjKm3_Kaitendai_c[];
int func_ov002_020b66a8(daObjKm3_Kaitendai_c *self,
                        ResourceDescriptor *descriptor);
int func_ov002_020b676c(daObjKm3_Kaitendai_c *self,
                        ResourceDescriptor *descriptor, s16 value);
extern s16 data_ov047_02112320;
extern s16 data_ov047_02112324;
}

/* ROM ordinal 4 -- func_ov047_021113bc, 0x021113bc, size 0x3c */
// @symbol func_ov047_021113bc
/* This function is still unnamed in the object table, but its allocation
 * size and vptr stores prove that it constructs daObjKm3_Kaitendai_c. */
extern "C" daObjKm3_Kaitendai_c *func_ov047_021113bc()
{
    daObjKm3_Kaitendai_c *actor =
        static_cast<daObjKm3_Kaitendai_c *>(_ZN7fBase_cnwEj(0x320));

    if (actor) {
        _ZN10dBgActor_cC2Ev(actor);
        *reinterpret_cast<int *>(actor) = (int)_ZTV16daObjKaitendai_c;
        *reinterpret_cast<int *>(actor) =
            (int)&_ZTV20daObjKm3_Kaitendai_c[2];
    }

    return actor;
}

extern "C" KaitendaiSpawnInfo data_ov047_02112358 = {
    func_ov047_021113bc,
    0x009c,
    0x00e3,
    2,
    0,
    0x00380000,
    0x02000000,
    0
};

/* ROM ordinal 3 -- _ZN20daObjKm3_Kaitendai_c13InitResourcesEv */
// @symbol _ZN20daObjKm3_Kaitendai_c13InitResourcesEv
/* Selects one of the two spawn values, then delegates setup with this class's
 * three-word resource descriptor. */
int daObjKm3_Kaitendai_c::InitResources()
{
    s16 value = data_ov047_02112320;

    if ((param1 & 0xff) == 1) {
        value = data_ov047_02112324;
    }

    return func_ov002_020b676c(this, &data_ov047_02112334, value);
}

/* ROM ordinal 2 -- _ZN20daObjKm3_Kaitendai_c16CleanupResourcesEv */
// @symbol _ZN20daObjKm3_Kaitendai_c16CleanupResourcesEv
int daObjKm3_Kaitendai_c::CleanupResources()
{
    return func_ov002_020b66a8(this, &data_ov047_02112334);
}
