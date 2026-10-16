/* storage 0x0238B900 runtime 0x03803798 size 0x110 (listed 0xFC + pool) wram ARM.
 * SPI-style handshake on 0x40001C0/0x40001C2: four busy-wait phases
 * (while (*r & 0x80) fn_02388CD8(arg)), command words 0x8A01/0xE4/0/0x8201/0,
 * then combine the reply bytes and return ((v4 | lo) & 0x7FF8) * 2.
 * Five numeric pools, one call. The per-phase arg is a variable holding 1
 * (re-inited to the same register, never a literal); the reply byte is built
 * as ((u16)(half & 0xFF) << 24) >> 16, which is what emits the LSL/LSR pairs
 * (a plain << 8 folds to one LSL). */

extern int fn_02388CD8(int a1);

int fn_0238B900(void) {
    volatile unsigned short *rC0 = (volatile unsigned short *)0x40001C0;
    volatile unsigned short *rC2 = (volatile unsigned short *)0x40001C2;
    int one = 1;
    int one2;
    int v4;
    while ((*rC0 & 0x80) != 0)
        fn_02388CD8(one);
    *rC0 = 0x8A01;
    *rC2 = 0xE4;
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
    return (unsigned short)(2 * ((v4 | (unsigned short)(*rC2 & 0xFF)) & 0x7FF8));
}
