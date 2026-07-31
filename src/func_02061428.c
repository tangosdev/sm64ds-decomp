typedef unsigned short u16;

extern int func_0206152c(void);
extern void _ZN4CP1519InvalidateDataCacheEjj(unsigned int addr, unsigned int n);
extern unsigned int *data_020a89ac;

int func_02061428(int count, ...)
{
    int r;
    int *p;
    u16 target;
    r = func_0206152c();
    if (r != 0)
        return r;
    _ZN4CP1519InvalidateDataCacheEjj(data_020a89ac[1], 2);
    target = *(u16 *)data_020a89ac[1];
    p = (int *)((unsigned int)&count & ~3) + 1;
    r = 3;
    if (count == 0)
        return 3;
    do {
        p++;
        if (p[-1] == target)
            r = 0;
    } while (--count != 0);
    return r;
}
