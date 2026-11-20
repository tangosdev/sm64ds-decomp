/* storage 0x02397E3C runtime 0x027C9DB4 linked 0x98 main ARM.
 * Clear two blocks, write the new pair, and put the saved word back. */

extern void fn_02382C80(int arg, void *dst, int len);

int fn_02397E3C(int a1, int a2) {
    unsigned char *base = *(unsigned char **)0x0380FFF4;
    int saved = *(int *)(base + 0x398);
    {
        volatile unsigned short zero = 0;
        fn_02382C80(zero, base + 0x2E0, 0x28);
    }
    {
        volatile unsigned short zero = 0;
        fn_02382C80(zero, *(unsigned char **)0x0380FFF4 + 0x308, 0xB4);
    }
    *(int *)(*(unsigned char **)0x0380FFF4 + 0x2E0) = a1;
    {
        unsigned short half = a2;
        *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x2E4) = half;
        *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x2E6) = half;
    }
    base = *(unsigned char **)0x0380FFF4;
    *(int *)(base + 0x398) = saved;
    return (int)base;
}
