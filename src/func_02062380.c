extern int WM_CheckStateEx(int a, int b);
extern void WM_SetCallbackTable(int i, int val);
extern int *func_02061548(void);
extern int WM_SendCommand(int a, int b, int c, int d, int e);

int func_02062380(int x)
{
    int r;
    int *p;
    r = WM_CheckStateEx(1, 0);
    if (r) return r;
    WM_SetCallbackTable(3, x);
    p = func_02061548();
    r = WM_SendCommand(3, 3, p[0], p[1], p[4]);
    if (r == 0) return 2;
    return r;
}
