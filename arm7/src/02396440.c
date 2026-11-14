/* storage 0x02396440 runtime 0x027C83B8 size 0xBC main.
 * Permission-bit builder: zeroes two mask halfwords, counts bytes via
 * fn_02395334, then sets bit (1 << table[v]) or bit 15 per byte.
 * One pool data ref (unk_27D663C table), two calls to fn_02395334. */

extern unsigned int fn_02395334(unsigned char *p); /* =0x027C72AC */
extern unsigned short unk_27D663C[]; /* =0x027D663C */

void fn_02396440(unsigned char *a1, unsigned short *a2) {
    unsigned int i;
    unsigned int count;
    unsigned char *base;
    unsigned short *table;
    a2[0] = 0;
    a2[1] = 0;
    count = fn_02395334(a1 + 1);
    i = 0;
    table = unk_27D663C;
    base = a1 + 2;
    for (; i < count; i++) {
        unsigned int v = fn_02395334(base + i);
        unsigned int idx = (v & 0x7F) - 1;
        unsigned int t;
        if (idx < 0x78 && (t = table[idx]) != 255) {
            *(a2 + 1) |= 1u << t;
            if (v & 0x80)
                a2[0] |= 1u << t;
        } else {
            *(a2 + 1) |= 0x8000;
            if (v & 0x80)
                a2[0] |= 0x8000;
        }
    }
}
