#include "types.h"
struct ParticleCallback;
extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 slot, u32 unk, Fix12i x, Fix12i y, Fix12i z, void* rot, struct ParticleCallback* callback);

void func_ov002_020f2790(char* c, int p1, int p2, int p3, s16 p4)
{
    *(int*)(c+0x508) = p3;
    *(s16*)(c+0x50c) = p4;
    *(u32*)(c+0x1fc) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(c+0x1fc),
        0x3e,
        p1<<12, p2<<12, 0,
        0,
        (struct ParticleCallback*)(c+0x200));
}
