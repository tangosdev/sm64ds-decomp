//cpp
// @symbol _ZN7dBgW_KcC2Ev
/* Recovered base-object constructor. CodeWarrior generates the dBgW base
 * construction and dBgW_Kc vptr store; the body initializes the CLPS handle
 * and collision-file pointer. objisolate keeps C2 here.
 */
#include "dBgW_Kc.h"

extern "C" void func_02038228(void *clps);

dBgW_Kc::dBgW_Kc()
{
    func_02038228(&clps);
    kclFile = 0;
}
