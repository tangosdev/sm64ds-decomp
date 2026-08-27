#include "types.h"
// @symbol _ZN8Particle19SetSelfDestructFlagEj
/* recovered: named members + shared header */
#include "Particle.h"
extern struct Particle* data_0209ee74;
void _ZN8Particle19SetSelfDestructFlagEj(u32 idx)
{
    char* a = (char*)data_0209ee74->mManager;
    a = *(char**)(a + 0x1c);
    u32* o = *(u32**)(a + idx * 0x20);
    *o |= 0x4000;
}
