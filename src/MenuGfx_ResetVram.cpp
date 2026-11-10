//cpp
#include "GX.h"

extern "C" {
extern void func_02054430(int x);
extern void MIi_CpuClear32(int val, int *dst, int len);
extern void MIi_CpuClearFast(unsigned val, void *dst, int len);

void MenuGfx_ResetVram(void) {
    volatile int a;
    volatile int b;
    volatile int c;

    GX::ResetBankForBG();
    GX::ResetBankForOBJ();
    GX::ResetBankForTex();
    GX::ResetBankForTexPltt();
    GX::ResetBankForSubBG();
    GX::ResetBankForSubOBJ();
    func_02054430(0x1ff);

    b = 0;
    MIi_CpuClear32(b, (int *)0x6800000, 0xa4000);

    GX::DisableBankForLCDC();

    a = 0xc0;
    MIi_CpuClearFast(a, (int *)0x7000000, 0x400);

    c = 0;
    MIi_CpuClear32(c, (int *)0x5000000, 0x400);
}
}
