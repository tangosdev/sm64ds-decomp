extern char *fn_0238E688(void);
extern void fn_023929DC(void);
extern int fn_02382810(int a1, int a2, int a3, int a4);
extern void fn_02392B34(void);

int fn_02392834(void) {
    char *p;
    int *obj;
    int *inner;
    int sample;
    int cc;
    int held;
    p = fn_0238E688();
    p += 0x1000;
    obj = *(int **)(p + 0x610);
    sample = *(volatile unsigned short *)0x380FFF0;
    inner = (int *)*obj;
    *(int *)((char *)inner + 0xCC) = sample;
    inner = (int *)*obj;
    cc = *(int *)((char *)inner + 0xCC);
    held = *(int *)((char *)inner + 0xD0);
    if (held != cc) {
        *(int *)((char *)inner + 0xD0) = cc;
        fn_023929DC();
    }
    inner = (int *)*obj;
    if (*(unsigned int *)((char *)inner + 0xD4) > 0x8C)
        return fn_02382810((int)obj + 0xD0, 0xD0, (int)fn_02392B34, 0xD0);
    *(int *)((char *)inner + 0x24) = 1;
    return fn_02382810((int)obj + 0xD0, 0xF0, (int)fn_02392B34, 0xF0);
}
