//cpp
// @symbol _ZN11SoundObject13InitResourcesEv

#include "decl_common.h"
#include "SoundObject.h"

#pragma opt_loop_invariants off

extern "C" void _ZN5Sound7PlaySubEjjj5Fix12IiEb(
    u32 a, u32 b, u32 c, Fix12i d, bool e);

int SoundObject::InitResources()
{
    dActor_c *actor;
    int g;

    if (param1 > 7)
        return 0;

    mLevelID = *(s32 *)((char *)data_ov002_0210c080 + param1 * 0xc);
    mTimerThreshold = *(s32 *)((char *)data_ov002_0210c084 + param1 * 0xc);
    mTimerReset = *(u16 *)((char *)data_ov002_0210c088 + param1 * 0xc);
    unk_0e0 = *(u8 *)((char *)data_ov002_0210c08a + param1 * 0xc);

    actor = 0;
    g = data_0208e430;

    if (g == 0x20 || g == 0x29 || g == 0x21 || g == 0x1e || g == 0x50 ||
        (g >= 0x19 && g <= 0x1d) || g == 0x4f || g == 0x22 || g == 0x2a || g == 0x21)
    {
        while (1)
        {
            actor = FindWithActorID(0x167, actor);
            if (actor == 0)
                break;
            if (actor != this)
            {
                actor->MarkForDestruction();
                _ZN5Sound7PlaySubEjjj5Fix12IiEb(g, 0x7f, 0, 0x7f000, 0);
            }
        }
    }

    mTimer = 0;
    mAreaId = -1;
    return 1;
}
