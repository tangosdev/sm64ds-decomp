#include "Ov007Spring.h"
#include "nitro/mi/memory.h"
#include "Ov007MenuMem.h"

extern void func_ov007_020c806c(void *c);

/* Allocates a link and clears it (func_ov007_020c806c). */
Ov007Link *func_ov007_020c80a4(void) {
    Ov007Link *c = func_ov007_020c3df4(0, sizeof(Ov007Link));
    func_ov007_020c806c(c);
    return c;
}

/* Long-branch veneer: long_calls emits the pooled `ldr ip,[pc]; bx ip`
 * absolute tail-call; bracketed because the pragma is positional. */
#pragma long_calls on
void func_ov007_020c8098(void *p) {
    func_ov007_020c3d1c(p);
}
#pragma long_calls off

void func_ov007_020c806c(void *c) {
    volatile int li = 0;
    MI_CpuFill32(c, li, 0x18);
}
