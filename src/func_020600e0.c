// Release counterpart of func_02060188: under an IRQ lock, validate the (a0,a1) owner,
// decrement the reference count, and on the last release reset the slot. Sibling idiom.
unsigned int _ZN3IRQ7DisableEv(void);
void _ZN3IRQ7RestoreEj(unsigned int);
void Crash(void);
void func_020580f0(void *p);
typedef struct { int *f0; char pad4[4]; int f8; int fc; int f10; int f14; } G;
extern G data_020a8180;
void func_020600e0(int a0, int a1)
{
    G *g = &data_020a8180;
    unsigned int s = _ZN3IRQ7DisableEv();
    if (g->f8 != a0 || g->fc == 0) {
        Crash();
    } else {
        if (g->f14 != a1) Crash();
        { int *pc = (int *)(((int)g + 0xc) & 0xFFFFFFFFFFFFFFFF); *pc = *pc - 1; }
        if (g->fc == 0) {
            g->f8 = -3;
            g->f14 = 0;
            func_020580f0(&g->f10);
        }
    }
    *g->f0 = 0;
    _ZN3IRQ7RestoreEj(s);
}
