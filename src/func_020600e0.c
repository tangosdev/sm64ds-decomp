unsigned int _ZN3IRQ7DisableEv(void);
void _ZN3IRQ7RestoreEj(unsigned int);
void Crash(void);
void OS_WakeupThread(void* p);
typedef struct { int* f0; char pad4[4]; int f8; int fc; int f10; int f14; } G;
extern G data_020a8180;
void func_020600e0(int a0, int a1)
{
    G* g = &data_020a8180;
    int gi = (int)&data_020a8180;
    unsigned int s = _ZN3IRQ7DisableEv();
    if (g->f8 != a0 || g->fc == 0) {
        Crash();
    } else {
        if (g->f14 != a1) Crash();
        { int* pc = (int*)((gi + 0xc) & 0xFFFFFFFFFFFFFFFF); *pc = *pc - 1; }
        if (g->fc == 0) {
            g->f8 = -3;
            g->f14 = 0;
            OS_WakeupThread(&g->f10);
        }
    }
    *g->f0 = 0;
    _ZN3IRQ7RestoreEj(s);
}
