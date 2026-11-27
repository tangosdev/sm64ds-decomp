/* storage 0x02395830 runtime 0x027C77A8 linked 0x1B0 main ARM.
 * RF init: copy 16 calibration halfwords from FLASH via fn_023A4340 into
 * the 0x4808000 block, fold the radio id word, then either burst-load the
 * channel table (mode 3) or poll bytes until the count runs out.
 * Pool +0x1A0: unk_27D6574, dat_0380FFF4, 0x4808184, 0x5B8. */
extern unsigned short unk_27D6574[];
extern int dat_0380FFF4;
extern int fn_023A4340(int, int, void *);
extern int fn_0238D594(int, int);
extern int fn_02395B9C(int);

int fn_02395830(void)
{
    unsigned int v10;
    int tmp;
    unsigned short *tbl;
    unsigned int i;
    int two;
    int v3;
    unsigned char *base;
    int v5;
    unsigned int v6;
    int mode;
    int result;
    i = 0;
    tmp = 0;
    two = 2;
    do {
        tbl = unk_27D6574;
        fn_023A4340((int)(i * 2 + 68), two, &tmp);
        {
            unsigned int addr = (unsigned int)tbl[i] + 0x4800000;
            *(unsigned short *)(addr + 0x8000) = (unsigned short)tmp;
        }
        i++;
    } while (i < 0x10);
    {
        unsigned char **pp = (unsigned char **)&dat_0380FFF4;
        unsigned int f = *(unsigned short *)((*pp) + 0x5B2);
        unsigned int hi = f >> 7;
        v10 = hi << 8;
        {
            unsigned int f2 = *(unsigned short *)((*pp) + 0x5B2);
            unsigned int lo = f2 & 0x7F;
            v10 = lo | (hi << 8);
        }
        *(unsigned short *)0x4808184 = (unsigned short)v10;
        v3 = 206;
        base = *pp;
        {
            unsigned int f3 = *(unsigned short *)(base + 0x5B2);
            v5 = fn_0238D594((int)((f3 & 0x7F) + 7), 8);
        }
        {
            unsigned char *p = base + 0x500;
            v6 = *(unsigned short *)(p + 0xB4);
            mode = *(unsigned short *)(p + 0xB0);
        }
    }
    if (mode == 3) {
        result = fn_023A4340((int)(v6 + 206), 1, (void *)(base + 0x5B8));
        {
            unsigned int j = 0;
            int zero = 0;
            int one = 1;
            for (; j < v6;) {
                v10 = (unsigned int)zero;
                fn_023A4340(v3, one, &v10);
                v10 |= (j << 8) + 0x50000;
                result = fn_02395B9C((int)v10);
                j++;
                v3++;
            }
        }
    } else {
        unsigned int *p10;
        unsigned char **pp2;
        result = 0;
        v10 = 0;
        p10 = &v10;
        pp2 = (unsigned char **)&dat_0380FFF4;
        while (v6 != 0) {
            fn_023A4340(v3, v5, p10);
            fn_02395B9C((int)v10);
            {
                unsigned char *b = *pp2;
                result = *(unsigned short *)(b + 0x5B0);
                if (result == 2) {
                    result = (int)(v10 >> 18);
                    if (result == 9) {
                        result = (int)(v10 & ~0x7C00);
                        *(int *)(b + 0x5BC) = result;
                    }
                }
            }
            v6--;
            v3 += v5;
        }
    }
    return result;
}
