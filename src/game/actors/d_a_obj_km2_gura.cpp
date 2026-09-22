//cpp
/* Genuine-TU reconstruction for ov045/daObjKm2_Gura_c.
 *
 * SM64DS preserves the daObjKm2_Gura_c RTTI class and the KM2_GURA registry
 * ID. daObjKm2_Gura_c_classInit, g_profile_KM2_GURA, and this probable
 * Nintendo-style filename are lineage-supported reconstructions. Historical
 * project aliases are TiltingPlatformBfs_Spawn and
 * TiltingPlatformBfs_SpawnInfo.
 *
 * mwccarm emits function sections in reverse source order. Keep the factory
 * first. The inline destructor in the class header emits the retail D1/D0 pair
 * with this class's RTTI and vtable.
 */

#include "daObjKm2_Gura_c.h"

struct GuraResourceDescriptor {
    void *entries[3];
};

extern "C" {
extern char data_ov045_02113220[];
extern char data_ov045_02113228[];
extern char data_ov045_021124f0[];
}

/* Retail places this descriptor between the class typeinfo and type-name
 * records. All three pointer destinations are present in ov045 relocations. */
extern "C" GuraResourceDescriptor data_ov045_02112fdc = {
    data_ov045_02113220,
    data_ov045_02113228,
    data_ov045_021124f0
};

struct GuraSpawnInfo {
    daObjKm2_Gura_c *(*classInit)();
    s16 profileIDAndExecuteOrder;
    s16 drawOrder;
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};

typedef char GuraSpawnInfo_size_must_be_0x1c[
    sizeof(GuraSpawnInfo) == 0x1c ? 1 : -1];

extern "C" {
extern void *_ZN7fBase_cnwEj(unsigned size);
extern void _ZN10dBgActor_cC2Ev(void *self);
int func_ov002_020b60fc(void *self, void *descriptor);
int func_ov002_020b6244(void *self, void *descriptor);
}

extern int _ZTV15daObjGuragura_c[];
extern int _ZTV15daObjKm2_Gura_c[];

/* Reconstructed source-style names. The class, profile ID, factory shape,
 * allocation size, and descriptor relationship are direct ROM evidence; the
 * exact original SM64DS symbol spellings do not survive. */
// @symbol daObjKm2_Gura_c_classInit
extern "C" daObjKm2_Gura_c *daObjKm2_Gura_c_classInit()
{
    daObjKm2_Gura_c *actor =
        static_cast<daObjKm2_Gura_c *>(_ZN7fBase_cnwEj(sizeof(daObjKm2_Gura_c)));

    if (actor) {
        _ZN10dBgActor_cC2Ev(actor);
        *reinterpret_cast<int *>(actor) = (int)_ZTV15daObjGuragura_c;
        *reinterpret_cast<int *>(actor) = (int)&_ZTV15daObjKm2_Gura_c[2];
    }

    return actor;
}

extern "C" GuraSpawnInfo g_profile_KM2_GURA = {
    daObjKm2_Gura_c_classInit,
    0x008d,
    0x00d4,
    2,
    0x00120000,
    0x00200000,
    0x02000000,
    0
};

// @symbol _ZN15daObjKm2_Gura_c13InitResourcesEv
int daObjKm2_Gura_c::InitResources()
{
    return func_ov002_020b6244(this, &data_ov045_02112fdc);
}

// @symbol _ZN15daObjKm2_Gura_c16CleanupResourcesEv
int daObjKm2_Gura_c::CleanupResources()
{
    return func_ov002_020b60fc(this, &data_ov045_02112fdc);
}
