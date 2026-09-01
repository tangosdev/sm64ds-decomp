//cpp
// @symbol _ZN18dScMgTrampoline2_c8OnKickedEv
// recovered name: dScMgTrampoline2_c_OnKicked
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
#include "dScMgTrampoline2_c.h"
/* recovered: renamed to Class_Method */
/* dScMgTrampoline2_c::OnKicked - recovered from vtable slot identity */
extern "C" {
extern unsigned char data_0209d45c[];
}

int dScMgTrampoline2_c::OnKicked()
{
    void *thiz = (void *)this;

    unsigned char *c = (unsigned char *)thiz;
    if (!_ZN14dScMgD3DBase_c8OnKickedEv(c)) return 0;
    if (*(int *)(c + 0x4628) == 0) {
        if (*(unsigned short *)(c + 0x4664) == 0) {
            data_0209d45c[0] = data_0209d45c[0] & ~8;
            SetBg2Offset(0, 0);
            *(int *)0x4000018 = 0;
        } else {
            data_0209d45c[0] = data_0209d45c[0] | 8;
            SetBg2Offset(0, 0xc0);
            *(int *)0x4000018 = 0xc00000;
        }
    }
    return 1;
}
