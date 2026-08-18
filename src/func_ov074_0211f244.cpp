//cpp
// @symbol func_ov074_0211f244
/* recovered: shared common types */
#include "common.h"
struct dActor_c { static dActor_c *FindWithActorID(unsigned int id, dActor_c *after); };
extern "C" int RandomIntInternal(int *seed);
extern "C" int data_0209e650;


extern "C" void func_ov074_0211f244(dActor_c *self)
{
    dActor_c *a = dActor_c::FindWithActorID(0xc7, 0);
    if (a == 0) return;
    do {
        if (a != self) {
            char *t = (char*)a;
            *(int*)(t + 0x9c) = -0xa000;
            int rv = (unsigned int)RandomIntInternal(&data_0209e650) >> 8;
            *(short*)(t + 0x5fc) = 0x30;
            *(int*)(t + 0x98) = ((rv & 0xf) << 0xc) + 0x16000;
            *(int*)(t + 0xa4) = 0;
            *(int*)(t + 0xa8) = ((rv & 0x1f) << 0xc) + 0x60000;
            *(int*)(t + 0xac) = 0;
            Vector3_16 pos = *(Vector3_16*)(t + 0x92);
            rv &= 0x1000;
            pos.y = (short)(pos.y + rv);
            short x = pos.x + 0;
            *(short*)(t + 0x92) = x;
            short y = pos.y + 0;
            *(short*)(t + 0x94) = y;
            short z = pos.z + 0;
            *(short*)(t + 0x96) = z;
            *(short*)(t + 0x8c) = x;
            *(short*)(t + 0x8e) = y;
            *(short*)(t + 0x90) = z;
            *(unsigned char*)(t + 0x609) = 1;
        }
        a = dActor_c::FindWithActorID(0xc7, a);
    } while (a != 0);
}
