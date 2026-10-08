extern void *func_02062734(void *c, unsigned m, void *acc, int bit);
extern void MultiCopyHalf(void *dst, void *src, int n);
extern void MultiStore16(unsigned short val, void *dst, int n);
extern unsigned _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned);

#pragma opt_common_subs off
void func_02062990(char *c, int bit, void *arg2)
{
    unsigned short mask = (unsigned short)(1 << bit);
    unsigned short v40e = *(unsigned short *)(c + 0x40e);
    unsigned short idx;
    volatile unsigned short zero;
    void *r;
    unsigned s;

    if ((v40e & mask) == 0) return;

    idx = *(unsigned short *)(c + 0x408);
    if ((mask & *(unsigned short *)(c + (idx << 8))) == 0) {
        if (*(unsigned short *)(c + 0x418) != 1) return;
        idx = (idx + 1) % 4;
        if ((mask & *(unsigned short *)(c + (idx << 8))) == 0) return;
    }

    r = func_02062734(c, v40e, (char *)(c + (idx << 8)) + 4, bit);
    if (arg2 != 0) {
        MultiCopyHalf(arg2, r, *(unsigned short *)(c + 0x410));
    } else {
        zero = 0;
        MultiStore16(zero, r, *(unsigned short *)(c + 0x410));
    }
    s = _ZN3IRQ7DisableEv();
    *(unsigned short *)(c + (idx << 8)) &= ~mask;
    *(unsigned short *)((c + 2) + (idx << 8)) |= mask;
    _ZN3IRQ7RestoreEj(s);
}
