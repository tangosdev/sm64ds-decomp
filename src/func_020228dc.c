#include "types.h"
extern char* PARTICLE_SYS_TRACKER;

extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 uniqueID, u32 effectID,
    Fix12i x, Fix12i y, Fix12i z,
    const void* dir, void* callback);

u32 func_020228dc(Fix12i x, Fix12i y, Fix12i z)
{
    char* base = PARTICLE_SYS_TRACKER;
    u32 result = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(base + 0x7a4),
        0x117,
        x, y, z,
        (void*)0,
        base + 0x7a8);
    *(u32*)(PARTICLE_SYS_TRACKER + 0x7a4) = result;
    return result;
}
