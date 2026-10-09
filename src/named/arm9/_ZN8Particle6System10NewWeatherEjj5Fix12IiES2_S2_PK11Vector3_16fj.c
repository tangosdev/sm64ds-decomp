#include "types.h"
// @symbol _ZN8Particle6System10NewWeatherEjj5Fix12IiES2_S2_PK11Vector3_16fj
#include "Particle.h"
extern struct Particle* data_0209ee74;

extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 uniqueID, u32 effectID,
    Fix12i x, Fix12i y, Fix12i z,
    const void* dir, void* callback);

u32 _ZN8Particle6System10NewWeatherEjj5Fix12IiES2_S2_PK11Vector3_16fj(
    u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, const void* dir, u8 numWeatherEffectsNow)
{
    char* callback = (char*)&data_0209ee74->mWeatherCallback;
    *(u8*)(callback + 4) = numWeatherEffectsNow;
    return _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        uniqueID, effectID, x, y, z, dir, (void*)callback);
}
