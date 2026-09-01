//cpp
// @symbol _ZN17dScMgTrampoline_c8OnKickedEv
#include "dScMgTrampoline_c.h"
// recovered name: dScMgTrampoline_c_OnKicked
/* recovered: renamed to Class_Method */
/* dScMgTrampoline_c::OnKicked - recovered from vtable slot identity */
extern "C" {
int _ZN14dScMgD3DBase_c8OnKickedEv(char* self);
void SetBg2Offset(int a, int b);
int func_ov004_020b04c0(void);
}
extern "C" unsigned char data_0209d45c;

int dScMgTrampoline_c::OnKicked()
{
    char *self = (char *)this;

    if (!_ZN14dScMgD3DBase_c8OnKickedEv(self))
        return 0;
    if (*(int*)(self + 0x4628) == 0) {
        if (*(unsigned short*)(self + 0x4664) == 0) {
            data_0209d45c &= ~8;
            SetBg2Offset(0, *(int*)(self + 0x5d94) + *(int*)(self + 0x5da0));
            *(volatile int*)0x4000018 = (*(int*)(self + 0x5d94) + *(int*)(self + 0x5da0)) << 16 & 0x1ff0000;
        } else {
            data_0209d45c |= 8;
            SetBg2Offset(0, *(int*)(self + 0x5d94) + 0xc0 + *(int*)(self + 0x5da0) + func_ov004_020b04c0());
            *(volatile int*)0x4000018 = (*(int*)(self + 0x5d94) + 0xc0 + *(int*)(self + 0x5da0) + func_ov004_020b04c0()) << 16 & 0x1ff0000;
        }
    }
    return 1;
}
