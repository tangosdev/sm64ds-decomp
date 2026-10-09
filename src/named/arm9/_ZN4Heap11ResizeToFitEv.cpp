//cpp
// @symbol _ZN4Heap11ResizeToFitEv
/* Heap::ResizeToFit() at 0x0203c390 -- hand the arena's unused tail back (slot
 * 15) and, on a fail-fast heap, treat a refusal as fatal.
 *
 * This file already held a correct C++ method. What it did not have was a
 * header: the whole class -- every field and all sixteen vtable slots -- was
 * declared locally, which is why the reconstruction that eventually became
 * include/Heap.h could sit here agreeing with the ROM and help nobody. It is
 * the same class now, from the shared header.
 *
 * The local copy did get two things wrong, which is the argument for having
 * one copy rather than eleven: it declared `bool VDeallocate' and `unsigned int
 * VSetNodeID', where both overrides return void. Neither error was reachable
 * from this file, so nothing could ever have caught them. */
#include "decl_common.h"
#include "Heap.h"

u32 Heap::ResizeToFit()
{
    u32 shrunk = VResizeToFit();
    if (!shrunk)
    {
        if (flags & 0x4000)
        {
            Rescue();
            Crash();
        }
    }
    return shrunk;
}
