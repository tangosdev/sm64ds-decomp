//cpp
// @symbol _ZN4Heap10SetDefaultEv
/* Heap::SetDefault() at 0x0203c324 -- install this heap as the process-wide
 * default and hand back the one it replaced, so a caller can put it back.
 *
 * Heap::SetupSolidHeapAsDefault is the caller that shows what the result is
 * for -- although it does not use it, saving the outgoing heap out of the
 * global itself instead.
 *
 * The old text had `int old = data_020a0ea0; data_020a0ea0 = ((int)this);'.
 * Both the global and the return were ints holding a heap pointer, and the
 * cast existed to make that typecheck. The global is 0x020a0ea0, which
 * config/arm9/symbols.txt already names _ZN6Memory14defaultHeapPtrE, so it can
 * be spelled as what it is and the compiler mangles it back to the same
 * symbol. */
#include "Heap.h"

namespace Memory { extern Heap* defaultHeapPtr; }   /* 0x020a0ea0 */

Heap* Heap::SetDefault()
{
    Heap* previous = Memory::defaultHeapPtr;
    Memory::defaultHeapPtr = this;
    return previous;
}
