//cpp
// @symbol _ZN4dBgW7DisableEv
#include "dBgW.h"
extern "C" {
int func_02039404(dBgW *self);
void func_020393f0(dBgW *self);
extern dBgW *data_020a0c80[];
}

int dBgW::Disable()
{
    int i = func_02039404(this);
    if (i != 0x18) {
        func_020393f0(this);
        data_020a0c80[i] = 0;
    }
    return 1;
}
