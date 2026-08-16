// @symbol func_ov006_021071fc
// recovered name: dScMgPanel_c_OnYoshiTryEat_021071fc
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
extern void FreeGfxSlotsById(int arg);
extern void func_ov006_021067a4(char *p);
extern void func_ov006_021063a0(char *p);
extern void func_ov006_02106168(char *p);

extern char *data_ov004_020beb68;
extern unsigned char data_0209d454;

void func_ov006_021071fc(char *self, int flag)
{
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
            *(int *)(data_ov004_020beb68 + 0xb4) = 0;

        p = data_ov004_020beb68;
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
