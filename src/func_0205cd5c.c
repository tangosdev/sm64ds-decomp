typedef unsigned int u32;
typedef unsigned short u16;

struct T {
    char pad0[0xc];
    u16 pending;
    u16 pad_e;
    u32 flags;
    u32 pad14;
    char *th;
};

extern void func_0205c788(void *p, int arg);
extern int func_0205d044(void *self);
extern void func_0205cfa4(int x);
extern u32 func_02059d1c(void);
extern void func_0205807c(u16 *p);
extern void func_02059d30(u32 f);

void func_0205cd5c(struct T *self, int arg)
{
    int b = (int)((self->flags & 0x100) != 0);

    if (b) {
        u32 *f = (u32 *)(((int)self + 0x10) & 0xFFFFFFFFFFFFFFFF);
        u32 v;
        char *t;
        int r;

        v = *f;
        t = self->th;
        *f = v & ~0x100;
        func_0205c788(t, arg);
        r = func_0205d044(self);
        if (r == 0) {
            return;
        }
        func_0205cfa4(r);
        return;
    }
    {
        char *p = self->th;
        u32 irq = func_02059d1c();
        u32 *f = (u32 *)(((int)self + 0x10) & 0xFFFFFFFFFFFFFFFF);

        *(int *)(p + 0x14) = arg;
        *f = *f & ~0x200;
        func_0205807c(&self->pending);
        func_02059d30(irq);
    }
}