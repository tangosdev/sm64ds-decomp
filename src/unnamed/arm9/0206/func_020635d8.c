extern int func_020614d0(void);
extern void WM_SetCallbackTable(int i, int val);
extern int WM_SendCommand(int a, int b, int c);

int func_020635d8(int a, int b)
{
    int r = func_020614d0();
    if (r != 0) return r;
    if (b != 0 && b != 1) return 6;
    WM_SetCallbackTable(0x19, a);
    r = WM_SendCommand(0x19, 1, b);
    if (r == 0) return 2;
    return r;
}
