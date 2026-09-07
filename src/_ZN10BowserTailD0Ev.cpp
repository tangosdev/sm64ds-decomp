//cpp
// @symbol _ZN10BowserTailD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is vtable slot 17: destroy, then return the object to the actor heap. The
 * hand-written version spelled that out -- store the vtable, call the member's
 * destructor, chain to dActor_c, call Memory::Deallocate. All four come from the same
 * `~BowserTail()` the D1 file declares; the deallocation is the inline
 * dActor_c::operator delete, which is why nothing here mentions the heap.
 *
 * The identical body in both files is not duplication: D1 and D0 are two of the
 * three functions the compiler emits from one destructor, and each file is bound
 * to one of them by config/arm9/overlays/ov060/delinks.txt.
 */
#include "BowserTail.h"

BowserTail::~BowserTail()
{
}
