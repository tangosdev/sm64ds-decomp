// @symbol func_ov015_02112c84
// recovered name: daObjBk_Ukisima_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjBk_Ukisima_c::CleanupResources - verified 2004/b56 byte-match (ov015), strict-reloc */
// Cross-overlay tail-call veneer. #pragma long_calls forces mwccarm to emit the pooled
// `ldr ip,[pc]; bx ip` indirect tail-call (a plain near `b` otherwise) that the ROM uses
// to reach another overlay. Loads the data pointer into r1; this stays in r0.
#pragma long_calls on
extern char data_ov015_021147a4[];

int func_ov015_02112c84(void *thisp)
{
    return func_020b66a8(thisp, data_ov015_021147a4);
}
