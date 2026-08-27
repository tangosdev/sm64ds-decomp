extern int _ZN3IRQ7DisableEv(void);
extern int func_0206152c(void);
extern void _ZN3IRQ7RestoreEj(int token);
extern char *WM_GetSystemWork(void);

int func_02061c88(int idx, int b, int c)
{
    int token = _ZN3IRQ7DisableEv();
    int r = func_0206152c();
    if (r != 0) {
        _ZN3IRQ7RestoreEj(token);
        return r;
    }
    {
        char *p = WM_GetSystemWork() + idx * 4;
        *(int *)(p + 0xac) = b;
        *(int *)(p + 0xec) = c;
    }
    _ZN3IRQ7RestoreEj(token);
    return 0;
}
