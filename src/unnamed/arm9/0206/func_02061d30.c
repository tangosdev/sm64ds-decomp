#include "types.h"
extern int WM_GetSystemWork(void);
extern int WM_CheckStateEx(int count, int a, int b, int c, int d, int e);
extern void _ZN4CP1519InvalidateDataCacheEjj(u32 a, u32 b);
extern void WM_SetCallbackTable(int i, int val);
extern int WM_SendCommand(int a, int b, int c);

int func_02061d30(int self, unsigned int idx)
{
    int h;
    int* o;
    int r;
    h = WM_GetSystemWork();
    r = WM_CheckStateEx(5, 7, 9, 8, 0xa, 0xb);
    if (r != 0) return r;
    o = *(int**)(h + 4);
    {
        u16 v = *(u16*)o;
        if (v == 7 || v == 9) {
            if (idx < 1) goto ret6;
            if (idx > 0xf) { ret6: return 6; }
            _ZN4CP1519InvalidateDataCacheEjj((u32)o + 0x17e, 2);
            if ((*(u16*)((char*)*(int**)(h + 4) + 0x17e) & (1u << idx)) == 0) return 7;
        } else {
            if (idx != 0) return 6;
        }
    }
    WM_SetCallbackTable(0xd, self);
    r = WM_SendCommand(0xd, 1, 1u << idx);
    if (r == 0) return 2;
    return r;
}
