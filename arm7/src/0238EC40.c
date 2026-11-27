extern int *fn_0238E688(void);
extern int fn_0238E184(int);
extern int fn_0238E158(void);
extern int fn_0238E0E4(void);
extern int fn_0238E500(void *);
extern void fn_02382DC0(void *, void *, unsigned int);
extern void fn_0238E57C(void *, void *);
extern short *fn_0238DE4C(void);
extern int fn_0238DEF4(short *);

#pragma opt_propagation off

void fn_0238EC40(unsigned char *a1) {
    int *o = fn_0238E688();
    unsigned char *s = **(unsigned char ***)((char *)o + 0x1610);
    fn_0238E688();
    fn_0238E184(a1[0x1F]);
    fn_0238E158();
    *(short *)(s + 0xB8) = fn_0238E0E4();
    a1 += 0x10;
    if (fn_0238E500(a1 + 0x1E) == 1)
        return;
    *(unsigned short *)(s + 0xAA) ^= 1;
    s = *(unsigned char **)(s + *(unsigned short *)(s + 0xAA) * 4 + 0xAC);
    fn_02382DC0(a1, s, (*(unsigned short *)(a1 + 6) + 0x2D) & ~1);
    fn_0238E57C(s + 0x18, a1 + 0x18);
    fn_0238E57C(s + 0x1E, a1 + 0x1E);
    {
        short *msg = fn_0238DE4C();
        msg[0] = 0x11;
        msg[1] = 0;
        msg[2] = 0xF;
        *(void **)(msg + 4) = s;
        fn_0238DEF4(msg);
    }
}
