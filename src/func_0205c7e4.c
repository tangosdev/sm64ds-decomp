enum Bool { FALSE, TRUE };

extern unsigned int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int i);
extern void* func_0205d044(void* self);
extern void func_0205cfa4(void* p);

int func_0205c7e4(char* self)
{
    void* r = 0;
    unsigned int saved;
    enum Bool bit;
    enum Bool notset;

    saved = _ZN3IRQ7DisableEv();
    bit = (enum Bool)((*(int*)(self + 0x10) & 8) != 0);
    notset = (enum Bool)(bit == FALSE);
    if (notset == FALSE) {
        int *p = (int*)(((unsigned long long)(self + 0x10)) & 0xFFFFFFFFFFFFFFFFULL);
        *p = *p & ~8;
        r = func_0205d044(self);
    }
    _ZN3IRQ7RestoreEj(saved);
    if (r != 0) {
        func_0205cfa4(r);
    }
    return notset;
}
