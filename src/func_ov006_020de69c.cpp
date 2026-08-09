//cpp
// @symbol func_ov006_020de69c
#include "dScMgCoin_c.h"

/* dScMgCoin_c::Behavior - recovered from ROM RTTI and vtable slot identity. */
typedef void (dScMgCoin_c::*PMF)();
struct Entry { PMF pmf; };
extern Entry data_ov006_02141810[];

extern "C" {
void func_ov006_020dc754(dScMgCoin_c *self);
void func_ov006_020dc298(dScMgCoin_c *self);
void func_ov006_020dc900(dScMgCoin_c *self);
void func_ov006_020dce3c(dScMgCoin_c *self);
int func_ov006_020de69c(dScMgCoin_c *self);
}

int func_ov006_020de69c(dScMgCoin_c *self)
{
    int idx = self->unk_51c8;
    (self->*data_ov006_02141810[idx].pmf)();
    func_ov006_020dc754(self);
    func_ov006_020dc298(self);
    func_ov006_020dc900(self);
    func_ov006_020dce3c(self);
    return 1;
}
