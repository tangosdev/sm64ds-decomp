//cpp
typedef int Fix12i;
struct Vector3 { int x, y, z; };
extern "C" unsigned _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned a, unsigned b, Fix12i c, Fix12i d, Fix12i e, void* f, void* g);
extern "C" unsigned _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(unsigned a, unsigned b, Fix12i c, Fix12i d, Fix12i e, void* f);
extern "C" void ApproachAngle(short* v, int a, int b, int c, int d);
extern "C" void _Z14ApproachLinearRsss(short* v, int a, int b);
extern "C" int func_ov070_0211f0a4(void* c);

extern "C" int func_ov070_0211f368(char* c)
{
    if (*(int*)(c + 0x3d8) != 0) {
        Vector3 v;
        int x, y, z;
        x = *(int*)(c + 0x5c);
        z = *(int*)(c + 0x64);
        y = *(int*)(c + 0x60) + 0x50000;
        ((int*)&v)[0] = x;
        ((int*)&v)[1] = y;
        ((int*)&v)[2] = z;
        *(unsigned*)(c + 0x3d0) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(unsigned*)(c + 0x3d0), 0x13a, v.x, v.y, v.z, 0, 0);
        *(unsigned*)(c + 0x3d4) = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
            *(unsigned*)(c + 0x3d4), 0x13b, v.x, v.y, v.z, 0);
    }
    ApproachAngle((short*)(c + 0x8c), -0x4000, 0xa, 0x200, 0x100);
    _Z14ApproachLinearRsss((short*)(c + 0x8c), -0x4000, 0x200);
    if (*(unsigned short*)(c + 0x100) == 0)
        func_ov070_0211f0a4(c);
    return 1;
}
