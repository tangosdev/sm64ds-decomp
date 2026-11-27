/* Packet stats at root+0x4F4, then a bit clear at +0x4EC.
 * The comma keeps the index load ahead of the second root deref.
 * Bit 0 of the window halfword is a 1-bit field so it shifts, not ands.
 * Pool: dat_0380FFF4, 0x4F4, 0x4EC, 0xFFFE. */
struct Bit0 {
    unsigned short b0:1;
    unsigned short rest:15;
};

extern int *dat_0380FFF4;
extern void fn_0239AAA8(unsigned short *pkt);
extern void fn_0239D638(int a1, unsigned short *a2);
extern int fn_023A07BC(int a1);
extern void fn_02396EFC(int a1);

void fn_023A0398(unsigned short *a1, int a2) {
    int *stats = (int *)((char *)dat_0380FFF4 + 0x4F4);
    unsigned short *prev = a1 - 8;

    fn_0239AAA8(a1);
    if ((a1[4] & 2) == 0) {
        ++stats[0];
        if (((unsigned int)a1[10] << 23) >> 31) {
            if ((a1[18] & 1) != 0)
                ++stats[5];
            else
                ++stats[4];
        } else if ((a1[12] & 1) != 0) {
            ++stats[5];
        } else {
            ++stats[4];
        }
    } else {
        ++stats[1];
    }
    if (((unsigned int)a1[10] << 17) >> 31)
        ++stats[6];

    fn_0239D638((int)((char *)dat_0380FFF4 + 0x158), prev);
    *(unsigned short *)((char *)dat_0380FFF4 + 0x3E4) = 0;
    {
        int idx = a1[1];
        char *root = (idx, (char *)dat_0380FFF4);

        if (((((int)*(unsigned short *)(root + 0x4EA)) >> idx) & 1) != 0 &&
            (a1[10] & 0x2000) == 0 &&
            *(unsigned short *)(*(char **)(root + 0x2E0) + idx * 0x1C) == 0x40) {
            unsigned short *mask = (unsigned short *)(root + 0x4EC);
            *mask &= ~(1 << idx);
        }
    }
    if (a2 != 0) {
        char *root = (char *)dat_0380FFF4;

        if (*(unsigned short *)(root + 0x160) != 0) {
            fn_023A07BC(0);
        } else {
            if ((unsigned short)(*(unsigned short *)(root + 0x314) + 0xFFFE) <= 1 &&
                *(unsigned short *)(root + 0x310) == 0x40 &&
                ((struct Bit0 *)(root + 0x316))->b0) {
                unsigned short slot = *(unsigned short *)(root + 0x384);

                if (*(unsigned short *)(*(char **)(root + 0x2E0) + slot * 0x1C + 0x16) == 0 &&
                    *(unsigned short *)(root + 0x38A) == 0)
                    fn_02396EFC(1);
            }
        }
    }
}
