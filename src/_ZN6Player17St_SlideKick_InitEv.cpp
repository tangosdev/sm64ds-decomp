//cpp
// @symbol _ZN6Player17St_SlideKick_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int, const Vector3&);
extern "C" int RandomIntInternal(int* seed);
extern "C" void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int, unsigned int, const Vector3&);
extern "C" void _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, int, unsigned int);
extern int data_ov002_0210e160;

int Player::St_SlideKick_Init()
{
    unsigned char* p = (unsigned char*)((void*)this);
    _ZN5Sound9PlayBank0EjRK7Vector3(0x11, *(Vector3*)(p + 0x74));
    unsigned int r = (unsigned int)RandomIntInternal(&data_ov002_0210e160);
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(p[0x6d9], data_ov002_020ff130[(r >> 4) & 1], *(Vector3*)(p + 0x74));
    _ZN6Player7SetAnimEji5Fix12IiEj(((void*)this), 0x67, 0x40000000, 0x1000, 0);
    int* yv = (int*)(((int)p + 0x98));
    *(int*)(p + 0xa8) = 0x14000;
    yv[0] += 0xf000;
    if (*(int*)(p + 0x98) > 0x28000) *(int*)(p + 0x98) = 0x28000;
    *(unsigned char*)(p + 0x6e5) = 0;
    *(unsigned char*)(p + 0x6e4) = 0;
    *(unsigned char*)(p + 0x6de) = 1;
    *(unsigned char*)(p + 0x6df) = 0;
    return 1;
}
