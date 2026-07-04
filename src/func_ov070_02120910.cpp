//cpp
extern "C" {
struct Vector3 { int x, y, z; };
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int n, const Vector3& v);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, void* f, int a, int b, unsigned int e);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int n, int a, int b, int c);
extern int data_ov070_0212360c[];

int func_ov070_02120910(char* c)
{
    _ZN5Sound9PlayBank0EjRK7Vector3(9, *(Vector3*)(c + 0x74));
    *(int*)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFF) &= ~1;
    *(int*)(c + 0x9c) = -0x2000;
    *(int*)(c + 0xa0) = -0x3c000;
    *(int*)(c + 0x5c) = *(int*)(c + 0x404);
    *(int*)(c + 0x60) = *(int*)(c + 0x408);
    *(int*)(c + 0x64) = *(int*)(c + 0x40c);
    *(int*)(c + 0x98) = 0xa000;
    *(int*)(c + 0xa8) = 0x28000;
    *(unsigned char*)(c + 0x430) = 0x2d;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, *(void**)((char*)data_ov070_0212360c + 4), 0x40000000, 0x1000, 0);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x43, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x44, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
    *(int*)(c + 0x420) = 2;
    return 1;
}
}
