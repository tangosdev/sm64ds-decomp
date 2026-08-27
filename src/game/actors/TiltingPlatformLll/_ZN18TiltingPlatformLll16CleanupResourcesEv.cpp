//cpp
// @symbol _ZN18TiltingPlatformLll16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Cross-overlay tail-call veneer. #pragma long_calls forces mwccarm to emit
 * the pooled `ldr ip,[pc]; bx ip` indirect tail-call (a plain near `b`
 * otherwise) that the ROM uses to reach another overlay. Loads the data
 * pointer into r1; this stays in r0.
 *
 * Same table as InitResources -- data_ov064_0211adb0 is handed to the load
 * worker and to the release worker alike, which is what makes the pair a
 * matched set rather than two unrelated veneers.
 */
#pragma long_calls on
#include "TiltingPlatformLll.h"

extern "C" {
extern int func_ov002_020b60fc(void *thisp, void *data);
extern char data_ov064_0211adb0[];
}

int TiltingPlatformLll::CleanupResources()
{
    return func_ov002_020b60fc(this, data_ov064_0211adb0);
}
