//cpp
#include "types.h"
#include "dScMgTrampoline2_c.h"
// @symbol _ZN18dScMgTrampoline2_c11OnAttacked2Ev
// recovered name: dScMgTrampoline2_c_OnAttacked2
/* recovered: renamed to Class_Method */
/* dScMgTrampoline2_c::OnAttacked2 - recovered from vtable slot identity */
extern "C" {
extern char* data_0209f5bc;
extern int data_ov006_0213fbc4;
}

extern "C" {
int _ZN11dScMgBase_c11OnAttacked2Ev(void);
unsigned int func_02012790(unsigned int a);
int func_ov006_020d0c38(u16* a, u16* b);
void func_02012718(void* a, int b);
int func_02054d88(void);
void MultiStore16(u16 val, char* dst, int nbytes);
}

int dScMgTrampoline2_c::OnAttacked2()
{
    char *self = (char *)this;

    u16 buf[5];

    if (!_ZN11dScMgBase_c11OnAttacked2Ev())
        return 0;

    {
        char* obj = data_0209f5bc;
        int (*vf)(char*) = *(int (**)(char*))(*(char**)obj + 0x14);
        if (vf(obj)) {
            data_ov006_0213fbc4 = 0;
        } else {
            char* obj2 = data_0209f5bc;
            int (*vf2)(char*) = *(int (**)(char*))(*(char**)obj2 + 0x1c);
            if (vf2(obj2))
                data_ov006_0213fbc4 = 1;
        }
    }

    if (data_ov006_0213fbc4 != 0)
        return 0;

    if (*(int*)(self + 0x4628) != 0)
        return 0;

    if (*(u8*)(self + 0x7bab) != 0) {
        s16 a = *(volatile s16*)(self + 0x7ba0);
        if ((a < 0x18 && *(volatile s16*)(self + 0x7b9c) < 0x18) ||
            (a > 0xe8 && *(volatile s16*)(self + 0x7b9c) > 0xe8)) {
            func_02012790(0xe);
        } else {
            buf[1] = *(s16*)(self + 0x7ba0);
            buf[2] = *(s16*)(self + 0x7ba2);
            buf[3] = *(s16*)(self + 0x7b9c);
            buf[4] = *(s16*)(self + 0x7b9e);
            if (func_ov006_020d0c38(&buf[1], &buf[3])) {
                func_02012718((void*)0x1af, (*(s16*)(self + 0x7ba0) + *(s16*)(self + 0x7b9c)) << 11);
            } else {
                func_02012790(0xe);
            }
        }
        {
            char* dst = (char*)func_02054d88();
            buf[0] = 0;
            MultiStore16(buf[0], dst, 0x6000);
        }
        *(u8*)(self + 0x7baa) = 0;
        *(u8*)(self + 0x7bab) = 0;
    }
    return 1;
}
