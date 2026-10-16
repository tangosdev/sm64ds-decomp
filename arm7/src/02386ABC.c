/* storage 0x02386ABC runtime 0x037FE954 size 0xE8 wram.
 * Pool +0xE8: 04000403, 04000508, 00008080, 04000509. */

extern int fn_02382AC4(void);
extern void fn_02382AD8(int);
extern void fn_023868A8(int);
extern int fn_02386758(void);

int fn_02386ABC(unsigned int mask16, unsigned int flags, unsigned int mask8) {
    int tok;
    int i;
    unsigned char *base;

    tok = fn_02382AC4();
    base = (unsigned char *)0x04000403;
    i = 0;
    while (i < 16 && mask16 != 0) {
        if ((mask16 & 1) != 0)
            base[i << 4] |= 0x80;
        i = i + 1;
        mask16 >>= 1;
    }
    if ((flags & 1) != 0) {
        if ((flags & 2) != 0)
            *(volatile unsigned short *)0x04000508 |= 0x8080;
        else
            *(volatile unsigned char *)0x04000508 |= 0x80;
    } else if ((flags & 2) != 0) {
        *(volatile unsigned char *)0x04000509 |= 0x80;
    }
    i = 0;
    while (i < 8 && mask8 != 0) {
        if ((mask8 & 1) != 0)
            fn_023868A8(i);
        i = i + 1;
        mask8 >>= 1;
    }
    fn_02382AD8(tok);
    return fn_02386758();
}
