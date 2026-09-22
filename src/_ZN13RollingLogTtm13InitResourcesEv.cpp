//cpp
// @symbol _ZN13RollingLogTtm13InitResourcesEv
/* recovered: real C++ method */
/* RollingLogTtm::InitResources() -- forwards `this` and the class's shared parameter block to
 * the generic implementation the family shares. */
/* Cross-overlay tail-call veneer. #pragma long_calls forces mwccarm to emit the
 * pooled `ldr ip,[pc]; bx ip` indirect tail-call (a plain near `b` otherwise)
 * that the ROM uses to reach another overlay. */
#include "RollingLogTtm.h"
#pragma long_calls on

extern "C" {
int func_ov080_021274ac(void *thisp, void *data);
extern char data_ov030_02115a04[];
}

int RollingLogTtm::InitResources()
{
    return func_ov080_021274ac(this, data_ov030_02115a04);
}
