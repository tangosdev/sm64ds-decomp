//cpp
// @symbol _ZN6Memory16operator_delete2EPv
/* Memory::operator_delete2(void*) at 0x0203cbcc -- a three-word tail-call veneer
 * to _ZdlPv, the global `operator delete(void*)', at 0x0203cbf0.
 *
 * Migrated as a real namespace function per runbook section 8. Unlike its
 * new-side twin the target here already has a real C++ name, so the call is
 * spelled as the operator it is rather than as an address.
 *
 * As with every veneer in the tree, this file used to declare both sides as
 * `void f(void)'. That byte-matches -- a tail call leaves r0-r3 alone -- and is
 * exactly why the wrong prototype went unchallenged. */
#include "types.h"

namespace Memory {

extern "C" void _ZdlPv(void* ptr);

void operator_delete2(void* ptr)
{
    _ZdlPv(ptr);
}

}
