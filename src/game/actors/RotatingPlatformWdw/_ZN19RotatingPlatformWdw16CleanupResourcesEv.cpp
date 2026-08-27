//cpp
// @symbol _ZN19RotatingPlatformWdw16CleanupResourcesEv
/* recovered: real C++ method */
/* RotatingPlatformWdw::CleanupResources() -- forwards `this` and the class's shared parameter block to
 * the generic implementation the family shares. */
/* Cross-overlay tail-call veneer. #pragma long_calls forces mwccarm to emit the
 * pooled `ldr ip,[pc]; bx ip` indirect tail-call (a plain near `b` otherwise)
 * that the ROM uses to reach another overlay. */
#include "RotatingPlatformWdw.h"
#pragma long_calls on

extern "C" {
int func_ov002_020b66a8(void *thisp, void *data);
extern char data_ov029_02113fd4[];
}

int RotatingPlatformWdw::CleanupResources()
{
    return func_ov002_020b66a8(this, data_ov029_02113fd4);
}
