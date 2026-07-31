extern int WM_CheckStateEx(int a, int b);
extern void _ZN4CP1514FlushDataCacheEjj(unsigned int a, unsigned int b);
extern void WM_SetCallbackTable(int i, int val);
extern int *func_02061548(void);
extern void func_0205a61c(void *src, void *dst, unsigned int size);
extern void func_0205a588(void *dst, int v, unsigned int size);
extern int WM_SendCommand(int a, int b);

int func_02061e38(int a, int b, void *c)
{
    int r;
    int *p;
    int *q;
    r = WM_CheckStateEx(1, 2);
    if (r != 0) return r;
    if (b == 0) return 6;
    _ZN4CP1514FlushDataCacheEjj(b, 0xc0);
    WM_SetCallbackTable(0xc, a);
    p = func_02061548();
    q = (int*)p[3];
    *(short*)q = 0xc;
    q[1] = b;
    if (c != 0) {
        func_0205a61c(c, (char*)q + 8, 0x18);
    } else {
        func_0205a588((char*)q + 8, 0, 0x18);
    }
    r = WM_SendCommand(0xc, 0);
    return (r == 0) ? 2 : r;
}
