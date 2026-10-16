extern int fn_0238E688(void);
extern void fn_02382DC0(void *, void *, unsigned int);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(unsigned short *);
extern unsigned short *fn_0238FCF0(unsigned short *, unsigned short);
extern void fn_023908C0(int, int);

void fn_023908FC(int *a1)
{
    unsigned short buf[0x102];
    unsigned short *v3;
    unsigned short *p;
    int v1;
    int *v4;
    int v0 = fn_0238E688() + 0x1000;
    v4 = *(int **)(v0 + 0x610);
    fn_0238E688();
    fn_02382DC0((void *)a1[1], (void *)(*v4 + 0xE4), 0x40);
    v3 = (unsigned short *)*v4;
    if (((1 << v3[0x8A]) & v3[0xF8]) == 0) {
        p = fn_0238DE4C();
        p[0] = 7;
        p[1] = 6;
        fn_0238DEF4(p);
    } else {
        p = fn_0238FCF0(buf, v3[0x79]);
        v1 = p[2];
        if (v1 != 0) {
            fn_023908C0(0x212, v1);
        } else {
            p = fn_0238DE4C();
            p[0] = 7;
            p[1] = 0;
            fn_0238DEF4(p);
        }
    }
}
