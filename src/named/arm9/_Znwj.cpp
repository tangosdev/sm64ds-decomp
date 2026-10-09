//cpp
// @symbol _Znwj
/* operator new(unsigned int) at 0x0203cbe4 (arm9) -- three-word tail-call
 * veneer to func_0203cc0c: `ldr ip,[pc] / bx ip / .word 0x0203cc0c'.
 *
 * Real signature restored (same rationale as Memory::operator_new2): the veneer
 * is emitted for the CALL, not for the argument list, so the old void/void
 * declaration was a lie that happened to match.
 *
 * Name remains hand-spelled under extern "C". mwccarm 2004/b56 (CW 1.2) rejects
 * `void* operator new(u32)` with "illegal 'operator' declaration" — the same
 * restriction documented on fBase_c::operator new in include/fBase_c.h and
 * the reason _ZdlPv keeps its mangled definition. The target keeps its address
 * name. */
#include "types.h"

extern "C" {

extern void* func_0203cc0c(u32 size);

void* _Znwj(u32 size)
{
    return func_0203cc0c(size);
}

}
