extern int func_02053200(int x);
extern int func_ov004_020af770(int a0, int a1, int a2, int a3, int a4, int a5, unsigned short a6);

void func_ov006_020deed8(int a0, void *a1, int a2, int a3, int s, int a5)
{
    unsigned char *sl;
    int a5_local;
    int e2;
    int e3;
    int neg1, zero, lim;

    sl = (unsigned char *)a1;
    e2 = a2 >> 12;
    e3 = a3 >> 12;
    a5_local = a5;
    s = func_02053200(s);
    neg1 = -1;
    zero = 0;
    lim = 0xffff;

    for (;;) {
        int r = func_ov004_020af770((int)sl, e2, e3, neg1, neg1, s, zero);
        if (r != 0) {
            if (a5_local == 2) {
                if ((unsigned)(*(int *)(sl + 4) << 0x10) >> 0x1c == 3) {
                    int *p = (int *)(((int)r + 4) & 0xFFFFFFFFFFFFFFFF);
                    *p = (*p & ~0xf000) | 0x4000;
                }
            }
        }
        if (*(unsigned short *)(sl + 6) == (unsigned short)lim)
            break;
        sl += 8;
    }
}
