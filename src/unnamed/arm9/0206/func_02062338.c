extern int WM_CheckStateEx(int a, int b);
extern void WM_SetCallbackTable(int i, int val);
extern int WM_SendCommand(int a, int b);

int func_02062338(int this)
{
    int r;
    r = WM_CheckStateEx(1, 1);
    if (r != 0) return r;
    WM_SetCallbackTable(4, this);
    r = WM_SendCommand(4, 0);
    if (r == 0) return 2;
    return r;
}
