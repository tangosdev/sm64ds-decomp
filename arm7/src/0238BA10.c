/* storage 0x0238BA10 runtime 0x038038A8 size 0x110 (listed 0xFC + pool) wram ARM.
 * Same SPI handshake skeleton as fn_0238B900 (command words 0x8A01/0xEC/0/0x8201/0),
 * returning (((v4 | lo) & 0x7F80) << 9) >> 16. Five numeric pools, one call. */

extern int fn_02388CD8(int a1);

unsigned fn_0238BA10(void) {
    volatile unsigned short *rC0 = (volatile unsigned short *)0x40001C0;
    volatile unsigned short *rC2 = (volatile unsigned short *)0x40001C2;
    int one = 1;
    int one2;
    int v4;
    while ((*rC0 & 0x80) != 0)
        fn_02388CD8(one);
    *rC0 = 0x8A01;
    *rC2 = 0xEC;
    one = 1;
    while ((*rC0 & 0x80) != 0)
        fn_02388CD8(one);
    *rC2 = 0;
    one = 1;
    while ((*rC0 & 0x80) != 0)
        fn_02388CD8(one);
    v4 = (int)(((unsigned)((unsigned short)(*rC2 & 0xFF)) << 24) >> 16);
    *rC0 = 0x8201;
    *rC2 = 0;
    one2 = 1;
    while ((*rC0 & 0x80) != 0)
        fn_02388CD8(one2);
    return (unsigned)((unsigned)((v4 | (unsigned short)(*rC2 & 0xFF)) & 0x7F80) << 9 >> 16);
}
