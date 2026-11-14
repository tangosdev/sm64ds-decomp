extern int *dat_0380FFF4;
extern void fn_02382C80(int, void *, int);
extern int fn_023A4340(int, int, void *);

int fn_02395A7C(void)
{
    volatile unsigned short v0;
    v0 = 0;
    fn_02382C80(v0, (char *)dat_0380FFF4 + 0x5B0, 0x10);
    fn_023A4340(0x40, 1, (char *)dat_0380FFF4 + 0x5B0);
    fn_023A4340(0x41, 1, (char *)dat_0380FFF4 + 0x5B2);
    fn_023A4340(0x42, 1, (char *)dat_0380FFF4 + 0x5B4);
    return fn_023A4340(0x43, 1, (char *)dat_0380FFF4 + 0x5B6);
}
