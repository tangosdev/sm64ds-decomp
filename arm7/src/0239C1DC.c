/* storage 0x0239C1DC runtime 0x027CE154 main ARM. */

extern int fn_023A43D8(int a);
extern void fn_0239763C(unsigned short *p);
extern void fn_02397508(unsigned int n, unsigned short *p);
extern void fn_023974A8(unsigned int n);
extern void fn_02397468(unsigned int n);
extern void fn_023970C8(unsigned int n, int zero);
extern void fn_0239707C(unsigned short *p);
extern void fn_02396C74(unsigned int n, unsigned short *p);
extern void fn_02395DA8(void);

int fn_0239C1DC(unsigned short *in, unsigned short *out) {
    char *base;
    unsigned int v;

    out[1] = 1;
    base = *(char **)0x0380FFF4;
    v = *(unsigned short *)(base + 0x2F2);
    if (v != 1 && v != 0)
        return 11;
    if (*(unsigned short *)(base + 0x310) != 0x20)
        return 1;
    v = in[8];
    if (v > 0x20u)
        return 5;
    if (v == 0)
        return 5;
    v = in[25];
    if (v < 10u)
        return 5;
    if (v > 0x3E8u)
        return 5;
    v = in[26];
    if (v == 0)
        return 5;
    if (v > 0xFFu)
        return 5;
    v = in[27];
    if ((v & 0xFFF0u) != 0)
        return 5;
    if (((1u << v) & *(unsigned short *)(base + 0x2F0)) == 0)
        return 5;
    {
        unsigned int hi;
        unsigned int field;

        hi = 0xFFFFF000u;
        field = in[28];
        if (field == 0)
            return 5;
        if ((field & hi) != 0)
            return 5;
        field = in[29];
        if (field == 0)
            return 5;
        if ((field & hi) != 0)
            return 5;
    }
    if (in[30] > 0x80u)
        return 5;
    if (fn_023A43D8(0) != 0)
        return 14;
    base = *(char **)0x0380FFF4;
    if (*(unsigned short *)(base + 0x2F2) == 0)
        fn_0239763C((unsigned short *)0x027D64F4);
    else
        fn_0239763C((unsigned short *)(base + 0x2E8));
    fn_02397508(in[8], in + 9);
    fn_023974A8(in[25]);
    fn_02397468(in[26]);
    fn_023970C8(in[27], 0);
    fn_0239707C(in + 28);
    fn_02396C74(in[30], in + 31);
    *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0xA0) = 0;
    fn_02395DA8();
    return 0;
}
