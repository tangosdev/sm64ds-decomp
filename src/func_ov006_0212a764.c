extern int RandomIntInternal(int *seed);
extern void func_0203d388(int *p, short angle);
extern int func_01ffabe4(int angle);
extern void func_ov004_020b04d0(int n);
extern void _ZN3G2x13SetBlendAlphaEPVttttt(
    volatile void *reg,
    unsigned short a, unsigned short b, int c, int d);
extern int data_0209d4b8;

#pragma opt_strength_reduction off

void func_ov006_0212a764(char *c)
{
    unsigned int r;
    int j;
    int *pang;
    int ang;
    int i2;
    int i;
    int angbase;

    *(int *)(c + 0x5fb8) = 0;
    *(int *)(c + 0x5fbc) = 0;
    *(int *)(c + 0x5fc0) = *(int *)(c + 0x5fb8);
    *(int *)(c + 0x5fc4) = *(int *)(c + 0x5fbc);
    func_ov004_020b04d0(0x20);
    for (i = 0; i < 0x16; i++) {
        *(unsigned char *)(c + i * 0x20 + 0x4f38) = 0;
        *(unsigned char *)(c + i * 0x20 + 0x4f3a) = 0;
        *(unsigned char *)(c + i * 0x20 + 0x4f39) = 0;
        *(unsigned char *)(c + i * 0x20 + 0x4f3b) = 0;
        *(int *)(c + i * 0x20 + 0x4f3c) = 0;
        *(int *)(c + i * 0x20 + 0x4f40) = 0;
        *(int *)(c + i * 0x20 + 0x4f4c) = 0;
        *(int *)(c + i * 0x20 + 0x4f50) = 0;
        *(int *)(c + i * 0x20 + 0x4f44) = 0;
        *(int *)(c + i * 0x20 + 0x4f48) = 0;
    }
    if (*(int *)(c + 0x5fe4) == 0x14) {
        r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        *(int *)(c + 0x5fd8) = ((r * 2) >> 15) + 1;
    } else {
        {
            int raw = RandomIntInternal(&data_0209d4b8);
            r = ((unsigned int)raw >> 16) & 0x7fff;
            if (((r * 10) >> 15) == 0) {
                r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
                *(int *)(c + 0x5fd8) = ((r * 2) >> 15) + 0x14;
            } else {
                r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
                *(int *)(c + 0x5fd8) = ((r * 8) >> 15) + 8;
            }
        }
    }
    r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    angbase = (r * 0x10000) >> 15;
    i2 = 0;
    if (*(int *)(c + 0x5fd8) > 0) {
        pang = (int *)(c + 0x4f3c);
        ang = 0;
        do {
            *(short *)(((int)(c + i2 * 0x20) + 0x4f54) & 0xFFFFFFFFFFFFFFFF) = angbase + func_01ffabe4(ang);
            *(int *)(((int)(c + i2 * 0x20) + 0x4f3c) & 0xFFFFFFFFFFFFFFFF) = 0;
            *(int *)(((int)(c + i2 * 0x20) + 0x4f40) & 0xFFFFFFFFFFFFFFFF) = 0x30000;
            func_0203d388(pang, *(short *)(((int)(c + i2 * 0x20) + 0x4f54) & 0xFFFFFFFFFFFFFFFF));
            *(int *)(((int)(c + i2 * 0x20) + 0x4f3c) & 0xFFFFFFFFFFFFFFFF) += 0x80000;
            *(int *)(((int)(c + i2 * 0x20) + 0x4f40) & 0xFFFFFFFFFFFFFFFF) += 0x60000;
            *(unsigned char *)((unsigned int)c + i2 * 0x20 + 0x4f38) = 1;
            i2++;
            ang += 0x10000;
            pang += 8;
        } while (i2 < *(int *)(c + 0x5fd8));
    }
    j = 0;
    if (*(int *)(c + 0x5fd8) > 0) {
        do {
            int k;
            r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            k = (*(int *)(c + 0x5fd8) * r) >> 15;
            if (k != j) {
                char *ej = c + j * 0x20;
                char *ek = c + k * 0x20;
                unsigned short t16;
                int t1, t2;
                t16 = *(unsigned short *)(((int)ej + 0x4f54) & 0xFFFFFFFFFFFFFFFF);
                *(unsigned short *)(((int)ej + 0x4f54) & 0xFFFFFFFFFFFFFFFF) = *(unsigned short *)(((int)ek + 0x4f54) & 0xFFFFFFFFFFFFFFFF);
                *(unsigned short *)(((int)ek + 0x4f54) & 0xFFFFFFFFFFFFFFFF) = t16;
                t1 = *(int *)(((int)ej + 0x4f3c) & 0xFFFFFFFFFFFFFFFF);
                t2 = *(int *)(((int)ej + 0x4f40) & 0xFFFFFFFFFFFFFFFF);
                *(int *)(((int)ej + 0x4f3c) & 0xFFFFFFFFFFFFFFFF) = *(int *)(((int)ek + 0x4f3c) & 0xFFFFFFFFFFFFFFFF);
                *(int *)(((int)ej + 0x4f40) & 0xFFFFFFFFFFFFFFFF) = *(int *)(((int)ek + 0x4f40) & 0xFFFFFFFFFFFFFFFF);
                *(int *)(((int)ek + 0x4f3c) & 0xFFFFFFFFFFFFFFFF) = t1;
                *(int *)(((int)ek + 0x4f40) & 0xFFFFFFFFFFFFFFFF) = t2;
            }
            j++;
        } while (j < *(int *)(c + 0x5fd8));
    }
    *(int *)(c + 0x5fc8) = -1;
    *(unsigned char *)(c + 0x5fcc) = 0;
    *(int *)(c + 0x5fe8) = 0;
    *(int *)(c + 0x5fd0) = 0;
    *(int *)(c + 0x5fd4) = 0;
    *(int *)(c + 0x5fec) = 2;
    *(unsigned char *)(c + 0x5fcd) = 0;
    *(unsigned short *)(c + 0x5ff4) = 0;
    *(volatile unsigned short *)0x4000050 = 0;
    _ZN3G2x13SetBlendAlphaEPVttttt((volatile void *)0x4001050, 4, 8, 6, 0xa);
}
