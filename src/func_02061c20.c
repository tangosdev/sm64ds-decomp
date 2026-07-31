typedef unsigned int u32;
typedef unsigned char u8;

extern int WM_GetSystemWork(void);
extern int func_0206152c(void);
extern void _ZN4CP1519InvalidateDataCacheEjj(unsigned int addr, unsigned int len);
extern void MultiCopy32Bytes(int *src, int *dst, int len);

int func_02061c20(int *arg)
{
    char *x = (char *)WM_GetSystemWork();
    int r = func_0206152c();
    if (r != 0) return r;
    if (arg == 0) return 6;
    _ZN4CP1519InvalidateDataCacheEjj(*(u32 *)(x + 4), 0x930);
    MultiCopy32Bytes(*(int **)(x + 4), arg, 0x930);
    return 0;
}
