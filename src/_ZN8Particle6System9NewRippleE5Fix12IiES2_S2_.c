#include "types.h"
extern char* PARTICLE_SYS_TRACKER;

extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 uniqueID, u32 effectID,
    Fix12i x, Fix12i y, Fix12i z,
    const void* dir, void* callback);

u32 _ZN8Particle6System9NewRippleE5Fix12IiES2_S2_(Fix12i x, Fix12i y, Fix12i z)
{
    char* base = PARTICLE_SYS_TRACKER;
    u32 result = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(base + 0x78c),
        0x109,
        x, y, z,
        (void*)0,
        base + 0x790);
    *(u32*)(PARTICLE_SYS_TRACKER + 0x78c) = result;
    return result;
}
