// @symbol func_ov060_021185c4
/* recovered: shared common types */
#include "common.h"
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void func_02012694(int a, void* b);
extern void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void* self, struct Vector3* v, int f);
extern void func_ov060_021184bc(void* c);

void func_ov060_021185c4(char* c)
{
    struct Vector3 v;
    *(int*)(c + 0x170) = 1;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xa8, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xa9, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xaa, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xab, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xac, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
    func_02012694(0x2f, c + 0x74);
    v.x = *(int*)(c + 0x5c);
    v.y = *(int*)(c + 0x60);
    v.z = *(int*)(c + 0x64);
    _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(c, &v, 0x7d0000);
    *(short*)(c + 0x1ac) = 0;
    func_ov060_021184bc(c);
}
