//cpp
// @symbol _ZN12dScMgPanel_c13OnYoshiTryEatEi
// recovered name: dScMgPanel_c_OnYoshiTryEat_021071fc
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
#include "dScMgPanel_c.h"
/* recovered: renamed to Class_Method, vtable slot 18 -- an override of
   dScMgBase_c::OnYoshiTryEat(int). The signature must repeat the base
   declaration exactly, or mwcc appends a slot instead of overriding. */
extern "C" {
extern void FreeGfxSlotsById(int arg);
extern void func_ov006_021067a4(char *p);
extern void func_ov006_021063a0(char *p);
extern void func_ov006_02106168(char *p);
}

extern "C" {
extern unsigned char data_0209d454;
}

void dScMgPanel_c::OnYoshiTryEat(int flag)
{
    char *self = (char *)this;

    char *p;

    if (flag == 0) {
        int *q = (int *)(((int)self + 0xbc));
        *q += 1;
        if (*(unsigned int *)(self + 0xbc) > 0x270e)
            *(int *)(self + 0xbc) = 0x270e;
    } else {
        *(unsigned char *)(self + 0x4fea) = 0;
        *(int *)(self + 0xbc) = 0;
        if (*(unsigned int *)(self + 0xbc) > 0x270e)
            *(int *)(self + 0xbc) = 0x270e;

        if (data_ov004_020beb68 != 0)
            *(int *)((char *)data_ov004_020beb68 + 0xb4) = 0;

        p = (char *)data_ov004_020beb68;
        func_ov004_020adb1c(p != 0 ? *(int *)(p + 0xb4) : 0);
    }

    FreeGfxSlotsById(0x1d);
    func_ov006_021067a4(self);
    func_ov006_021063a0(self);
    func_ov006_02106168(self);
    func_ov006_02104b24(self);

    *(unsigned char *)(self + 0x4fe2) = 3;
    *(int *)(self + 0x4ca8) = 0;
    func_ov004_020b0cac(0xd, 0x80, 0x40, 0, -1, 0xd);
    SetSubBg0Offset(0, 0);

    data_0209d454 &= ~1;
}
