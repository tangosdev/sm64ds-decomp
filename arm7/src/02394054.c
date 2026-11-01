#pragma opt_propagation off

extern void *fn_0238E688(void);
extern unsigned short *fn_0238FB5C(unsigned short *msg);
extern unsigned short *fn_0238FB88(unsigned short *msg);
extern void fn_0239401C(void);
extern unsigned short *fn_0238FBCC(unsigned short *msg);
extern unsigned short fn_0238E1A0(unsigned value);
extern void fn_0238FD50(unsigned short *msg, unsigned short a, int b, int c);
extern unsigned short *fn_0238FB2C(unsigned short *msg);
extern void fn_02382C98(void *src, void *dst, int n);
extern unsigned short *fn_0238FBE4(unsigned short *msg);
extern void fn_0238E57C(void *dst, void *src);
extern unsigned short *fn_0238FD90(unsigned short *msg, int a);

int fn_02394054(unsigned short *outCode, unsigned short *outVal) {
    unsigned short buf[0x100];
    char *base = (char *)fn_0238E688();
    char *hi = base + 0x1000;
    char **pp = *(char ***)(hi + 0x610);
    unsigned short *ret = fn_0238FB5C(buf);
    if (ret[2] != 0) {
        *outCode = 0x304;
        *outVal = ret[2];
        return 0;
    }
    ret = fn_0238FB88(buf);
    if (ret[2] != 0) {
        *outCode = 0x302;
        *outVal = ret[2];
        return 0;
    }
    fn_0239401C();
    ret = fn_0238FBCC(buf);
    if (ret[2] != 0) {
        *outCode = 0x283;
        *outVal = ret[2];
        return 0;
    }
    {
        unsigned short h = ret[3];
        *(unsigned short *)(*pp + 0x1F0) = h;
        *(unsigned short *)(*pp + 0x1F2) = fn_0238E1A0(((unsigned)h << 15) >> 16);
    }
    fn_0238FD50(buf, 0xFFFF, 0x28, 5);
    *(unsigned short *)(*pp + 0x1E8) = 2;
    *(unsigned short *)(*pp + 0x1EA) = 1;
    ret = fn_0238FB2C(buf);
    if (ret[2] != 0) {
        *outCode = 0x306;
        *outVal = ret[2];
        return 0;
    }
    fn_02382C98(ret + 3, *pp + 0x28, 8);
    *(unsigned short *)(*pp + 0x30) = ret[7];
    *(unsigned short *)(*pp + 0x34) = ret[8];
    *(unsigned short *)(*pp + 0x36) = ret[9];
    *(unsigned short *)(*pp + 0x32) = ret[10];
    ret = fn_0238FBE4(buf);
    if (ret[2] != 0) {
        *outCode = 0x281;
        *outVal = ret[2];
        return 0;
    }
    fn_0238E57C(*pp + 0xDC, ret + 3);
    ret = fn_0238FD90(buf, 1);
    if (ret[2] == 0)
        goto ok;
    *outCode = 0x215;
    *outVal = ret[2];
    return 0;
ok:
    return 1;
}
