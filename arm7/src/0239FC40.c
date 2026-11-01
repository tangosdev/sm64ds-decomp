extern int *dat_0380FFF4;
extern int fn_02380884(int);
extern int fn_023808C0(int);
extern void fn_02399B30(void);

int fn_0239FC40(void)
{
    int v0 = fn_02380884(0x1000000);
    *(unsigned short *)0x48080B4 = 2;
    if (((unsigned short *)((int)dat_0380FFF4 + 0x400))[0x10] != 0)
        fn_02399B30();
    return fn_023808C0(v0);
}
