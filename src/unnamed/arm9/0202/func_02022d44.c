#include "types.h"
/* func_02022d44 at 0x02022d44 (60 bytes)
 * Particle::System::New wrapper -- passes args through and adds *tracker+0x7f0 as callback.
 */
struct ParticleCallback;
struct ParticleSys;

extern struct ParticleSys *data_0209ee74; /* 0x0209ee74 */

extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z,
    void *dir, struct ParticleCallback *callback); /* 0x02022d80 */

u32 func_02022d44(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, void *dir)
{
    return _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        uniqueID, effectID, x, y, z, dir,
        (struct ParticleCallback *)((u8 *)data_0209ee74 + 0x7f0));
}
