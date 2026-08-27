// @symbol func_ov006_020f3294
// recovered name: dScMgLuigi_c_OnYoshiTryEat_020f3294
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
extern void FreeGfxSlotsById(int arg);
extern void func_ov006_020f2ec0(char *c);

extern char *data_ov004_020beb68;
extern unsigned char data_0209d45c;
extern unsigned char data_0209d454;

void func_ov006_020f3294(char *c, int arg1)
{
    char *p;
    int *q;

    if (*(unsigned char *)(c + 0x5000 + 0x459) != 0) {
        *(unsigned char *)(((int)c + 0x5457)) += 1;

        q = (int *)(((int)c + 0xbc));
        *q += 1;
        if ((unsigned int)*(int *)(c + 0xbc) > 0x270e)
            *(int *)(c + 0xbc) = 0x270e;
    }

    if (arg1 == 0x12) {
        *(short *)(c + 0x5100 + 0x72) = 0xa;
        *(int *)(c + 0xbc) = 0;
        if ((unsigned int)*(int *)(c + 0xbc) > 0x270e)
            *(int *)(c + 0xbc) = 0x270e;

        if (data_ov004_020beb68 != 0)
            *(int *)(data_ov004_020beb68 + 0xb4) = 0;

        p = data_ov004_020beb68;
        func_ov004_020adb1c(p != 0 ? *(int *)(p + 0xb4) : 0);
    }

    FreeGfxSlotsById(0x1d);
    func_ov006_020f2ec0(c);
    func_ov006_020f2e20(c);

    *(int *)(c + 0x4000 + 0xf78) = 0;
    data_0209d45c |= 4;
    data_0209d454 &= ~4;

    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
}
