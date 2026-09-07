//cpp
// @symbol _ZN6Cannon8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "Cannon.h"

extern "C" int func_ov098_0213a984(Cannon *c);

struct CannonStateEntry {
    void (Cannon::*state)();
};
extern CannonStateEntry data_ov098_0213c8fc[];

int Cannon::Behavior()
{
    if (mVariant != 1)
        (this->*data_ov098_0213c8fc[mState].state)();

    func_ov098_0213a984(this);
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}
