//cpp
// @symbol _ZN23FloatOnWaterPlatformJrb13InitResourcesEv
#include "FloatOnWaterPlatformJrb.h"

// Cross-overlay tail-call veneer. #pragma long_calls forces mwccarm to emit the pooled
// `ldr ip,[pc]; bx ip` indirect tail-call (a plain near `b` otherwise) that the ROM uses
// to reach another overlay. func_ov002_020b5e58 is daObjFloatBoard_c's shared file-load
// helper, out of this task's scope, kept under its existing name.
#pragma long_calls on
extern "C" {
extern int func_ov002_020b5e58(void *self, void *data);
extern char data_ov016_02114b8c[];
}

int FloatOnWaterPlatformJrb::InitResources()
{
    return func_ov002_020b5e58(this, data_ov016_02114b8c);
}
