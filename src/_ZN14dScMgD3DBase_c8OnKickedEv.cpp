//cpp
#include "dScMgD3DBase_c.h"
extern "C" {
    extern unsigned char data_0209d464;
    extern int data_ov006_02141a44;
    extern void func_ov006_020e7508(void);
    extern void func_ov006_020e759c(void);
}
// @symbol _ZN14dScMgD3DBase_c8OnKickedEv
// recovered name: dScMgJump2_c_OnKicked  -- WRONG, see below
/* recovered: renamed to Class_Method, declarations from a shared header */
/* dScMgD3DBase_c::OnKicked - slot 24.
 *
 * THE RECOVERED NAME ABOVE NAMES THE WRONG CLASS, and it is kept only so
 * the correction is visible.  This body is at ov006:0x020e6e78, and word
 * 24 of THREE vtables points here: _ZTV14dScMgD3DBase_c,
 * _ZTV11dScMgJump_c and _ZTV12dScMgJump2_c.  A body that appears in a
 * class's table AND in both its children's is supplied by that class, so
 * it is dScMgD3DBase_c::OnKicked and the two children inherit it.  The
 * recovered name was assigned from one child's table without looking at
 * the other two.
 *
 * The chain below is the same argument from the bodies' own calls: this
 * one calls dScMgBase_c's 0x020ae140, and dScMgTrampoline_c and
 * dScMgTrampoline2_c -- the other two children -- call THIS one. */
extern "C" int _ZN11dScMgBase_c8OnKickedEv(void* self);
extern "C" void Camera_UpdateMatrices(int arg);

extern "C" {
extern unsigned char data_0209f5f8;
}

int dScMgD3DBase_c::OnKicked()
{
    char *self = (char *)this;

    if (_ZN11dScMgBase_c8OnKickedEv(self) == 0) return 0;
    if (*(int*)(self + 0x4628) == 0) {
        if (data_0209d464 == 0) return 0;
        if (*(unsigned short*)(self + 0x4664) == 0)
            *(unsigned short*)(self + 0x4664) = 1;
        else
            *(unsigned short*)(self + 0x4664) = 0;
        int v = (int)(self + 0x466c + (*(unsigned short*)(self + 0x4664)) * 0xbc);
        data_ov006_02141a44 = v;
        Camera_UpdateMatrices(v);
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
