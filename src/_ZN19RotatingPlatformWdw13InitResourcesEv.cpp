//cpp
// @symbol _ZN19RotatingPlatformWdw13InitResourcesEv
/* recovered: real C++ method */
/* RotatingPlatformWdw::InitResources() -- forwards `this` and the class's shared parameter block to
 * the generic implementation the family shares. */
#include "RotatingPlatformWdw.h"


extern "C" {
int func_ov002_020b676c(void *self, void *a, short arg2);
extern short data_ov029_02113fc4;
extern void *data_ov029_02113fd4;
}

int RotatingPlatformWdw::InitResources()
{
    return func_ov002_020b676c(this, &data_ov029_02113fd4, data_ov029_02113fc4);
}
