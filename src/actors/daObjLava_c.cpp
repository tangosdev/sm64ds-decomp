//cpp
/**
 * Popping lava bubbles.
 *
 * No model. InitResources succeeds with nothing to load. Behavior
 * (re)issues particle 0xb7 at the closest player's position every
 * frame, feeding last frame's handle back so the emitter is recycled.
 *
 * daObjLava_c_classInit is reconstructed (RTTI daObjLava_c, LAVA
 * registry). Retail does not store that spelling.
 *
 * deslop
 * Leftover: particle call is still func_02022c3c, a 6-arg wrapper
 *   around Particle::System::New that plants the default callback
 *   at data_0209ee74+0x808.
 */

#include "daObjLava_c.h"
#include "Player.h"

enum {
    kLavaBubbleEffect = 0xb7
};

extern "C" u32 func_02022c3c(u32 handle, u32 effectID,
                             Fix12i x, Fix12i y, Fix12i z, const void *dir);

// @symbol daObjLava_c_classInit
extern "C" daObjLava_c *daObjLava_c_classInit()
{
    return new daObjLava_c();
}

struct LavaSpawnInfo {
    daObjLava_c *(*classInit)();
    s16 executePriority; /* +4: also LAVA registry id 0x00c4 = 196 */
    s16 renderPriority;  /* +6 */
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};
typedef char LavaSpawnInfo_size_must_be_0x1c[sizeof(LavaSpawnInfo) == 0x1c ? 1 : -1];

// @symbol g_profile_LAVA
extern "C" LavaSpawnInfo g_profile_LAVA = {
    daObjLava_c_classInit,
    0x00c4,
    0x0014,
    0,
    0,
    0x1000,
    0,
    0
};

// @symbol _ZN11daObjLava_c13InitResourcesEv
s32 daObjLava_c::InitResources()
{
    return 1;
}

// @symbol _ZN11daObjLava_c8BehaviorEv
s32 daObjLava_c::Behavior()
{
    /* The ROM forms &player->mPosX once and loads all three words off it, which
     * is what taking the position as a Vector3 does; three separate member
     * loads off the Player pointer keep the 0x5c/0x60/0x64 offsets instead.
     * Keep this spelling until dActor_c::Pos() is on the shared header. */
    Vector3 *pos = (Vector3 *)&ClosestPlayer()->mPosX;
    mEffectHandle = func_02022c3c(mEffectHandle, kLavaBubbleEffect,
        pos->x, pos->y, pos->z, 0);
    return 1;
}

// @symbol _ZN11daObjLava_cD0Ev
/* D0 is the DELETING destructor: destroy through this class and its bases, then
 * return the object to its heap. The deallocation is an inline operator delete,
 * which is why no heap call appears in the retail bytes. Neither destructor is
 * defined here: the body is inline in the class header, and this block records
 * only the ROM ordinal the manifest licenses. */

// @symbol _ZN11daObjLava_cD1Ev
/* D1 is the complete-object destructor: store this class's vtable over the one
 * the base constructor left, then run the dActor_c subobject destructor. This
 * class adds no member with a non-trivial destructor, which is why D1 is only
 * 0x24 bytes. Inline in the header; this block records only the ROM ordinal. */
