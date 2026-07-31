unsigned int _ZN3IRQ7DisableEv(void);
void _ZN3IRQ7RestoreEj(unsigned int);
void Crash(void);
void OS_SleepThread(void* p);
typedef struct { int* f0; char pad4[4]; int f8; int fc; int f10; int f14; } G;
extern G data_020a8180;
void func_02060188(int a0, int a1)
{
    int gi = (int)&data_020a8180;
    unsigned int s = _ZN3IRQ7DisableEv();
    G* g = (G*)gi;
    if (g->f8 == a0) {
        if (g->f14 != a1) Crash();
    } else {
        while (*(volatile int*)&g->f8 != -3) { OS_SleepThread(&g->f10); }
        g->f8 = a0;
        g->f14 = a1;
    }
    { int* pc = (int*)((gi + 0xc) & 0xFFFFFFFFFFFFFFFF); *pc = *pc + 1; }
    *g->f0 = 0;
    _ZN3IRQ7RestoreEj(s);
}
