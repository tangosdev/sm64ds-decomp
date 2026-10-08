extern void MultiStore16(unsigned short val, char *dst, int nbytes);
extern int func_020685cc(int a, void *b, int flag);
extern void MultiStore32Bytes(unsigned val, void *dst, int len);
extern void MultiCopyHalf(int a, void *dst, int n);
extern int func_02068514(int p);

struct Src {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
    unsigned char f18;
};

void func_02068680(char *dst, struct Src *src, int flag)
{
    volatile unsigned short tmp;
    volatile int z;
    tmp = 0;
    MultiStore16(tmp, dst, 0x4b8);
    *(unsigned char *)(dst + 0x4aa) = 0;
    if (func_020685cc(src->fc, dst, 1) == 0 | func_020685cc(src->f10, dst, 0) == 0) {
        *(unsigned char *)(dst + 0x4aa) = 1;
        z = 0;
        MultiStore32Bytes(z, dst, 0x220);
    }
    *(int *)(dst + 0x4b0) = src->f14;
    if (flag) {
        MultiCopyHalf(flag, dst + 0x220, 0x16);
    }
    *(unsigned char *)(dst + 0x236) = src->f18;
    MultiCopyHalf(src->f4, dst + 0x238, (unsigned)(func_02068514(src->f4) << 0x11) >> 0x10);
    MultiCopyHalf(src->f8, dst + 0x298, (unsigned)(func_02068514(src->f8) << 0x11) >> 0x10);
    *(unsigned char *)(dst + 0x358) = 1;
    *(short *)(dst + 0x35a) = 1;
    *(short *)(dst + 0x4a8) = 1;
}
