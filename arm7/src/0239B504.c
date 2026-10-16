extern int *fn_02394E7C(int a1, int a2);
extern void fn_02394FC4(int a1);
extern void fn_023969D8(void *dst, void *src);
extern void fn_0239AE84(int a1, int *obj);
extern int fn_02395334(unsigned char *p);
extern void fn_02395354(unsigned char *p, int v);

int fn_0239B504(void *src, unsigned short val, unsigned char *bytes) {
    int *base;
    int *obj;
    unsigned int i;
    unsigned char *srcb;
    unsigned char *dst;
    base = *(int *volatile *)0x0380FFF4;
    obj = fn_02394E7C((int)((char *)base + 0x14C), 0x3A);
    if (obj == 0) {
        fn_02394FC4(1);
        return 0;
    }
    *(unsigned short *)((char *)obj + 0xC) = 0x87;
    *(unsigned short *)((char *)obj + 0xE) = 0x15;
    fn_023969D8((char *)obj + 0x10, src);
    *(unsigned short *)((char *)obj + 0x16) = val;
    *(unsigned short *)((char *)obj + 0x18) = (unsigned short)fn_02395334(bytes + 1);
    i = 0;
    srcb = bytes + 2;
    dst = (unsigned char *)obj + 0x1A;
    while (i < *(unsigned short *)((char *)obj + 0x18)) {
        if (i >= 0x20)
            break;
        fn_02395354(dst + i, fn_02395334(srcb + i));
        i++;
    }
    while (i < 0x20) {
        fn_02395354((unsigned char *)obj + 0x1A + i, 0);
        i++;
    }
    base = *(int *volatile *)0x0380FFF4;
    fn_0239AE84((int)((char *)base + 0x14C), obj);
    return 1;
}
