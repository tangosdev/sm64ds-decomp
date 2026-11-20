/* storage 0x02398E88 runtime 0x027CAE00 size 0x230 + pool 0x25C main ARM.
 * Mode dispatch on the masked select half: three command cases with
 * counter updates, then a shared status gate before queueing (0, 14). */
extern void fn_023986F4(void);
extern void fn_02398ACC(int);
extern void fn_0239477C(int, int);
extern void fn_027CEE84(void);
void fn_02398E88(void) {
    int *v0 = (int *)0x380FFF4;
    int v1 = *v0 + 996;
    int v2;
    int v3;
    unsigned v4;
    unsigned v5;
    int v6;
    int *v7;
    *(unsigned short *)0x4808010 = 2;
    v2 = *(volatile int *)v0;
    if (((unsigned short *)v2)[392] == 18) {
        fn_027CEE84();
    } else {
        v3 = *(volatile unsigned short *)0x48080B8 & 0xF00;
        switch (v3) {
        case 0x300:
            if ((*(int *)(v1 + 140) & 2) != 0)
                fn_023986F4();
            ++((int *)*(int *)0x380FFF4)[324];
            fn_0239477C(0, 8);
            break;
        case 0x800: {
            int w = *(int *)(v1 + 68);
            if (((unsigned short *)v1)[79] <= (unsigned short)(*(unsigned short *)(w + 4) & 0xFF)) {
                *(unsigned short *)0x48080AC = 2;
                ++*(unsigned short *)(v1 + 170);
            }
            ++*(unsigned short *)(*(int *)(v1 + 144) + 24);
            break;
        }
        case 0xB00: {
            v4 = *(unsigned short *)(v1 + 152);
            v5 = *(unsigned short *)(*(int *)(v1 + 68) + 2);
            v6 = *(int *)(v1 + 144) + 26;
            if (v5 > 1
                && ((((unsigned)((unsigned short *)v2)[383] << 27) >> 31) != 0)
                && ((*(volatile unsigned short *)0x480819C & 1) == 0))
                *(unsigned short *)0x4808290 ^= 1;
            if ((((unsigned short *)*(int *)0x380FFF4)[782] & 0x40) != 0) {
                while (v5 > 1) {
                    v5 = (v5 << 15) >> 16;
                    if ((v5 & 1) != 0)
                        ++*(unsigned short *)(v6 + 6);
                    v4 = (v4 << 15) >> 16;
                    if ((v4 & 1) != 0)
                        v6 += *(unsigned short *)(((volatile int *)v1)[36] + 22);
                }
            }
            break;
        }
        default:
            break;
        }
        if (v3 != 0x800 && (*(volatile unsigned short *)0x48080B0 & 2) == 0) {
            v7 = (int *)0x380FFF4;
            if (((unsigned short *)*v7)[528] != 0) {
                *(unsigned short *)0x48080B4 = 2;
                *(unsigned short *)0x4808048 = 0;
                if ((*(volatile unsigned short *)0x4808010 & 0x1000) == 0)
                    fn_02398ACC(2);
                else
                    ++*(unsigned short *)((char *)*v7 + 1170);
                ++((unsigned short *)*(int *)0x380FFF4)[584];
            }
            *(unsigned short *)0x48080AE = 2;
        }
        fn_0239477C(0, 14);
    }
}
