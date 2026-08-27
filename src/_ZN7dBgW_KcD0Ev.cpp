//cpp
// @symbol _ZN7dBgW_KcD0Ev
/* Recovered deleting destructor. This is the same source-level destructor as
 * D1/D2; CodeWarrior also generates the inherited heap deallocation for D0.
 */
#include "dBgW_Kc.h"

extern "C" void func_02038224(void *clps);

dBgW_Kc::~dBgW_Kc()
{
    func_02038224(&clps);
}
