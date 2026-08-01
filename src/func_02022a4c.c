#include "types.h"
extern char* data_0209ee74;

extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 uniqueID, u32 effectID,
    Fix12i x, Fix12i y, Fix12i z,
    const void* dir, void* callback);

u32 func_02022a4c(Fix12i x, Fix12i y, Fix12i z)
{
    char* base = data_0209ee74;
    u32 result = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(base + 0x774),
        0xdf,
        x, y, z,
        (void*)0,
        base + 0x76c);
    *(u32*)(data_0209ee74 + 0x774) = result;
    return result;
}
