#pragma opt_common_subs off
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);
extern unsigned char data_ov006_0212efcc[];

/* A: base = c + OFF, index = i*32   (used for read-modify-write) */
#define A16(off) (*(unsigned short *)(c + (off) + i * 32))
#define A8(off)  (*(unsigned char  *)(c + (off) + i * 32))
#define A32(off) (*(int            *)(c + (off) + i * 32))
/* B: base = c + i*32, immediate OFF (used for plain read / plain store) */
#define B16(off) (*(unsigned short *)(c + i * 32 + (off)))
#define B8(off)  (*(unsigned char  *)(c + i * 32 + (off)))
#define B32(off) (*(int            *)(c + i * 32 + (off)))

void func_ov006_0211db7c(char *c, int i)
{
    A16(0x4bb2) += 1;
    if (B16(0x4bb2) >= 8) {
        B16(0x4bb2) = 0;
        A8(0x4bbd) += 1;
        if (B8(0x4bbd) >= 8)
            B8(0x4bbd) = 0;
        B8(0x4bb8) = data_ov006_0212efcc[B8(0x4bbd)];
    }
    if (B32(0x4ba0) >> 12 <= 0x80) {
        B32(0x4ba0) = 0x80000;
    } else {
        A32(0x4ba0) += B32(0x4ba8);
        if (B32(0x4ba8) <= -0x600)
            A32(0x4ba8) += 0x20;
    }
    if (B16(0x4bb4) != 0) {
        A16(0x4bb4) -= 1;
        return;
    }
    _ZN5Sound12PlayBank2_2DEj(0x1f3);
    B8(0x4bba) = 2;
    B8(0x4bbd) = 0;
    B8(0x4bb8) = 0;
    B16(0x4bb2) = 0;
    B32(0x4ba8) = 0;
}
