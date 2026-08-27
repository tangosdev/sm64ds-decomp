//cpp
// @symbol _ZN18TiltingPlatformLll13InitResourcesEv
/* recovered: shared header, real C++ method
 *
 * Cross-overlay tail-call veneer. #pragma long_calls forces mwccarm to emit
 * the pooled `ldr ip,[pc]; bx ip` indirect tail-call (a plain near `b`
 * otherwise) that the ROM uses to reach another overlay. Loads the data
 * pointer into r1; this stays in r0.
 *
 * The real body is ov002's, shared with the other platform classes; all
 * TiltingPlatformLll contributes is its own table. That is why the method touches
 * no field of its own.
 */
#pragma long_calls on
#include "TiltingPlatformLll.h"

extern "C" {
extern int func_ov002_020b6244(void *thisp, void *data);
extern char data_ov064_0211adb0[];
}

int TiltingPlatformLll::InitResources()
{
    return func_ov002_020b6244(this, data_ov064_0211adb0);
}
