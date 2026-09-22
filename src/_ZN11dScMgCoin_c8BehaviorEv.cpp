//cpp
// @symbol _ZN11dScMgCoin_c8BehaviorEv
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
}

int dScMgCoin_c::Behavior()
{
    int idx = unk_51c8;
    (this->*data_ov006_02141810[idx].pmf)();
    func_ov006_020dc754(this);
    func_ov006_020dc298(this);
    func_ov006_020dc900(this);
    func_ov006_020dce3c(this);
    return 1;
}
