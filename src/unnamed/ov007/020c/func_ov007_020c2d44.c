struct S {
    int pad[6];
    int *f18;
    unsigned short *f1c;
    int pad20;
    int *f24;
    int *f28;
};

extern int func_020538b8(int x, int y);
extern int func_020531a4(int a);

void func_ov007_020c2d44(struct S *thiz, int n) {
    int ang, dy, dx;
    if (n > 0) {
        dy = thiz->f28[n] - thiz->f28[n - 1];
        dx = thiz->f24[n] - thiz->f24[n - 1];
        ang = func_020538b8(dx, dy);
    } else {
        dx = 0;
        dy = 0;
        ang = 0;
    }
    int a = (int)(((long long)dy * dy + 0x800) >> 12);
    int b = (int)(((long long)dx * dx + 0x800) >> 12);
    int len = func_020531a4(a + b);
    thiz->f18[n] = len;
    thiz->f1c[n] = -ang;
    if (n == 1) {
        thiz->f1c[0] = thiz->f1c[1];
    }
}
