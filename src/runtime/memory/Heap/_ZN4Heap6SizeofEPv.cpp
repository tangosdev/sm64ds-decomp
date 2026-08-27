//cpp
// @symbol _ZN4Heap6SizeofEPv
/* Heap::Sizeof(void*) at 0x0203c454 -- asks slot 9 how large an allocated block
 * is, and treats -1 as the failure report.
 *
 * The result is held in an `int' because that is what this function returns;
 * slot 9 is declared u32, which is what the overrides return. The choice is
 * BYTE-UNOBSERVABLE, and that was measured, not assumed: compiling this with
 * `u32 size' and `size == (u32)-1' is byte-identical under 2004/b56. An earlier
 * revision of this comment claimed the u32 spelling "would turn that into an
 * unsigned compare and change the instruction", which is simply false -- the
 * guard is an equality, so it lowers to cmp/bne on the Z flag and there is no
 * signed or unsigned variant of that to pick between. Runbook section 2 asks
 * for "byte-unobservable", not "verified"; the earlier wording was the exact
 * overclaim it warns about.
 *
 * Was a cast of `this' to a local `struct Base' with nine anonymous virtuals
 * padding `m' out to index 9. */
#include "decl_common.h"
#include "Heap.h"

int Heap::Sizeof(void* ptr)
{
    int size = VSizeof(ptr);
    if (size == -1 && (flags & 0x4000))
        Crash();
    return size;
}
