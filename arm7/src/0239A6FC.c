/* Set one bit in the byte table at (halfword +0x390) + 0x480425C.
 * a1 == 0 sets bit 0 of the byte at +4. Otherwise the halfword at
 * entry[a1] + 2 selects the bit of the byte at +5. IRQ mask 0x1000000
 * stays cleared across the update. */

extern int *dat_0380FFF4;
extern int fn_02380884(int);
extern int fn_023808C0(int);
extern unsigned int fn_02395334(unsigned int addr);
extern void fn_02395354(unsigned int addr, unsigned int val);

int fn_0239A6FC(int a1) {
    unsigned int base;
    int saved;
    unsigned short off;

    off = *(unsigned short *)((char *)dat_0380FFF4 + 0x390);
    base = off + 0x480425C;
    saved = fn_02380884(0x1000000);
    if (a1 == 0) {
        unsigned int b = fn_02395334(base + 4) | 1;
        fn_02395354(base + 4, b & 0xFF);
    } else {
        unsigned int half;
        unsigned int addr;
        char *ent = *(char **)((char *)dat_0380FFF4 + 0x2E0);
        unsigned int b;

        half = *(unsigned short *)(ent + a1 * 0x1C + 2);
        addr = base + 5 + (half >> 3);
        b = fn_02395334(addr) | (1 << (half & 7));
        fn_02395354(addr, b & 0xFF);
    }
    return fn_023808C0(saved);
}
