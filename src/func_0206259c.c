extern int func_02061548(void);
extern int WM_CheckStateEx(int a, int b);
extern void WM_SetCallbackTable(int i, int val);
extern int WM_SendCommand(int a, int b, int c);

int func_0206259c(int a, int b)
{
    int r;
    func_02061548();
    r = WM_CheckStateEx(1, 9);
    if (r != 0)
        return r;
    WM_SetCallbackTable(0x23, a);
    r = WM_SendCommand(0x23, 1, b);
    if (r == 0)
        r = 2;
    return r;
}
