//cpp
// @symbol func_ov006_020e6e78
// @emits dScMgJump2_c_OnKicked
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgJump2_c::OnKicked - recovered from vtable slot identity */
extern "C" int func_ov004_020ae140(void* self);
extern "C" void func_ov006_020c0134(int arg);

extern unsigned char data_0209f5f8;

extern "C" int dScMgJump2_c_OnKicked(char* self)
{
    if (func_ov004_020ae140(self) == 0) return 0;
    if (*(int*)(self + 0x4628) == 0) {
        if (data_0209d464 == 0) return 0;
        if (*(unsigned short*)(self + 0x4664) == 0)
            *(unsigned short*)(self + 0x4664) = 1;
        else
            *(unsigned short*)(self + 0x4664) = 0;
        int v = (int)(self + 0x466c + (*(unsigned short*)(self + 0x4664)) * 0xbc);
        data_ov006_02141a44 = v;
        func_ov006_020c0134(v);
        if (*(unsigned short*)(self + 0x4664) == 1) {
            func_ov006_020e7508();
        } else {
            func_ov006_020e759c();
            if (data_0209f5f8 == 0) {
                *(int*)0x4001000 &= ~0xe000;
                data_0209f5f8 = 1;
            }
        }
    }
    return 1;
}
