#include "types.h"
// @symbol _ZN8Particle6System9NewRippleE5Fix12IiES2_S2_
#include "Particle.h"
extern struct Particle* data_0209ee74;

extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 uniqueID, u32 effectID,
    Fix12i x, Fix12i y, Fix12i z,
    const void* dir, void* callback);

u32 _ZN8Particle6System9NewRippleE5Fix12IiES2_S2_(Fix12i x, Fix12i y, Fix12i z)
{
    struct Particle* tracker = data_0209ee74;
    u32 result = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        tracker->mRippleSystemID,
        0x109,
        x, y, z,
        (void*)0,
        (void*)&tracker->mRippleCallback);
    data_0209ee74->mRippleSystemID = result;
    return result;
}
