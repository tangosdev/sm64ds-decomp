extern int *dat_0380FFF4;
extern int fn_0238D594(int, int);
extern void fn_023822A8(void *, int, int, void *, volatile int *);
extern int fn_023957B8(int *);

int fn_02395744(int a1)
{
    volatile int v3;
    int v1;
    int r;
    v3 = 1;
    v1 = fn_0238D594(a1 * 0x82EA, 0x40);
    fn_023822A8((char *)dat_0380FFF4 + 0x5EC, v1, v1 >> 31, fn_023957B8, &v3);
    do {
        r = v3;
    } while (r != 0);
    return r;
}
