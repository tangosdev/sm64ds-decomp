//cpp
// @symbol _ZN4dBgW6EnableEP8dActor_c
#include "dBgW.h"
extern "C" {
void func_020395fc(dBgW *self, dActor_c *actor);
void func_020393fc(dBgW *self, int slot);
extern dBgW *data_020a0c80[];
}

int dBgW::Enable(dActor_c *actor)
{
    s32 i = 0;
    for (;;) {
        if (data_020a0c80[i] == 0) {
            func_020395fc(this, actor);
            func_020393fc(this, i);
            data_020a0c80[i] = this;
            return 1;
        }
        i++;
        if (i >= 0x18)
            break;
    }
    return 0;
}
