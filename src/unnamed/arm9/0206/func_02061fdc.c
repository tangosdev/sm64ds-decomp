extern int WM_CheckStateEx(int a, int b);
extern void WM_SetCallbackTable(int i, int val);
extern int WM_SendCommand(int a, int b);

int func_02061fdc(int this)
{
    int r;
    r = WM_CheckStateEx(1, 2);
    if (r != 0) return r;
    WM_SetCallbackTable(8, this);
    r = WM_SendCommand(8, 0);
    if (r == 0) return 2;
    return r;
}
