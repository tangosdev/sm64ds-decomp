extern unsigned int fn_02395334(unsigned int addr);
extern void fn_02395354(unsigned int addr, unsigned int val);

int fn_02397508(unsigned int a1, unsigned int src) {
    int flag;
    int *base;
    unsigned int i;
    int *pp;
    unsigned int zero;
    unsigned int dest;
    unsigned int n;

    flag = 0;
    if (a1 > 0x20)
        return 5;
    base = *(int **)0x0380FFF4;
    if (*(unsigned short *)((char *)base + 0x310) == 0x40 &&
        *(unsigned short *)((char *)base + 0x2F2) == 1) {
        if (*(unsigned short *)((char *)base + 0x31A) != a1)
            return 6;
        if (*(unsigned short *)((char *)base + 0x38E) != 0)
            flag = 1;
    }
    i = 0;
    while (i < a1) {
        fn_02395354(*(int *)0x0380FFF4 + 0x31C + i, fn_02395334(src));
        src += 1;
        i++;
    }
    zero = 0;
    while (i < 0x20) {
        fn_02395354(*(int *)0x0380FFF4 + 0x31C + i, zero);
        i++;
    }
    pp = (int *)0x0380FFF4;
    *(unsigned short *)((char *)*pp + 0x300 + 0x1A) = a1;
    if (flag != 0) {
        base = (int *)*pp;
        dest = *(unsigned int *)((char *)base + 0x464) + 0x26 +
               *(unsigned short *)((char *)base + 0x38E);
        n = 0;
        while (n < a1) {
            fn_02395354(dest + n, fn_02395334(*pp + 0x31C + n));
            n++;
        }
    }
    return 0;
}
