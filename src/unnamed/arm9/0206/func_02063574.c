#include "types.h"
extern int func_020614d0(void);
extern void WM_SetCallbackTable(int i, int val);
extern int WM_SendCommand(int a, int b, int c, int d, int e);

int func_02063574(int a, int b, int c, int d) {
    int r = func_020614d0();
    if (r != 0) return r;
    WM_SetCallbackTable(0x1d, a);
    r = WM_SendCommand(0x1d, 3, b, c, d);
    if (r == 0) return 2;
    return r;
}
