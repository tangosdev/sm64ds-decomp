//cpp
// @symbol _ZN14daObjC1_Trap_c8BehaviorEv
/* daObjC1_Trap_c::Behavior -- vtable slot 6, ov010 0x021115e0. Attributed by the
 * vtable: _ZTV14daObjC1_Trap_c carries this address at slot 6, where _ZTV10dBgActor_c
 * carries fBase_c's generic 0x02043b24. */
#include "daObjC1_Trap_c.h"

typedef void (daObjC1_Trap_c::*PMF)();
extern "C" void func_ov010_0211146c(daObjC1_Trap_c* self);
extern "C" void func_ov010_021113f0(daObjC1_Trap_c* self);
extern PMF data_ov010_02112d28[];

int daObjC1_Trap_c::Behavior()
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
