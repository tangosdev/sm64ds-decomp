#include "types.h"
struct Obj {
    char pad[0x64];
    u32 unk64;
};

typedef struct {
    u32 *f0;            /* 0x00 */
    char pad4[0x24];    /* 0x04 */
    void (*cb)(void *); /* 0x28 */
    void *cb_arg;       /* 0x2c */
    char pad30[4];      /* 0x30 */
    u32 flags;          /* 0x34 */
    char pad38[4];      /* 0x38 */
    struct Obj obj;     /* 0x3c */
    char pada4[0x30];   /* 0xa4 */
    u16 fd4;            /* 0xd4 */
} G;

extern G data_020a8180;

u32 _ZN3IRQ7DisableEv(void);
void _ZN3IRQ7RestoreEj(u32 state);
void OS_WakeupThread(u16 *self);
void func_02058048(struct Obj *self);

void func_02060e38(void) {
    int gi = (int)&data_020a8180;
    G *g = (G *)gi;
    void (*cb)(void *) = g->cb;
    void *arg = g->cb_arg;
    u32 s;
    u32 *pf;

    *g->f0 = 0;
    s = _ZN3IRQ7DisableEv();
    pf = (u32 *)((gi + 0x34));
    {
        u32 val = *pf;
        val &= ~0xc;
        *pf = val;
    }
    OS_WakeupThread(&g->fd4);
    if (g->flags & 0x10) {
        func_02058048(&g->obj);
    }
    _ZN3IRQ7RestoreEj(s);
    if (cb) {
        cb(arg);
    }
}
