//cpp
// @symbol _ZN12dScMgSlot1_c9betIcon_c8BehaviorEv
#include "dScMgSlot1_c.h"

extern "C" {
int TouchArea_Update(void *c, int x);
void func_ov006_0210c2d4(void *c);
void func_ov004_020b1b40(int x);
void _ZN5Sound12PlayBank2_2DEj(unsigned int);
}

void dScMgSlot1_c::betIcon_c::Behavior()
{
    int val;
    if (TouchArea_Update(this, -1))
        func_ov006_0210c2d4(this);
    val = unk_020;
    if (val != 0) {
        if ((val & 3) == 0) {
            func_ov004_020b1b40(1);
            _ZN5Sound12PlayBank2_2DEj(0x149);
        }
        unk_020 -= 1;
    }
    dThIcon_c::Behavior();
}
