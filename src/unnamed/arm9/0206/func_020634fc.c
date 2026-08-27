extern int *WM_GetSystemWork(void);
extern int WM_CheckStateEx(int a, int b);
extern void WM_SetCallbackTable(int i, int val);
extern int WM_SendCommand(int a, int b);

int func_020634fc(int a, int b, int c, int d, unsigned short e)
{
    int *p;
    short *hdr;
    int r;
    p = WM_GetSystemWork();
    r = WM_CheckStateEx(1, 2);
    if (r) return r;
    WM_SetCallbackTable(0x1e, a);
    hdr = (short *)(p[3]);
    hdr[0] = 0x1e;
    hdr[1] = b;
    hdr[2] = c;
    hdr[3] = d;
    hdr[4] = e;
    r = WM_SendCommand(0x1e, 0);
    if (r == 0) return 2;
    return r;
}
