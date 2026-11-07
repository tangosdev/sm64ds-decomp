/* storage 0x023929DC runtime 0x027C4954 size 0xE8 + pool 0x18 = 0x100 linked main ARM.
 * Torn halfword read with conditional reload; one unsigned `t` carries the whole
 * high-half/ORR/mask/accumulator chain so it sits in R12 end to end (separate
 * high/combined/masked vars split the chain across R1/R12/LR with an extra AND,
 * and decl shuffles only move it to R0). Loop uses word indices [51]/[53] for
 * +0xCC/+0xD4: byte-addr spelling CSEs the address into an ADD and misses the
 * indexed loads plus the writeback `LDR R2,[R3,#0xCC]!` form of the +=.
 * No new relocs (sole call fn_0238E688 is already in AM40 syms). */
extern int *fn_0238E688(void);
unsigned fn_023929DC(void) {
    int *result;
    unsigned t;
    unsigned first;
    unsigned second;
    unsigned factor;
    int v3;
    int v4;
    result = (int *)fn_0238E688()[1412];
    *(int *)((int)(*(int *)result) + 0xCC) <<= 6;
    first = *(volatile unsigned short *)0x048080F8;
    t = *(volatile unsigned short *)0x048080FA;
    second = *(volatile unsigned short *)0x048080F8;
    if (first > second)
        t = *(volatile unsigned short *)0x048080FA;
    t = second | (t << 16);
    t &= 0x3FFFC0;
    factor = *(volatile unsigned short *)0x04000006;
    t = 0x3FFFC0 & ((127 * (0x107 - factor) + (t << 1)) >> 1);
    if (*(unsigned *)((int)(*(int *)result) + 0xCC) > t) {
        *(int *)((int)(*(int *)result) + 0xD4) = 0;
        return 0;
    }
    v3 = 1;
    v4 = 0x414B;
    while (v3 < 30) {
        int v5 = *(int *)result;
        unsigned v6 = ((unsigned *)v5)[51];
        if (v6 > t) {
            ((int *)v5)[53] = v6 - t;
            {
                int v7 = *(int *)result;
                unsigned r = ((unsigned *)v7)[53];
                if (r > 0x4000) {
                    r = 0;
                    ((unsigned *)v7)[53] = 0;
                }
                return r;
            }
        }
        ((int *)v5)[51] += v4;
        v3++;
    }
    return (unsigned)result;
}
