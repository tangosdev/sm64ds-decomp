extern int *fn_02394E7C(int a1, int a2);
extern void fn_02394FC4(int a1);
extern int fn_0238D594(int a1, int a2);
extern void fn_02395354(unsigned char *p, int v);
extern int fn_02395334(unsigned char *p);
extern void fn_023969D8(void *dst, void *src);
extern void fn_02382C98(int a1, int a2, int a3);
extern void fn_0239AE84(int a1, int *obj);

int fn_0239B25C(unsigned short *src) {
    int *base;
    int *obj;
    unsigned int n;
    unsigned int i;
    unsigned char *p;
    unsigned char *dst;
    base = *(int **)0x0380FFF4;
    obj = fn_02394E7C((int)((char *)base + 0x14C),
                      *(unsigned short *)((char *)base + 0x300 + 0x9C) + 0x3E);
    if (obj == 0) {
        fn_02394FC4(1);
        return 0;
    }
    *(unsigned short *)((char *)obj + 0xC) = 0x8D;
    base = *(int **)0x0380FFF4;
    *(unsigned short *)((char *)obj + 0xE) = (unsigned short)(
        fn_0238D594(*(unsigned short *)((char *)base + 0x300 + 0x9C) + 1, 2) + 0x16);
    fn_02395354((unsigned char *)obj + 0x1F, *(unsigned short *)((char *)src + 0x12) & 0xFF);
    fn_02395354((unsigned char *)obj + 0x1E, *(unsigned short *)((char *)src + 0xE) & 0xFF);
    fn_023969D8((char *)obj + 0x2E, (char *)src + 0x1E);
    base = *(int **)0x0380FFF4;
    *(volatile unsigned short *)((char *)obj + 0x16) =
        *(unsigned short *)((char *)base + 0x300 + 0x9C);
    n = *(volatile unsigned short *)((char *)obj + 0x16);
    if (n != 0) {
        base = *(int **)0x0380FFF4;
        if ((*(unsigned short *)((char *)base + 0x300 + 0x9E) & 1) != 0) {
            p = (unsigned char *)(*(int *)((char *)base + 0x398) + 1);
            dst = (unsigned char *)obj + 0x3C;
            i = 0;
            while (i < *(volatile unsigned short *)((char *)obj + 0x16)) {
                fn_02395354(dst, fn_02395334(p));
                p++;
                dst++;
                i++;
            }
        } else {
            fn_02382C98(*(int *)((char *)base + 0x398), (int)((char *)obj + 0x3C), (int)(n + 1));
        }
    }
    base = *(int *volatile *)0x0380FFF4;
    fn_0239AE84((int)((char *)base + 0x14C), obj);
    return 1;
}
