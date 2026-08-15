// @symbol func_ov063_0211c7b0
// recovered name: BooCage_Kill
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daTBasket_c::Kill - name recovered from the vtable slot it fills.
   The body is a decompilation verified against the ROM, not an
   inferred stub. */
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int c, void *v, unsigned int e);
void func_ov063_0211c7b0(char *c) {
    short *t;
    if (*(unsigned char*)(c + 0x150) != 0) {
        *(short*)(c + 0x100 + 0x3a) = 0;
        return;
    }
    *(short*)(c + 0x100 + 0x3a) = 0x80;
    t = (short *)(((int)c + 0x8e));
    *t = (short)(*t + *(short*)(c + 0x100 + 0x3a));
    func_ov063_0211c82c(c);
    *(unsigned int*)(c + 0x148) = _ZN5Sound8PlayLongEjjjRK7Vector3s(*(unsigned int*)(c + 0x148), 3, 0x8f, c + 0x74, 0);
}
