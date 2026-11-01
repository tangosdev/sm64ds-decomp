/* storage 0x0239A650 runtime 0x027CC5C8 size 0xAC main ARM.
 * Clear one bit of a saved-byte array. The mask is applied to the read
 * result on its own, and the unsigned-char narrow happens at the write,
 * so the address setup lands between those two ands. */

extern int *dat_0380FFF4;
extern int fn_02380884(int);
extern int fn_023808C0(int);
extern unsigned int fn_02395334(unsigned int);
extern void fn_02395354(unsigned int, unsigned int);

int fn_0239A650(int a1) {
    unsigned short *q = (unsigned short *)((char *)dat_0380FFF4 + 0x300);
    unsigned base = q[0x48] + 0x0480425Cu;
    int saved = fn_02380884(0x1000000);

    if (a1 == 0) {
        unsigned v = fn_02395334(base + 4) & 0xFEu;
        fn_02395354(base + 4, (unsigned char)v);
    } else {
        unsigned short *rec = (unsigned short *)(*(int *)((char *)dat_0380FFF4 + 0x2E0) + a1 * 0x1C);
        unsigned idx = rec[1];
        unsigned addr = base + 5 + (idx >> 3);
        unsigned v = fn_02395334(addr) & ~(1u << (idx & 7));
        fn_02395354(addr, (unsigned char)v);
    }
    return fn_023808C0(saved);
}
