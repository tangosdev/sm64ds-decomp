#include "types.h"
/* FS_FindArchiveByName. The two parameters were missing from this file until
   run link100 lane WM7: the ARM body calls func_0205d304 as its very first
   instruction after the prologue, so r0 and r1 still hold this function's own
   name pointer and length and the call needs no argument setup at all. That
   reads as a call with no arguments and it was decompiled as one, which is
   faithful to the ARM and wrong on every host with a stack calling convention:
   every caller in the image already passes the name and the length
   (src/func_0205cc80.c, src/func_020423dc.c, src/func_020424c0.c,
   src/func_02042254.c, src/func_0201a2f8.c, src/func_0205d714.c,
   src/func_02067bfc.c), the callee dropped them, and the lookup then walked
   data_020a8048 for a key it had never been given and answered null. Spelling
   the parameters and forwarding them is byte-identical under the pinned
   compiler: tools/match.py reports 2004/b56 MATCH before and after. */
struct Node {
    u32 key;
    struct Node *next;
};

extern struct Node *data_020a8048;

int func_0205d304(unsigned char *s, int n);
u32 _ZN3IRQ7DisableEv(void);
void _ZN3IRQ7RestoreEj(u32 state);

struct Node *func_0205d23c(unsigned char *name, int len) {
    u32 r5 = (u32)func_0205d304(name, len);
    u32 irqState = _ZN3IRQ7DisableEv();
    struct Node *r4 = data_020a8048;
    while (r4 != 0 && r4->key != r5) {
        r4 = r4->next;
    }
    _ZN3IRQ7RestoreEj(irqState);
    return r4;
}
