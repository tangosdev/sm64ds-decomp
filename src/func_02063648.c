#include "types.h"
extern int data_020a9440[];
extern int WM_CheckStateEx(int count, ...);
extern void MultiCopy_Int(int *dst, int *src, int len);
extern void _ZN4CP1514FlushDataCacheEjj(unsigned int a, unsigned int b);
extern void WM_SetCallbackTable(int i, int val);
extern int WM_SendCommand(int a, int b, int c, int d, int e, int f, int g, int h);

int func_02063648(int a0, int *src, int len, int a3, u16 arg5, u16 arg6, u8 arg7)
{
    int ret = WM_CheckStateEx(2, 7, 9);
    if (ret != 0) return ret;
    if (src == 0) return 6;
    if ((unsigned int)len > 0x70) return 6;
    MultiCopy_Int(src, data_020a9440, len);
    _ZN4CP1514FlushDataCacheEjj((unsigned int)data_020a9440, len);
    WM_SetCallbackTable(0x18, a0);
    ret = WM_SendCommand(0x18, 6, (int)data_020a9440, len, a3, arg5, arg6, arg7);
    if (ret == 0) ret = 2;
    return ret;
}
