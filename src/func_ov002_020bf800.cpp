//cpp
typedef unsigned int u32;
typedef short s16;

struct Fix12 { int v; Fix12(int a) : v(a) {} };
struct Vector3_16f { s16 x, y, z; };

extern "C" {
unsigned int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int a, unsigned int b, int c, int d, int e, void const *f, void *g);
}

extern "C" void func_ov002_020bf800(char *c, Vector3_16f v)
{
    int xv = *(int *)(c + 0x5c) - v.x * 0x2bc;
    int zv = *(int *)(c + 0x64) - v.z * 0x2bc;
    int yv = *(int *)(c + 0x60) - v.y * 0x2bc;
    Fix12 a(xv), b(yv), d(zv);
    *(u32 *)(c + 0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32 *)(c + 0x628), 0xe7, *(int *)&a, *(int *)&b, *(int *)&d, &v, 0);
}
