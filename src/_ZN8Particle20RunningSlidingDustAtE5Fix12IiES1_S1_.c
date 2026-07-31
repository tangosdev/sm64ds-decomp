// @symbol _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Particle.h"
typedef signed int s32;
typedef unsigned int u32;
typedef s32 Fix12i;


extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 uniqueID, u32 effectID,
    Fix12i x, Fix12i y, Fix12i z,
    const void* dir, void* callback);

void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(Fix12i x, Fix12i y, Fix12i z)
{
    *(u32*)(PARTICLE_SYS_TRACKER + 0x750) =
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(u32*)(PARTICLE_SYS_TRACKER + 0x750),
            0xda,
            x, y, z,
            (void*)0,
            (void*)(PARTICLE_SYS_TRACKER + 0x754));
}
