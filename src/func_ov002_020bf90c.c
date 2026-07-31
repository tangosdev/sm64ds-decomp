#include "types.h"
extern void func_02022b04(Fix12i x, Fix12i y, Fix12i z);
extern void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(Fix12i x, Fix12i y, Fix12i z);
extern int func_0201226c(int a0, int a1, int a2, int a3, int a4, short a5);
extern int data_0208e430;

void func_ov002_020bf90c(char* c)
{
    if (*(int*)(c + 0x98) == 0) return;
    if (*(u8*)(c + 0x6de) != 0) return;
    if (*(u8*)(c + 0x707) != 0) return;
    if (data_0208e430 == 0x2e) return;

    if (*(u8*)(c + 0x703) != 0)
        func_02022b04(*(Fix12i*)(c + 0x5c), *(Fix12i*)(c + 0x60), *(Fix12i*)(c + 0x64));
    else
        _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(*(Fix12i*)(c + 0x5c), *(Fix12i*)(c + 0x60), *(Fix12i*)(c + 0x64));

    *(int*)(c + 0x620) = func_0201226c(
        *(int*)(c + 0x620),
        0,
        *(int*)(c + 0x66c) + 0xe2,
        (int)(c + 0x74),
        *(int*)(c + 0x98),
        0);
}
