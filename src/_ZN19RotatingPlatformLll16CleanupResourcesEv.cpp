//cpp
// @symbol _ZN19RotatingPlatformLll16CleanupResourcesEv
#include "RotatingPlatformLll.h"

// Cross-overlay tail-call veneer. #pragma long_calls forces mwccarm to emit the pooled
// `ldr ip,[pc]; bx ip` indirect tail-call (a plain near `b` otherwise) that the ROM uses
// to reach another overlay. func_ov002_020b66a8 is daObjKaitendai_c's shared cleanup
// helper, out of this task's scope, kept under its existing name.
#pragma long_calls on
extern "C" {
extern int func_ov002_020b66a8(void *self, void *data);
extern int data_ov022_02113da4[];
}

int RotatingPlatformLll::CleanupResources()
{
    return func_ov002_020b66a8(this, data_ov022_02113da4);
}
