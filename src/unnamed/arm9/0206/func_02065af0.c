extern int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int);
extern int *func_02065b88(void);
extern void MultiCopy_Int(int *dst, int *src, int len);
extern int data_020a94c0;
extern int data_020a9570;

int func_02065af0(int *thiz)
{
    int saved = _ZN3IRQ7DisableEv();
    int *r;
    if (data_020a94c0 != 1) {
        _ZN3IRQ7RestoreEj(saved);
        return 0;
    }
    r = func_02065b88();
    if (r == 0 || *r != 0) {
        _ZN3IRQ7RestoreEj(saved);
        return 0;
    }
    MultiCopy_Int(thiz, &data_020a9570, 0x9c);
    _ZN3IRQ7RestoreEj(saved);
    return 1;
}
