extern int *dat_0380FFF4;
extern int fn_02380884(int);
extern int fn_023808C0(int);

int fn_0239FC90(void)
{
    int v0 = (int)dat_0380FFF4 + 0x3E4;
    int v1 = fn_02380884(0x1000000);
    *(unsigned short *)0x48080B4 = 0xC0;
    *(unsigned short *)(v0 + 0x50) = 0;
    *(unsigned short *)(v0 + 0x64) = 0;
    return fn_023808C0(v1);
}
