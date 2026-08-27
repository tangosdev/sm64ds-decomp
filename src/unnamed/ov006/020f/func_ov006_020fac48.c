extern void func_ov006_020fa924(char *c);
extern unsigned char data_0209d45c;

void func_ov006_020fac48(char *c, int idx)
{
    int n;
    unsigned short t;
    char *p_row;
    int neg;
    int b;
    int a;
    int lo;
    int hi;
    unsigned char flag;

    if (*(int *)((c + 0x5000) + 0xc10) == 2)
        return;

    n = idx * 0x14;
    t = *(unsigned short *)((c + 0x5c0c) + n);
    if (t != 0) {
        *(unsigned short *)((c + 0x5c0c) + n) = t - 1;
        return;
    }

    flag = data_0209d45c;
    p_row = c + n;
    neg = 0x1000;
    data_0209d45c = flag | 4;
    *(int *)((c + 0x5bfc) + n) = 0x100000;
    *(int *)((c + 0x5c00) + n) = 0x40000;
    neg = -neg;
    /* Mix (c+n) and p_row so mwccarm colors r4/r5 like the ROM. */
    *(int *)(((c + n) + 0x5000) + 0xc04) = neg;
    *(int *)((p_row + 0x5000) + 0xc08) = neg;
    a = *(int *)((c + 0x5c00) + n);
    b = *(int *)((c + 0x5bfc) + n);
    lo = b >> 12;
    hi = a >> 12;
    *(unsigned char *)(((c + n) + 0x5000) + 0xc0f) = 1;
    *(volatile int *)0x4000018 = (0x1ff & lo) | (0x1ff0000 & (hi << 16));
    func_ov006_020fa924(c);
}
