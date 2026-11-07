extern void *fn_0238E688(void);
extern void fn_02392ADC(void);
extern int fn_02382810(int a1, int a2, int a3, int a4);
extern void fn_02392B34(void);

int fn_02392724(void) {
    char *p;
    int *obj;
    int inner;
    volatile unsigned short *hp;
    unsigned int t;
    int w;
    p = (char *)fn_0238E688();
    p += 0x1000;
    obj = *(int **)(p + 0x610);
    inner = *obj;
    hp = (volatile unsigned short *)inner;
    t = hp[0x6C];
    if (t >= 0x3C) {
        w = *(unsigned short *)0x4000006;
        if (w >= 0xD1 && w <= 0xD4)
            *(unsigned short *)0x4000006 = (unsigned short)w;
        *(unsigned short *)(*obj + 0xD8) = 0;
    } else {
        ++hp[0x6C];
    }
    fn_02392ADC();
    return fn_02382810((int)obj + 0xD0, 0x104, (int)fn_02392B34, 0x104);
}
