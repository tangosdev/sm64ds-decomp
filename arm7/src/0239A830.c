extern int *dat_0380FFF4;
extern int fn_02380884(int);
extern void fn_023808C0(int);
extern void fn_02396CC0(void);

int fn_0239A830(int a1)
{
    unsigned int i;
    int h;
    unsigned int m;
    int **w;
    unsigned short *p;
    unsigned short *q;
    unsigned short *e;

    h = fn_02380884(0x1000000);
    i = 1;
    m = 2;
    w = &dat_0380FFF4;
    while (i < 0x10) {
        p = (unsigned short *)((int)*w + 0x400);
        if ((p[0x79] & m) == 0) {
            q = (unsigned short *)((int)*w + 0x4F2);
            *q |= m;
            ++*(unsigned short *)((int)*w + 0x4F0);
            if (((unsigned short *)((int)*w + 0x400))[0x78] == 1)
                fn_02396CC0();
            e = (unsigned short *)((char *)*(int *)((int)dat_0380FFF4 + 0x2E0)
                                   + 0x1C * a1);
            e[1] = i;
            fn_023808C0(h);
            return i;
        }
        ++i;
        m <<= 1;
    }
    fn_023808C0(h);
    return 0;
}
