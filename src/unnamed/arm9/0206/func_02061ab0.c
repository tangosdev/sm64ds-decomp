extern int WM_GetSystemWork(void);
extern int WM_CheckStateEx(int count, ...);
extern void _ZN4CP1519InvalidateDataCacheEjj(unsigned int a, unsigned int b);

struct H { char p0[4]; char *f4; };

int func_02061ab0(void)
{
    struct H *h = (struct H *)WM_GetSystemWork();
    char *p;
    int flag;
    int v;

    if (WM_CheckStateEx(2, 7, 8) != 0) return 0;

    _ZN4CP1519InvalidateDataCacheEjj((unsigned int)(h->f4 + 0x10), 4);
    p = h->f4;
    if (*(int *)(p + 0x10) == 1) return 0;

    _ZN4CP1519InvalidateDataCacheEjj((unsigned int)(h->f4 + 0x184), 2);
    p = h->f4;
    flag = (*(unsigned short *)(p + 0x184) == 0);

    _ZN4CP1519InvalidateDataCacheEjj((unsigned int)(p + 0x190), 2);
    p = h->f4;
    v = *(unsigned short *)(p + 0x190);
    if (flag != 1) return (((v + 0x51) & ~0x1f) << 1);

    _ZN4CP1519InvalidateDataCacheEjj((unsigned int)(p + 0xf2), 2);
    p = h->f4;
    return (((((v + 0xc) * *(unsigned short *)(p + 0xf2)) + 0x29) & ~0x1f) << 1);
}
