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
    dActor_c *player = (dActor_c *)ClosestPlayer();
    const Vector3 &pos = player->Pos();
    mEffectHandle = func_02022c3c(mEffectHandle, kLavaBubbleEffect,
        pos.x, pos.y, pos.z, 0);
    return 1;
}
