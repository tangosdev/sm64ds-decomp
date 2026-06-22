extern unsigned int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int savedState);
extern void * func_02059364(void *a, void *n);
extern void * func_0205929c(void *a, void *n);

typedef unsigned int u32;

struct Elem { void *w0; void *w4; void *w8; };  /* 0xc */

struct G637c {
    int head;       /* 0x0 */
    char pad[0xc];
    struct Elem *base;  /* 0x10 */
};

extern struct G637c *data_020a637c[];

void func_020590fc(int idx, int sel, void *arg)
{
    struct G637c *g;
    struct Elem *e;
    void *a;
    u32 saved = _ZN3IRQ7DisableEv();
    g = data_020a637c[idx];
    if (sel < 0)
        sel = g->head;
    a = (char*)arg - 0x20;
    e = &g->base[sel];
    e->w8 = func_02059364(e->w8, a);
    e->w4 = func_0205929c(e->w4, a);
    _ZN3IRQ7RestoreEj(saved);
}
