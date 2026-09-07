//cpp
// @symbol _ZN11dScMgBase_cC2Ev
#include "dScMgBase_c.h"

extern "C" {
int func_ov004_020adc3c(void *scene);
int func_02013580(int value, int arg);
void func_ov004_020adc00(int value);

extern char data_0209b308;
extern short data_ov004_020bc070[];
}

dScMgBase_c::dScMgBase_c()
    : unk_0a4(0), unk_0c2(1)
{
    mMenuOpen = 0;
    unk_462c = 0;
    unk_4630 = 0;
    unk_4648 = 0;

    data_ov004_020beb68 = this;
    unk_0bc = *(u32 *)(&data_0209b308 + 0x30);

    mSceneKind = data_ov004_020bc070[(param1 >> 16) & 0xff];
    *(s32 *)((char *)this + 8) =
        *(u32 *)((char *)this + 8) & 0xffff;
    unk_050 = 0;
    unk_054 = 0;

    if (actorID == 0x16e || actorID == 0x185 ||
        actorID == 0x16d || actorID == 0x182)
        mTimeLimit = 0x3c;
    else
        mTimeLimit = 0x78;

    unk_060 = 0;
    mFrameCounter = 0;
    unk_464c = 0;
    unk_4654 = 0;
    unk_4658 = 0;
    unk_064 = -1;

    func_ov004_020adc00(func_02013580(func_ov004_020adc3c(this), 0));
}
