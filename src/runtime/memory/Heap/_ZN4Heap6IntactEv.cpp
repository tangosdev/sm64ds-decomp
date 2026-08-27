//cpp
// @symbol _ZN4Heap6IntactEv
/* Heap::Intact() at 0x0203c664. Asks the concrete heap whether its bookkeeping
 * is still consistent (slot 6), and latches a global the first time one says no.
 *
 * This file used to reach the vtable by casting `this' to a locally declared
 * `struct Base { virtual void v0(); ... virtual bool m(); }' -- six anonymous
 * slots of padding so that `m' landed on index 6. The cast is gone: Heap now
 * declares VIntact as its own slot-6 pure virtual, so this is an ordinary
 * virtual call and the slot index is the header's business, not this file's. */
#include "decl_common.h"
#include "Heap.h"

bool Heap::Intact()
{
    bool intact = VIntact();
    if (intact)
        return intact;

    /* Latched once and never cleared -- the first corrupted heap in the run is
       the one worth reporting. */
    if (!data_020a0e98)
        data_020a0e98 = 1;
    return intact;
}
