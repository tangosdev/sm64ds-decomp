#include "types.h"
// @symbol _ZN8Particle6System12NewBigSplashE5Fix12IiES2_S2_
#include "Particle.h"
extern struct Particle* data_0209ee74;

extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 uniqueID, u32 effectID,
    Fix12i x, Fix12i y, Fix12i z,
    const void* dir, void* callback);

void _ZN8Particle6System12NewBigSplashE5Fix12IiES2_S2_(Fix12i x, Fix12i y, Fix12i z)
{
    data_0209ee74->mBigSplashSystemID =
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            data_0209ee74->mBigSplashSystemID,
            0xdd,
            x, y, z,
            (void*)0,
            (void*)&data_0209ee74->mBigSplashCallback);
}
