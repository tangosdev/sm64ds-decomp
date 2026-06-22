extern int func_0206152c(void);
extern void _ZN4CP1519InvalidateDataCacheEjj(unsigned int addr, unsigned int n);
extern unsigned int *data_020a89ac;

int func_020614d0(void)
{
    int r = func_0206152c();
    if (r != 0) return r;
    _ZN4CP1519InvalidateDataCacheEjj(data_020a89ac[1], 2);
    return *(unsigned short *)data_020a89ac[1] <= 1 ? 3 : 0;
}
