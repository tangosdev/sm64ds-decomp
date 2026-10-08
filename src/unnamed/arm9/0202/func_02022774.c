#include "types.h"
/* data_0209ee74 is a global SysTracker*, so it IS the base pointer.
   Each access reloads the global since it's not cached. */
extern char* data_0209ee74;

extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 uniqueID, u32 effectID,
    Fix12i x, Fix12i y, Fix12i z,
    const void* dir, void* callback);

void func_02022774(Fix12i x, Fix12i y, Fix12i z, s16 ang1, s16 ang2)
{
    *(s16*)(data_0209ee74 + 0x7da) = ang1;
    *(s16*)(data_0209ee74 + 0x7dc) = ang2;
    *(u32*)(data_0209ee74 + 0x7d0) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(data_0209ee74 + 0x7d0),
        0x4f,
        x, y, z,
        (void*)0,
        data_0209ee74 + 0x7d4);
}
