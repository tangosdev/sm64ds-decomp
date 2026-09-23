//cpp
// @symbol _ZN15daObjHmMaruta_c16CleanupResourcesEv
/* recovered: real C++ method */
/* daObjHmMaruta_c::CleanupResources() -- forwards `this` and the class's shared parameter block to
 * the generic implementation the family shares. */
/* Cross-overlay tail-call veneer. #pragma long_calls forces mwccarm to emit the
 * pooled `ldr ip,[pc]; bx ip` indirect tail-call (a plain near `b` otherwise)
 * that the ROM uses to reach another overlay. */
#include "daObjHmMaruta_c.h"
#pragma long_calls on

extern "C" {
int func_ov080_021270dc(void *thisp, void *data);
extern char data_ov030_02115a04[];
}

int daObjHmMaruta_c::CleanupResources()
{
    return func_ov080_021270dc(this, data_ov030_02115a04);
}
