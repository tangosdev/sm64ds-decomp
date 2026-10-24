extern int *fn_0238E688(void);
extern void fn_0238E448(void *dst, void *src);
extern unsigned short *fn_0238FBFC(unsigned short *a1, unsigned short a2, void *a3);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(unsigned short *msg);

int fn_02391F54(int *a1) {
    unsigned short big[256];
    unsigned char small[0x84];
    char *base;
    char *hi;
    int **pp;
    int *obj;
    unsigned char bits;
    unsigned short h;
    unsigned short *slot;
    unsigned short *msg;
    unsigned short st;

    base = (char *)fn_0238E688();
    hi = base + 0x1000;
    pp = *(int ***)(hi + 0x610);
    obj = *pp;
    *(int *)((char *)obj + 0xE4) = a1[1];
    *(unsigned short *)((char *)obj + 0xE8) = (unsigned short)a1[2];
    *(unsigned short *)((char *)obj + 0xEA) = (unsigned short)a1[3];
    *(unsigned short *)((char *)obj + 0xEC) = (unsigned short)a1[4];
    *(unsigned short *)((char *)obj + 0xEE) = (unsigned short)a1[5];
    bits = (unsigned char)a1[6];
    *(unsigned short *)((char *)obj + 0xF0) = (bits & 1) != 0;
    *(unsigned short *)((char *)obj + 0xF4) = (bits & 2) != 0;
    *(unsigned short *)((char *)obj + 0xF6) = (bits & 4) != 0;
    *(unsigned short *)((char *)obj + 0xF8) = (bits & 8) != 0;
    fn_0238E448(small, (char *)obj + 0xE4);
    h = *(unsigned short *)((char *)obj + 0xE8);
    slot = fn_0238FBFC(big, (unsigned short)(h + 16), small);
    msg = fn_0238DE4C();
    msg[0] = 0x18;
    st = slot[2];
    msg[1] = (int)((st == 0) ? (char *)0 : (char *)1);
    if (slot[2] != 0) {
        msg[2] = 0x245;
        msg[3] = slot[2];
    }
    return fn_0238DEF4(msg);
}
