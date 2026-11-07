/* storage 0x023869F8 runtime 0x037FE890 size 0xBC wram.
 * Pool +0xBC: 04000508, 04000509. */

extern int fn_02382AC4(void);
extern void fn_02382AD8(int);
extern void fn_02386864(int);
extern void fn_02383634(int, int);
extern int fn_02386758(void);

int fn_023869F8(unsigned int mask16, unsigned int flags, unsigned int mask8, int arg) {
    int tok;
    int i;

    tok = fn_02382AC4();
    i = 0;
    while (i < 8 && mask8 != 0) {
        if ((mask8 & 1) != 0)
            fn_02386864(i);
        i = i + 1;
        mask8 >>= 1;
    }
    i = 0;
    while (i < 16 && mask16 != 0) {
        if ((mask16 & 1) != 0)
            fn_02383634(i, arg);
        i = i + 1;
        mask16 >>= 1;
    }
    if ((flags & 1) != 0)
        *(volatile unsigned char *)0x04000508 = 0;
    if ((flags & 2) != 0)
        *(volatile unsigned char *)0x04000509 = 0;
    fn_02382AD8(tok);
    return fn_02386758();
}
