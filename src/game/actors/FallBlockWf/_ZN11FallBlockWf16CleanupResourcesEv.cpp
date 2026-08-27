//cpp
// @symbol _ZN11FallBlockWf16CleanupResourcesEv
/* recovered: real C++ method */
/* FallBlockWf::CleanupResources() -- forwards `this` and the class's shared parameter block to
 * the generic implementation the family shares. */
/* Cross-overlay tail-call veneer. #pragma long_calls forces mwccarm to emit the
 * pooled `ldr ip,[pc]; bx ip` indirect tail-call (a plain near `b` otherwise)
 * that the ROM uses to reach another overlay. */
#include "FallBlockWf.h"
#pragma long_calls on

extern "C" {
int func_ov098_0213a2cc(void *thisp, void *data);
extern char data_ov015_02114880[];
}

int FallBlockWf::CleanupResources()
{
    return func_ov098_0213a2cc(this, data_ov015_02114880);
}
