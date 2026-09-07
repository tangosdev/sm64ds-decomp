//cpp
// @symbol _ZN9Butterfly6State6Ev
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
#include "Butterfly.h"
extern "C" void _ZN7fBase_c18MarkForDestructionEv(void*);


extern "C" {
extern int _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned a, unsigned b, const struct Vector3 *pos, const struct Vector3_16 *rot, int i, int j);
extern void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(void *c, int a, int b, int d, int e);

}

void Butterfly::State6()
{
    char *c = (char*)this;
    int st = *(int*)(c+0x3e8);
    if (st == 0x14) {
        if (*(unsigned char*)(c+0x3f0) != 1) {
            *(int*)(c+0x3e0) = 0;
            *(unsigned char*)(c+0x3f1) = 0;
            return;
        }
        int sub = *(signed char*)(c+0xcc);
        _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x114, 0, (struct Vector3*)(c+0x5c), 0, sub, -1);
        _ZN7fBase_c18MarkForDestructionEv(c);
        return;
    }
    if (st <= 0x14) return;
    (*(int *)(((int)c + 0x3e0))) += 0x40;
    if (*(int*)(c+0x3e0) < 0x800) return;
    _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(c, 0x32000, 0x32000, 0x1000000, 0x320000);
    *(short*)(c+0x3ee) = 0;
    *(int*)(c+0x3e8) = 0;
    *(int*)(c+0x3e4) = 7;
}
