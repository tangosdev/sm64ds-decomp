extern unsigned int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int saved);
extern int func_02065edc(unsigned int x);
extern void CpuCopy8(const void *src, void *dst, unsigned int size);

extern char *data_020a9db8;

void *func_020671b4(unsigned int idx)
{
    unsigned int saved = _ZN3IRQ7DisableEv();
    if (data_020a9db8 != 0 && func_02065edc(idx) != 0) {
        char *base = data_020a9db8;
        CpuCopy8(base + 0x1340 + (idx - 1) * 0x16, base + 0x1772, 0x16);
        _ZN3IRQ7RestoreEj(saved);
        return data_020a9db8 + 0x1772;
    }
    _ZN3IRQ7RestoreEj(saved);
    return 0;
}
