typedef unsigned int u32;
typedef unsigned short u16;

extern int *func_02061548(void);
extern int func_02061428(int a, int b, int c);
extern void _ZN4CP1519InvalidateDataCacheEjj(u32 addr, u32 size);

u32 func_02061b9c(void)
{
    int *r4;
    int *p;

    r4 = func_02061548();
    if (func_02061428(2, 7, 8) != 0)
        return 0;

    _ZN4CP1519InvalidateDataCacheEjj((u32)(r4[1] + 0x10), 4);

    p = (int *)r4[1];
    if (p[4] == 1)
        return 0;

    _ZN4CP1519InvalidateDataCacheEjj((u32)((char *)p + 0x18e), 4);

    return (*(u16 *)((char *)r4[1] + 0x100 + 0x8e) + 0x1f) & ~0x1f;
}
