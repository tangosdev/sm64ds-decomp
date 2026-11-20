/* storage 0x023A05A4 runtime 0x027D251C linked 0x14C main.
 * Copies a 0x1C block, then samples 0x04808044 three times.
 * The +0xC/-7 and +0xC/-1 address math is kept as two steps.
 */
extern int *dat_0380FFF4;
extern void fn_02395388(int dst, int src, int src2, int count);
extern void fn_023953C4(int dst, int src, int src2, int count);
extern void fn_02395400(int dst, int src, int count);

void fn_023A05A4(int a1, int a2) {
    char *b = (char *)a2 + 0x10;
    volatile unsigned short *mmio;
    unsigned short s0;
    unsigned short s1;
    unsigned short s2;
    int **w;
    char *p;

    if (*(unsigned short *)(b + 0x14) & 0x4000) {
        if (*(unsigned short *)((char *)a2 + 0xC) == 0xFFFF)
            fn_02395388(a1, (int)(b + 8), (int)(b + 0x2C), *(unsigned short *)(b + 6));
        else
            fn_02395388(a1, (int)(b + 8), *(int *)(b + 0x2C), *(unsigned short *)(b + 6));
        mmio = (volatile unsigned short *)0x04808044;
        s0 = *mmio;
        s1 = *mmio;
        *(unsigned short *)((char *)a1 + 0x24) = (unsigned short)(s0 + (s1 << 8));
        s2 = *mmio;
        w = &dat_0380FFF4;
        *(unsigned short *)((char *)a1 + 0x26) =
            (unsigned short)((s2 & 0xFF) | (*(unsigned short *)((char *)*w + 0x2FA) << 14));
        if (*(unsigned short *)((char *)*w + 0x61C) & 8) {
            p = (char *)a1 + 0xC;
            p += *(unsigned short *)(b + 0x12);
            p -= 7;
            p = (char *)((unsigned)p & ~1u);
            *(unsigned short *)p = 0;
            *(unsigned short *)(p + 2) = 0;
        }
    } else if (*(unsigned short *)((char *)a2 + 0xC) == 0xFFFF) {
        fn_02395400(a1, (int)(b + 8), *(unsigned short *)(b + 6) + 0x24);
    } else {
        fn_023953C4(a1, (int)(b + 8), *(int *)(b + 0x2C), *(unsigned short *)(b + 6));
    }
    if (*(unsigned short *)((char *)dat_0380FFF4 + 0x61C) & 4) {
        p = (char *)a1 + 0xC;
        p += *(unsigned short *)(b + 0x12);
        p -= 1;
        p = (char *)((unsigned)p & ~3u);
        *(unsigned short *)p = 0xB6B8;
        *(unsigned short *)(p + 2) = 0x1D46;
    }
}
