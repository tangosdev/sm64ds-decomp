typedef int (*FnT)(int a, int b, int c);

extern char data_020a1fc0[];
extern unsigned int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int);
extern unsigned int __aeabi_uidiv(unsigned int n, unsigned int d);

void func_02041224(char *thiz, int n)
{
    int r5;
    int r6;
    unsigned int saved;
    unsigned int q;
    char *g;
    if (n < 0) {
        n = *(int *)(thiz + 0xc74);
        *(int *)(thiz + 0xc74) = 0;
    }
    if ((unsigned int)*(int *)(thiz + 0xc74) >= (unsigned int)n) return;
    *(int *)(thiz + 0xc74) = n;
    r6 = *(int *)(thiz + 0xc70);
    if (r6 != 0) {
        g = data_020a1fc0;
        saved = _ZN3IRQ7DisableEv();
        ((FnT)(*(int *)(g + 0x14)))(*(int *)(g + 0x18), 0, r6);
        _ZN3IRQ7RestoreEj(saved);
    }
    {
        int dm = *(int *)(data_020a1fc0 + 0x28) - 0xe;
        q = __aeabi_uidiv(*(int *)(thiz + 0xc74) + dm - 1, dm);
    }
    r5 = ((q + 0x1f) >> 2) << 4;
    g = data_020a1fc0;
    saved = _ZN3IRQ7DisableEv();
    r5 = ((FnT)(*(int *)(g + 0x14)))(*(int *)(g + 0x18), r5, 0);
    _ZN3IRQ7RestoreEj(saved);
    *(int *)(thiz + 0xc70) = r5;
    *(int *)(thiz + 0x80) = *(int *)(thiz + 0xc70);
}
