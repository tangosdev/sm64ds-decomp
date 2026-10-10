//cpp
/**
 * Jolly Roger Bay floating plank (`ita` = board).
 *
 * No fields. InitResources hands this overlay's model and collision
 * files to daObjFloatBoard_c's shared ov002 helper. CleanupResources
 * is the base's.
 *
 * daObjKi_Ita_c_classInit / g_profile_KI_ITA are reconstructed (RTTI
 * daObjKi_Ita_c, KI_ITA registry). Retail does not store those spellings.
 *
 * deslop
 * Leftover: data_ov016_02114b8c is still the linker name of this
 *   overlay's three-word file table. Those words sit between this
 *   class's RTTI and type-name; this TU does not own them.
 */

#include "daObjKi_Ita_c.h"
#include "SharedFilePtr.h"

extern "C" {
extern daObjFloatBoard_c_Resources data_ov016_02114b8c;
}

/* File 1601 / 1602 resource handles. The static initializer registers
 * their destructors; the spellings are this TU's, mapped onto
 * func_02017acc / func_02017ab4 and func_02017b4c /
 * SharedFilePtr_Destruct_Clsn. */
struct KiItaModelFilePtr : SharedFilePtr {
    u32 words[2];

    KiItaModelFilePtr(u32 fileID);
    ~KiItaModelFilePtr();
};

struct KiItaCollisionFilePtr : SharedFilePtr {
    u32 words[2];

    KiItaCollisionFilePtr(u32 fileID);
    ~KiItaCollisionFilePtr();
};

struct KiItaSpawnInfo {
    daObjKi_Ita_c *(*classInit)();
    s16 executePriority; /* +4: also KI_ITA registry id 0x003c = 60 */
    s16 renderPriority;  /* +6 */
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
    0x003c,
    0x00b8,
    2,
    0,
    0x00250000,
    0x02000000,
    0
};

// @symbol _ZN13daObjKi_Ita_c13InitResourcesEv
int daObjKi_Ita_c::InitResources()
{
    return func_ov002_020b5e58(&data_ov016_02114b8c);
}

/* Definitions stay after the last .text function so they do not insert
 * a function into the ROM-ascending run. Construction order is the model
 * handle, then the collision handle. */
KiItaModelFilePtr data_ov016_02114e44(1601);
KiItaCollisionFilePtr data_ov016_02114e4c(1602);
