extern int func_0204ef9c(void *p);
extern void func_0205cd34(void *o);
extern int _ZN4cstd6strlenEPKc(const char *s);
extern int func_0205cc80(void *thiz, void *a, void *b);
extern int func_0205cb68(void *p, int a, int b0, int b4, int c0, int c4, void *fn1, void *fn2);
extern void func_0205cbe4(void *thiz);

int func_0204ee40(void *self, char *name, char *p)
{
    char *fatb;
    char *fntb;
    char *fimg;
    int i;
    char *cur;
    int n;
    char *fimg_off;

    fatb = 0;
    fntb = 0;
    fimg = 0;

    if (func_0204ef9c(p) == 0)
        return 0;

    cur = p + *(unsigned short *)(p + 0xc);
    n = *(unsigned short *)(p + 0xe);
    i = 0;
    if (n > 0) {
        do {
            unsigned int m = *(unsigned int *)cur;
            switch (m) {
            case 0x46415442:
                fatb = cur;
                break;
            case 0x464e5442:
                fntb = cur;
                break;
            case 0x46494d47:
                fimg = cur;
                break;
            }
            cur += *(int *)(cur + 4);
            i++;
        } while (i < n);
    }

    func_0205cd34(self);
    *(char **)((char *)self + 0x50) = p;
    *(char **)((char *)self + 0x54) = fatb;
    fimg_off = fimg + 8;
    *(char **)((char *)self + 0x58) = fimg_off;

    if (func_0205cc80(self, name, (void *)_ZN4cstd6strlenEPKc(name)) == 0)
        return 0;

    if (func_0205cb68(self, (int)fimg_off, (int)(fatb + 0xc) - (int)fimg_off,
                      *(int *)(fatb + 4) - 0xc, (int)(fntb + 8) - (int)fimg_off,
                      *(int *)(fntb + 4) - 8, 0, 0))
        return 1;

    func_0205cbe4(self);
    return 0;
}
