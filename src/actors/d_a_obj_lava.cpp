//cpp
/* ov002/daObjLava_c -- reconstructed translation unit (5 functions).
 *
 * ROM run 0x020b6d28..0x020b6e08, plus the class's .data run at
 * 0x021093a0..0x0210945c (_ZTI, _ZTS, the spawn-info record and _ZTV).
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in the reverse of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x020b6d28  src/_ZN11daObjLava_cD1Ev.cpp
 *   [1] 0x020b6d4c  src/_ZN11daObjLava_cD0Ev.cpp
 *   [2] 0x020b6d84  src/_ZN11daObjLava_c8BehaviorEv.c   -> daObjLava_c::Behavior()
 *   [3] 0x020b6dd0  src/_ZN11daObjLava_c13InitResourcesEv.cpp
 *   [4] 0x020b6dd8  src/daObjLava_c_Spawn.c
 */

#include "daObjLava_c.h"
#include "Player.h"
#include "types.h"

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- daObjLava_c_Spawn, 0x020b6dd8, size 0x30                   */
/* -------------------------------------------------------------------------- */
/* The class factory: the only referent of this actor's spawn-info record, it
 * allocates sizeof(daObjLava_c), runs the dActor_c subobject constructor and
 * then plants this class's vptr over the one the base left.
 *
 * The vptr is `&_ZTV11daObjLava_c[2]`, NOT `(int)_ZTV11daObjLava_c`. The
 * addend-0 spelling is correct only for a TU that IMPORTS its vtable, where the
 * linker's symbol already denotes the address point; this TU EMITS the vtable,
 * so mwcc's own symbol denotes the object start and the +8 bias to the address
 * point has to be written out. */
// @symbol daObjLava_c_Spawn
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void *_ZN7fBase_cnwEj(u32 size);
extern void _ZN8dActor_cC2Ev(void *self);
extern int _ZTV11daObjLava_c[];

int *daObjLava_c_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(216);
    if (p) { _ZN8dActor_cC2Ev(p); p[0] = (int)&_ZTV11daObjLava_c[2]; }
    return p;
}
}

/* The actor profile record, 0x021093bc. Its first word is the only reference to
 * the factory above, which is what makes this TU the owner of the whole
 * 0x021093a0..0x0210945c data run. Same seven-field shape as every other actor
 * profile; the values are read straight from retail. */
struct LavaSpawnInfo {
    int *(*classInit)();
    s16 profileIDAndExecuteOrder;   /* 0x00c4 */
    s16 drawOrder;                  /* 0x0014 */
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;              /* 0x1000 == 1.0 */
    Fix12i clipDistance;
    Fix12i farDistance;
};
typedef char LavaSpawnInfo_size_must_be_0x1c[sizeof(LavaSpawnInfo) == 0x1c ? 1 : -1];

// @symbol daObjLava_c_SpawnInfo
extern "C" LavaSpawnInfo daObjLava_c_SpawnInfo = {
    daObjLava_c_Spawn,
    0x00c4,
    0x0014,
    0,
    0,
    0x1000,
    0,
    0
};

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- daObjLava_c::InitResources, 0x020b6dd0, size 0x8           */
/* -------------------------------------------------------------------------- */
/* Vtable slot 0. Nothing to load: the actor is a pure particle emitter, so the
 * whole body is the success return. */
// @symbol _ZN11daObjLava_c13InitResourcesEv
s32 daObjLava_c::InitResources()
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- daObjLava_c::Behavior, 0x020b6d84, size 0x4c              */
/* -------------------------------------------------------------------------- */
/* Vtable slot 6, carried as the unnamed func_ov002_020b6d84 until now.
 *
 * Every frame it re-issues particle effect 0xb7 -- the lava bubble -- at the
 * closest player's position, feeding last frame's handle back in so the system
 * recycles the same emitter instead of stacking new ones. */
// @symbol _ZN11daObjLava_c8BehaviorEv
extern "C" u32 func_02022c3c(u32 handle, u32 effectID,
                             Fix12i x, Fix12i y, Fix12i z, const void *dir);

s32 daObjLava_c::Behavior()
{
    /* The ROM forms &player->mPosX once and loads all three words off it, which
     * is what taking the position as a Vector3 does; three separate member
     * loads off the Player pointer keep the 0x5c/0x60/0x64 offsets instead. */
    Vector3 *pos = (Vector3 *)&ClosestPlayer()->mPosX;
    mEffectHandle = func_02022c3c(mEffectHandle, 0xb7, pos->x, pos->y, pos->z, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN11daObjLava_cD0Ev, 0x020b6d4c, size 0x38               */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjLava_cD0Ev
/* D0 is the DELETING destructor: destroy through this class and its bases, then
 * return the object to its heap. The deallocation is an inline operator delete,
 * which is why no heap call appears in the retail bytes. */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN11daObjLava_cD1Ev, 0x020b6d28, size 0x24               */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjLava_cD1Ev
/* D1 is the complete-object destructor: store this class's vtable over the one
 * the base constructor left, then run the dActor_c subobject destructor. This
 * class adds no member with a non-trivial destructor, which is why D1 is only
 * 0x24 bytes.
 *
 * Neither destructor is defined here: the body is inline in the class header,
 * and these two blocks record only the ROM ordinals the manifest licenses. See
 * the header for why inline is required rather than merely tidy. */
