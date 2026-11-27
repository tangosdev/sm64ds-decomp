/* storage 0x02385BDC runtime 0x037FDA74 size 0xF0 wram ARM.
 * Pool word is the constant 0xFFFF. */

extern void fn_02384124(int a1);

int fn_02385BDC(int a1) {
    volatile unsigned char *flags;
    int zero;

    zero = 0;
    *(int *)(a1 + 0x24) = zero;
    *(int *)(a1 + 0x28) = zero;
    flags = (volatile unsigned char *)a1;
    *flags |= 2;
    *flags &= ~4;
    *flags &= ~8;
    *flags &= ~0x10;
    *flags &= ~0x20;
    *flags |= 0x40;
    *flags &= ~0x80;
    *(unsigned char *)(a1 + 0x3b) = zero;
    *(unsigned short *)(a1 + 2) = zero;
    *(unsigned char *)(a1 + 0x12) = 64;
    *(unsigned char *)(a1 + 4) = 127;
    *(unsigned char *)(a1 + 5) = 127;
    *(unsigned short *)(a1 + 0xa) = zero;
    *(unsigned char *)(a1 + 8) = zero;
    *(unsigned char *)(a1 + 9) = zero;
    *(unsigned char *)(a1 + 6) = zero;
    *(unsigned short *)(a1 + 0xc) = zero;
    *(unsigned char *)(a1 + 0xe) = 255;
    *(unsigned char *)(a1 + 0xf) = 255;
    *(unsigned char *)(a1 + 0x10) = 255;
    *(unsigned char *)(a1 + 0x11) = 255;
    *(unsigned char *)(a1 + 7) = 2;
    *(unsigned char *)(a1 + 0x14) = 60;
    *(unsigned char *)(a1 + 0x15) = zero;
    *(unsigned short *)(a1 + 0x16) = zero;
    *(unsigned char *)(a1 + 0x13) = zero;
    *(unsigned short *)(a1 + 0x1e) = 0xFFFF;
    fn_02384124(a1 + 0x18);
    *(int *)(a1 + 0x20) = 0;
    *(int *)(a1 + 0x3c) = 0;
    return 0;
}
