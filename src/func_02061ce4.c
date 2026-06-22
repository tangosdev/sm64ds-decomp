extern int _ZN3IRQ7DisableEv(void);
extern int func_0206152c(void);
extern void _ZN3IRQ7RestoreEj(int token);
extern void *func_02061548(void);

int func_02061ce4(int x)
{
    int token = _ZN3IRQ7DisableEv();
    int r = func_0206152c();
    if (r != 0) {
        _ZN3IRQ7RestoreEj(token);
        return r;
    }
    *(int *)((unsigned char *)func_02061548() + 0xa8) = x;
    _ZN3IRQ7RestoreEj(token);
    return 0;
}
