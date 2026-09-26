//cpp
// @symbol _ZN16dScMgPachinko2_c8BehaviorEv
// recovered name: dScMgPachinko2_c_Behavior
#include "dScMgPachinko2_c.h"
/* Slot 6. unk_5660 selects which helpers run this frame. */
extern "C" {
extern void func_ov006_02100084(void *c);
extern void func_ov006_021024e0(void *c);
extern void func_ov006_020fffec(void *c);
extern void func_ov006_0210265c(void *c);
extern void func_ov006_02102ef4(void *c);
extern void func_ov006_02103ac0(void *c);
extern void func_ov006_02102e8c(void *c);
extern void func_ov006_0210246c(void *c);
extern void func_ov006_020fff84(void *c);
extern void func_ov006_02100380(void *c);
extern void func_ov006_0210258c(void *c);
extern void func_ov006_02100554(void *c);
extern void func_ov006_021001ac(void *c);
extern void func_ov004_020b0a54(void *c);
}

s32 dScMgPachinko2_c::Behavior()
{
    switch (unk_5660) {
    case 0:
        func_ov006_02100084(this);
        func_ov006_021024e0(this);
        func_ov006_020fffec(this);
        unk_566c = 0x10;
        unk_5660 = 1;
        break;
    case 1:
        if (mPromptBlinkCount == 0) {
            mPromptEnabled = 1;
            mPromptBlinkCount = 1;
            mPromptBlinkTimer = 0;
        }
        func_ov006_0210265c(this);
        func_ov006_02102ef4(this);
        func_ov006_02103ac0(this);
        func_ov006_02102e8c(this);
        func_ov006_0210246c(this);
        func_ov006_020fff84(this);
        func_ov006_02100380(this);
        func_ov006_0210258c(this);
        break;
    case 2:
        func_ov006_02102e8c(this);
        func_ov006_0210246c(this);
        func_ov006_020fff84(this);
        func_ov006_02100380(this);
        func_ov006_02100554(this);
        func_ov006_021001ac(this);
        break;
    case 3:
        func_ov006_02102e8c(this);
        func_ov006_0210246c(this);
        func_ov006_020fff84(this);
        func_ov006_021001ac(this);
        if (unk_566e != 0) {
            unk_566e--;
            if ((s16)unk_566e <= 0) {
                func_ov004_020b0a54((void *)0x10);
                mPromptEnabled = 0;
            }
        }
        break;
    }
    return 1;
}
