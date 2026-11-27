extern int *dat_0380FFF4;
extern int fn_0238D7A0(unsigned int, int);
extern void fn_023822A8(void *, int, int, void *, volatile int *);
extern int fn_023957B8(int *);

int fn_023956D0(int a1)
{
    volatile int v3;
    int r;
    v3 = 1;
    fn_023822A8((char *)dat_0380FFF4 + 0x5EC,
                fn_0238D7A0((unsigned int)(a1 * 0x82EA) >> 6, 1000), 0,
                fn_023957B8, &v3);
    do {
        r = v3;
    } while (r != 0);
    return r;
}
