//cpp
// @symbol _ZN18RotatingPlatformWf16CleanupResourcesEv
#include "RotatingPlatformWf.h"

// Cross-overlay tail-call veneer. #pragma long_calls forces mwccarm to emit the pooled
// `ldr ip,[pc]; bx ip` indirect tail-call (a plain near `b` otherwise) that the ROM uses
// to reach another overlay. Loads the data pointer into r1; this stays in r0.
#pragma long_calls on
extern "C" {
extern int func_ov002_020b66a8(void *self, void *data);
extern char data_ov015_021147a4[];
}

int RotatingPlatformWf::CleanupResources()
{
    return func_ov002_020b66a8(this, data_ov015_021147a4);
}
