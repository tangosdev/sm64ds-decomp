#include "types.h"
struct Node {
    void *key;
    struct Node *next;
};

extern struct Node *data_020a8048;

void *func_0205d304(void);
u32 _ZN3IRQ7DisableEv(void);
void _ZN3IRQ7RestoreEj(u32 state);

struct Node *func_0205d23c(void) {
    void *r5 = func_0205d304();
    u32 irqState = _ZN3IRQ7DisableEv();
    struct Node *r4 = data_020a8048;
    while (r4 != 0 && r4->key != r5) {
        r4 = r4->next;
    }
    _ZN3IRQ7RestoreEj(irqState);
    return r4;
}
