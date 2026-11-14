/* Flag-gated device setup: if the word at base + 0x178 is set, take the lock
 * word, check the flag at base + 0x40C, bump the counter at +0x40E, run
 * fn_023A05A4 on the words at +0x170/+0x414, set bit 0x2000 for modes above
 * 1, publish the scaled word to 0x48080A8, release the lock. The address of
 * the device pointer is kept in r3 via w; q uses += so the add reuses r1. */

extern int *dat_0380FFF4;
extern int fn_02380884(int);
extern int fn_023808C0(int);
extern int fn_023A05A4(int, int);

int fn_023A06F0(void) {
    unsigned v6;
    unsigned v5;
    char *p = (char *)dat_0380FFF4 + 0x100;
    unsigned short v1 = *(unsigned short *)(p + 0x78);
    if (v1 != 0) {
        int v3;
        int **w;
        char *q;
        v3 = fn_02380884(0x1000000);
        w = &dat_0380FFF4;
        q = (char *)(*w);
        q += 0x400;
        if (*(unsigned short *)(q + 0xC) != 0)
            return fn_023808C0(v3);
        *(unsigned short *)(q + 0xC) = 1;
        ++*(unsigned short *)((int)(*w) + 0x40E);
        v5 = *(unsigned *)((char *)dat_0380FFF4 + 0x170);
        v6 = *(unsigned *)((char *)dat_0380FFF4 + 0x414);
        *(unsigned *)((char *)dat_0380FFF4 + 0x418) = v5 + 16;
        fn_023A05A4((int)v6, (int)v5);
        if (v1 > 1)
            *(unsigned short *)(v6 + 12) |= 0x2000;
        *(unsigned short *)0x48080A8 = (unsigned short)(((v6 & 0x3FFF) >> 1) | 0x8000);
        return fn_023808C0(v3);
    }
    return (int)p;
}
