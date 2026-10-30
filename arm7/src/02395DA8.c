/* storage 0x02395DA8 runtime 0x027C7D20 size 0x5A4 main. Sound voice setup:
 * programs a bank of MMIO halfwords, runs four init hooks, then applies a
 * per-mode configuration (switch on state+0x314), including a 64-bit phase
 * multiply in mode 1. Fifteen call sites to eleven callees; pool is
 * MMIO addrs + small immediates. */

extern void fn_02395CB8(void); /* =0x027C7C30 */
extern void fn_023952FC(unsigned int first, unsigned int second); /* =0x027C7274 */
extern int fn_0239A37C(void); /* =0x027CC2F4 */
extern int fn_023A40A4(int a1); /* =0x027D601C */
extern int fn_0239E06C(int a1); /* =0x027CFFE4 */
extern void fn_023A0A94(int a1); /* =0x027D2A0C */
extern int fn_023969F4(int a1); /* =0x027C896C */
extern unsigned long long fn_0238D38C(unsigned long long a1, unsigned long long a2); /* =0x03805224 */
extern void fn_0239F690(int a1); /* =0x027D1608 */
extern void fn_02396CF0(void); /* =0x027C8C68 */
extern int fn_02396EFC(int a1); /* =0x027C8E74 */

int fn_02395DA8(void) {
    int result;
    int bit;
    int i;

    fn_02395CB8();
    fn_023952FC(*(volatile unsigned short *)0x4808044 + (*(volatile unsigned short *)0x4808044 << 8),
                *(volatile unsigned short *)0x4808044);
    *(unsigned short *)(*(char **)0x0380FFF4 + 0x378) = 1;
    if (((((unsigned int)*(unsigned short *)(*(char **)0x0380FFF4 + 0x2FE)) << 29) >> 31) == 1)
        *(unsigned short *)(*(char **)0x0380FFF4 + 0x378) |= 0x20;
    if (*(unsigned short *)(*(char **)0x0380FFF4 + 0x2F8) != 0)
        *(unsigned short *)(*(char **)0x0380FFF4 + 0x378) |= 0x10;
    *(unsigned short *)(*(char **)0x0380FFF4 + 0x316) &= ~4;
    *(unsigned short *)0x4808032 = 0x8000;
    *(unsigned short *)0x4808134 = 0xFFFF;
    *(unsigned short *)0x480802A = 0;
    *(unsigned short *)0x4808028 = 0;
    *(unsigned short *)0x48080E8 = 1;
    *(unsigned short *)0x4808038 = 15;
    fn_023A0A94(fn_0239E06C(fn_023A40A4(fn_0239A37C())));
    *(unsigned short *)0x4808030 = 0x8000;
    switch (*(unsigned short *)(*(char **)0x0380FFF4 + 0x314)) {
    case 0:
        *(unsigned short *)0x4808010 = 0xFFFF;
        *(unsigned short *)0x4808012 = 63;
        *(unsigned short *)0x48081AE = 0x1FFF;
        if (*(unsigned short *)(*(char **)0x0380FFF4 + 0x61C) & 8)
            *(unsigned short *)0x48081AA = 0x400;
        *(unsigned short *)0x48080D0 = 0xFFFF;
        *(unsigned short *)0x48080E0 = 8;
        *(unsigned short *)0x4808008 = 0;
        *(unsigned short *)0x480800A = 0;
        *(unsigned short *)0x48080E8 = 0;
        *(unsigned short *)0x4808004 = 1;
        fn_023969F4(64);
        break;
    case 1: {
        unsigned short *pp;
        unsigned long long v7;
        unsigned long long t;

        *(unsigned short *)0x4808010 = 0xFFF;
        *(unsigned short *)0x4808012 = 0x703F;
        *(unsigned short *)0x48081AE = 0x1FFF;
        *(unsigned short *)0x48081AA = 0;
        if (*(unsigned short *)(*(char **)0x0380FFF4 + 0x61C) & 8)
            *(unsigned short *)0x48081AA |= 0x400;
        *(unsigned short *)0x48080D0 = 0x301;
        *(unsigned short *)0x48080E0 = 13;
        *(unsigned short *)0x4808008 = 0xE000;
        *(unsigned short *)0x480800A = 0;
        *(unsigned short *)0x4808004 = 1;
        pp = (unsigned short *)&t;
        pp[0] = *(unsigned short *)0x48080F8;
        pp[1] = *(unsigned short *)0x48080FA;
        pp[2] = *(unsigned short *)0x48080FC;
        pp[3] = *(unsigned short *)0x48080FE;
        v7 = (unsigned int)(*(unsigned short *)(*(char **)0x0380FFF4 + 0x36A) << 10);
        t = fn_0238D38C(t, v7);
        t = t + 1;
        t *= v7;
        *(unsigned short *)0x48080F6 = pp[3];
        *(unsigned short *)0x48080F4 = pp[2];
        *(unsigned short *)0x48080F2 = pp[1];
        *(unsigned short *)0x48080F0 = pp[0] | 1;
        *(unsigned short *)0x48080EA = 1;
        fn_0239F690(fn_023969F4(64));
        *(unsigned short *)0x48080AE = 2;
        break;
    }
    case 2:
        *(unsigned short *)0x4808010 = 0xFFFF;
        *(unsigned short *)0x4808012 = 0xE03F;
        if (*(unsigned short *)(*(char **)0x0380FFF4 + 0x61C) & 0x20)
            *(unsigned short *)0x4808012 |= 0xC0;
        *(unsigned short *)0x48081AE = 0x1FFF;
        *(unsigned short *)0x48081AA = 0;
        if (*(unsigned short *)(*(char **)0x0380FFF4 + 0x61C) & 8)
            *(unsigned short *)0x48081AA |= 0x400;
        if (*(unsigned short *)(*(char **)0x0380FFF4 + 0x61C) & 0x20)
            *(unsigned short *)0x48081AA |= 0x68;
        if (*(unsigned short *)(*(char **)0x0380FFF4 + 0x360) & 1)
            *(unsigned short *)0x48080D0 = 0x581;
        else
            *(unsigned short *)0x48080D0 = 0x181;
        *(unsigned short *)0x48080E0 = 11;
        *(unsigned short *)0x4808008 = 0;
        *(unsigned short *)0x480800A = 0;
        *(unsigned short *)0x4808004 = 1;
        *(unsigned short *)0x48080E8 = 1;
        *(unsigned short *)0x48080EA = 1;
        fn_023969F4(32);
        break;
    case 3:
        *(unsigned short *)0x4808010 = 0xFFFF;
        *(unsigned short *)0x4808012 = 0xC03F;
        *(unsigned short *)0x48081AE = 0x1FFF;
        *(unsigned short *)0x48081AA = 0;
        if (*(unsigned short *)(*(char **)0x0380FFF4 + 0x61C) & 8)
            *(unsigned short *)0x48081AA |= 0x400;
        if (*(unsigned short *)(*(char **)0x0380FFF4 + 0x360) & 1)
            *(unsigned short *)0x48080D0 = 0x401;
        else
            *(unsigned short *)0x48080D0 = 1;
        *(unsigned short *)0x48080E0 = 11;
        *(unsigned short *)0x4808008 = 0;
        *(unsigned short *)0x480800A = 0;
        *(unsigned short *)0x4808004 = 1;
        *(unsigned short *)0x48080E8 = 1;
        *(unsigned short *)0x48080EA = 1;
        *(unsigned short *)0x4808048 = 0;
        fn_023969F4(32);
        break;
    case 4:
        *(unsigned short *)0x4808010 = 0xFFFF;
        *(unsigned short *)0x4808012 = 0;
        *(unsigned short *)0x48081AE = 0;
        *(unsigned short *)0x4808008 = 0;
        *(unsigned short *)0x480800A = 0;
        *(unsigned short *)0x4808004 = 1;
        fn_023969F4(32);
        break;
    default:
        break;
    }
    *(unsigned short *)0x4808048 = 0;
    fn_02396CF0();
    result = 2;
    *(unsigned short *)0x48080AE = result;
    bit = *(unsigned short *)(*(char **)0x0380FFF4 + 0x316) << 31;
    bit = (unsigned int)bit >> 31;
    if (bit == 1)
        result = fn_02396EFC(result);
    for (i = 4000; i != 0; --i) {
        result = *(unsigned short *)0x480819C & 0x80;
        if (result != 0)
            break;
    }
    return result;
}
