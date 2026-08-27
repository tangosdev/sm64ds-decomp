//cpp
// @symbol _ZN11FallBlockWf13InitResourcesEv
/* recovered: real C++ method */
/* FallBlockWf::InitResources() -- forwards `this` and the class's shared parameter block to
 * the generic implementation the family shares. */
/* Cross-overlay tail-call veneer. #pragma long_calls forces mwccarm to emit the
 * pooled `ldr ip,[pc]; bx ip` indirect tail-call (a plain near `b` otherwise)
 * that the ROM uses to reach another overlay. */
#include "FallBlockWf.h"
#pragma long_calls on

extern "C" {
int func_ov098_0213a794(void *thisp, void *data);
extern char data_ov015_02114880[];
}

int FallBlockWf::InitResources()
{
    return func_ov098_0213a794(this, data_ov015_02114880);
}
