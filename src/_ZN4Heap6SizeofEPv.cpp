//cpp
// @symbol _ZN4Heap6SizeofEPv
/* Heap::Sizeof(void*) at 0x0203c454 -- asks slot 9 how large an allocated block
 * is, and treats -1 as the failure report.
 *
 * The result is held in an `int' on purpose. Slot 9 is declared u32 (that is
 * what the overrides return), but the guard here is a comparison against -1,
 * and the ROM does it signed. Widening the local to u32 would turn that into an
 * unsigned compare and change the instruction.
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
