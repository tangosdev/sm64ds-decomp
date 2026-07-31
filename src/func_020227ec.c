#include "types.h"
extern char* PARTICLE_SYS_TRACKER;

extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 uniqueID, u32 effectID,
    Fix12i x, Fix12i y, Fix12i z,
    const void* dir, void* callback);

void func_020227ec(Fix12i x, Fix12i y, Fix12i z, s16 ang1, s16 ang2)
{
    *(s16*)(PARTICLE_SYS_TRACKER + 0x7ca) = ang1;
    *(s16*)(PARTICLE_SYS_TRACKER + 0x7cc) = ang2;
    *(u32*)(PARTICLE_SYS_TRACKER + 0x7c0) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(PARTICLE_SYS_TRACKER + 0x7c0),
        0x52,
        x, y, z,
        (void*)0,
        PARTICLE_SYS_TRACKER + 0x7c4);
}
