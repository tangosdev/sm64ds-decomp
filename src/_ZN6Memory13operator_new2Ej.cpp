//cpp
// @symbol _ZN6Memory13operator_new2Ej
/* Memory::operator_new2(u32) at 0x0203cbd8 -- a three-word tail-call veneer to
 * func_0203cc0c: `ldr ip,[pc] / bx ip / .word 0x0203cc0c'.
 *
 * Migrated as a real namespace function per runbook section 8: the veneer is
 * emitted for the CALL, not for the argument list, so restoring the real
 * signature costs nothing. Its size comes from the mangled name -- `Ej', u32 --
 * and the file used to declare BOTH this function and its target as taking
 * nothing at all, which byte-matches because a tail call never touches r0-r3.
 *
 * The target keeps its address name. Naming it would be a claim of its own, and
 * migration is per-reference. */
#include "types.h"

namespace Memory {

extern "C" void* func_0203cc0c(u32 size);

void* operator_new2(u32 size)
{
    return func_0203cc0c(size);
}

}
