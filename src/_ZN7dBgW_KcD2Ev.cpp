//cpp
// @symbol _ZN7dBgW_KcD2Ev
/* Recovered base-object destructor. The body releases the CLPS handle;
 * CodeWarrior generates the dBgW_Kc vptr restore and dBgW base destruction.
 * objisolate keeps D2 here.
 */
#include "dBgW_Kc.h"

extern "C" void func_02038224(void *clps);

dBgW_Kc::~dBgW_Kc()
{
    func_02038224(&clps);
}
