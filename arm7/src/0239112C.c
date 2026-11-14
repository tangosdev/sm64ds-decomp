extern int fn_0238E688(void);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(unsigned short *);
extern unsigned short *fn_0238FB88(unsigned short *);
extern int fn_023910F0(int, int);

void fn_0239112C(void)
{
    unsigned short buf[0x102];
    unsigned short *v5 = buf;
    unsigned short *p;
    int v1;
    int v0 = fn_0238E688() + 0x1000;
    int *v4 = *(int **)(v0 + 0x610);
    if (*(unsigned short *)*v4 != 5) {
        p = fn_0238DE4C();
        p[0] = 0xB;
        p[1] = 3;
        fn_0238DEF4(p);
    } else {
        unsigned short *p = fn_0238FB88(v5);
        v1 = p[2];
        if (v1 != 0) {
            fn_023910F0(0x302, v1);
        } else {
            *(short *)*v4 = 2;
            p = fn_0238DE4C();
            p[0] = 0xB;
            p[1] = 0;
            fn_0238DEF4(p);
        }
    }
}
