#include "types.h"
extern char* data_0209ee74;

extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 uniqueID, u32 effectID,
    Fix12i x, Fix12i y, Fix12i z,
    const void* dir, void* callback);

void func_02022b04(Fix12i x, Fix12i y, Fix12i z)
{
    *(u32*)(data_0209ee74 + 0x75c) =
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(u32*)(data_0209ee74 + 0x75c),
            0x61,
            x, y, z,
            (void*)0,
            (void*)(data_0209ee74 + 0x760));
}
