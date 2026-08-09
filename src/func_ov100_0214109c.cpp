//cpp
// @symbol func_ov100_0214109c
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
typedef int Fix12;


extern "C" {
extern int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned a, unsigned b, const struct Vector3 *pos, const struct Vector3_16 *rot, int i, int j);
extern void _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(void *c, Fix12 a, Fix12 b, Fix12 d, Fix12 e);

void func_ov100_0214109c(void *t) {
    char *c = (char*)t;
    int st = *(int*)(c+0x3e8);
    if (st == 0x14) {
        if (*(unsigned char*)(c+0x3f0) != 1) {
            *(int*)(c+0x3e0) = 0;
            *(unsigned char*)(c+0x3f1) = 0;
            return;
        }
        int sub = *(signed char*)(c+0xcc);
        _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(0x114, 0, (struct Vector3*)(c+0x5c), 0, sub, -1);
        ActorBase_MarkForDestruction(c);
        return;
    }
    if (st <= 0x14) return;
    (*(int *)(((int)c + 0x3e0))) += 0x40;
    if (*(int*)(c+0x3e0) < 0x800) return;
    _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(c, 0x32000, 0x32000, 0x1000000, 0x320000);
    *(short*)(c+0x3ee) = 0;
    *(int*)(c+0x3e8) = 0;
    *(int*)(c+0x3e4) = 7;
}
}
