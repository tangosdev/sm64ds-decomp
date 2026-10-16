extern int *dat_0380FFF4;
extern int fn_02380884(int);
extern int fn_023808C0(int);
extern void fn_0239F7F8(int, int);
extern void fn_0239FB20(int);

int fn_0239FB90(void)
{
    unsigned short *v0 = (unsigned short *)((int)dat_0380FFF4 + 0x3E4);
    int v1 = fn_02380884(0x1000000);
    if (((unsigned short *)((int)dat_0380FFF4 + 0x300))[0xA] == 1) {
        *(unsigned short *)0x48080B4 = 9;
        if (v0[20] != 0)
            fn_0239FB20(2);
        fn_0239F7F8(2, 1);
    } else {
        *(unsigned short *)0x48080B4 = 1;
    }
    if (*v0 != 0)
        fn_0239FB20(0);
    fn_0239F7F8(0, 1);
    return fn_023808C0(v1);
}
