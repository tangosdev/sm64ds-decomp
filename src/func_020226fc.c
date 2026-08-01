#include "types.h"
struct ParticleCallback;
struct ParticleSys;

extern struct ParticleSys* volatile data_0209ee74; /* 0x0209ee74 */

extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 slot, u32 unk, Fix12i x, Fix12i y, Fix12i z, void* rot, struct ParticleCallback* callback);

void func_020226fc(Fix12i x, Fix12i y, Fix12i z, s16 arg3, s16 arg4)
{
    struct ParticleSys* sys1;
    struct ParticleSys* sys2;
    struct ParticleSys* sys3;
    struct ParticleSys* sys4;
    u32 result;

    sys1 = data_0209ee74;
    *(s16*)((u8*)sys1 + 0x7ea) = arg3;
    sys2 = data_0209ee74;
    *(s16*)((u8*)sys2 + 0x7ec) = arg4;
    sys3 = data_0209ee74;
    result = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)((u8*)sys3 + 0x7e0),
        0x50,
        x, y, z,
        0,
        (struct ParticleCallback*)((u8*)sys3 + 0x7e4));
    sys4 = data_0209ee74;
    *(u32*)((u8*)sys4 + 0x7e0) = result;
}
