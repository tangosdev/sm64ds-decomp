typedef unsigned int u32;

typedef struct Obj Obj;

typedef struct Owner {
    int pad[6];
    Obj *cur; /* 0x18 */
} Owner;

struct Obj {
    int f0;
    int f4;
    Owner *owner;         /* 0x08 */
    u32 flags;            /* 0x0c */
    int f10;
    int f14;              /* 0x14 */
    unsigned short queue; /* 0x18 */
};

extern u32 _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(u32);
extern void func_020580f0(unsigned short *self);
extern int func_0205cf5c(Obj *);

int func_0205d3f4(Obj *t)
{
    int woken = 0;
    u32 irq = _ZN3IRQ7DisableEv();
    int busy = (t->flags & 1) ? 1 : 0;
    if ((int)(((long long)busy) & 0xFFFFFFFFFFFFFFFFLL) != 0) {
        woken = (t->owner->cur != t) ? 1 : 0;
        if (woken) {
            *(u32 *)(int)(((long long)(int)((int)t + 0xc)) & 0xFFFFFFFFFFFFFFFFLL) |= 4;
        }
        do {
            func_020580f0(&t->queue);
            busy = (t->flags & 1) ? 1 : 0;
        } while ((int)(((long long)busy) & 0xFFFFFFFFFFFFFFFFLL) != 0);
    }
    _ZN3IRQ7RestoreEj(irq);
    if (woken) {
        return func_0205cf5c(t);
    }
    return t->f14 == 0;
}
