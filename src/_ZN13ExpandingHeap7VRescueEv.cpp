//cpp
// @symbol _ZN13ExpandingHeap7VRescueEv
/* ExpandingHeap::VRescue() at 0x0203c630 -- Heap vtable slot 7. Four bytes:
 * `bx lr'. No recovery is attempted.
 *
 * The previous comment guessed that the name "appears to be called just
 * before/by Crash, perhaps to rescue heap data before a fatal exception". The
 * caller is known and it is Heap::ResizeToFit, which calls Rescue() and then
 * Crash() when a fail-fast heap cannot shrink -- so the guess had the order
 * right and the direction wrong: Rescue runs before Crash because Crash is what
 * follows a failure, not because Crash calls it. */
#include "ExpandingHeap.h"

void ExpandingHeap::VRescue()
{
}
