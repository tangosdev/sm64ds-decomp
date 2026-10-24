extern int *fn_02394E7C(int a1, int a2);
extern void fn_02394FC4(int a1);
extern void fn_023969D8(void *dst, void *src);
extern void fn_0239AE84(int a1, int *obj);

int fn_0239B488(void *src, unsigned short val) {
    int *base;
    int *obj;
    base = *(int *volatile *)0x0380FFF4;
    obj = fn_02394E7C((int)((char *)base + 0x14C), 0x18);
    if (obj == 0) {
        fn_02394FC4(1);
        return 0;
    }
    *(unsigned short *)((char *)obj + 0xC) = 0x88;
    *(unsigned short *)((char *)obj + 0xE) = 4;
    fn_023969D8((char *)obj + 0x10, src);
    *(unsigned short *)((char *)obj + 0x16) = val;
    base = *(int *volatile *)0x0380FFF4;
    fn_0239AE84((int)((char *)base + 0x14C), obj);
    return 1;
}
