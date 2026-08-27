// @symbol func_ov002_020c9a04
/* recovered: shared common types */
#include "common.h"
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void *c, unsigned int a, int b, int f, unsigned int g);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);

void func_ov002_020c9a04(char *c)
{
    struct Vector3 v;

    *(unsigned char *)(c + 0x6e3) = 0x11;
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x9a, 0x40000000, 0x1000, 0);

    *(int *)(c + 0x98) = 0;
    *(int *)(c + 0xa8) = 0;

    v.x = *(int *)(c + 0x5c);
    v.y = *(int *)(c + 0x60);
    v.z = *(int *)(c + 0x64);
    v.y = *(int *)(c + 0x60) + 0x28000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x5f, v.x, v.y, v.z);

    v.y = *(int *)(c + 0x60) + (int)(((long long)*(int *)(c + 0x84) * 0x50000 + 0x800) >> 12);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x60, v.x, v.y, *(volatile int *)&v.z);
}
