//cpp
// @symbol _ZN12FallBlockBbh13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "FallBlockBbh.h"
// Cross-overlay tail-call veneer. #pragma long_calls forces mwccarm to emit the pooled
// `ldr ip,[pc]; bx ip` indirect tail-call (a plain near `b` otherwise) that the ROM uses
// to reach another overlay. Loads the data pointer into r1; this stays in r0.
#pragma long_calls on

int FallBlockBbh::InitResources()
{
    return func_ov098_0213a794(((void *)this), data_ov063_0211eb10);
}
