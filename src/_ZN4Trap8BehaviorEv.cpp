//cpp
// @symbol _ZN4Trap8BehaviorEv
/* Trap::Behavior -- vtable slot 6, ov010 0x021115e0. Attributed by the
 * vtable: _ZTV4Trap carries this address at slot 6, where _ZTV10dBgActor_c
 * carries fBase_c's generic 0x02043b24. */
#include "Trap.h"

typedef void (Trap::*PMF)();
extern "C" void func_ov010_0211146c(Trap* self);
extern "C" void func_ov010_021113f0(Trap* self);
extern PMF data_ov010_02112d28[];

int Trap::Behavior()
{
    if (mIsSpawner) {
        mPlayerDist = DistToCPlayer();
        return 1;
    }
    (this->*data_ov010_02112d28[mState])();
    func_ov010_0211146c(this);
    func_ov010_021113f0(this);
    return 1;
}
