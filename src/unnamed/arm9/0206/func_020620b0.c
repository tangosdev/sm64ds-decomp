#include "types.h"
extern int WM_CheckStateEx(int a, int b);
extern void func_02062024(void *o);
extern void WM_SetCallbackTable(int i, int val);
extern void _ZN4CP1514FlushDataCacheEjj(u32 a, u32 b);
extern int WM_SendCommand(int a, int b, void *c);

int func_020620b0(int self, char *o)
{
    int r;
    u16 t12;
    int add0, add1;
    r = WM_CheckStateEx(1, 2);
    if (r != 0) return r;
    if (o == 0) return 6;
    if (*(u16*)(o + 4) != 0) {
        if (*(int*)o == 0) return 6;
    }
    t12 = *(u16*)(o + 0x12);
    add0 = *(u16*)(o + 0x32) + (t12 != 0 ? 0x2a : 0);
    if (add0 > 0x200) goto ret6;
    add1 = *(u16*)(o + 0x34) + (t12 != 0 ? 6 : 0);
    if (add1 > 0x200) { ret6: return 6; }
    func_02062024(o);
    WM_SetCallbackTable(7, self);
    _ZN4CP1514FlushDataCacheEjj((u32)o, 0x40);
    if (*(u16*)(o + 4) != 0) {
        _ZN4CP1514FlushDataCacheEjj(*(u32*)o, *(u16*)(o + 4));
    }
    r = WM_SendCommand(7, 1, o);
    if (r == 0) return 2;
    return r;
}
