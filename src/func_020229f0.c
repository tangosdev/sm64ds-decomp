#include "types.h"
extern char* data_0209ee74;

extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 uniqueID, u32 effectID,
    Fix12i x, Fix12i y, Fix12i z,
    const void* dir, void* callback);

u32 func_020229f0(Fix12i x, Fix12i y, Fix12i z)
{
    char* base = data_0209ee74;
    u32 result = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(base + 0x780),
        0xc4,
        x, y, z,
        (void*)0,
        base + 0x784);
    *(u32*)(data_0209ee74 + 0x780) = result;
    return result;
}
