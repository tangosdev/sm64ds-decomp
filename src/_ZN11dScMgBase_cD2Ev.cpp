//cpp
// @symbol _ZN11dScMgBase_cD2Ev
/* recovered: real C++ base-subobject destructor
 *
 * The ordinary destructor body clears the shared minigame pointer and tears
 * down the touch-icon array. CodeWarrior supplies this class's vptr restore
 * and the dScene_c/fBase_c base-subobject teardown for the D2 variant.
 */
#include "dScMgBase_c.h"

dScMgBase_c::~dScMgBase_c()
{
    data_ov004_020beb68 = 0;
    func_ov004_020b929c((char *)this + 0xf4);
}
