#include "types.h"
extern char* PARTICLE_SYS_TRACKER;

extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 uniqueID, u32 effectID,
    Fix12i x, Fix12i y, Fix12i z,
    const void* dir, void* callback);

void func_02022b04(Fix12i x, Fix12i y, Fix12i z)
{
    *(u32*)(PARTICLE_SYS_TRACKER + 0x75c) =
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(u32*)(PARTICLE_SYS_TRACKER + 0x75c),
            0x61,
            x, y, z,
            (void*)0,
            (void*)(PARTICLE_SYS_TRACKER + 0x760));
}
