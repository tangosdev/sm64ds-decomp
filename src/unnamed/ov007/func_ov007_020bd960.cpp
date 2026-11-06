//cpp
#include "GX.h"

extern "C" {
extern void func_02054430(int x);
extern void MultiStore_Int(int val, int *dst, int len);
extern void MultiStore32Bytes(unsigned val, void *dst, int len);

void func_ov007_020bd960(void) {
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
    MultiStore_Int(b, (int *)0x6800000, 0xa4000);

    GX::DisableBankForLCDC();

    a = 0xc0;
    MultiStore32Bytes(a, (int *)0x7000000, 0x400);

    c = 0;
    MultiStore_Int(c, (int *)0x5000000, 0x400);
}
}
