// @symbol _ZN8Particle19SetSelfDestructFlagEj
/* recovered: named members + shared header */
#include "Particle.h"
typedef unsigned int u32;
extern char* data_0209ee74;
void _ZN8Particle19SetSelfDestructFlagEj(u32 idx)
{
    char* a = data_0209ee74;
    a = *(char**)(a + 4);
    a = *(char**)(a + 0x1c);
    u32* o = *(u32**)(a + idx * 0x20);
    *o |= 0x4000;
}
