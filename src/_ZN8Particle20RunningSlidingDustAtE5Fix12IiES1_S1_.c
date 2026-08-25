#include "types.h"
// @symbol _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Particle.h"
extern struct Particle* data_0209ee74;

extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 uniqueID, u32 effectID,
    Fix12i x, Fix12i y, Fix12i z,
    const void* dir, void* callback);

void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(Fix12i x, Fix12i y, Fix12i z)
{
    data_0209ee74->mRunningSlidingDustSystemID =
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            data_0209ee74->mRunningSlidingDustSystemID,
            0xda,
            x, y, z,
            (void*)0,
            (void*)&data_0209ee74->mRunningSlidingDustCallback);
}
