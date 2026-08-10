#include "types.h"
extern u32 _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(u32 saved);
extern void OS_WakeupThread(u16 *p);
extern int func_0205c5e4(void *self, int a);
extern void *func_0205d044(void *pool);

typedef struct Node {
    char pad0[8];
    void *pool;      /* 0x8 */
    u32 flags;       /* 0xc */
    int f10;         /* 0x10 */
    char pad14[4];   /* 0x14 */
    u16 f18;         /* 0x18 */
} Node;

void func_0205cfa4(Node *self)
{
    void *pool = self->pool;
    if (self == 0) return;
    do {
        u32 saved = _ZN3IRQ7DisableEv();
        int b = (int)((self->flags & 4) != 0);
        if (b != 0) {
            OS_WakeupThread(&self->f18);
            return;
        }
        *(u32 *)((u32)self + 0xc) |= 8u;
        _ZN3IRQ7RestoreEj(saved);
        if (func_0205c5e4(self, self->f10) == 6) return;
        self = (Node *)func_0205d044(pool);
    } while (self != 0);
}
