#include "private/wm_views.h"
extern void WM_SetCallbackTable(int i, int val);

int func_0206259c(int a, int b)
{
    int r;
    WM_GetSystemWork();
    r = WM_CheckStateEx(1, 9);
    if (r != 0)
        return r;
    WM_SetCallbackTable(0x23, a);
    r = WM_SendCommand(0x23, 1, b);
    if (r == 0)
        r = 2;
    return r;
}
